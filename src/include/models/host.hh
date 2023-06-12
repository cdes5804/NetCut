#ifndef HOST_HH
#define HOST_HH

#include <string>

enum class Status { NORMAL, CUT };

class Host {
 private:
  std::string ip_address_;
  mutable std::string mac_address_;
  mutable Status status_;

 public:
  Host(const std::string &ip_address, const std::string &mac_address);
  bool operator<(const Host &other) const;
  std::string GetIp() const;
  std::string GetMac() const;
  bool IsCut() const;
  void SetStatus(Status status) const;
  void SetMacAddress(const std::string &mac_address) const;
};

#endif