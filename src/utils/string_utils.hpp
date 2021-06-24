#ifndef _STRING_HPP
#define _STRING_HPP

#include <string>
#include <vector>

namespace String {
    // Remove whitespaces in the beginning of a string.
    std::string& ltrim(std::string &s);

    // Remove whitespaces in the end of a string.
    std::string& rtrim(std::string &s);

    // Remove whitespaces in both the beginning and the end of a string.
    std::string& trim(std::string &s);

    // Split a string by a delimiter, which can be either a charater or a string,
    // and return each token in a std::vector.
    std::vector<std::string> split(std::string s, const std::string &delimiter);
}

#endif