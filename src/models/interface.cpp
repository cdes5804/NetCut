#include "models/interface.hpp"

#include <net/if.h>
#include <arpa/inet.h>

Interface::Interface(const std::string &name, const std::string &ip, const std::string &netmask) {
    this->name = name;
    this->ip = ip;
    this->netmask = netmask;
}

Interface::Interface() {
    // empty Interface, interface equivalence of null
}

std::string Interface::get_name() const {
    return name;
}

std::string Interface::get_ip() const {
    return ip;
}

std::string Interface::get_netmask() const {
    return netmask;
}

bool Interface::is_same_subnet(const std::string &ip) const {
    struct in_addr ip_address;
    struct in_addr subnet_mask;

    inet_pton(AF_INET, this->ip.c_str(), &ip_address);
    inet_pton(AF_INET, this->netmask.c_str(), &subnet_mask);

    uint32_t ip_start = ntohl(ip_address.s_addr & subnet_mask.s_addr);
    uint32_t ip_end = ntohl(ip_address.s_addr | ~(subnet_mask.s_addr));

    inet_pton(AF_INET, ip.c_str(), &ip_address);
    uint32_t check_start = ntohl(ip_address.s_addr & subnet_mask.s_addr);
    uint32_t check_end = ntohl(ip_address.s_addr | ~(subnet_mask.s_addr));
    return ip_start == check_start && ip_end == check_end;
}