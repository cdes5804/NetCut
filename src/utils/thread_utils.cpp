#include "thread_utils.hpp"

#include <unistd.h>

std::map<std::string, std::map<std::string, std::atomic<bool>>> Thread::spoof_signal;
std::map<std::string, std::map<std::string, std::thread>> Thread::spoof_thread;
std::map<std::string, std::atomic<bool>> Thread::listen_signal;
std::map<std::string, std::thread> Thread::listening_thread;

bool Thread::stop_thread(const std::string &target_ip, const std::string &spoof_src_ip) {
    if (spoof_thread[target_ip][spoof_src_ip].joinable()) {
        spoof_signal[target_ip][spoof_src_ip].store(false, std::memory_order_relaxed);
        spoof_thread[target_ip][spoof_src_ip].join();
        return true;
    }
    return false;
}

bool Thread::stop_thread(const std::string &ip) {
    if (listening_thread[ip].joinable()) {
        listen_signal[ip].store(false, std::memory_order_relaxed);
        listening_thread[ip].join();
        return true;
    }
    return false;
}

void Thread::stop_all_threads() {
    for (auto &[ip, thd] : listening_thread) {
        stop_thread(ip);
    }

    for (auto &[target_ip, mp] : spoof_thread) {
        for (auto &[spoof_src_ip, thd] : mp) {
            stop_thread(target_ip, spoof_src_ip);
        }
    }
}