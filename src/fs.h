#ifndef FS_H
#define FS_H

#include <map>
#include <vector>
#include <string>

//only a single level of hierarchy is supported
namespace fs{

//key: parent folder/category, empty string if md file is a direct child, value: page path
std::map<std::string, std::vector<std::string>> get_md_files_map(const std::string& parent_path);
std::string get_file_contents(const char* filename);

}
#endif