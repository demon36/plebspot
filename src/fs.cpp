#include "fs.h"
#include "util.h"

#include <filesystem>
#include <fstream>
#include <mustache.hpp>

using namespace std;
//only a single level of hierarchy is supported
namespace fs{

std::string get_file_contents(const char* filename){
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

void post_comment(const string& post_path, const string& comment){
	string comments_file_path = post_path + ".comments";
	ofstream comments_file(comments_file_path, std::ios::app);
	comments_file << kainjow::mustache::html_escape(comment) << "\n";
	comments_file.flush();
}

vector<comment> get_comments(const string& post_path){
	vector<comment> comments;
	string comments_file_path = post_path + ".comments";
	ifstream comments_file(comments_file_path, std::ios::in);
	for( std::string line; getline( comments_file, line ); ) {
		comments.emplace_back(comment{"", "", line});
	}
	return comments;
}



}

