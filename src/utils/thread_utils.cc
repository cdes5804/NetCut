#include "utils/thread_utils.hh"

#include <unistd.h>

std::map<std::string, std::map<std::string, std::atomic<bool>>> thread_utils::spoof_signal;
std::map<std::string, std::map<std::string, std::thread>> thread_utils::spoof_thread;
std::map<std::string, std::atomic<bool>> thread_utils::listen_signal;
std::map<std::string, std::thread> thread_utils::listening_thread;

bool thread_utils::StopThread(const std::string &target_ip, const std::string &spoof_src_ip) {
  if (spoof_thread[target_ip][spoof_src_ip].joinable()) {
    spoof_signal[target_ip][spoof_src_ip].store(false, std::memory_order_relaxed);
    spoof_thread[target_ip][spoof_src_ip].join();
    return true;
  }
  return false;
}

bool thread_utils::StopThread(const std::string &ip_address) {
  if (listening_thread[ip_address].joinable()) {
    listen_signal[ip_address].store(false, std::memory_order_relaxed);
    listening_thread[ip_address].join();
    return true;
  }
  return false;
}

void thread_utils::StopAllThreads() {
  for (auto &[ip, thd] : listening_thread) {
    StopThread(ip);
  }

  for (auto &[target_ip, mp] : spoof_thread) {
    for (auto &[spoof_src_ip, thd] : mp) {
      StopThread(target_ip, spoof_src_ip);
    }
  }
}