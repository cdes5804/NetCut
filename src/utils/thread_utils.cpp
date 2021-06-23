#include "thread_utils.hpp"

#include <unistd.h>

std::map<std::string, std::atomic<bool>> Thread::spoofing_signal;
std::map<std::string, std::thread> Thread::spoofing_thread;
std::mutex Thread::listening_mtx;
std::atomic<bool> Thread::listening_signal;
std::thread Thread::listening_thread;

bool Thread::stop_spoofing_thread(const std::string &target_ip) {
    if (spoofing_thread[target_ip].joinable()) {
        spoofing_signal[target_ip].store(false, std::memory_order_relaxed);
        spoofing_thread[target_ip].join();
        return true;
    }
    return false;
}

bool Thread::stop_listening_thread() {
    if (listening_thread.joinable()) {
        listening_signal.store(false, std::memory_order_relaxed);
        listening_thread.join();
        return true;
    }
    return false;
}

void Thread::stop_all_threads() {
    stop_listening_thread();
    for (auto iter = spoofing_thread.begin(); iter != spoofing_thread.end(); ++iter) {
        stop_spoofing_thread(iter->first);
    }
}