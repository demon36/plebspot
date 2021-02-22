#include "render.h"

#include "util.h"
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

void fill_comments(mustache::data& page_data, const vector<comments::comment>& comments){
	mustache::data comments_list{mustache::data::type::list};
	for(const comments::comment& c : comments){
		mustache::data comment_data;
		comment_data.set("author", c.author);
		comment_data.set("date", c.date);
		comment_data.set("message", c.message);
		comments_list << comment_data;
	}
	page_data.set("comments_list", comments_list);
}

util::outcome<mustache::mustache> get_template(){
	util::outcome<string> template_out = util::get_file_contents(fmt::format("./{}/{}", TEMPLATES_DIR, config::html_tmpl));
	if(!template_out.is_success()){
		return template_out.get_error();
	}
	return mustache::mustache(template_out.get_result());
}

util::outcome<string> render_home_page(){
	stringstream ss;
	util::outcome<mustache::mustache> template_out = get_template();
	OUTCOME_ERR_CHECK(template_out);
	mustache::data home_data;
	fill_generic_date(home_data);
	home_data.set("keywords", config::blog_keywords);
	home_data.set("page_desc", config::blog_desc);
	template_out.get_result().render(home_data, ss);
	return ss.str();
}

void fill_document_data(mustache::data& page_data, const string& path){//fill with data extracted from the markdown file
	md::md_doc doc = md::make_md_doc(path);

	page_data.set("page_desc", doc.title);//title cannot be empty, default value is filename
	page_data.set("page_url", doc.url);
	if(!doc.author.empty())		page_data.set("author", doc.author);
	if(!doc.date.empty())		page_data.set("date", doc.date);
	if(!doc.category.empty())	page_data.set("category", doc.category);
	if(doc.keywords.empty()){
		page_data.set("keywords", config::blog_keywords);
	} else {
		page_data.set("keywords", doc.keywords);
	}

}

util::outcome<string> render_post(const string& path, const string& req_ip, const string& alert_msg, const comments::comment& com){
	stringstream ss;
	util::outcome<mustache::mustache> template_out = get_template();
	OUTCOME_ERR_CHECK(template_out);
	mustache::data post_data;
	fill_generic_date(post_data);
	fill_document_data(post_data, path);

	vector<comments::comment> coms = comments::get_comments(path);
	fill_comments(post_data, coms);
	
	if(config::comments_enabled){
		post_data.set("comments_enabled", true);
		post_data.set("comment_token", comments::gen_token(path, req_ip, coms.size()));
	}
	
	util::outcome<string> contents_out = util::get_file_contents(path.c_str());
	if(!contents_out.is_success()){
		return contents_out;
	}
	
	post_data.set("content", md::render_md_to_html(contents_out.get_result()));
	if(!alert_msg.empty()){
		post_data.set("alert_msg", alert_msg);
		post_data.set("comment_msg", com.message);
		post_data.set("comment_author", com.author);
	}
	template_out.get_result().render(post_data, ss);
	return ss.str();
}

util::outcome<string> render_page(const string& path){
	stringstream ss;
	util::outcome<mustache::mustache> template_out = get_template();
	OUTCOME_ERR_CHECK(template_out);
	mustache::mustache post_tmpl = template_out.get_result();
	mustache::data post_data;
	fill_generic_date(post_data);
	fill_document_data(post_data, path);
	util::outcome<string> contents_out = util::get_file_contents(path.c_str());
	OUTCOME_ERR_CHECK(contents_out);
	post_data.set("content", md::render_md_to_html(contents_out.get_result()));
	post_tmpl.render(post_data, ss);
	return ss.str();
}

}