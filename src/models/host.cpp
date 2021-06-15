#include "models/host.hpp"

#include <arpa/inet.h>

Host::Host(const string &ip_address, const string &mac_address) {
    this->ip_address = ip_address;
    this->mac_address = mac_address;
    status = Status::NORMAL;
}

bool Host::operator<(const Host &other) const {
    uint32_t self_ip = inet_addr(ip_address.c_str());
    uint32_t other_ip = inet_addr(other.ip_address.c_str());

    return self_ip < other_ip;
}

string Host::get_ip() const {
    return ip_address;
}

string Host::get_mac() const {
    return mac_address;
}

bool Host::is_cut() const {
    return status == Status::CUT;
}

void Host::set_status(Status status) const {
    this->status = status;
}

void Host::set_mac_address(const string &mac_address) const {
    this->mac_address = mac_address;
}