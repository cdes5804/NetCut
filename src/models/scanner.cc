#include "models/scanner.hh"
#include "models/arp.hh"
#include "models/host.hh"

#include <chrono>
#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>

bool NetworkScanner::IsNetwork(const struct ifaddrs *ifaddr) const {
  sa_family_t family = ifaddr->ifa_addr->sa_family;
  unsigned int flags = ifaddr->ifa_flags;
  bool is_up = (flags & IFF_UP) != 0;
  bool is_loopback = (flags & IFF_LOOPBACK) != 0;

  return family == AF_INET && is_up && !is_loopback;
}

bool NetworkScanner::IsLink(const struct ifaddrs *ifaddr) const {
  sa_family_t family = ifaddr->ifa_addr->sa_family;
  unsigned int flags = ifaddr->ifa_flags;
  bool is_up = (flags & IFF_UP) != 0;
  bool is_loopback = (flags & IFF_LOOPBACK) != 0;

  return family == AF_PACKET && is_up && !is_loopback;
}

std::string NetworkScanner::GetIp(const struct ifaddrs *ifaddr) const {
  char buffer[INET_ADDRSTRLEN];

  if (getnameinfo(ifaddr->ifa_addr, sizeof(struct sockaddr_in), buffer, INET_ADDRSTRLEN, nullptr, 0, NI_NUMERICHOST) !=
      0) {
    std::cerr << "get_ip: Unable to get interface IP.\n";
    exit(EXIT_FAILURE);
  }

  return std::string{buffer};
}

std::string NetworkScanner::GetNetMask(const struct ifaddrs *ifaddr) const {
  char buffer[INET_ADDRSTRLEN];

  if (getnameinfo(ifaddr->ifa_netmask, sizeof(struct sockaddr_in), buffer, INET_ADDRSTRLEN, nullptr, 0,
                  NI_NUMERICHOST) != 0) {
    std::cerr << "get_ip: Unable to get interface IP.\n";
    exit(EXIT_FAILURE);
  }

  return std::string{buffer};
}

std::string NetworkScanner::GetInterfaceName(const struct ifaddrs *ifaddr) const {
  return std::string{ifaddr->ifa_name};
}

void NetworkScanner::ScanSubnet(const Interface &interface) {
  ARP arp(interface);
  arp.Listen(arp_table_);

  struct in_addr ip_address;
  struct in_addr subnet_mask;

  inet_pton(AF_INET, interface.GetIp().c_str(), &ip_address);
  inet_pton(AF_INET, interface.GetNetMask().c_str(), &subnet_mask);

  uint32_t start_ip = ntohl(ip_address.s_addr & subnet_mask.s_addr);
  uint32_t end_ip = ntohl(ip_address.s_addr | ~(subnet_mask.s_addr));

  for (uint32_t ip_addr = start_ip; ip_addr <= end_ip; ip_addr++) {
    uint32_t network_byte_ip = htonl(ip_addr);

    struct in_addr target_ip_struct;
    target_ip_struct.s_addr = network_byte_ip;

    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &target_ip_struct, buffer, INET_ADDRSTRLEN);

    std::string target_ip = std::string{buffer};

    if (target_ip == interface.GetIp()) {
      continue;
    }

    arp.Request(target_ip);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(3));  // wait some time for the arp response
}

std::vector<Host> NetworkScanner::ScanNetworks() {
  struct ifaddrs *ifaddrs;

  if (getifaddrs(&ifaddrs) == -1) {
    std::cerr << "scan_networks: Unable to get interface information.\n";
    exit(EXIT_FAILURE);
  }

  for (struct ifaddrs *ifaddr = ifaddrs; ifaddr != nullptr; ifaddr = ifaddr->ifa_next) {
    if (IsNetwork(ifaddr)) {
      std::string ip_address = GetIp(ifaddr);
      std::string netmask = GetNetMask(ifaddr);
      std::string interface_name = GetInterfaceName(ifaddr);

      interfaces_.emplace_back(interface_name, ip_address, netmask);
      ScanSubnet(interfaces_.back());
    }
  }

  freeifaddrs(ifaddrs);

  std::vector<Host> hosts;
  for (auto &[ip_address, mac_address] : arp_table_) {
    hosts.emplace_back(ip_address, mac_address);
  }

  return hosts;
}

Interface NetworkScanner::GetInterfaceByIp(const std::string &ip_address) const {
  for (const Interface &interface : interfaces_) {
    if (interface.IsSameSubnet(ip_address)) {
      return interface;
    }
  }
  return Interface{};
}