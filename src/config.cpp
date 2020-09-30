#include "config.h"

#include "fs.h"
#include "util.h"

#include <fstream>

using namespace std;

namespace config{

string blog_title = "my blog";
string blog_desc = "random stuff";
string html_tmpl = "plain.html";
int http_port = 1993;

void load(){
	string file_contents = fs::get_file_contents(CONFIG_FILE);
	map<string, string> config_items = util::parse_pairs(file_contents);

	for( const auto& config_item : config_items) {
		if (config_item.first == "blog_title"){
			blog_title = config_item.second;
		} else if (config_item.first == "http_port"){
			http_port = atoi(config_item.second.c_str());
		} else if (config_item.first == "blog_desc"){
			blog_desc = config_item.second;
		} else if (config_item.first == "html_tmpl"){
			html_tmpl = config_item.second;
		}
	}
}

}