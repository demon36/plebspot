#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include "util.h"

#define CONFIG_FILE		"pleb.yml"
#define PAGES_DIR		"pages"
#define POSTS_DIR		"posts"
#define STATIC_DIR		"static"
#define TEMPLATES_DIR	"template"

#define DEFINE_CONFIG_TEXT_FIELD(NAME, DEFAULT_VAL) const std::string& NAME = (text_fields[#NAME] = DEFAULT_VAL);
#define DEFINE_CONFIG_BOOL_FIELD(NAME, DEFAULT_VAL) const bool& NAME = (bool_fields[#NAME] = DEFAULT_VAL);
#define DEFINE_CONFIG_INT_FIELD(NAME, DEFAULT_VAL) const int& NAME = (int_fields[#NAME] = DEFAULT_VAL);

struct config{

	std::map<std::string, std::string> text_fields;
	std::map<std::string, bool> bool_fields;
	std::map<std::string, int> int_fields;

	DEFINE_CONFIG_TEXT_FIELD(blog_title, "my_blog");
	DEFINE_CONFIG_TEXT_FIELD(blog_desc, "random stuff");
	DEFINE_CONFIG_TEXT_FIELD(html_tmpl, "plain.html");
	DEFINE_CONFIG_TEXT_FIELD(blog_keywords, "fun, diy");
	DEFINE_CONFIG_INT_FIELD(http_port, 80);
	DEFINE_CONFIG_INT_FIELD(https_port, 443);
	DEFINE_CONFIG_BOOL_FIELD(comments_enabled, true);
	DEFINE_CONFIG_BOOL_FIELD(use_ssl, false);
	DEFINE_CONFIG_TEXT_FIELD(cert_path, "certs/cert.pem");
	DEFINE_CONFIG_TEXT_FIELD(cert_key_path, "certs/key.pem");

	static config fields;

	static util::error load();

};

#endif