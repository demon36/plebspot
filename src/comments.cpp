#include "comments.h"
#include "util.h"
#include "config.h"

#include <cstdio>
#include <fstream>
#include <mustache.hpp>
#include <fmt/core.h>
#include <plusaes/plusaes.hpp>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <base64.h>
#include <filesystem>
extern "C" {
#include <captcha.h>
}

using namespace std;
using namespace util;

namespace comments{

const std::vector<unsigned char> AES_KEY = plusaes::key_from_string(&"SZHRLWIUOBR2BXYC");
const unsigned char AES_IV[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

const unsigned long CAPTCHA_AES_PADDED_LEN = plusaes::get_padded_encrypted_size(sizeof(token));
const string COMMENTS_FILE_SUFFIX = ".comments";
const size_t MAX_COMMENT_MESSAGE_LENGTH = 256;
const size_t MAX_COMMENTS_COUNT_DIFF = 10;
const char COMMENT_PARTS_DELIMITER = '&';

std::string serialize_token(const token& tok){
	vector<unsigned char> encrypted(CAPTCHA_AES_PADDED_LEN);
	plusaes::encrypt_cbc((unsigned char*)&tok, sizeof(token), &AES_KEY[0], AES_KEY.size(), &AES_IV, &encrypted[0], encrypted.size(), true);
	return base64_encode(string((char*)encrypted.data(), encrypted.size()), true);
}

token deserialize_token(const std::string& tok_str){
	unsigned long padded_size = 0;
	string decoded_token = base64_decode(tok_str);
	vector<unsigned char> decrypted(CAPTCHA_AES_PADDED_LEN);
	plusaes::Error err = plusaes::decrypt_cbc((unsigned char*)decoded_token.c_str(), decoded_token.size(), &AES_KEY[0], AES_KEY.size(), &AES_IV, &decrypted[0], decrypted.size(), &padded_size);
	if(err != plusaes::kErrorOk){
		return token{};
	}
	token tok{};
	memcpy(&tok, decrypted.data(), sizeof(token));
	return tok;
}

std::string gen_token(const string& post_path, const string& ip, std::size_t num_comments){
	token tok{};
	int num_letters = 26;
	srand((unsigned) time(NULL) * getpid());
	
	//set captcha answer
	for (size_t i = 0; i < CAPTCHA_LEN-1; ++i){
		tok.captcha_answer[i] = (unsigned char)(rand() % (num_letters-1));
	}
	tok.captcha_answer[5] = 0;

	//ip
	if(!ip.empty()) {
		sscanf(ip.c_str(), "%hhu.%hhu.%hhu.%hhu", &tok.ip[0], &tok.ip[1], &tok.ip[2], &tok.ip[3]);
	}

	//post path hash & num comments
	tok.post_id_hash = hash<string>{}(post_path);
	tok.num_comments = num_comments;

	return serialize_token(tok);
}

std::vector<unsigned char> gen_captcha_gif(const string& token_str){
	token tok = deserialize_token(token_str);
	std::vector<unsigned char> gif(gifsize);
	captcha_for_letters(gif.data(), (unsigned char*)&tok.captcha_answer[0]);
	return gif;
}

bool ips_match(unsigned char ip1[4], unsigned char ip2[4]){
	//todo: enhance this
	return ip1[0] == ip2[0] &&
		ip1[1] == ip2[1] &&
		ip1[2] == ip2[2] &&
		ip1[3] == ip2[3];
}

bool captcha_answers_match(char c1[CAPTCHA_LEN], char c2[CAPTCHA_LEN]){
	//todo: enhance this
	return c1[0] == c2[0] &&
		c1[1] == c2[1] &&
		c1[2] == c2[2] &&
		c1[3] == c2[3] &&
		c1[4] == c2[4] &&
		c1[5] == c2[5];
}

size_t diff(size_t a, size_t b){
	return a >= b ? a - b : b - a;
}

bool validate_captcha(const string& post_path, const string& token_str, const string& user_input, const std::string& ip){
	//todo: test
	unsigned char request_ip[4];
	char user_input_cstr[CAPTCHA_LEN];
	memcpy(user_input_cstr, user_input.c_str(), CAPTCHA_LEN);
	sscanf(ip.c_str(), "%hhu.%hhu.%hhu.%hhu", &request_ip[0], &request_ip[1], &request_ip[2], &request_ip[3]);
	token tok = deserialize_token(token_str);
	translate_letter_ids((unsigned char*)&tok.captcha_answer[0]);//translate letters ids to actual letters
	bool test = captcha_answers_match(tok.captcha_answer, user_input_cstr);
	test = ips_match(tok.ip, request_ip);
	test = tok.post_id_hash == std::hash<string>{}(post_path);
	test = diff(get_comments(post_path).size(), tok.num_comments) < MAX_COMMENTS_COUNT_DIFF;
	return test && captcha_answers_match(tok.captcha_answer, user_input_cstr) &&
		ips_match(tok.ip, request_ip) &&
		tok.post_id_hash == std::hash<string>{}(post_path) &&
		diff(get_comments(post_path).size(), tok.num_comments) < MAX_COMMENTS_COUNT_DIFF;
}

util::error post_comment(const string& post_path, const comments::comment& comment, const string& token, const string& captcha_answer){
	if(!config::fields.comments_enabled){
		return errors::comments_disabled;
	}

	if(!filesystem::exists(post_path)){
		return errors::comment_post_not_exist;
	}

	if(util::trim(comment.author).empty()){
		return errors::comment_author_not_provided;	
	}

	if(util::trim(comment.message).empty()){
		return errors::comment_message_not_provided;
	}

	if(util::trim(comment.message).size() > MAX_COMMENT_MESSAGE_LENGTH){
		return errors::comment_message_too_large;
	}

	if(util::trim(captcha_answer).empty()){
		return errors::captcha_answer_not_provided;
	}

	if(!validate_captcha(post_path, token, captcha_answer, comment.author_ip)){
		return errors::captcha_wrong_answer;
	}

	string author = comment.author;
	string msg = comment.message;
	//todo: optimize and add more constraints	
	util::replace_all(author, "\n", "");
	util::replace_all(msg, "\n", "<br/>");

	string comments_file_path = post_path + COMMENTS_FILE_SUFFIX;
	ofstream comments_file(comments_file_path, std::ios::app);
	comments_file
		<< author << COMMENT_PARTS_DELIMITER
		<< comment.date << COMMENT_PARTS_DELIMITER
		<< kainjow::mustache::html_escape(msg) << "\n";
	comments_file.flush();

	return errors::success;
}

vector<comment> get_comments(const string& post_path){
	vector<comment> comments;
	string comments_file_path = post_path + COMMENTS_FILE_SUFFIX;
	ifstream comments_file(comments_file_path, std::ios::in);
	for( std::string line; getline( comments_file, line ); ) {
		if(util::trim(line).empty()){
			continue;
		}
		string author = line.substr(0, line.find(COMMENT_PARTS_DELIMITER));
		string date = line.substr(author.size() + 1, line.find(COMMENT_PARTS_DELIMITER, author.size() + 1) - author.size() - 1);
		string msg = line.substr(author.size() + date.size() + 2, -1);
		comments.emplace_back(comment{util::trim(author), util::trim(date), util::trim(msg)});
	}
	return comments;
}

void test_serialization(){
	token t;
	t.captcha_answer[0] = 'c';
	t.captcha_answer[1] = 'r';
	t.captcha_answer[2] = 't';
	t.captcha_answer[3] = 'p';
	t.ip[0] = 192;
	t.ip[1] = 168;
	t.ip[2] = 1;
	t.ip[3] = 36;
	t.post_id_hash = 256;
	t.num_comments = 254;
	string raw = serialize_token(t);
	token out = deserialize_token(raw);
	fmt::print("token.captcha_answer = {}{}{}{}, token.ip = {}.{}.{}.{}, token.post_id_hash = {}, token.num_comments = {}\n", 
		out.captcha_answer[0], out.captcha_answer[1], out.captcha_answer[2], out.captcha_answer[3],
		out.ip[0], out.ip[1], out.ip[2], out.ip[3],
		out.post_id_hash,
		out.num_comments);
}

void test_captcha(){
	string token = gen_token("mypost.md", "192.168.1.1", 3);
	printf("token = %s\n", token.c_str());
	vector<unsigned char> gif = gen_captcha_gif(token);
	ofstream gifstream("./captcha.gif");
	gifstream.write((const char*)gif.data(), gif.size());
	gifstream.close();
	string user_input;
	printf("enter captcha\n");
	cin >> user_input;
	if(validate_captcha("mypost.md", token, user_input, "192.168.1.1")){
		printf("valid token\n");
	} else {
		printf("invalid token\n");
	}
}

void test(){
	test_serialization();
	test_captcha();
}

}