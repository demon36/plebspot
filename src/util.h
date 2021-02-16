#ifndef UTIL_H

#include <map>
#include <string>

namespace util{

std::string get_file_contents(const std::string& filename);
std::pair<std::string, std::string> split(const std::string& s, const std::string& token);
std::string trim(const std::string &s);
std::map<std::string, std::string> parse_pairs(const std::string& kv_pairs);
void replace_all(std::string& str, const std::string& from, const std::string& to);
std::string get_current_time();
std::string to_absolute_url(const std::string& host, const std::string& path, bool https = true);
::tm parse_date_rfc822(const std::string& str);
std::string format_date_w3c(const ::tm& time);

}

#define UTIL_H
#endif