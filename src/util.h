#ifndef UTIL_H

#include <map>
#include <string>

namespace util{

std::string trim(const std::string &s);
std::map<std::string, std::string> parse_pairs(const std::string& kv_pairs);

}

#define UTIL_H
#endif