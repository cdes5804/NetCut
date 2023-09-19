#include "models/scanner.hpp"
#include "models/host.hpp"
#include "models/arp.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>

bool NetworkScanner::is_network(const struct ifaddrs *ifaddr) const {
    if (ifaddr->ifa_addr == NULL) {
        return false;
    }

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
        std::cerr << "get_ip: Unable to get interface IP.\n";
        exit(EXIT_FAILURE);
    }

    return std::string(buffer);
}

std::string NetworkScanner::get_interface_name(const struct ifaddrs *ifaddr) const {
    return std::string(ifaddr->ifa_name);
}

void NetworkScanner::scan_subnet(const Interface &interface) {
    ARP arp(interface);
    arp.listen(arp_table);

    struct in_addr ip_address;
    struct in_addr subnet_mask;

    inet_pton(AF_INET, interface.get_ip().c_str(), &ip_address);
    inet_pton(AF_INET, interface.get_netmask().c_str(), &subnet_mask);

    uint32_t start_ip = ntohl(ip_address.s_addr & subnet_mask.s_addr);
    uint32_t end_ip = ntohl(ip_address.s_addr | ~(subnet_mask.s_addr));

    for (uint32_t ip_addr = start_ip; ip_addr <= end_ip; ip_addr++) {
        uint32_t network_byte_ip = htonl(ip_addr);

        struct in_addr target_ip_struct;
        target_ip_struct.s_addr = network_byte_ip;

        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &target_ip_struct, buffer, INET_ADDRSTRLEN);

        string target_ip = string(buffer);

        if (target_ip == interface.get_ip()) {
            continue;
        }
        
        arp.request(target_ip);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3)); // wait some time for the arp response
}

std::vector<Host> NetworkScanner::scan_networks() {
    struct ifaddrs *ifaddrs;

    if (getifaddrs(&ifaddrs) == -1) {
        std::cerr << "scan_networks: Unable to get interface information.\n";
        exit(EXIT_FAILURE);
    }

    for (struct ifaddrs *ifaddr = ifaddrs; ifaddr != NULL; ifaddr = ifaddr->ifa_next) {
        if (is_network(ifaddr)) {
            string ip = get_ip(ifaddr);
            string netmask = get_netmask(ifaddr);
            string interface_name = get_interface_name(ifaddr);

            interfaces.emplace_back(interface_name, ip, netmask);
            scan_subnet(interfaces.back());
        }
    }

    freeifaddrs(ifaddrs);

    std::vector<Host> hosts;
    for (auto &[ip_address, mac_address] : arp_table) {
        hosts.emplace_back(ip_address, mac_address);
    }

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
