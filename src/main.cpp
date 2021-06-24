#include "models/controller.hpp"
#include "routes/api.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept> 
#include <string>

// Check if the given flag is specified by the user.
bool check_cmd_flag_exists(char **begin, char **end, const std::string &flag) {
    return std::find(begin, end, flag) != end;
}

// Retrieve the value of the given flag from the command line.
std::string get_cmd_flag(char **begin, char **end, const std::string &flag) {
    char **itr = std::find(begin, end, flag);
    if (itr != end && ++itr != end) {
        return std::string(*itr);
    }

    return "";
}

// Given a flag, try to retrieve its value if it's specified by the user,
// otherwise return the default value.
uint32_t parse_cmd_flag(int argc, char *argv[], const std::string &flag, const uint32_t default_value) {
    if (check_cmd_flag_exists(argv, argv + argc, flag)) {
        std::string value = get_cmd_flag(argv, argv + argc, flag);
        if (value.empty()) {
            std::cerr << "Please specify a port after [" << flag << "].\n";
            exit(EXIT_FAILURE);
        } else {
            try {
                return std::stoul(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << '\n';
                exit(EXIT_FAILURE);
            }
        }
    } else {
        return default_value;
    }
}

// Parse the command line arguments, and return a std::map with either
// user-specified value or default value for each flag.
std::map<std::string, uint32_t> parse_arguments(int argc, char *argv[]) {
    constexpr uint32_t DEFAULT_PORT = 9090;
    constexpr uint32_t DEFAULT_ATTACK_INTERVAL_MS = 10000;
    constexpr uint32_t DEFAULT_IDLE_THRESHOLD = 3;
    constexpr uint32_t DEFAULT_SERVER_THREADS = 2;

    std::map<std::string, uint32_t> args;

    args["port"] = parse_cmd_flag(argc, argv, "--port", DEFAULT_PORT);
    args["attack_interval"] = parse_cmd_flag(argc, argv, "--attack_interval", DEFAULT_ATTACK_INTERVAL_MS);
    args["idle_threshold"] = parse_cmd_flag(argc, argv, "--idle_threshold", DEFAULT_IDLE_THRESHOLD);
    args["server_threads"] = parse_cmd_flag(argc, argv, "--server_threads", DEFAULT_SERVER_THREADS);

    return args;
}

int main(int argc, char *argv[]) {
    std::map<std::string, uint32_t> args = parse_arguments(argc, argv);
    
    start_server(args);

    return EXIT_SUCCESS;
}