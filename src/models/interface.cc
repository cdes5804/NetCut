#include "models/interface.hh"

#include <arpa/inet.h>
#include <net/if.h>

Interface::Interface(std::string name, std::string ip_address, std::string netmask)
    : name_(std::move(name)), ip_(std::move(ip_address)), netmask_(std::move(netmask)) {}

// empty Interface, interface equivalence of null
Interface::Interface() = default;

std::string Interface::GetName() const { return name_; }

std::string Interface::GetIp() const { return ip_; }

std::string Interface::GetNetMask() const { return netmask_; }

bool Interface::IsSameSubnet(const std::string &ip_address) const {
  struct in_addr addr;
  struct in_addr subnet_mask;

  inet_pton(AF_INET, this->ip_.c_str(), &addr);
  inet_pton(AF_INET, this->netmask_.c_str(), &subnet_mask);

  uint32_t ip_start = ntohl(addr.s_addr & subnet_mask.s_addr);
  uint32_t ip_end = ntohl(addr.s_addr | ~(subnet_mask.s_addr));

  inet_pton(AF_INET, ip_address.c_str(), &addr);
  uint32_t check_start = ntohl(addr.s_addr & subnet_mask.s_addr);
  uint32_t check_end = ntohl(addr.s_addr | ~(subnet_mask.s_addr));
  return ip_start == check_start && ip_end == check_end;
}