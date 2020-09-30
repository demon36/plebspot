#include "render.h"

#include "fs.h"
#include "md.h"
#include "config.h"

#include <sstream>
#include <fmt/core.h>
#include <mustache.hpp>

using namespace std;
using namespace kainjow;

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

void fill_generic_date(mustache::data& page_data){
	page_data.set("blog_title", config::blog_title);
	page_data.set("blog_desc", config::blog_desc);

	mustache::data pages_list{mustache::data::type::list};
	mustache::data cat_pages_list{mustache::data::type::list};
	get_md_list_data(PAGES_DIR, pages_list, cat_pages_list);
	page_data.set("pages_list", pages_list);
	page_data.set("cat_pages_list", cat_pages_list);

	mustache::data posts_list{mustache::data::type::list};
	mustache::data cat_posts_list{mustache::data::type::list};
	get_md_list_data(POSTS_DIR, posts_list, cat_posts_list);
	page_data.set("posts_list", posts_list);
	page_data.set("cat_posts_list", cat_posts_list);
}

mustache::mustache get_template(){
	return mustache::mustache(fs::get_file_contents(fmt::format("./{}/{}", TEMPLATES_DIR, config::html_tmpl.c_str()).c_str()));
}

std::string render_home_page(){
	stringstream ss;
	mustache::mustache home_tmpl = get_template();
	mustache::data home_data;
	fill_generic_date(home_data);

	home_tmpl.render(home_data, ss);
	return ss.str();
}

string render_post(const string& path){
	stringstream ss;
	mustache::mustache post_tmpl = get_template();
	mustache::data post_data;
	fill_generic_date(post_data);

	string file_contents = fs::get_file_contents(path.c_str());
	post_data.set("post_title", path);
	post_data.set("content", md::render_md_to_html(file_contents));
	post_tmpl.render(post_data, ss);
	return ss.str();
}

}