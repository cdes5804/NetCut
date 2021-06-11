#include "mac_utils.hpp"

#include <iostream>
#include <random>

#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>

void Mac::string_mac_to_byte(unsigned char *buffer, const std::string &mac_address) {
    uint32_t int_buffer[6];
    sscanf(mac_address.c_str(), "%02X:%02X:%02X:%02X:%02X:%02X",
            &int_buffer[0], &int_buffer[1], &int_buffer[2],
            &int_buffer[3], &int_buffer[4], &int_buffer[5]);
    
    for (int i = 0; i < MAC_LENGTH; i++) {
        buffer[i] = int_buffer[i];
    }
}

void Mac::get_interface_mac_address(unsigned char *buffer, int sd, const std::string &interface_name) {
    struct ifreq ifr;

    strcpy(ifr.ifr_name, interface_name.c_str());

    if (ioctl(sd, SIOCGIFHWADDR, &ifr) == -1) {
        std::cerr << "get_mac_address: Failed to get interface mac address.\n";
        exit(EXIT_FAILURE);
    }

    memcpy(buffer, ifr.ifr_hwaddr.sa_data, MAC_LENGTH);
}

void Mac::get_random_mac_address(unsigned char *buffer) {
    // impersonate CISCO interface
    buffer[0] = 204;
    buffer[1] = 70;
    buffer[2] = 214;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> uchar_distribution(0, 255);
    buffer[3] = uchar_distribution(rng);
    buffer[4] = uchar_distribution(rng);
    buffer[5] = uchar_distribution(rng);
}