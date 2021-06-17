#ifndef _CONTROL_HPP
#define _CONTROL_HPP

#include "models/host.hpp"
#include "models/arp.hpp"
#include "models/scanner.hpp"

#include <set>
#include <map>
#include <vector>
#include <chrono>
#include <string>

enum class ACTION_STATUS { TARGET_NOT_FOUND, CUT_SUCCESS, RECOVER_SUCCESS };

typedef std::chrono::high_resolution_clock Clock;
class Controller {
  private:
    NetworkScanner scanner;
    std::set<Host> hosts;
    std::map<Host, ARP> arp;
    std::map<Host, std::string> fake_mac_address;
    const uint32_t attack_interval_ms;
    const uint32_t scan_interval_ms;
    Clock::time_point last_scan_time;
    void attack(const Host &target);
    void recover(const Host &target);
    std::string get_fake_mac_address() const;

  public:
    Controller(const uint32_t attack_interval_ms, const uint32_t scan_interval_ms);
    void scan_targets();
    std::vector<Host> get_targets() const;
    Host get_host(const std::string &target_ip) const;
    void recover_all_hosts();
    ACTION_STATUS action(const std::string &target_ip);
};

#endif