#include "render.h"

#include "fs.h"
#include "md.h"
#include "config.h"

#include <sstream>
#include <fmt/core.h>
#include <mustache.hpp>

#define PAGES_DIR	"pages"
#define POSTS_DIR	"posts"

using namespace std;
using namespace kainjow;

extern const char* tmpl_home;
extern const char* tmpl_post;

namespace render{

void get_md_list_data(const string& dir, mustache::data& uncat_items, mustache::data& cat_items){
	map<string, vector<string>> pages = fs::get_md_files_map(dir);
	for(const auto& catPages : pages){
		if(catPages.first.empty()){
			for(const string& page_title : catPages.second){
				mustache::data page_data;
				page_data.set("title", page_title);
				page_data.set("url", page_title);
				uncat_items << page_data;
			}
		} else {
			mustache::data child_pages_list{mustache::data::type::list};
			mustache::data parent_data;
			parent_data.set("cat_title", catPages.first);
			for(const string& page_title : catPages.second){
				mustache::data page_data;
				page_data.set("title", page_title);
				page_data.set("url", string(catPages.first) + "/" + page_title);
				child_pages_list << page_data;
			}
			parent_data.set("children", child_pages_list);
			cat_items << parent_data;
		}
	}
}

std::string render_home_page(){
	stringstream ss;
	mustache::mustache home_tmpl(tmpl_home);
	mustache::data home_data;
	home_data.set("blog_title", config::blog_title);

	mustache::data pages_list{mustache::data::type::list};
	mustache::data cat_pages_list{mustache::data::type::list};
	get_md_list_data(PAGES_DIR, pages_list, cat_pages_list);
	home_data.set("pages_list", pages_list);
	home_data.set("cat_pages_list", cat_pages_list);

	mustache::data posts_list{mustache::data::type::list};
	mustache::data cat_posts_list{mustache::data::type::list};
	get_md_list_data(POSTS_DIR, posts_list, cat_posts_list);
	home_data.set("posts_list", posts_list);
	home_data.set("cat_posts_list", cat_posts_list);

	home_tmpl.render(home_data, ss);
	return ss.str();
}


string render_post(const string& path){
	// Jinja2CppLight::Template post_template(tmpl_post);
	// post_template.setValue("blog_title", config::blog_title);
	// post_template.setValue("post_title", path);
	// post_template.setValue("content", md::render_md_to_html(path));
	using namespace kainjow;
	stringstream ss;
	mustache::mustache post_tmpl(tmpl_post);
	mustache::data post_data;
	post_data.set("blog_title", config::blog_title);
	post_data.set("post_title", path);
	post_data.set("content", md::render_md_to_html(path));
	post_tmpl.render(post_data, ss);
	return ss.str();
}

}