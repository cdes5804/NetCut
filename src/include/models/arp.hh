#ifndef ARP_HH
#define ARP_HH

#include "models/host.hh"
#include "models/interface.hh"

#include <cstdint>
#include <map>
#include <string>

constexpr int PROTO_ARP = 0x0806;
constexpr int ETH2_HEADER_LEN = 14;
constexpr int HW_TYPE = 1;
constexpr int PROTOCOL_TYPE = 0x800;
constexpr int MAC_LENGTH = 6;
constexpr int IPV4_LENGTH = 4;
constexpr int ARP_REQUEST = 0x01;
constexpr int ARP_REPLY = 0x02;
constexpr int BUF_SIZE = 512;

struct ArpHeader {
  uint16_t hardware_type_;
  uint16_t protocol_type_;
  uint8_t hardware_len_;
  uint8_t protocol_len_;
  uint16_t opcode_;
  uint8_t sender_mac_[MAC_LENGTH];
  uint8_t sender_ip_[IPV4_LENGTH];
  uint8_t target_mac_[MAC_LENGTH];
  uint8_t target_ip_[IPV4_LENGTH];
};

enum class Operation { REQUEST, SPOOF, RECOVER };

class ARP {
 private:
  int socket_fd_;
  Interface interface_;
  struct sockaddr_ll PrepareArp(uint8_t *buffer, const uint32_t &dst_ip, Operation operation = Operation::REQUEST,
                                const std::string &dst_mac = "", const uint32_t &src_ip = 0,
                                const std::string &src_mac = "") const;
  void InternalListen(std::map<std::string, std::string> &arp_table) const;
  void InternalSpoof(const Host &target, const std::string &spoof_src_ip, int socket_fd, int attack_interval_ms,
                     const std::string &fake_mac_address) const;

 public:
  explicit ARP(Interface interface);
  ARP();
  ~ARP();
  void Request(const std::string &target_ip) const;
  void Listen(std::map<std::string, std::string> &arp_table) const;
  void Spoof(const Host &target, const Host &spoof_src, int attack_interval_ms,
             const std::string &fake_mac_address) const;
  void Recover(const Host &target, const Host &spoof_src) const;
};

#endif
