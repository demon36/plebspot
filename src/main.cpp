#include <httplib.h>
#include <filesystem>
#include <fmt/core.h>

#include <string>
#include <sstream>

#include "fs.h"
#include "md.h"
#include "render.h"
#include "config.h"
#include "captcha.h"
#include "util.h"

using namespace std;
using namespace httplib;

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
			"html_tmpl:  " << config::html_tmpl << "\n";
	}
}

void serve(){
	config::load();
	Server svr;
	svr.Get("/", [](const Request& req, Response& res) {
		res.set_content(render::render_home_page(), "text/html");
	});

	svr.Get(R"(/pages/(([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		auto pagePath = req.matches[1];
		res.set_content(render::render_post(string("pages/") + pagePath.str()), "text/html");
	});

	svr.Get(R"(/posts/(([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		auto postPath = req.matches[1];
		res.set_content(render::render_post(string("posts/") + postPath.str()), "text/html");
	});

    svr.Post(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+)/post_comment)", [&](const Request& req, Response& res) {
        auto postPath = req.matches[1];
        if (req.has_param("token") && req.has_param("comment") && req.has_param("captcha")) {
            string token = req.get_param_value("token");
            string captcha = req.get_param_value("captcha");
            string comment = req.get_param_value("comment");
            if (!util::trim(comment).empty() && comment.size() <= 256 && captcha::validate(token, captcha)){
                fs::post_comment(postPath, comment);
                res.set_content(render::render_post(postPath.str()), "text/html");
                return;
            }
        }
        res.set_content(render::render_post(postPath.str(), "bad captcha"), "text/html");
    });

	svr.set_error_handler([](const Request& req, Response& res) {
		fmt::print("error while serving url {}\n", req.path);
	});

	svr.Get(R"(/static/([a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		string target_path = string("static/") + req.matches[1].str();
		fmt::print("requesting static uri {}\n", target_path);
		if(filesystem::exists(target_path)){
			res.set_content(fs::get_file_contents(target_path.c_str()), "");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	svr.Get(R"(/captcha/(.+))", [&](const Request& req, Response& res) {
		string token = req.matches[1].str();
		std::vector<unsigned char> gif = captcha::gen_gif(token);
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
	} else {
		help();
	}
}
