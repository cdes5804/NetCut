#include "models/host.hpp"

#include <arpa/inet.h>

Host::Host() : status(Status::NOT_EXIST) {}
Host::Host(const std::string &ip_address)
        : ip_address(ip_address), mac_address(""), status(Status::NOT_EXIST) {}

Host::Host(const std::string &ip_address, const std::string &mac_address)
        : ip_address(ip_address), mac_address(mac_address), status(Status::NORMAL) { }

bool Host::operator<(const Host &other) const {
    uint32_t self_ip = inet_addr(ip_address.c_str());
    uint32_t other_ip = inet_addr(other.ip_address.c_str());

    return self_ip < other_ip;
}

bool Host::operator==(const Host &other) const {
    return ip_address == other.ip_address;
}

std::string Host::get_ip() const {
    return ip_address;
}

std::string Host::get_mac() const {
    return mac_address;
}

Status Host::get_status() const {
    return status;
}

bool Host::is_cut() const {
    return status == Status::CUT;
}

void Host::set_status(Status status) const {
    this->status = status;
}

void Host::set_mac_address(const std::string &mac_address) const {
    this->mac_address = mac_address;
}