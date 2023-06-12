#include "models/host.hh"

#include <arpa/inet.h>

Host::Host(const std::string &ip_address, const std::string &mac_address) {
  this->ip_address_ = ip_address;
  this->mac_address_ = mac_address;
  this->status_ = Status::NORMAL;
}

bool Host::operator<(const Host &other) const {
  uint32_t self_ip = inet_addr(ip_address_.c_str());
  uint32_t other_ip = inet_addr(other.ip_address_.c_str());

  return self_ip < other_ip;
}

std::string Host::GetIp() const { return ip_address_; }

std::string Host::GetMac() const { return mac_address_; }

bool Host::IsCut() const { return status_ == Status::CUT; }

void Host::SetStatus(Status status) const { this->status_ = status; }

void Host::SetMacAddress(const std::string &mac_address) const { this->mac_address_ = mac_address; }