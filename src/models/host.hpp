#ifndef _HOST_HPP
#define _HOST_HPP

#include <string>

using std::string;

enum class Status { NORMAL, CUT };

class Host {
  private:
    string ip_address;
    mutable string mac_address;
    mutable Status status;
    
  public:
    Host(const string &ip_address, const string &mac_address);
    bool operator<(const Host &other) const;
    string get_ip() const;
    string get_mac() const;
    bool is_cut() const;
    void set_status(Status status) const;
    void set_mac_address(const string &mac_address) const;
};

#endif