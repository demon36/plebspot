#include <httplib.h>
#include <filesystem>
#include <fmt/core.h>

#include <string>
#include <sstream>

#include "md.h"
#include "render.h"
#include "config.h"
#include "captcha.h"
#include "util.h"
#include "comments.h"

using namespace std;
using namespace httplib;
using namespace err;

extern const char* plain_html_tmpl;
extern const char* sample_post_md;

void init(){
	filesystem::create_directory(PAGES_DIR);
	filesystem::create_directory(POSTS_DIR);
	filesystem::create_directory(POSTS_DIR "/my_category");
	filesystem::create_directory(STATIC_DIR);
	filesystem::create_directory(TEMPLATES_DIR);
	string plain_tmpl_path = string(TEMPLATES_DIR) + "/" + config::html_tmpl;
	if(!filesystem::exists(plain_tmpl_path)){
		ofstream plain_tmpl(plain_tmpl_path);
		plain_tmpl << plain_html_tmpl;
	}
	ofstream sample_post_stream(POSTS_DIR "/my_category/sample_post.md");
	sample_post_stream << sample_post_md;
	if(!filesystem::exists(CONFIG_FILE)){
		ofstream config_stream(CONFIG_FILE);
		config_stream <<
			"blog_title: " << config::blog_title << "\n"
			"blog_desc:  " << config::blog_desc << "\n"
			"blog_keywords:  " << config::blog_keywords << "\n"
			"http_port:  " << config::http_port << "\n"
			"html_tmpl:  " << config::html_tmpl << "\n"
			"comments_enabled:  " << (config::comments_enabled ? "true" : "false") << "\n";
	}
}

void serve(){
	config::load();
	Server svr;
	svr.Get("/", [](const Request& req, Response& res) {
		res.set_content(render::render_home_page(), "text/html");
	});

	svr.Get(R"(/(pages/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
		auto page_path = req.matches[1];
		res.set_content(render::render_post(page_path.str()), "text/html");
	});

	svr.Get(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
		auto post_path = req.matches[1];
		res.set_content(render::render_post(post_path.str()), "text/html");
	});

	svr.Post(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md)/post_comment)", [&](const Request& req, Response& res) {
		auto post_path = req.matches[1];

		comments::comment com = {
			.author = req.get_param_value("author"),
			.date = util::get_current_time(),
			.message = req.get_param_value("comment"),
			.author_ip = req.remote_addr,
		};

		errors err_code = comments::post_comment(
			post_path.str(), com, req.get_param_value("token"), req.get_param_value("captcha"));

		if(err_code == errors::success) {
			res.set_content(render::render_post(post_path.str()), "text/html");
		} else {
			res.set_content(
				render::render_post(post_path.str(), err::to_string(err_code), com),
				"text/html"
				);
		}
	});

	svr.set_error_handler([](const Request& req, Response& res) {
		fmt::print("error while serving url {}\n", req.path);
	});

	svr.Get(R"(/(static/[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		string target_path = req.matches[1].str();
		fmt::print("requesting static uri {}\n", target_path);
		if(filesystem::exists(target_path)){
			res.set_content(util::get_file_contents(target_path.c_str()), "");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	svr.Get(R"(/captcha/(.+))", [&](const Request& req, Response& res) {
		string token = req.matches[1].str();
		std::vector<unsigned char> gif = comments::gen_captcha_gif(token);
		if(gif.empty()){
			return res.status = 404;
		} else {
		    res.set_content((char*)gif.data(), gif.size(), "");
			return res.status = 200;
		}
	});

	const char* ip = "0.0.0.0";
	fmt::print("plebspot is listening on {}:{}\n", ip, config::http_port);
	svr.listen(ip, config::http_port);
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
	if(argc == 2 && strcmp(argv[1], "init") == 0){
		init();
	} else if(argc == 2 && strcmp(argv[1], "serve") == 0){
		serve();
	} else if(argc == 2 && strcmp(argv[1], "test") == 0){
		comments::test();
	} else {
		help();
	}
}
