#ifndef UTIL_H

#include <map>
#include <string>

namespace util{

std::pair<std::string, std::string> split(const std::string& s, const std::string& token);
std::string trim(const std::string &s);
std::map<std::string, std::string> parse_pairs(const std::string& kv_pairs);

}

#define UTIL_H
#endif