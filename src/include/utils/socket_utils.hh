#ifndef SOCKET_HH
#define SOCKET_HH

#include <map>
#include <string>

namespace socket_utils {
extern std::map<std::string, int> sockets;
int OpenSocket(const std::string &ip_address);                              // opens a socket associated with the ip
void BindSocket(int socket_fd, const std::string &interface_name);  // binds socket to the provided interface
int GetInterfaceIndex(int socket_fd, const std::string &interface_name);
void CloseSockets();
}  // namespace socket_utils

#endif