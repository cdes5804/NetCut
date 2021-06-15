#ifndef _CONTROL_HPP
#define _CONTROL_HPP

#include "models/host.hpp"
#include "models/arp.hpp"
#include "models/scanner.hpp"

#include <set>
#include <map>
#include <vector>

class Controller {
  private:
    NetworkScanner scanner;
    std::set<Host> hosts;
    std::map<Host, ARP> arp;
    int attack_interval_ms;
    void scan_targets();
    void attack(const Host &target);
    void recover(const Host &target);
    void recover_all_hosts();

  public:
    Controller(const int attack_interval_ms);
    void show_targets();
    std::vector<size_t> get_targets();
    void action(size_t index);
};

#endif