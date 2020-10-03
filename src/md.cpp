#include "md.h"

#include "config.h"
#include "util.h"

#include <fstream>
#include <cstring>
#include <fmt/core.h>
//hoedown
#include "html.h"
#include "document.h"

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
	std::ifstream file_stream(p.string());
	for(string line; getline(file_stream, line);){
		//get metadata in form [title: my custom post title]::
		if(line.find(MD_COMMENT_PREFIX) == 0 && line.find(MD_COMMENT_POSTFIX) == line.size()-MD_COMMENT_POSTFIX.size()){
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
				if(!j.is_directory()){
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

}
