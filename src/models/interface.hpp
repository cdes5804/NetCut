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
    std::string get_name() const;
    std::string get_ip() const;
    std::string get_netmask() const;
    int get_index() const;
    int get_socket_fd() const;
    std::string get_mac_address() const;
    std::string get_gateway_ip() const;
    bool is_same_subnet(const std::string &ip) const;
};

#endif