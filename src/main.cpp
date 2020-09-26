#include <httplib.h>
#include <filesystem>
#include <fmt/core.h>

#include <string>
#include <sstream>

#include "fs.h"
#include "md.h"
#include "html.h"

using namespace std;

int main(void)
{
	using namespace httplib;

	Server svr;

	svr.Get("/", [](const Request& req, Response& res) {
		try{
			res.set_content(html::render_home_page(), "text/html");
		}catch(const exception& ex){
			cout << ex.what() << endl;
		}
	});

	svr.Get(R"(/pages/(([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		auto pagePath = req.matches[1];
		res.set_content(md::render_md_to_html(string("./pages/") + pagePath.str()), "text/html");
	});

	svr.Get(R"(/posts/(([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		auto postPath = req.matches[1];
		res.set_content(md::render_md_to_html(string("./posts/") + postPath.str()), "text/html");
	});

	svr.set_error_handler([](const Request& req, Response& res) {
		fmt::print("error\n");
	});

	svr.Get("/stop", [&](const Request& req, Response& res) {
		svr.stop();
	});

	svr.Get("/favicon.ico", [&](const Request& req, Response& res) {
		if(filesystem::exists("./favicon.ico")){
			res.set_content(fs::get_file_contents("./favicon.ico"), "image/webp");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	svr.Get(R"(/static/([a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		string target_path = string("./static/") + req.matches[1].str();
		fmt::print("requesting static uri {}\n", target_path);
		if(filesystem::exists(target_path)){
			res.set_content(fs::get_file_contents(target_path.c_str()), "");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	const char* ip = "0.0.0.0";
	unsigned int port = 1993;
	fmt::print("starting to listen on {}:{}\n", ip, port);
	svr.listen(ip, port);
}
