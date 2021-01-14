#ifndef RENDER_H
#define RENDER_H

#include <string>
#include "comments.h"

namespace render{

std::string render_home_page();
std::string render_post(const std::string& path, const std::string& req_ip, const std::string& alert_msg = "", const comments::comment& com = {});
std::string render_page(const std::string& path);
std::string render_dir_as_md_list(const std::string& dir);

}

#endif