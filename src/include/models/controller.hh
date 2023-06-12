#ifndef CONTROL_HH
#define CONTROL_HH

#include "models/arp.hh"
#include "models/host.hh"
#include "models/scanner.hh"

#include <map>
#include <set>
#include <vector>

class Controller {
 private:
  NetworkScanner scanner_;
  std::set<Host> hosts_;
  std::map<Host, ARP> arp_;
  std::map<Host, std::string> fake_mac_address_;
  int attack_interval_ms_;
  void ScanTargets();
  void Attack(const Host &target);
  void Recover(const Host &target);
  void RecoverAllHosts();
  std::string GetFakeMacAddress() const;

 public:
  explicit Controller(int attack_interval_ms);
  void ShowTargets();
  std::vector<size_t> GetTargets();
  void Action(size_t index);
};

#endif