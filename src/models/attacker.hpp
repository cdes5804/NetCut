#ifndef _ATTACKER_HPP
#define _ATTACKER_HPP

#include "models/host.hpp"
#include "models/interface.hpp"

#include <map>
#include <string>

class Attacker {
  private:
    const uint32_t BUF_SIZE = 512;
    const uint32_t attack_interval_ms;
    std::map<std::string, std::string> fake_mac_address;

    // Internal method for launching ARP spoofing attack againts the given target.
    void _attack(const Interface interface, const Host target, const Host gateway);

  public:
    Attacker(const uint32_t attack_interval_ms);

    // Launching ARP spoofing attack against the given target.
    void attack(const Interface &interface, const Host &target, const Host &gateway);

    // Stop ARP spoofing and recover the Internet connection of the given target.
    void recover(const Interface &interface, const Host &target, const Host &gateway);
};

#endif