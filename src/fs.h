#ifndef FS_H
#define FS_H

#include <map>
#include <vector>
#include <string>

//only a single level of hierarchy is supported
namespace fs{

struct comment
{
	std::string author;
	std::string date;
	std::string message;	
};

std::string get_file_contents(const char* filename);
void post_comment(const std::string& post_path, const std::string& comment);
std::vector<comment> get_comments(const std::string& post_path);

}
#endif