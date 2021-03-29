#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include "http.h"
#include <filesystem>
#include <future>
#include <fmt/core.h>
#include "config.h"
#include "comments.h"
#include "md.h"
#include "render.h"
#include "config.h"
#include "captcha.h"

using namespace httplib;
using namespace util;
using namespace std;

namespace http{

#define HTTP_OUTCOME_ERR_CHECK(o, resp) if(!o.is_success()) return resp.status = 500

error serve(){
	error e = config::load();
	if(e != errors::success){
		return e;
	}

	unique_ptr<Server> svr;
	unique_ptr<Server> http_redirect_svr;
	if(config::fields.use_ssl){
		svr.reset(new SSLServer(config::fields.cert_path.c_str(), config::fields.cert_key_path.c_str()));
		http_redirect_svr.reset(new Server());
		http_redirect_svr->Get(R"(.+)", [](const Request& req, Response& res){
			string host = req.headers.find("Host")->second;
			if(host.find(":") != host.npos){
				host = host.substr(0, host.find(":"));
			}
			if(config::fields.https_port != 443){
				host += ":" + to_string(config::fields.https_port);
			}
			string url = fmt::format("https://{}{}", host, req.target);
			fmt::print("redirecting request to url = {}\n", url);
			res.set_redirect(url);
		});
	} else {
		svr.reset(new Server());
	}
	
	svr->Get("/", [](const Request& req, Response& res) {
		outcome<string> home_out = render::render_home_page();
		HTTP_OUTCOME_ERR_CHECK(home_out, res);
		res.set_content(home_out.get_result(), "text/html");
		return res.status = 200;
	});

	svr->Get(R"(/(pages/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
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

	svr->Get(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
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

	svr->Post(R"(/(posts/([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+\.md))", [&](const Request& req, Response& res) {
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
			return res.status = 200;
		} else {
			outcome<string> post_out = render::render_post(post_path.str(), req.remote_addr, errors::to_string(err_code), com);
			res.set_content(post_out.get_result(), "text/html");
			return res.status = 500;
		}
	});

	svr->set_error_handler([](const Request& req, Response& res) {
		fmt::print("error while serving url {}\n", req.path);
	});

	svr->Get(R"(/(static/[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
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

	svr->Get(R"(/captcha/(.+))", [&](const Request& req, Response& res) {
		string token = req.matches[1].str();
		std::vector<unsigned char> gif = comments::gen_captcha_gif(token);
		if(gif.empty()){
			return res.status = 404;
		} else {
		    res.set_content((char*)gif.data(), gif.size(), "");
			return res.status = 200;
		}
	});

	svr->Get("/rss.xml", [&](const Request& req, Response& res) {
		string host;
		if(req.headers.find("Host") != req.headers.end()){
			host = req.headers.find("Host")->second;
		}
		
		util::outcome<string> rss_out = md::gen_rss(host);
		HTTP_OUTCOME_ERR_CHECK(rss_out, res);
		res.set_content(rss_out.get_result(), "application/rss+xml");
		return res.status = 200;
	});

	svr->Get("/sitemap.xml", [&](const Request& req, Response& res) {
		string host;
		if(req.headers.find("Host") != req.headers.end()){
			host = req.headers.find("Host")->second;
		}

		outcome<string> sitemap_out = md::gen_sitemap(host);
		HTTP_OUTCOME_ERR_CHECK(sitemap_out, res);
		res.set_content(sitemap_out.get_result(), "application/xml");
		return res.status = 200;
	});

	svr->Get("/robots.txt", [&](const Request& req, Response& res) {
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

	const char* ip = "0.0.0.0";
	int primary_server_port = config::fields.use_ssl ? config::fields.https_port : config::fields.http_port;
	future<bool> redirect_server_future;
	if(config::fields.use_ssl){
		fmt::print("plebspot is attempting to listen on redirect address {}:{}\n", ip, config::fields.http_port);
		redirect_server_future = async(&Server::listen, http_redirect_svr.get(), ip, config::fields.http_port, 0);
	}
	fmt::print("plebspot is attempting to listen on primary address {}:{}\n", ip, primary_server_port);
	if(!svr->listen(ip, primary_server_port)){
		return errors::failed_to_listen;
	};

	if(!redirect_server_future.get()){
		return errors::failed_to_listen;
	};
	
	return errors::success;
}


}