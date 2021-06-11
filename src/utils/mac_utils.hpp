#ifndef _MAC_HPP
#define _MAC_HPP

#include <string>

namespace Mac {
    constexpr int MAC_LENGTH = 6;
    void string_mac_to_byte(unsigned char *buffer, const std::string &mac_address);
    void get_interface_mac_address(unsigned char *buffer, int sd, const std::string &interface_name);
    void get_random_mac_address(unsigned char *buffer);
}

#endif