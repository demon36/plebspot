#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#define CONFIG_FILE		"pleb.yml"
#define PAGES_DIR		"pages"
#define POSTS_DIR		"posts"
#define STATIC_DIR		"static"
#define TEMPLATES_DIR	"template"

namespace config{
extern std::string blog_title;
extern std::string blog_desc;
extern std::string html_tmpl;
extern int http_port;

void load();
}

#endif
