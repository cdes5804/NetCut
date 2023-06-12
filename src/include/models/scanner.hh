#ifndef SCANNER_HH
#define SCANNER_HH

#include "models/host.hh"
#include "models/interface.hh"

#include <map>
#include <string>
#include <vector>

#include <ifaddrs.h>

class NetworkScanner {
 private:
  std::map<std::string, std::string> arp_table_;
  std::vector<Interface> interfaces_;
  bool IsNetwork(const struct ifaddrs *ifaddr) const;
  bool IsLink(const struct ifaddrs *ifaddr) const;
  std::string GetIp(const struct ifaddrs *ifaddr) const;
  std::string GetNetMask(const struct ifaddrs *ifaddr) const;
  std::string GetInterfaceName(const struct ifaddrs *ifaddr) const;
  void ScanSubnet(const Interface &interface);

 public:
  std::vector<Host> ScanNetworks();
  Interface GetInterfaceByIp(const std::string &ip_address) const;
};

#endif