#ifndef _MAC_HPP
#define _MAC_HPP

#include <string>

namespace Mac {
    constexpr int MAC_LENGTH = 6;
    void string_mac_to_byte(unsigned char *buffer, const std::string &mac_address);
    std::string byte_mac_to_string(unsigned char* buffer);
    std::string get_interface_mac_address(int sd, const std::string &interface_name);
    std::string get_random_mac_address();
}

#endif