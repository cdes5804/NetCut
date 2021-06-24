#include "models/controller.hpp"
#include "utils/mac_utils.hpp"
#include "utils/socket_utils.hpp"
#include "utils/string_utils.hpp"
#include "utils/thread_utils.hpp"

#include <algorithm>
#include <iostream>

typedef std::chrono::high_resolution_clock Clock;

Controller::Controller(const uint32_t attack_interval_ms, const uint32_t idle_threshold)
    : attacker(attack_interval_ms), scanner(idle_threshold) {
    scanner.listen();
}

void Controller::scan_targets() {
    std::vector<Host> active_hosts = scanner.scan_networks();
    for (const Host &active_host : active_hosts) {
        hosts.insert(active_host);
    }

    auto iter = hosts.begin();
    while (iter != hosts.end()) {
        if (!iter->is_cut() && std::find(active_hosts.begin(), active_hosts.end(), Host(iter->get_ip())) == active_hosts.end()) {
            iter = hosts.erase(iter);
        } else {
            iter = std::next(iter);
        }
    }
}

std::vector<Host> Controller::get_targets() const {
    return std::vector<Host>(hosts.begin(), hosts.end());
}

Host Controller::get_host(const std::string &target_ip) const {
    for (const Host &host : hosts) {
        if (host.get_ip() == target_ip) {
            return host;
        }
    }

    return Host();
}

ACTION_STATUS Controller::action(const std::string &target_ip) {
    auto iter = hosts.find(Host(target_ip));
    if (iter == hosts.end()) {
        return ACTION_STATUS::TARGET_NOT_FOUND;
    }

    if (iter->is_cut()) {
        recover(*iter);
        return ACTION_STATUS::RECOVER_SUCCESS;
    } else {
        attack(*iter);
        return ACTION_STATUS::CUT_SUCCESS;
    }
}

void Controller::attack(const Host &target) {
    Interface interface = scanner.get_interface_by_ip(target.get_ip());
    Host gateway = get_host(interface.get_gateway_ip());
    attacker.attack(interface, target, gateway);
    target.set_status(Status::CUT);
}

void Controller::recover(const Host &target) {
    Interface interface = scanner.get_interface_by_ip(target.get_ip());
    Host gateway = get_host(interface.get_gateway_ip());
    attacker.recover(interface, target, gateway);
    target.set_status(Status::NORMAL);
}

void Controller::recover_all_hosts() {
    for (const Host &host : hosts) {
        if (host.is_cut()) {
            recover(host);
        }
    }
}