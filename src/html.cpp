#include "html.h"

#include "fs.h"

#include <sstream>
#include <fmt/core.h>

#define PAGES_DIR	"pages"
#define POSTS_DIR	"posts"

using namespace std;

namespace html{

std::string render_home_page(){
	stringstream ss;
	ss << render_dir_as_md_list(PAGES_DIR) << render_dir_as_md_list(POSTS_DIR);
	return ss.str();
}

string render_dir_as_md_list(const string& dir){
	stringstream ss;
	ss << "<h4>" << dir << "</h4><ul>";
	map<string, vector<string>> pages = fs::get_md_files_map(dir);
	for(const auto& catPages : pages){
		if(catPages.first.empty()){
			for(const string& page : catPages.second){
				ss << fmt::format("<li><a href='/{0}/{1}'>{1}</a></li>", dir, page);
			}
		} else {
			ss << "<li>" << catPages.first << "<ul>";
			for(const string& page : catPages.second){
				ss << fmt::format("<li><a href='/{0}/{1}/{2}'>{2}</a></li>", dir, catPages.first, page);
			}
			ss << "</li></ul>";
		}
	}
	ss << "</ul>" << endl;
	return ss.str();
}


}