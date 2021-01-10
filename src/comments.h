#ifndef COMMENTS_H
#define COMMENTS_H

#include <string>
#include <vector>
#include "errors.h"

namespace comments{

struct comment
{
	std::string author;
	std::string date;
	std::string message;	
};

std::string gen_token();
std::vector<unsigned char> gen_captcha_gif(const std::string& token);
bool validate_captcha(const std::string& token, const std::string& user_input);
err::errors post_comment(const std::string& post_path, const comments::comment& comment, const std::string& token, const std::string& captcha_answer);
std::vector<comment> get_comments(const std::string& post_path);
void test();

}
#endif
