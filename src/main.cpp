#include "routes/api.hpp"
#include "models/controller.hpp"

#include <algorithm>
#include <string>
#include <stdexcept> 
#include <iostream>
#include <map>

bool check_cmd_option_exists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

std::string get_cmd_option(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return std::string(*itr);
    }

    return "";
}

uint32_t parse_cmd_option(int argc, char *argv[], const std::string &option, const uint32_t default_value) {
    if (check_cmd_option_exists(argv, argv + argc, option)) {
        std::string value = get_cmd_option(argv, argv + argc, option);
        if (value.empty()) {
            std::cerr << "Please specify a port after [" << option << "].\n";
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

std::map<std::string, uint32_t> parse_arguments(int argc, char *argv[]) {
    constexpr uint32_t DEFAULT_PORT = 9090;
    constexpr uint32_t DEFAULT_ATTACK_INTERVAL_MS = 10000;
    constexpr uint32_t DEFAULT_SCAN_INTERVAL_MS = 5000;

    std::map<std::string, uint32_t> args;

    args["port"] = parse_cmd_option(argc, argv, "--port", DEFAULT_PORT);
    args["attack_interval"] = parse_cmd_option(argc, argv, "--attack_interval", DEFAULT_ATTACK_INTERVAL_MS);
    args["scan_interval"] = parse_cmd_option(argc, argv, "--scan_interval", DEFAULT_SCAN_INTERVAL_MS);

    return args;
}

int main(int argc, char *argv[]) {
    std::map<std::string, uint32_t> args = parse_arguments(argc, argv);
    Controller controller(args["attack_interval"], args["scan_interval"]);

    start_server(args["port"], controller);

    return EXIT_SUCCESS;
}