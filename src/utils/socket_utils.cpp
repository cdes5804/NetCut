#include "socket_utils.hpp"

#include <iostream>

#include <linux/if_arp.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

std::map<std::string, int> Socket::sockets;

int Socket::open_socket(const std::string &ip) {
    if ((sockets[ip] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) <= 0) {
        std::cerr << "open_socket: Failed to open socket.\n";
        exit(EXIT_FAILURE);
    }

    struct timeval tout;
    tout.tv_sec = 0;
    tout.tv_usec = 1000;
    setsockopt(sockets[ip], SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout));
    return sockets[ip];
}

void Socket::bind_socket(int sd, const std::string &interface_name) {
    struct ifreq ifr;

    strcpy(ifr.ifr_name, interface_name.c_str());

    if (ioctl(sd, SIOCGIFINDEX, &ifr) == -1) {
        std::cerr << "bind_socket: Failed to get interface index.\n";
        exit(EXIT_FAILURE);
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;

    if (bind(sd, (struct sockaddr *)&sll, sizeof(struct sockaddr_ll)) < 0) {
        std::cerr << "bind_socket: Failed to bind socket.\n";
        exit(EXIT_FAILURE);
    }
}

int Socket::get_interface_index(int sd, const std::string &interface_name) {
    struct ifreq ifr;
    strcpy(ifr.ifr_name, interface_name.c_str());
    if (ioctl(sd, SIOCGIFINDEX, &ifr) == -1) {
        std::cerr << "get_interface_index: Failed to get interface index.\n";
        exit(EXIT_FAILURE);
    }

    return ifr.ifr_ifindex;
}

void Socket::close_sockets() {
    for (auto &[ip, sd] : sockets) {
        close(sd);
    }
}