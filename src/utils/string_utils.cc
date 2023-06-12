#include "utils/string_utils.hh"

#include <algorithm>

std::string &string_utils::LTrim(std::string &str) {
  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(), [](unsigned char chr) { return std::isspace(chr) == 0; }));

  return str;
}

std::string &string_utils::RTrim(std::string &str) {
  str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char chr) { return std::isspace(chr) == 0; }).base(),
            str.end());

  return str;
}

std::string &string_utils::Trim(std::string &str) { return LTrim(RTrim(str)); }

std::vector<std::string> string_utils::Split(std::string str, const std::string &delimiter) {
  size_t pos = 0;
  std::vector<std::string> tokens;
  while ((pos = str.find(delimiter)) != std::string::npos) {
    tokens.emplace_back(str.substr(0, pos));
    str.erase(0, pos + delimiter.length());
  }

  tokens.emplace_back(str);
  return tokens;
}