#ifndef MAC_HH
#define MAC_HH

#include <string>

namespace mac_utils {
constexpr int MAC_LENGTH = 6;
void StringToByte(unsigned char *buffer, const std::string &mac_address);
std::string ByteToString(unsigned char *buffer);
std::string GetInterfaceMacAddress(int socket_fd, const std::string &interface_name);
std::string GetRandomMacAddress();
}  // namespace mac_utils

#endif