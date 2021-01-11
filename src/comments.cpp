#include "comments.h"
#include "util.h"

#include <cstdio>
#include <fstream>
#include <mustache.hpp>
#include <fmt/core.h>
#include <plusaes/plusaes.hpp>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <base64.h>
#include <captcha.h>
#include <filesystem>

using namespace std;

namespace comments{

const std::vector<unsigned char> AES_KEY = plusaes::key_from_string(&"SZHRLWIUOBR2BXYC");
const unsigned char AES_IV[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

const unsigned long CAPTCHA_AES_PADDED_LEN = plusaes::get_padded_encrypted_size(sizeof(token));
const string COMMENTS_FILE_SUFFIX = ".comments";
const size_t MAX_COMMENT_MESSAGE_LENGTH = 256;

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
	token tok;
	int num_letters = 26;
	srand((unsigned) time(NULL) * getpid());
	
	//set captcha answer
	for (size_t i = 0; i < CAPTCHA_LEN-1; ++i){
		tok.captcha_answer[i] = (unsigned char)(rand() % (num_letters-1));
	}
	tok.captcha_answer[5] = 0;

	//ip
	sscanf(ip.c_str(), "%hhu.%hhu.%hhu.%hhu", &tok.ip[0], &tok.ip[1], &tok.ip[2], &tok.ip[3]);

	//post path hash & num comments
	tok.post_id_hash = hash<string>{}(post_path);
	tok.num_comments = num_comments;

	return serialize_token(tok);
}

std::vector<unsigned char> gen_captcha_gif(const std::string& token_str){
	token tok = deserialize_token(token_str);
	std::vector<unsigned char> gif(gifsize);
	captcha_for_letters(gif.data(), &tok.captcha_answer[0]);
	return gif;
}

bool validate_captcha(const std::string& token_str, const std::string& user_input){
	token tok = deserialize_token(token_str);
	translate_letter_ids(&tok.captcha_answer[0]);
	decrypted.resize(CAPTCHA_LEN);
	return user_input == string(&tok.captcha_answer[0]);
}

err::errors post_comment(const string& post_path, const comments::comment& comment, const string& token, const string& captcha_answer){
//todo: assert comments are enabled
//todo: only accept token if num comments is incremented by less than ~5
//todo: store ip, post id, num comments in captcha token
//todo: store comment author and date
	std::error_code err_code;
	if(!filesystem::exists(post_path)){
		return err::errors::comment_post_not_exist;
	}

	if(util::trim(comment.author).empty()){
		return err::errors::comment_author_not_provided;	
	}

	if(util::trim(comment.message).empty()){
		return err::errors::comment_message_not_provided;
	}

	if(util::trim(comment.message).size() > MAX_COMMENT_MESSAGE_LENGTH){
		return err::errors::comment_message_too_large;
	}

	if(util::trim(captcha_answer).empty()){
		return err::errors::captcha_answer_not_provided;
	}

	if(!validate_captcha(token, captcha_answer)){
		return err::errors::captcha_wrong_answer;
	}

	string comments_file_path = post_path + COMMENTS_FILE_SUFFIX;
	ofstream comments_file(comments_file_path, std::ios::app);
	comments_file << kainjow::mustache::html_escape(comment.message) << "\n";
	comments_file.flush();

	return err::errors::success;
}

vector<comment> get_comments(const string& post_path){
	vector<comment> comments;
	string comments_file_path = post_path + COMMENTS_FILE_SUFFIX;
	ifstream comments_file(comments_file_path, std::ios::in);
	for( std::string line; getline( comments_file, line ); ) {
		comments.emplace_back(comment{"anonymous", "30-9-1993", line});
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
	printf("token.captcha_answer = %c%c%c%c, token.ip = %hhu.%hhu.%hhu.%hhu, token.post_id_hash = %llu, token.num_comments = %llu\n", 
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
	if(validate_captcha(token, user_input)){
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