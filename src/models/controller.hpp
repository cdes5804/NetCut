#ifndef _CONTROL_HPP
#define _CONTROL_HPP

#include "models/attacker.hpp"
#include "models/host.hpp"
#include "models/scanner.hpp"

#include <set>
#include <string>
#include <vector>

enum class ACTION_STATUS { TARGET_NOT_FOUND, CUT_SUCCESS, RECOVER_SUCCESS };

typedef std::chrono::high_resolution_clock Clock;
class Controller {
  private:
    Attacker attacker;
    NetworkScanner scanner;
    std::set<Host> hosts;

    void attack(const Host &target);
    void recover(const Host &target);

  public:
    Controller(const uint32_t attack_interval_ms, const uint32_t idle_threshold);
    void scan_targets();
    std::vector<Host> get_targets() const;
    Host get_host(const std::string &target_ip) const;
    void recover_all_hosts();
    ACTION_STATUS action(const std::string &target_ip);
};

#endif