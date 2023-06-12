#include "utils/mac_utils.hh"

#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include <bsd/string.h>
#include <net/if.h>
#include <sys/ioctl.h>

void mac_utils::StringToByte(unsigned char *buffer, const std::string &mac_address) {
  uint32_t int_buffer[MAC_LENGTH];
  sscanf(mac_address.c_str(), "%02X:%02X:%02X:%02X:%02X:%02X", &int_buffer[0], &int_buffer[1], &int_buffer[2],
         &int_buffer[3], &int_buffer[4], &int_buffer[5]);

  for (int i = 0; i < MAC_LENGTH; i++) {
    buffer[i] = int_buffer[i];
  }
}

std::string mac_utils::ByteToString(unsigned char *buffer) {
  std::stringstream str_stream;
  str_stream << std::hex;

  for (int i = 0; i < MAC_LENGTH; i++) {
    str_stream << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
    if (i != MAC_LENGTH - 1) {
      str_stream << ":";
    }
  }

  return str_stream.str();
}

std::string mac_utils::GetInterfaceMacAddress(int socket_fd, const std::string &interface_name) {
  struct ifreq ifr;
  unsigned char buffer[MAC_LENGTH];

  strlcpy(ifr.ifr_name, interface_name.c_str(), sizeof(ifr.ifr_name));

  if (ioctl(socket_fd, SIOCGIFHWADDR, &ifr) == -1) {
    std::cerr << "get_mac_address: Failed to get interface mac address.\n";
    exit(EXIT_FAILURE);
  }

  memcpy(buffer, ifr.ifr_hwaddr.sa_data, MAC_LENGTH);
  return ByteToString(buffer);
}

std::string mac_utils::GetRandomMacAddress() {
  unsigned char buffer[MAC_LENGTH];

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

  return ByteToString(buffer);
}