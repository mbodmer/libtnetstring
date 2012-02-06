#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <boost/variant.hpp>


namespace tnetstring {

/**
 * TNetstring value implemented as a recursive boost::variant.
 *
 * Access the variant with:   boost::get<T>(tns_value)
 * Check for type with:       tns_value.type() == typeid(T)
 */
typedef boost::make_recursive_variant<
	const char*
	, int
	, std::string
	, double
	, bool
	, std::vector<boost::recursive_variant_>
	, std::unordered_map<std::string, boost::recursive_variant_>
>::type TNetstring_value;

/**
 * TNetstring list, used to create (possibly recursive) lists of TNetstring_values.
 */
typedef std::vector<TNetstring_value> TNetstring_list;

/**
 * TNetstring dictionary, used to create (possibly recursive) dictionaries of TNetstring_values.
 */
typedef std::unordered_map<std::string, TNetstring_value> TNetstring_dict;

}  // ::tnetstring

