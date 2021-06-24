#ifndef _SCANNER_HPP
#define _SCANNER_HPP

#include "models/host.hpp"
#include "models/interface.hpp"

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <ifaddrs.h>

class NetworkScanner {
  private:
    std::map<std::string, std::string> arp_table;
    std::map<std::string, uint32_t> response_count;
    std::vector<Interface> interfaces;
    uint32_t scan_count;
    const uint32_t idle_threshold;

    bool is_network(const struct ifaddrs *ifaddr) const;
    bool is_link(const struct ifaddrs *ifaddr) const;
    std::string get_ip(const struct ifaddrs *ifaddr) const;
    std::string get_netmask(const struct ifaddrs *ifaddr) const;
    std::string get_interface_name(const struct ifaddrs *ifaddr) const;
    void scan_subnet(const Interface &interface);
    void scan_interface();
    std::string find_gateway(const std::string &interface_name);
    void _listen();

  public:
    NetworkScanner(const uint32_t idle_threshold);
    std::vector<Host> scan_networks(const uint32_t waiting_time_ms=10);
    Interface get_interface_by_ip(const std::string &ip) const;
    void listen();
};

#endif