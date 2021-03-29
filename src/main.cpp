#include <fmt/core.h>
#include <fmt/os.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "md.h"
#include "config.h"
#include "rss.png.h"
#include "comments.h"
#include "util.h"
#include "http.h"

#define VERSION "0.9.8"

using namespace std;
using namespace util;

extern const char* plain_html_tmpl;
extern const char* sample_post_md;

void init(){
	filesystem::create_directory(PAGES_DIR);
	filesystem::create_directory(POSTS_DIR);
	filesystem::create_directory(POSTS_DIR "/my_category");
	filesystem::create_directory(STATIC_DIR);
	filesystem::create_directory(TEMPLATES_DIR);
	string plain_tmpl_path = string(TEMPLATES_DIR) + "/" + config::fields.html_tmpl;
	if(!filesystem::exists(plain_tmpl_path)){
		ofstream plain_tmpl(plain_tmpl_path);
		plain_tmpl << plain_html_tmpl;
	}

	ofstream rss_png_stream(STATIC_DIR "/rss.png", ofstream::binary);
	rss_png_stream.write((const char*)res_rss_png, res_rss_png_len);
	rss_png_stream.close();

	ofstream sample_post_stream(POSTS_DIR "/my_category/sample_post.md");
	sample_post_stream << sample_post_md;
	if(!filesystem::exists(CONFIG_FILE)){
		auto out = fmt::output_file(CONFIG_FILE);
		out.print("blog_title: {}\n"
			"blog_desc:  {}\n"
			"blog_keywords:  {}\n"
			"http_port:  {}\n"
			"html_tmpl:  {}\n"
			"comments_enabled:  {}\n",
			config::fields.blog_title, config::fields.blog_desc, config::fields.blog_keywords, config::fields.http_port,
			config::fields.html_tmpl, (config::fields.comments_enabled ? "true" : "false")
		);
	}
}

void help(){
	cout << "usage:\n"
		"\tplebspot init\n"
		"\t\tcreate sample files and folder needed for operation\n"
		"\tplebspot serve\n"
		"\t\tserves plebspot http application on port 1993 or port configured in pleb.yml\n";
}

int main(int argc, char const *argv[])
{
	error e = util::errors::failed_to_listen;
	string s = util::errors::to_string(e);
	if(argc == 2 && strcmp(argv[1], "init") == 0){
		init();
	} else if(argc == 2 && strcmp(argv[1], "serve") == 0){
		http::serve();
	} else if(argc == 2 && strcmp(argv[1], "test") == 0){
		comments::test();
	} else if(argc == 2 && strcmp(argv[1], "--version") == 0){
		fmt::print("{}\n", VERSION);
	} else {
		help();
	}
}
