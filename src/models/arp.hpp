#ifndef _ARP_HPP
#define _ARP_HPP

#include "models/host.hpp"
#include "models/interface.hpp"

#include <string>
#include <map>
#include <cstdint>

constexpr int PROTO_ARP = 0x0806;
constexpr int ETH2_HEADER_LEN = 14;
constexpr int HW_TYPE = 1;
constexpr int PROTOCOL_TYPE = 0x800;
constexpr int MAC_LENGTH = 6;
constexpr int IPV4_LENGTH = 4;
constexpr int ARP_REQUEST = 0x01;
constexpr int ARP_REPLY = 0x02;
constexpr int BUF_SIZE = 512;

struct arp_header {
    unsigned short hardware_type;
    unsigned short protocol_type;
    unsigned char hardware_len;
    unsigned char  protocol_len;
    unsigned short opcode;
    unsigned char sender_mac[MAC_LENGTH];
    unsigned char sender_ip[IPV4_LENGTH];
    unsigned char target_mac[MAC_LENGTH];
    unsigned char target_ip[IPV4_LENGTH];
};

enum class Operation { REQUEST, SPOOF, RECOVER };

class ARP {
  private:
    int sd;
    Interface interface;
    struct sockaddr_ll prepare_arp(unsigned char *buffer, const uint32_t &dst_ip, Operation op=Operation::REQUEST,
                                    const std::string &dst_mac="", const uint32_t &src_ip=0, const std::string &src_mac="") const;
    void _listen(std::map<std::string, std::string> &arp_table) const;
    void _spoof(const Host &target, const std::string &spoof_src_ip, const int sd, const int attack_interval_ms, const std::string &fake_mac_address) const;

  public:
    ARP(const Interface &_interface);
    ARP();
    ~ARP();
    void request(const std::string &target_ip) const;
    void listen(std::map<std::string, std::string> &arp_table) const;
    void spoof(const Host &target, const Host &spoof_src, const int attack_interval_ms, const std::string &fake_mac_address) const;
    void recover(const Host &target, const Host &spoof_src) const;
};

#endif
