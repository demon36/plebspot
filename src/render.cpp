#include "render.h"

#include "fs.h"
#include "md.h"
#include "config.h"
#include "captcha.h"

#include <fmt/core.h>
#include <mustache.hpp>

using namespace std;
using namespace kainjow;

namespace render{

void get_md_list_data(const std::map<string, vector<md::md_doc>>& docs_map, mustache::data& uncat_items, mustache::data& cat_items){
	for(const auto& catDocs : docs_map){
		if(catDocs.first.empty()){
			for(const md::md_doc& doc : catDocs.second){
				mustache::data page_data;
				page_data.set("title", doc.title);
				page_data.set("url", doc.url);
				page_data.set("date", doc.date);
				uncat_items << page_data;
			}
		} else {
			mustache::data child_pages_list{mustache::data::type::list};
			mustache::data parent_data;
			parent_data.set("cat_title", catDocs.first);
			for(const md::md_doc& doc : catDocs.second){
				mustache::data page_data;
				page_data.set("title", doc.title);
				page_data.set("url", doc.url);
				page_data.set("date", doc.date);
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
	std::map<string, vector<md::md_doc>> pages = md::get_md_docs(md::doc_type::page);
	get_md_list_data(pages, pages_list, cat_pages_list);
	page_data.set("pages_list", pages_list);
	page_data.set("cat_pages_list", cat_pages_list);

	mustache::data posts_list{mustache::data::type::list};
	mustache::data cat_posts_list{mustache::data::type::list};
	std::map<string, vector<md::md_doc>> posts = md::get_md_docs(md::doc_type::post);
	get_md_list_data(posts, posts_list, cat_posts_list);
	page_data.set("posts_list", posts_list);
	page_data.set("cat_posts_list", cat_posts_list);
}

void fill_comments(mustache::data& page_data, const vector<fs::comment>& comments){
	mustache::data comments_list{mustache::data::type::list};
	for(const fs::comment& c : comments){
		mustache::data comment_data;
		comment_data.set("author", c.author);
		comment_data.set("date", c.date);
		comment_data.set("message", c.message);
		comments_list << comment_data;
	}
	page_data.set("comments_list", comments_list);
}

mustache::mustache get_template(){
	return mustache::mustache(fs::get_file_contents(fmt::format("./{}/{}", TEMPLATES_DIR, config::html_tmpl.c_str()).c_str()));
}

std::string render_home_page(){
	stringstream ss;
	mustache::mustache home_tmpl = get_template();
	mustache::data home_data;
	fill_generic_date(home_data);
	home_data.set("keywords", config::blog_keywords);
	home_data.set("page_desc", config::blog_desc);
	home_tmpl.render(home_data, ss);
	return ss.str();
}

string render_post(const string& path, const string& alert_msg){
	stringstream ss;
	mustache::mustache post_tmpl = get_template();
	mustache::data post_data;
	fill_generic_date(post_data);

	md::md_doc doc = md::make_md_doc(path);
	string file_contents = fs::get_file_contents(path.c_str());

	post_data.set("page_desc", doc.title);//title cannot be empty, default value is filename
	post_data.set("page_url", doc.url);
	if(!doc.author.empty())		post_data.set("author", doc.author);
	if(!doc.date.empty())		post_data.set("date", doc.date);
	if(!doc.category.empty())	post_data.set("category", doc.category);
	if(doc.keywords.empty()){
		post_data.set("keywords", config::blog_keywords);
	} else {
		post_data.set("keywords", doc.keywords);
	}

	fill_comments(post_data, fs::get_comments(path));
	post_data.set("content", md::render_md_to_html(file_contents));
	post_data.set("comment_token", captcha::gen_token());
	if(!alert_msg.empty()){
        post_data.set("alert_msg", alert_msg);
	}
	post_tmpl.render(post_data, ss);
	return ss.str();
}

}