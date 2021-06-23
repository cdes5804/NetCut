#ifndef _ARP_HPP
#define _ARP_HPP

#include <models/interface.hpp>

#include <string>

#include <linux/if_arp.h>

namespace Arp {
    constexpr int PROTO_ARP = 0x0806;
    constexpr int ETH2_HEADER_LEN = 14;
    constexpr int HW_TYPE = 1;
    constexpr int PROTOCOL_TYPE = 0x800;
    constexpr int MAC_LENGTH = 6;
    constexpr int IPV4_LENGTH = 4;
    constexpr int ARP_REQUEST = 0x01;
    constexpr int ARP_REPLY = 0x02;
    constexpr int BUF_SIZE = 512;

    // Structure containing the header fields of a arp packet.
    struct arp_header {
        unsigned short hardware_type;
        unsigned short protocol_type;
        unsigned char hardware_len;
        unsigned char protocol_len;
        unsigned short opcode;
        unsigned char sender_mac[MAC_LENGTH];
        unsigned char sender_ip[IPV4_LENGTH];
        unsigned char target_mac[MAC_LENGTH];
        unsigned char target_ip[IPV4_LENGTH];
    };

    // Simple structure representing an arp response.
    // is_valid indicates whether it's a valid response.
    // If it's valid, it contains the sender's ip and mac address.
    struct arp_response {
        std::string ip_address;
        std::string mac_address;
        bool is_valid;
    };

    // There are some common fields to fill for both broadcast and unicast arp message.
    // Fill them in this function.
    void prepare_arp_common_fields(
        struct ethhdr *ether_hdr,
        struct arp_header *arp_hdr,
        struct sockaddr_ll *socket_address,
        const Interface &interface
    );

    // Given the source mac address in string, fill them in the arp packet appropriately.
    void fill_arp_src_mac(
        struct ethhdr *ether_hdr,
        struct arp_header *arp_hdr,
        struct sockaddr_ll *socket_address,
        const std::string &src_mac_address
    );

    // Given the destination mac address in string, fill them in the arp packet appropriately.
    void fill_arp_dst_mac(
        struct ethhdr *ether_hdr,
        struct arp_header *arp_hdr,
        const std::string &dst_mac_address
    );

    // Given the source and destination IP in strings, fill the corresponding fields in arp packet.
    void fill_arp_ips(
        struct arp_header *arp_hdr,
        const std::string &src_ip,
        const std::string &dst_ip
    );

    // Prepare arp packet from broadcast.
    struct sockaddr_ll prepare_arp(
        unsigned char *buffer,
        const std::string &dst_ip,
        const Interface &interface
    );

    // Prepare arp packet for spoofing and recovering.
    struct sockaddr_ll prepare_arp(
        unsigned char *buffer,
        const std::string dst_ip,
        const std::string &dst_mac,
        const std::string &src_ip,
        const std::string &src_mac,
        const Interface &interface
    );

    // Given a raw packet, try to parse it as an arp response and return an arp_response object indicating the result.
    arp_response parse_arp_response(unsigned char *buffer);
}

#endif