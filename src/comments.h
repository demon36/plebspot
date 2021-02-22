#ifndef COMMENTS_H
#define COMMENTS_H

#include <string>
#include <vector>
#include "errors.h"

namespace comments{

const unsigned long CAPTCHA_LEN = 6;//including null terminator

struct comment
{
	std::string author;
	std::string date;
	std::string message;
	std::string author_ip;
};

struct token
{
	char captcha_answer[CAPTCHA_LEN];//array items are indices of english letters
	unsigned char ip[4];
	size_t post_id_hash;
	size_t num_comments;
};

std::string serialize_token(const token& tok);
token deserialize_token(const std::string& tok_str);
std::string gen_token(const std::string& post_path, const std::string& ip, std::size_t num_comments);
std::vector<unsigned char> gen_captcha_gif(const std::string& token);
bool validate_captcha(const std::string& post_path, const std::string& token, const std::string& user_input, const std::string& ip);
util::error post_comment(const std::string& post_path, const comments::comment& comment, const std::string& token, const std::string& captcha_answer);
std::vector<comment> get_comments(const std::string& post_path);
void test();

}
#endif
