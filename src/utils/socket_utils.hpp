#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#include <map>
#include <string>

namespace Socket {
    const unsigned int DEFAULT_RECEIVE_TIMEOUT_MICROSEC = 1000;
    extern std::map<std::string, int> sockets;

    // Opens a available socket file descriptor.
    int open_socket();

    // Given an IP address, retrieve the socket associated with it.
    // Create a new one if it hasn't already existed.
    int get_socket(const std::string &ip);

    // Set the receive timeout in microsecond for the given socket,
    // so it will not block when there is nothing to receive.
    void set_socket_recv_timeout(const int sd, const unsigned int microsecond);

    // Bind the socket to a specific interface.
    void bind_socket(const int sd, const std::string &interface_name);

    // Get the interface index kept by the OS.
    int get_interface_index(const int sd, const std::string &interface_name);

    // Close all sockets opened previously.
    void close_all_sockets();
}

#endif