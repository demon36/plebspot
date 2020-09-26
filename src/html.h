#ifndef HTML_H
#define HTML_H

#include <string>

namespace html{

std::string render_home_page();
std::string render_dir_as_md_list(const std::string& dir);

}
#endif