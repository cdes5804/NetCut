#ifndef _HOST_HPP
#define _HOST_HPP

#include <string>

enum class Status { NORMAL, CUT, NOT_EXIST };

class Host {
  private:
    std::string ip_address;
    mutable std::string mac_address;
    mutable Status status;
    
  public:
    Host(); // empty host, host equivalence of null
    Host(const std::string &ip_address); // temporay host, used for searching
    Host(const std::string &ip_address, const std::string &mac_address);
    bool operator<(const Host &other) const;
    std::string get_ip() const;
    std::string get_mac() const;
    Status get_status() const;
    bool is_cut() const;
    void set_status(Status status) const;
    void set_mac_address(const std::string &mac_address) const;
};

#endif