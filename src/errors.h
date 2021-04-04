#ifndef ERRORS_H
#define ERRORS_H

#include <vector>
#include <string>

namespace util {

typedef char error;

#define OUTCOME_ERR_CHECK(o) if(!o.is_success()) return o.get_error()

namespace errors{
	
static char counter = 0;
static std::vector<std::string> names;
static char add_error_def(const std::string& value){
	names.push_back(value);
	return counter++;
}

#define _UTIL_DEFINE_ERROR(name) const static error name = add_error_def(#name)

_UTIL_DEFINE_ERROR(success);
_UTIL_DEFINE_ERROR(comments_disabled);
_UTIL_DEFINE_ERROR(comment_post_not_exist);
_UTIL_DEFINE_ERROR(comment_message_not_provided);
_UTIL_DEFINE_ERROR(comment_author_not_provided);
_UTIL_DEFINE_ERROR(comment_message_too_large);
_UTIL_DEFINE_ERROR(captcha_answer_not_provided);
_UTIL_DEFINE_ERROR(captcha_wrong_answer);
_UTIL_DEFINE_ERROR(captcha_expired);
_UTIL_DEFINE_ERROR(file_missing);
_UTIL_DEFINE_ERROR(failed_to_listen);
_UTIL_DEFINE_ERROR(markdown_metadata_bad_format);

inline std::string to_string(error e){
	return names[e];
}

}

	
}

#endif