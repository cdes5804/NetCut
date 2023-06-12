#ifndef SYNC_HH
#define SYNC_HH

#include <atomic>
#include <map>
#include <string>
#include <thread>

namespace thread_utils {
extern std::map<std::string, std::map<std::string, std::atomic<bool>>>
    spoof_signal;  // attack_signal[target_ip][spoof_src_ip] signal to start attack
extern std::map<std::string, std::map<std::string, std::thread>> spoof_thread;
extern std::map<std::string, std::atomic<bool>> listen_signal;
extern std::map<std::string, std::thread> listening_thread;
bool StopThread(const std::string &target_ip, const std::string &spoof_src_ip);
bool StopThread(const std::string &ip_address);
void StopAllThreads();
}  // namespace thread_utils

#endif