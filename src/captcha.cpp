#include "captcha.h"

#include <fmt/core.h>
#include <plusaes/plusaes.hpp>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <base64.h>
#include <captcha.h>

using namespace std;

namespace captcha{

const std::vector<unsigned char> AES_KEY = plusaes::key_from_string(&"SZHRLWIUOBR2BXYC");
const unsigned char AES_IV[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
const unsigned long CAPTCHA_LEN = 6;//including null terminator
const unsigned long CAPTCHA_AES_PADDED_LEN = plusaes::get_padded_encrypted_size(CAPTCHA_LEN);

std::string gen_token(){
	int num_letters = 26;
	array<unsigned char, 6> random_ids;
	srand((unsigned) time(NULL) * getpid());
	for (size_t i = 0; i < random_ids.size()-1; ++i){
		random_ids[i] = (unsigned char)(rand() % (num_letters-1));
	}
	random_ids[5] = 0;
	vector<unsigned char> encrypted(CAPTCHA_AES_PADDED_LEN);
	plusaes::encrypt_cbc((unsigned char*)random_ids.data(), random_ids.size(), &AES_KEY[0], AES_KEY.size(), &AES_IV, &encrypted[0], encrypted.size(), true);
	return base64_encode(string((char*)encrypted.data(), encrypted.size()), true);
}

std::vector<unsigned char> gen_gif(const std::string& token){
	unsigned long padded_size = 0;
	string decoded_token = base64_decode(token);
	vector<unsigned char> decrypted(CAPTCHA_AES_PADDED_LEN);
	plusaes::Error err = plusaes::decrypt_cbc((unsigned char*)decoded_token.c_str(), decoded_token.size(), &AES_KEY[0], AES_KEY.size(), &AES_IV, &decrypted[0], decrypted.size(), &padded_size);
	if(err != plusaes::kErrorOk){
		return vector<unsigned char>();
	}
	cout<<"decrypted = "<<(char*)decrypted.data()<<endl;
	std::vector<unsigned char> gif(gifsize);
	captcha_for_letters(gif.data(), (unsigned char*)decrypted.data());
	return gif;
}

bool validate(const std::string& token, const std::string& user_input){
	unsigned long padded_size = 0;
	string decoded_token = base64_decode(token);
	vector<unsigned char> decrypted(CAPTCHA_AES_PADDED_LEN);
	plusaes::decrypt_cbc((unsigned char*)decoded_token.c_str(), decoded_token.size(), &AES_KEY[0], AES_KEY.size(), &AES_IV, &decrypted[0], decrypted.size(), &padded_size);
	translate_letter_ids((unsigned char*)decrypted.data());
	decrypted.resize(CAPTCHA_LEN);
	return user_input == string((char*)decrypted.data());
}

void test(){
	string token = captcha::gen_token();
	printf("token = %s\n", token.c_str());
	vector<unsigned char> gif = captcha::gen_gif(token);
	ofstream gifstream("./captcha.gif");
	gifstream.write((const char*)gif.data(), gif.size());
	gifstream.close();
	string user_input;
	printf("enter captcha\n");
	cin >> user_input;
	if(captcha::validate(token, user_input)){
		printf("valid token\n");
	} else {
		printf("invalid token\n");
	}
}

}