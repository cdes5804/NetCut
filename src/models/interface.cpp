#include "models/interface.hpp"
#include "utils/mac_utils.hpp"
#include "utils/socket_utils.hpp"

#include <arpa/inet.h>
#include <net/if.h>

Interface::Interface(const std::string &name, const std::string &ip, const std::string &netmask) :
    name(name),
    ip(ip),
    netmask(netmask),
    socket_fd(Socket::open_socket()),
    index(Socket::get_interface_index(socket_fd, name)),
    mac_address(Mac::get_interface_mac_address(socket_fd, name)) {
    Socket::bind_socket(socket_fd, name);
}

Interface::Interface() : socket_fd(0), index(0) {}

Interface::~Interface() {
    Socket::close_socket(this->socket_fd);
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

int Interface::get_index() const {
    return index;
}

int Interface::get_socket_fd() const {
    return socket_fd;
}

std::string Interface::get_mac_address() const {
    return mac_address;
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