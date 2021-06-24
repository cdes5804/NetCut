#include "models/scanner.hpp"
#include "utils/arp_utils.hpp"
#include "utils/thread_utils.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

NetworkScanner::NetworkScanner(const uint32_t idle_threshold)
    : scan_count(0), idle_threshold(idle_threshold) {
    scan_interface();
}

bool NetworkScanner::is_network(const struct ifaddrs *ifaddr) const {
    sa_family_t family = ifaddr->ifa_addr->sa_family;
    unsigned int flags = ifaddr->ifa_flags;
    bool is_up = (flags & IFF_UP) != 0;
    bool is_loopback = (flags & IFF_LOOPBACK) != 0;

    return family == AF_INET && is_up && !is_loopback;
}

bool NetworkScanner::is_link(const struct ifaddrs *ifaddr) const {
    sa_family_t family = ifaddr->ifa_addr->sa_family;
    unsigned int flags = ifaddr->ifa_flags;
    bool is_up = (flags & IFF_UP) != 0;
    bool is_loopback = (flags & IFF_LOOPBACK) != 0;

    return family == AF_PACKET && is_up && !is_loopback;
}

std::string NetworkScanner::get_ip(const struct ifaddrs *ifaddr) const {
    char buffer[INET_ADDRSTRLEN];

    if (getnameinfo(ifaddr->ifa_addr, sizeof(struct sockaddr_in), buffer, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST) != 0) {
        std::cerr << "get_ip: Unable to get interface IP.\n";
        exit(EXIT_FAILURE);
    }

    return std::string(buffer);
}

std::string NetworkScanner::get_netmask(const struct ifaddrs *ifaddr) const {
    char buffer[INET_ADDRSTRLEN];

    if (getnameinfo(ifaddr->ifa_netmask, sizeof(struct sockaddr_in), buffer, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST) != 0) {
        std::cerr << "get_ip: Unable to get interface netmask.\n";
        exit(EXIT_FAILURE);
    }

    return std::string(buffer);
}

std::string NetworkScanner::get_interface_name(const struct ifaddrs *ifaddr) const {
    return std::string(ifaddr->ifa_name);
}

void NetworkScanner::scan_subnet(const Interface &interface) {
    struct in_addr ip_address;
    struct in_addr subnet_mask;

    inet_pton(AF_INET, interface.get_ip().c_str(), &ip_address);
    inet_pton(AF_INET, interface.get_netmask().c_str(), &subnet_mask);

    uint32_t start_ip = ntohl(ip_address.s_addr & subnet_mask.s_addr);
    uint32_t end_ip = ntohl(ip_address.s_addr | ~(subnet_mask.s_addr));

    for (uint32_t ip_addr = start_ip; ip_addr <= end_ip; ++ip_addr) {
        uint32_t network_byte_ip = htonl(ip_addr);

        struct in_addr target_ip_struct;
        target_ip_struct.s_addr = network_byte_ip;

        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &target_ip_struct, buffer, INET_ADDRSTRLEN);

        std::string target_ip = std::string(buffer);

        if (target_ip == interface.get_ip()) {
            continue;
        }

        Arp::request(target_ip, interface);
    }
}

void NetworkScanner::scan_interface() {
    struct ifaddrs *ifaddrs;

    if (getifaddrs(&ifaddrs) == -1) {
        std::cerr << "scan_networks: Unable to get interface information.\n";
        exit(EXIT_FAILURE);
    }

    for (struct ifaddrs *ifaddr = ifaddrs; ifaddr != NULL; ifaddr = ifaddr->ifa_next) {
        if (is_network(ifaddr)) {
            std::string ip = get_ip(ifaddr);
            std::string netmask = get_netmask(ifaddr);
            std::string interface_name = get_interface_name(ifaddr);
            std::string gateway_ip = find_gateway(interface_name);

            interfaces.emplace_back(interface_name, ip, netmask, gateway_ip);
        }
    }

    freeifaddrs(ifaddrs);
}

std::string NetworkScanner::find_gateway(const std::string &interface_name) {
    constexpr uint8_t INTERFACE_NAME_LEN = 16;
    char iface[INTERFACE_NAME_LEN];
    struct in_addr addr;
    long unsigned int destination, gateway;
    memset(&addr, 0, sizeof(struct in_addr));
    std::ifstream fin("/proc/net/route");
    std::string line;

    while (std::getline(fin, line)) {
        if (sscanf(line.c_str(), "%s%lx%lx", iface, &destination, &gateway) == 3) {
            if (destination == 0 && std::string(iface) == interface_name) {
                fin.close();
                addr.s_addr = gateway;
                return std::string(inet_ntoa(addr));
            }
        }
    }

    fin.close();
    return "";
}

void NetworkScanner::_listen() {
    constexpr unsigned int BUF_SIZE = 512;
    unsigned char buffer[BUF_SIZE];
    while (Thread::listening_signal.load(std::memory_order_relaxed)) {
        for (const Interface &interface : interfaces) {
            ssize_t recv_len = recvfrom(interface.get_socket_fd(), buffer, BUF_SIZE, 0, NULL, NULL);
            if (recv_len == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    errno = 0;
                    continue;
                } else {
                    std::cerr << "_listen: Failed to receive packet.\n";
                    return;
                }
            }

            Arp::arp_response response = Arp::parse_arp_response(buffer);
            if (response.is_valid) {
                Thread::listening_mtx.lock();
                response_count[response.ip_address] = scan_count;
                arp_table[response.ip_address] = response.mac_address;
                Thread::listening_mtx.unlock();
            }
        }
    }
}

std::vector<Host> NetworkScanner::scan_networks(const uint32_t waiting_time_ms) {
    scan_count++;

    for (const Interface &interface : interfaces) {
        scan_subnet(interface);
    }

    // Wait some time for the arp response to arrive.
    std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_ms));

    std::vector<Host> hosts;
    Thread::listening_mtx.lock();
    auto iter = arp_table.begin();
    while (iter != arp_table.end()) {
        if (scan_count - response_count[iter->first] > idle_threshold) {
            response_count.erase(iter->first);
            iter = arp_table.erase(iter);
        } else {
            hosts.emplace_back(iter->first, iter->second);
            iter = std::next(iter);
        }
    }
    Thread::listening_mtx.unlock();

    return hosts;
}

Interface NetworkScanner::get_interface_by_ip(const std::string &ip) const {
    for (const Interface &interface : interfaces) {
        if (interface.is_same_subnet(ip)) {
            return interface;
        }
    }
    return Interface();
}

void NetworkScanner::listen() {
    Thread::listening_signal.store(true);
    Thread::listening_thread = std::thread(&NetworkScanner::_listen, this);
}