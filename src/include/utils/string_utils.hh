#ifndef STRING_HH
#define STRING_HH

#include <string>
#include <vector>

namespace string_utils {
std::string &LTrim(std::string &str);
std::string &RTrim(std::string &str);
std::string &Trim(std::string &str);
std::vector<std::string> Split(std::string str, const std::string &delimiter);
}  // namespace string_utils

#endif