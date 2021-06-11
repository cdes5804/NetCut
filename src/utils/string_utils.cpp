#include "string_utils.hpp"

#include <algorithm>

std::string& String::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    return s;
}

std::string& String::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

std::string& String::trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::vector<std::string> String::split(std::string s, const std::string &delimiter) {
    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        tokens.emplace_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }

    tokens.emplace_back(s);
    return tokens;
}