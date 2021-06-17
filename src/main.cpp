#include "routes/api.hpp"

#include <algorithm>
#include <string>
#include <iostream>

bool check_cmd_option_exists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

std::string get_cmd_option(char ** begin, char ** end, const std::string &option) {
    if (!check_cmd_option_exists(begin, end, option)) {
        return "";
    }

    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return std::string(*itr);
    }

    return "";
}

int main(int argc, char *argv[]) {
    std::string port = get_cmd_option(argv, argv + argc, "--port");
    if (port.empty()) {
        std::cerr << "Please use [--port] to specify the port to run the application.\n";
        exit(EXIT_FAILURE);
    }

    start_server(std::stol(port));

    return EXIT_SUCCESS;
}