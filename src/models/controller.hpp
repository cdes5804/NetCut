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

    // Launch ARP spoofing against the given target.
    void attack(const Host &target);

    // Stop attack and recover the target's Internet connection.
    void recover(const Host &target);

  public:
    Controller(const uint32_t attack_interval_ms, const uint32_t idle_threshold);

    // Call the Scanner to scan the network.
    void scan_targets();

    // Retrieve the hosts discovered by the Scanner.
    std::vector<Host> get_targets() const;

    // Given an IP address, retrieve the corresponding host or return an empty host indicating its non-existence.
    Host get_host(const std::string &target_ip) const;

    // Recover all hosts's Internet connection if they have benn attacked.
    // Called when the application is exiting.
    void recover_all_hosts();

    // Take action against the host with the given IP address.
    // Attack if the target is in NORMAL state, or recover if it is in CUT state.
    ACTION_STATUS action(const std::string &target_ip);
};

#endif