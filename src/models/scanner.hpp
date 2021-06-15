#ifndef _SCANNER_HPP
#define _SCANNER_HPP

#include "models/host.hpp"
#include "models/interface.hpp"

#include <vector>
#include <string>
#include <map>

#include <ifaddrs.h>

class NetworkScanner {
  private:
    std::map<std::string, std::string> arp_table;
    std::vector<Interface> interfaces;
    bool is_network(const struct ifaddrs *ifaddr) const;
    bool is_link(const struct ifaddrs *ifaddr) const;
    std::string get_ip(const struct ifaddrs *ifaddr) const;
    std::string get_netmask(const struct ifaddrs *ifaddr) const;
    std::string get_interface_name(const struct ifaddrs *ifaddr) const;
    void scan_subnet(const Interface &interface);

  public:
    std::vector<Host> scan_networks();
    Interface get_interface_by_ip(const std::string &ip) const;
};

#endif