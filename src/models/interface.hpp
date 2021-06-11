#ifndef _INTERFACE_HPP
#define _INTERFACE_HPP

#include <string>

class Interface {
  private:
    std::string name;
    std::string ip;
    std::string netmask;

  public:
    Interface(const std::string &name, const std::string &ip, const std::string &netmask);
    Interface();
    std::string get_name() const;
    std::string get_ip() const;
    std::string get_netmask() const;
    bool is_same_subnet(const std::string &ip) const;
};

#endif