#include "utils/socket_utils.hh"

#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <bsd/string.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

std::map<std::string, int> socket_utils::sockets;

int socket_utils::OpenSocket(const std::string &ip_address) {
  sockets[ip_address] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
  if (sockets[ip_address] <= 0) {
    std::cerr << "open_socket: Failed to open socket.\n";
    exit(EXIT_FAILURE);
  }

  struct timeval tout;
  tout.tv_sec = 0;
  tout.tv_usec = 1000;
  setsockopt(sockets[ip_address], SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout));
  return sockets[ip_address];
}

void socket_utils::BindSocket(int socket_fd, const std::string &interface_name) {
  struct ifreq ifr;

  strlcpy(ifr.ifr_name, interface_name.c_str(), sizeof(ifr.ifr_name));

  if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) == -1) {
    std::cerr << "bind_socket: Failed to get interface index.\n";
    exit(EXIT_FAILURE);
  }

  struct sockaddr_ll sll;
  memset(&sll, 0, sizeof(struct sockaddr_ll));
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = ifr.ifr_ifindex;

  if (bind(socket_fd, reinterpret_cast<struct sockaddr *>(&sll), sizeof(struct sockaddr_ll)) < 0) {
    std::cerr << "bind_socket: Failed to bind socket.\n";
    exit(EXIT_FAILURE);
  }
}

int socket_utils::GetInterfaceIndex(int socket_fd, const std::string &interface_name) {
  struct ifreq ifr;
  strlcpy(ifr.ifr_name, interface_name.c_str(), sizeof(ifr.ifr_name));
  if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) == -1) {
    std::cerr << "get_interface_index: Failed to get interface index.\n";
    exit(EXIT_FAILURE);
  }

  return ifr.ifr_ifindex;
}

void socket_utils::CloseSockets() {
  for (auto &[ip, socket_fd] : sockets) {
    close(socket_fd);
  }
}