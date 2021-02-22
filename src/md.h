#ifndef MD_H
#define MD_H

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include "util.h"

namespace md{

typedef std::chrono::time_point<std::chrono::system_clock> timestamp;

enum class doc_type{
	page,
	post,
};

struct md_doc{
	std::string title;
	std::string author;
	std::string url;
	std::string date;
	std::string keywords;
	std::string category;
	//to be used with caching:
	// md::timestamp created_ts;
	// md::timestamp modified_ts;
	// std::string html;
};

md_doc make_md_doc(std::filesystem::path p);
std::map<std::string, std::vector<md_doc>> get_md_docs(doc_type type);
std::string render_doc(doc_type type, std::string category, std::string md_file_name);
std::string render_md_to_html(const std::string& markdown);
util::outcome<std::string> gen_rss(const std::string& host);
std::string gen_sitemap(const std::string& host);

}

#endif