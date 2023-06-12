#ifndef INTERFACE_HH
#define INTERFACE_HH

#include <string>

class Interface {
 private:
  std::string name_;
  std::string ip_;
  std::string netmask_;

 public:
  Interface(std::string name, std::string ip_address, std::string netmask);
  Interface();
  std::string GetName() const;
  std::string GetIp() const;
  std::string GetNetMask() const;
  bool IsSameSubnet(const std::string &ip_address) const;
};

#endif