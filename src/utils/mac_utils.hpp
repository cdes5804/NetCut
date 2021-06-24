#ifndef _MAC_HPP
#define _MAC_HPP

#include <string>

namespace Mac {
    constexpr int MAC_LENGTH = 6;

    // Transform a string mac address, e.g. 1A:2B:3C... to bytes.
    void string_mac_to_byte(unsigned char *buffer, const std::string &mac_address);

    // Transform a byte sequence of a mac address to its string form, e.g. 1A:2B:3C....
    std::string byte_mac_to_string(unsigned char* buffer);

    // Given a interface, retrieve its mac address as a string.
    std::string get_interface_mac_address(int sd, const std::string &interface_name);

    // Randomly generate a mac address, with identifier being Cisco.
    std::string get_random_mac_address();
}

#endif