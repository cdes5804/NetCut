#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#include <map>
#include <string>

namespace Socket {
    const unsigned int DEFAULT_RECEIVE_TIMEOUT_MICROSEC = 1000;

    // Opens a available socket file descriptor.
    int open_socket();

    // Set the receive timeout in microsecond for the given socket,
    // so it will not block when there is nothing to receive.
    void set_socket_recv_timeout(const int sd, const unsigned int microsecond);

    // Bind the socket to a specific interface.
    void bind_socket(const int sd, const std::string &interface_name);

    // Get the interface index.
    int get_interface_index(const int sd, const const std::string &interface_name);

    // Close the given socket.
    void close_socket(const int sd);
}

#endif