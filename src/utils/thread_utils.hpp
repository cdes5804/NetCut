#ifndef _SYNC_HPP
#define _SYNC_HPP

#include <atomic>
#include <map>
#include <string>
#include <thread>
#include <mutex>

namespace Thread {
    extern std::map<std::string, std::atomic<bool>> spoofing_signal;
    extern std::map<std::string, std::thread> spoofing_thread;
    extern std::mutex listening_mtx;
    extern std::atomic<bool> listening_signal;
    extern std::thread listening_thread;
    bool stop_spoofing_thread(const std::string &target_ip);
    bool stop_listening_thread();
    void stop_all_threads();
}

#endif