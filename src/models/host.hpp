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
    bool operator==(const Host &other) const;

    // Get the IP of the host.
    std::string get_ip() const;

    // Get the MAC address of the host.
    std::string get_mac() const;

    // Get the Internet connection status of the host.
    // Either NORMAL, CUT, or NOT_EXIST.
    Status get_status() const;

    // Check if the host is currently under attack.
    bool is_cut() const;

    // Set the Internet connection status of the host.
    void set_status(Status status) const;
};

#endif