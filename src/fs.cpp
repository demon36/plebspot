#include "fs.h"

#include <filesystem>
#include <fstream>

using namespace std;
//only a single level of hierarchy is supported
namespace fs{

map<string, vector<string>> get_md_files_map(const string& parent_path){
	map<string, vector<string>> md_files_map;
	for(auto& i: filesystem::directory_iterator(parent_path)){
		if(i.is_directory()){
			for(auto& j: filesystem::directory_iterator(i)){
				if(!j.is_directory()){
					md_files_map[i.path().filename().string()].push_back(j.path().filename().string());
				}
			}
		} else {
			md_files_map[""].push_back(i.path().filename().string());
		}
	}
	return md_files_map;
}

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if(in){
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

}

