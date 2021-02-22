#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "util.h"

#define CONFIG_FILE		"pleb.yml"
#define PAGES_DIR		"pages"
#define POSTS_DIR		"posts"
#define STATIC_DIR		"static"
#define TEMPLATES_DIR           "template"

namespace config{
extern std::string blog_title;
extern std::string blog_desc;
extern std::string blog_keywords;
extern std::string html_tmpl;
extern int http_port;
extern bool comments_enabled;

util::error load();
}

#endif
