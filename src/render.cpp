#include "render.h"

#include "fs.h"
#include "md.h"
#include "config.h"

#include <sstream>
#include <fmt/core.h>
#include <Jinja2CppLight.h>

#define PAGES_DIR	"pages"
#define POSTS_DIR	"posts"

using namespace std;

extern const char* tmpl_home;
extern const char* tmpl_post;

namespace render{

std::string render_home_page(){
	Jinja2CppLight::Template home_template(tmpl_home);
	home_template.setValue("blog_title", config::blog_title);
	home_template.setValue("pages_list", render_dir_as_md_list(PAGES_DIR));
	home_template.setValue("posts_list", render_dir_as_md_list(POSTS_DIR));
	return home_template.render();
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

string render_post(const string& path){
	Jinja2CppLight::Template post_template(tmpl_post);
	post_template.setValue("blog_title", config::blog_title);
	post_template.setValue("post_title", path);
	post_template.setValue("content", md::render_md_to_html(path));
	return post_template.render();
}

}