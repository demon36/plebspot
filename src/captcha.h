#ifndef CAPTCHA_H
#define CAPTCHA_H

#include <string>
#include <vector>

namespace captcha{

std::vector<unsigned char> gen_token();
std::vector<unsigned char> gen_gif(const std::vector<unsigned char>& token);
bool validate(const std::vector<unsigned char>& token, const std::string user_input);
void test();

}

#endif
