#include <httplib.h>
#include <filesystem>
#include <fmt/core.h>

#include <string>
#include <sstream>

#include "md.h"
#include "rss.png.h"
#include "render.h"
#include "config.h"
#include "captcha.h"
#include "util.h"
#include "comments.h"

#define VERSION "0.9.8"

using namespace std;
using namespace httplib;
using namespace util;

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

	ofstream rss_png_stream(STATIC_DIR "/rss.png", ofstream::binary);
	rss_png_stream.write((const char*)res_rss_png, res_rss_png_len);
	rss_png_stream.close();

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

#define HTTP_OUTCOME_ERR_CHECK(o, resp) if(!o.is_success()) return resp.status = 500

error serve(){
	error e = config::load();
	if(e != errors::success){
		return e;
	}
	
	Server svr;
	svr.Get("/", [](const Request& req, Response& res) {
		outcome<string> home_out = render::render_home_page();
		HTTP_OUTCOME_ERR_CHECK(home_out, res);
		res.set_content(home_out.get_result(), "text/html");
	});

	svr.Get(R"(/(pages/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
		auto page_path = req.matches[1];
		if(filesystem::exists(page_path.str())){
			util::outcome<string> page_out = render::render_page(page_path.str());
			HTTP_OUTCOME_ERR_CHECK(page_out, res);
			res.set_content(page_out.get_result(), "text/html");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	svr.Get(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
		auto post_path = req.matches[1];
		if(filesystem::exists(post_path.str())){
			outcome<string> post_out = render::render_post(post_path.str(), req.remote_addr);
			HTTP_OUTCOME_ERR_CHECK(post_out, res);
			res.set_content(post_out.get_result(), "text/html");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	svr.Post(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
		auto post_path = req.matches[1];

		comments::comment com = {
			.author = req.get_param_value("author"),
			.date = util::get_current_time(),
			.message = req.get_param_value("comment"),
			.author_ip = req.remote_addr,
		};

		error err_code = comments::post_comment(
			post_path.str(), com, req.get_param_value("token"), req.get_param_value("captcha"));

		if(err_code == errors::success) {
			outcome<string> post_out = render::render_post(post_path.str(), req.remote_addr);
			HTTP_OUTCOME_ERR_CHECK(post_out, res);
			res.set_content(post_out.get_result(), "text/html");
		} else {
			outcome<string> post_out = render::render_post(post_path.str(), req.remote_addr, errors::to_string(err_code), com);
			res.set_content(post_out.get_result(), "text/html");
		}
	});

	svr.set_error_handler([](const Request& req, Response& res) {
		fmt::print("error while serving url {}\n", req.path);
	});

	svr.Get(R"(/(static/[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		string target_path = req.matches[1].str();
		if(filesystem::exists(target_path)){
			outcome<string> o = util::get_file_contents(target_path.c_str());
			HTTP_OUTCOME_ERR_CHECK(o, res);
			res.set_content(o.get_result(), "");
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

	svr.Get("/rss.xml", [&](const Request& req, Response& res) {
		string host;
		if(req.headers.find("Host") != req.headers.end()){
			host = req.headers.find("Host")->second;
		}
		
		util::outcome<string> rss_out = md::gen_rss(host);
		HTTP_OUTCOME_ERR_CHECK(rss_out, res);
		res.set_content(rss_out.get_result(), "application/rss+xml");
		return res.status = 200;
	});

	svr.Get("/sitemap.xml", [&](const Request& req, Response& res) {
		string host;
		if(req.headers.find("Host") != req.headers.end()){
			host = req.headers.find("Host")->second;
		}

		outcome<string> sitemap_out = md::gen_sitemap(host);
		HTTP_OUTCOME_ERR_CHECK(sitemap_out, res);
		res.set_content(sitemap_out.get_result(), "application/xml");
		return res.status = 200;
	});

	svr.Get("/robots.txt", [&](const Request& req, Response& res) {
		string host;
		if(req.headers.find("Host") != req.headers.end()){
			host = req.headers.find("Host")->second;
		}

		string content = "User-agent: *\nAllow: /\n\n";
		if(!host.empty()){
			content += "Sitemap: http://" + host + "/sitemap.xml";
		}
		res.set_content(content, "text/plain");
		return res.status = 200;
	});

	/*
User-agent: *
Allow: /

Sitemap: http://www.example.com/sitemap.xml

	*/

	const char* ip = "0.0.0.0";
	fmt::print("plebspot is listening on {}:{}\n", ip, config::http_port);
	if(!svr.listen(ip, config::http_port)){
		return errors::failed_to_listen;
	};
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
		serve();
	} else if(argc == 2 && strcmp(argv[1], "test") == 0){
		comments::test();
	} else if(argc == 2 && strcmp(argv[1], "--version") == 0){
		fmt::print("{}\n", VERSION);
	} else {
		help();
	}
}
