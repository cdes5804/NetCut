#ifndef _INTERFACE_HPP
#define _INTERFACE_HPP

#include <string>

class Interface {
  private:
    const std::string name;
    const std::string ip;
    const std::string netmask;
    const int socket_fd;
    const int index;
    const std::string mac_address;
    const std::string gateway_ip;

  public:
    Interface(); // empty Interface, interface equivalence of null
    Interface(const std::string &name, const std::string &ip, const std::string &netmask, const std::string &gateway_ip);

    // Get the name of the interface.
    std::string get_name() const;

    // Get the IP address of the interface.
    std::string get_ip() const;

    // Get the netmask of the interface.
    std::string get_netmask() const;

    // Get the interface index kept by the OS of the interface.
    int get_index() const;

    // Get the socket associated with the interface.
    int get_socket_fd() const;

    // Get the MAC address of the interface.
    std::string get_mac_address() const;

    // Get the gateway IP of the subnet that the interface belongs in.
    std::string get_gateway_ip() const;

    // Given an IP address, check if it is under the same subnet of the interface.
    bool is_same_subnet(const std::string &ip) const;
};

#endif