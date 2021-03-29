#include "config.h"

#include <fstream>
#include <fmt/core.h>
#include "util.h"

using namespace std;
using namespace util;

config config::fields;

util::error config::load(){
	outcome<string> file_contents = util::get_file_contents(CONFIG_FILE);
	OUTCOME_ERR_CHECK(file_contents);
	map<string, string> config_file_items = util::parse_pairs(file_contents.get_result());

	for(auto& field : fields.text_fields) {
		if(config_file_items.find(field.first) != config_file_items.end()){
			field.second.assign(util::trim(config_file_items[field.first]));
			fmt::print("config::params.{} = {}\n", field.first, field.second);
		}
	}

	for(auto& field : fields.int_fields) {
		if(config_file_items.find(field.first) != config_file_items.end()){
			field.second = atoi(config_file_items[field.first].c_str());//todo: error check
			fmt::print("config::params.{} = {}\n", field.first, field.second);
		}
	}

	for(auto& field : fields.bool_fields) {
		if(config_file_items.find(field.first) != config_file_items.end()){
			field.second = config_file_items[field.first]  == "true";
			fmt::print("config::params.{} = {}\n", field.first, field.second);
		}
	}

	return errors::success;
}
