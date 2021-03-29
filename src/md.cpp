#include "md.h"

#include "config.h"
#include "util.h"

#include <fstream>
#include <cstring>
#include <fmt/core.h>
//hoedown
#include "html.h"
#include "document.h"

#include <pugixml.hpp>

/*
md comments syntax
[//]: # (comment here)
[comment here]::
*/
/*
	doc_key{cat} -> md_doc{title, author, created_ts, modified_ts, url, html}
*/
enum renderer_type {
	RENDERER_HTML,
	RENDERER_HTML_TOC
};

struct extension_category_info {
	unsigned int flags;
	const char *option_name;
	const char *label;
};

struct extension_info {
	unsigned int flag;
	const char *option_name;
	const char *description;
};

struct html_flag_info {
	unsigned int flag;
	const char *option_name;
	const char *description;
};

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16

using namespace std;


namespace md{

const string MD_COMMENT_PREFIX = "[//]: # (";
const string MD_COMMENT_POSTFIX = ")";

md_doc make_md_doc(filesystem::path p){
	string parent_cat;
	if(p.parent_path().stem() != PAGES_DIR && p.parent_path().stem() != POSTS_DIR){
		parent_cat = p.parent_path().stem().string();
	}

	md_doc doc = {
		.title = p.filename().string(),
		.url = "/" + p.string(),
		.category = parent_cat,
	};

#ifdef _WIN32
	util::replace_all(doc.url, "\\", "/");
#endif

	std::ifstream file_stream(p.string());
	for(string line; getline(file_stream, line);){
		//get metadata in form [title: my custom post title]::
		if(line.find(MD_COMMENT_PREFIX) == 0 && line.rfind(MD_COMMENT_POSTFIX) == line.size()-MD_COMMENT_POSTFIX.size()){
			pair<string, string> kv = util::split(
				line.substr(MD_COMMENT_PREFIX.size(),
				line.size()-MD_COMMENT_PREFIX.size()-MD_COMMENT_POSTFIX.size()),
				":");
			if(kv.first == "title"){
				doc.title = kv.second;
			} else if(kv.first == "keywords"){
				doc.keywords = kv.second;
			} else if(kv.first == "author"){
				doc.author = kv.second;
			} else if(kv.first == "date"){
				doc.date = kv.second;
			}
		} else {
			break;
		}
	}
	return doc;
}

map<string, vector<md_doc>> get_md_docs(doc_type type){
	map<string, vector<md_doc>> docs_map;
	string scan_path = type == doc_type::page ? PAGES_DIR : POSTS_DIR;
	for(auto& i: filesystem::directory_iterator(scan_path)){
		if(i.is_directory()){
			for(auto& j: filesystem::directory_iterator(i)){
				if(!j.is_directory() && j.path().extension() == ".md"){
					docs_map[i.path().filename().string()].push_back(make_md_doc(j.path()));
				}
			}
		} else {
			docs_map[""].push_back(make_md_doc(i.path()));
		}
	}
	return docs_map;
}

string render_doc(doc_type type, string category, string md_file_name){
	switch(type){
		case doc_type::page:
			return render_md_to_html(fmt::format("./{}/{}/{}", PAGES_DIR, category, md_file_name));
		case doc_type::post:
			return render_md_to_html(fmt::format("./{}/{}/{}", POSTS_DIR, category, md_file_name));
	}
	return "";
}

string render_md_to_html(const string& md_str){
	hoedown_buffer* ob;
	hoedown_renderer *renderer = NULL;
	void (*renderer_free)(hoedown_renderer *) = NULL;
	hoedown_document *document;

	/* Parse options */
	renderer_type renderer_t = RENDERER_HTML;
	int toc_level = 0;
	hoedown_html_flags html_flags = hoedown_html_flags(0);
	hoedown_extensions extensions = HOEDOWN_EXT_FENCED_CODE;
	size_t max_nesting = DEF_MAX_NESTING;

	/* Create the renderer */
	switch (renderer_t) {
		case RENDERER_HTML:
			renderer = hoedown_html_renderer_new(html_flags, toc_level);
			renderer_free = hoedown_html_renderer_free;
			break;
		case RENDERER_HTML_TOC:
			renderer = hoedown_html_toc_renderer_new(toc_level);
			renderer_free = hoedown_html_renderer_free;
			break;
	};

	/* Perform Markdown rendering */
	ob = hoedown_buffer_new(DEF_OUNIT);
	document = hoedown_document_new(renderer, extensions, max_nesting);

	hoedown_document_render(document, ob, (uint8_t*)md_str.c_str(), md_str.size());

	/* Cleanup */
	hoedown_document_free(document);
	renderer_free(renderer);

	string contentString((const char*)ob->data, ob->size);
	hoedown_buffer_free(ob);
	return contentString;
}

/* example:
<?xml version="1.0" encoding="UTF-8" ?>
<rss version="2.0">
<channel>
 <title>RSS Title</title>
 <description>This is an example of an RSS feed</description>
 <link>http://www.example.com/main.html</link>
 <copyright>2020 Example.com All rights reserved</copyright>
 <lastBuildDate>Mon, 06 Sep 2010 00:01:00 +0000 </lastBuildDate>
 <pubDate>Sun, 06 Sep 2009 16:20:00 +0000</pubDate>
 <ttl>1800</ttl>

 <item>
  <title>Example entry</title>
  <description>Here is some text containing an interesting description.</description>
  <link>http://www.example.com/blog/post/1</link>
  <guid isPermaLink="false">7bd204c6-1655-4c27-aeee-53f933c5395f</guid>
  <pubDate>Sun, 06 Sep 2009 16:20:00 +0000</pubDate>
 </item>

</channel>
</rss>
*/
util::outcome<string> gen_rss(const string& host){
	pugi::xml_document doc;
	auto declaration_node = doc.append_child(pugi::node_declaration);
	declaration_node.append_attribute("version") = "1.0";
	declaration_node.append_attribute("encoding") = "utf-8";
	declaration_node.append_attribute("standalone") = "yes";

	auto rss_node = doc.append_child("rss");
	rss_node.append_attribute("version") = "2.0";

	auto channel_node = rss_node.append_child("channel");
	channel_node.append_child("title").append_child(pugi::node_pcdata).set_value(config::fields.blog_title.c_str());
	channel_node.append_child("link").append_child(pugi::node_pcdata).set_value(util::to_absolute_url(host, "/rss.xml").c_str());
	channel_node.append_child("description").append_child(pugi::node_pcdata).set_value(config::fields.blog_desc.c_str());
	channel_node.append_child("generator").append_child(pugi::node_pcdata).set_value("plebspot - pugixml");
	channel_node.append_child("lastBuildDate").append_child(pugi::node_pcdata).set_value(util::get_current_time().c_str());

	map<string, vector<md_doc>> posts = get_md_docs(doc_type::post);
	//todo: sort by date ?
	for(const auto& it : posts){
		for(const md_doc& md : it.second){
			auto post_node = channel_node.append_child("item");
			post_node.append_child("title").append_child(pugi::node_pcdata).set_value(md.title.c_str());
			//todo: add author email to configuration
			post_node.append_child("author").append_child(pugi::node_pcdata).set_value(md.author.c_str());
			post_node.append_child("link").append_child(pugi::node_pcdata).set_value(util::to_absolute_url(host, md.url).c_str());
			post_node.append_child("guid").append_child(pugi::node_pcdata).set_value(util::to_absolute_url(host, md.url).c_str());
			post_node.append_child("pubDate").append_child(pugi::node_pcdata).set_value(md.date.c_str());
			post_node.append_child("category").append_child(pugi::node_pcdata).set_value(md.category.c_str());
			util::outcome<string> contents_outcome = util::get_file_contents(md.url.substr(1, -1));
			if(!contents_outcome.is_success()){
				return contents_outcome;
			}
			string post_html = md::render_md_to_html(contents_outcome.get_result());//remove preceding separator
			post_node.append_child("description").append_child(pugi::node_pcdata).set_value(post_html.c_str());
		}
	}

	stringstream ss;
	doc.save(ss, "\t");
	return util::outcome<string>(ss.str());
}

/* example:
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
	<url>
		<loc>http://www.example.com/foo.html</loc>
		<lastmod>2018-06-04</lastmod>
	</url>
</urlset> 
*/
util::outcome<string> gen_sitemap(const string& host){
	pugi::xml_document doc;
	auto declaration_node = doc.append_child(pugi::node_declaration);
	declaration_node.append_attribute("version") = "1.0";
	declaration_node.append_attribute("encoding") = "utf-8";

	auto urlset_node = doc.append_child("urlset");
	urlset_node.append_attribute("xmlns") = "http://www.sitemaps.org/schemas/sitemap/0.9";

	map<string, vector<md_doc>> posts = get_md_docs(doc_type::post);
	map<string, vector<md_doc>> pages = get_md_docs(doc_type::page);
	posts.insert(pages.begin(), pages.end());
	//todo: sort by date ?
	for(const auto& it : posts){
		for(const md_doc& md : it.second){
			auto url_node = urlset_node.append_child("url");
			string absolute_url = util::to_absolute_url(host, md.url);
			url_node.append_child("loc").append_child(pugi::node_pcdata).set_value(absolute_url.c_str());
			string w3c_date;
			if(md.date.empty()){
				w3c_date = util::format_date_w3c(util::get_last_write_time(md.url.substr(1)));
			}else{
				w3c_date = util::format_date_w3c(util::parse_date_rfc822(md.date));
			}
			url_node.append_child("lastmod").append_child(pugi::node_pcdata).set_value(w3c_date.c_str());
		}
	}

	stringstream ss;
	doc.save(ss, "\t");
	return ss.str();
}

}
