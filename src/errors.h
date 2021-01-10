#ifndef ERRORS_H
#define ERRORS_H

namespace err {

enum class errors {
	success,
	comment_post_not_exist,
	comment_message_not_provided,
	comment_author_not_provided,
	comment_message_too_large,
	captcha_answer_not_provided,
	captcha_wrong_answer,
	captcha_expired,
};

inline std::string to_string(errors e){
	switch(e) {
		case errors::success:
			return "success";
		case errors::comment_post_not_exist:
			return "post does not exist";
		case errors::comment_message_not_provided:
			return "comment not provided";
		case errors::comment_author_not_provided:
			return "comment author not provided";
		case errors::comment_message_too_large:
			return "comment length too large";
		case errors::captcha_answer_not_provided:
			return "captcha answer not provided";
		case errors::captcha_wrong_answer:
			return "wrong captcha answer";
		case errors::captcha_expired:
			return "captcha expired";
		default:
			return "";
	}
}

}

#endif