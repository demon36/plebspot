#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#define CONFIG_FILE "pleb.yml"

namespace config{


	extern std::string blog_title;
	extern std::string favicon_path;
	extern int http_port;

	bool load();

}

#endif
