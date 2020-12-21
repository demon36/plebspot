#ifndef CAPTCHA_H
#define CAPTCHA_H

#include <string>
#include <vector>

namespace captcha{

std::string gen_token();
std::vector<unsigned char> gen_gif(const std::string& token);
bool validate(const std::string& token, const std::string user_input);
void test();

}

#endif
