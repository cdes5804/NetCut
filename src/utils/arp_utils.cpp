#include "utils/arp_utils.hpp"
#include "utils/mac_utils.hpp"

#include <iostream>

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <string.h>

void Arp::prepare_arp_common_fields(
    struct ethhdr *ether_hdr,
    struct arp_header *arp_hdr,
    struct sockaddr_ll *socket_address,
    const Interface &interface
) {
    socket_address->sll_family = AF_PACKET;
    socket_address->sll_protocol = htons(ETH_P_ARP);
    socket_address->sll_ifindex = interface.get_index();
    socket_address->sll_hatype = htons(ARPHRD_ETHER);
    socket_address->sll_pkttype = 0; // not relevant when sending packet.
    socket_address->sll_halen = MAC_LENGTH;
    socket_address->sll_addr[6] = 0x00;
    socket_address->sll_addr[7] = 0x00;

    ether_hdr->h_proto = htons(ETH_P_ARP);
    arp_hdr->hardware_type = htons(HW_TYPE);
    arp_hdr->protocol_type = htons(ETH_P_IP);
    arp_hdr->hardware_len = MAC_LENGTH;
    arp_hdr->protocol_len = IPV4_LENGTH;
}

void Arp::fill_arp_src_mac(
    struct ethhdr *ether_hdr,
    struct arp_header *arp_hdr,
    struct sockaddr_ll *socket_address,
    const std::string &src_mac_address
) {
    unsigned char mac_address[MAC_LENGTH];
    Mac::string_mac_to_byte(mac_address, src_mac_address);

    memcpy(ether_hdr->h_source, mac_address, MAC_LENGTH);
    memcpy(arp_hdr->sender_mac, mac_address, MAC_LENGTH);
    memcpy(socket_address->sll_addr, mac_address, MAC_LENGTH);
}

void Arp::fill_arp_dst_mac(
    struct ethhdr *ether_hdr,
    struct arp_header *arp_hdr,
    const std::string &dst_mac_address
) {
    if (dst_mac_address.empty()) {
        memset(ether_hdr->h_dest, 0xff, MAC_LENGTH);
        memset(arp_hdr->target_mac, 0x00, MAC_LENGTH);
        return;
    }

    unsigned char mac_address[MAC_LENGTH];
    Mac::string_mac_to_byte(mac_address, dst_mac_address);

    memcpy(ether_hdr->h_dest, mac_address, MAC_LENGTH);
    memcpy(arp_hdr->target_mac, mac_address, MAC_LENGTH);
}

void Arp::fill_arp_ips(
    struct arp_header *arp_hdr,
    const std::string &src_ip,
    const std::string &dst_ip
) {
    int32_t src_ip_byte = static_cast<int32_t>(inet_addr(src_ip.c_str()));
    int32_t dst_ip_byte = static_cast<int32_t>(inet_addr(dst_ip.c_str()));

    memcpy(arp_hdr->sender_ip, &src_ip_byte, sizeof(int32_t));
    memcpy(arp_hdr->target_ip, &dst_ip_byte, sizeof(int32_t));
}

struct sockaddr_ll Arp::prepare_arp(
    unsigned char *buffer,
    const std::string &dst_ip,
    const Interface &interface
) {
    memset(buffer, 0, BUF_SIZE);
    struct sockaddr_ll socket_address;

    memset(&socket_address, 0, sizeof(socket_address));

    struct ethhdr *ether_hdr = (struct ethhdr *)buffer;
    struct arp_header *arp_hdr = (struct arp_header *)(buffer + ETH2_HEADER_LEN);

    prepare_arp_common_fields(ether_hdr, arp_hdr, &socket_address, interface);

    fill_arp_dst_mac(ether_hdr, arp_hdr, std::string());
    fill_arp_src_mac(ether_hdr, arp_hdr, &socket_address, interface.get_mac_address());
    fill_arp_ips(arp_hdr, interface.get_ip(), dst_ip);

    arp_hdr->opcode = htons(ARP_REQUEST);
    return socket_address;
}

struct sockaddr_ll Arp::prepare_arp(
    unsigned char *buffer,
    const std::string &dst_ip,
    const std::string &dst_mac,
    const std::string &src_ip,
    const std::string &src_mac,
    const Interface &interface
) {
    memset(buffer, 0, BUF_SIZE);
    unsigned char mac_address[MAC_LENGTH];
    struct sockaddr_ll socket_address;

    memset(&socket_address, 0, sizeof(socket_address));
    struct ethhdr *ether_hdr = (struct ethhdr *)buffer;
    struct arp_header *arp_hdr = (struct arp_header *)(buffer + ETH2_HEADER_LEN);

    prepare_arp_common_fields(ether_hdr, arp_hdr, &socket_address, interface);

    fill_arp_dst_mac(ether_hdr, arp_hdr, dst_mac);
    fill_arp_src_mac(ether_hdr, arp_hdr, &socket_address, src_mac);
    fill_arp_ips(arp_hdr, src_ip, dst_ip);

    arp_hdr->opcode = htons(ARP_REPLY);
    return socket_address;
}

Arp::arp_response Arp::parse_arp_response(unsigned char *buffer) {
    struct ethhdr *ether_hdr = (struct ethhdr *)buffer;
    struct arp_header *arp_hdr = (struct arp_header *)(buffer + ETH2_HEADER_LEN);
    arp_response response;

    if (ntohs(ether_hdr->h_proto) != PROTO_ARP || ntohs(arp_hdr->opcode) != ARP_REPLY) {
        response.is_valid = false;
        return response;
    }

    struct in_addr sender_addr;
    memset(&sender_addr, 0, sizeof(struct in_addr));
    memcpy(&sender_addr.s_addr, arp_hdr->sender_ip, sizeof(uint32_t));
    std::string sender_ip = std::string(inet_ntoa(sender_addr));
    std::string sender_mac = Mac::byte_mac_to_string(arp_hdr->sender_mac);

    response.is_valid = true;
    response.ip_address = sender_ip;
    response.mac_address = sender_mac;
    return response;
}

void Arp::request(const std::string &target_ip, const Interface &interface) {
    unsigned char buffer[BUF_SIZE];
    struct sockaddr_ll socket_address = prepare_arp(buffer, target_ip, interface);

    if (sendto(interface.get_socket_fd(), buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
        std::cerr << "request: Failed to send arp request.\n";
    }
}