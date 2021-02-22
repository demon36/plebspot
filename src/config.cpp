#include "config.h"

#include "util.h"

#include <fstream>

using namespace std;
using namespace util;

namespace config{

string blog_title = "my blog";
string blog_desc = "random stuff";
string html_tmpl = "plain.html";
string blog_keywords = "fun, diy";
int http_port = 1993;
bool comments_enabled = true;

util::error load(){
	outcome<string> file_contents = util::get_file_contents(CONFIG_FILE);
	if(!file_contents.is_success()){
		return file_contents.get_error();
	}
	
	map<string, string> config_items = util::parse_pairs(file_contents.get_result());

	//todo: enhance this
	for( const auto& config_item : config_items) {
		string value = util::trim(config_item.second);
		if (config_item.first == "blog_title"){
			blog_title = value;
		} else if (config_item.first == "http_port"){
			http_port = atoi(value.c_str());
		} else if (config_item.first == "blog_desc"){
			blog_desc = value;
		} else if (config_item.first == "blog_keywords"){
			blog_keywords = value;
		} else if (config_item.first == "html_tmpl"){
			html_tmpl = value;
		} else if (config_item.first == "comments_enabled"){
			comments_enabled = value == "true";
		}
	}
	
	return errors::success;
}

}