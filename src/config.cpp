#include "config.h"

#include <fstream>
using namespace std;

namespace config{

string blog_title = "my blog";
string blog_desc = "random stuff";
string favicon_path = "./" STATIC_DIR "/favicon.ico";
string html_tmpl = "plain.html";
int http_port = 1993;

string trim(const std::string &s){
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

bool load(){
	ifstream cfg_stream(CONFIG_FILE);
	if(!cfg_stream.good()){
		return false;
	}

	for( std::string line; getline( cfg_stream, line ); ) {
		size_t colon_idx = line.find(":");
		if(colon_idx == line.npos){
			return false;
		} else {
			string key = trim(line.substr(0, colon_idx));
			string value = trim(line.substr(colon_idx+1));
			if (key == "blog_title"){
				blog_title = value;
			} else if (key == "favicon_path"){
				favicon_path = value;
			} else if (key == "http_port"){
				http_port = atoi(value.c_str());
			} else if (key == "blog_desc"){
				blog_desc = value;
			} else if (key == "html_tmpl"){
				html_tmpl = value;
			}
		}
	}
	return true;
}

}