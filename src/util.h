#ifndef UTIL_H

#include <map>
#include <string>

namespace util{

std::string get_file_contents(const std::string& filename);
std::pair<std::string, std::string> split(const std::string& s, const std::string& token);
std::string trim(const std::string &s);
std::map<std::string, std::string> parse_pairs(const std::string& kv_pairs);
std::string html_encode(const std::string& value);
std::string get_current_time();

}

#define UTIL_H
#endif