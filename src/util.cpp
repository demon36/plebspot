#include "util.h"

#include <ctime>
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#define stat _stat
#else
#include <unistd.h>
#endif

using namespace std;

namespace util{

outcome<string> get_file_contents(const string& filename){
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if(in){
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return outcome<string>(contents);
	}
	return outcome<string>(errors::file_missing);
}

pair<string, string> split(const string& s, const string& token){
	size_t token_idx = s.find(token);
	if(token_idx != s.npos){
		return make_pair(trim(s.substr(0, token_idx)), trim(s.substr(token_idx+1)));
	} else {
		return make_pair("", "");
	}
}

string trim(const string &s){
	string::const_iterator it = s.begin();
	while (it != s.end() && isspace(*it))
		it++;

	string::const_reverse_iterator rit = s.rbegin();
	while (rit.base() != it && isspace(*rit))
		rit++;

	return string(it, rit.base());
}

map<string, string> parse_pairs(const string& kv_pairs){
	map<string, string> output;
	size_t line_start_idx = 0;
	size_t line_end_idx = 0;//kv_pairs.npos;
	size_t colon_idx = 0;//kv_pairs.npos;
	while( line_end_idx+2 < kv_pairs.size() ) {
		line_start_idx = line_end_idx == 0 ? line_end_idx : line_end_idx+1;
		colon_idx = kv_pairs.find(':', line_start_idx);
		line_end_idx = kv_pairs.find('\n', colon_idx);
		if(line_end_idx == kv_pairs.npos){
			line_end_idx = kv_pairs.size();
		}
		string key = kv_pairs.substr(line_start_idx, colon_idx-line_start_idx);
		string value = kv_pairs.substr(colon_idx+1, line_end_idx-colon_idx);
		output[trim(key)] = trim(value);
	}
	return output;
}

void replace_all(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string get_current_time(){
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %z", timeinfo);
	return string(buffer);
}

string to_absolute_url(const string& host, const string& path, bool https){
	if(host.empty()){
		return path;//not much we can do
	}else{
		return fmt::format(https ? "https://{}{}" : "http://{}{}", host, path);
	}
}

::tm get_last_write_time(const std::string& path){
	struct stat result;
	if(stat(path.c_str(), &result) == 0){
		return *std::localtime(&result.st_mtime);
	} else {
		return ::tm{};
	}
}

::tm parse_date_rfc822(const string& str){
    ::tm tm = {};
    stringstream ss(str);
    ss >> get_time(&tm, "%d %b %Y %H:%M:%S %z");
    if(ss.fail()){//try to parse timeless string
        ss >> get_time(&tm, "%d %b %Y");
		if(ss.fail()){
			time_t rawtime;
			time(&rawtime);
			return *localtime(&rawtime);
		}
    }
    return tm;
}

string format_date_w3c(const ::tm& time){
    string buf(11, (char)0);
    strftime((char*)buf.c_str(), 11, "%Y-%m-%d", &time);
    return buf;
}


}