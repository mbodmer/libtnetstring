#pragma once

#include <exception>
#include <boost/exception/all.hpp>

namespace tnetstring {

struct Exception : virtual std::exception, virtual boost::exception {};

struct Parse_exception : virtual Exception {};

typedef boost::error_info<struct tag_error_msg, std::string> Error_msg_info;

typedef boost::error_info<struct tag_count, int> Count_info;

typedef boost::error_info<struct tag_size, int> Size_info;

typedef boost::error_info<struct tag_parse_pos, int> Parse_pos_info;

typedef boost::error_info<struct tag_parse_char, char> Parse_char_info;

typedef boost::error_info<struct tag_parse_value, std::string> Parse_value_info;

} // ::tnetstring

