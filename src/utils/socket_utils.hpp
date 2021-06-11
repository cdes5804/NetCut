#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#include <map>
#include <string>

namespace Socket {
    extern std::map<std::string, int> sockets;
    int open_socket(const std::string &ip); // opens a socket associated with the ip
    void bind_socket(int sd, const std::string &interface_name); // binds socket to the provided interface
    int get_interface_index(int sd, const std::string &interface_name);
    void close_sockets();
}

#endif