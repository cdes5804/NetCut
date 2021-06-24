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

    // Given an ifaddrs struct, check if it is of family AF_INET.
    // We can retrieve the IP address of the associated interface from it.
    bool is_network(const struct ifaddrs *ifaddr) const;

    // Given an ifaddrs struct, check if it is of family AF_PACKET.
    // We can retrieve the MAC address of the associated interface from it.
    bool is_link(const struct ifaddrs *ifaddr) const;

    // Retrieve the IP address of an interface.
    std::string get_ip(const struct ifaddrs *ifaddr) const;

    // Retrieve the netmask of an interface.
    std::string get_netmask(const struct ifaddrs *ifaddr) const;

    // Retrieve the interface name of an interface.
    std::string get_interface_name(const struct ifaddrs *ifaddr) const;

    // Given an interface, scan for active hosts under the same subnet.
    void scan_subnet(const Interface &interface);

    // Scan the local server for active interfaces.
    void scan_interface();

    // Given an interface name, find the gateway IP for the subnet that the interface belongs in.
    std::string find_gateway(const std::string &interface_name);

    // Internal function for listening to ARP reply.
    void _listen();

  public:
    NetworkScanner(const uint32_t idle_threshold);

    // Public method for scanning the network and retrieve active hosts.
    // After sending ARP request packets, we wait for `waiting_time_ms` for the reply to arrive.
    std::vector<Host> scan_networks(const uint32_t waiting_time_ms=10);

    // Given an IP address, find the interface that is under the same subnet.
    Interface get_interface_by_ip(const std::string &ip) const;

    // Public method for starting to listen for ARP reply.
    void listen();
};

#endif