#ifndef MD_H
#define MD_H

#include <string>

namespace md{
	std::string render_md_to_html(const std::string& markdown);
}

#endif