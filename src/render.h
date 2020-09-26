#ifndef RENDER_H
#define RENDER_H

#include <string>

namespace render{

std::string render_home_page(const std::string& blog_title);
std::string render_dir_as_md_list(const std::string& dir);

}

#endif