#pragma once

#include <sstream>
#include <boost/lexical_cast.hpp>

namespace tnetstring {

/**
 * Encoder.
 * Internally used to encode TNetstrings to streams.
 */
class Encoder : public boost::static_visitor<> {

public:
	/** CTOR */
	Encoder(std::ostream& outstream): outstream_(outstream) {}

	/** DTOR */
	virtual ~Encoder() {}

	/** String netstring encoding */
	void operator()(const std::string& str_val) const {
		outstream_tnetstring(str_val, TNETSTRING_TAG_STRING);
	}

	/** Cstring / nullptr netstring encoding */
	void operator()(const char* cstring_val) const {
		if (cstring_val != nullptr) {
			const std::string msg = cstring_val;
			outstream_tnetstring(msg, TNETSTRING_TAG_STRING);
		} else {
			outstream_ << TNETSTRING_NULL;
		}
	}

	/** Integer netstring encoding */
	void operator()(const int& int_val) const {
		try {
			const std::string msg = boost::lexical_cast<std::string>(int_val);
			outstream_tnetstring(msg, TNETSTRING_TAG_INT);
		} catch (boost::bad_lexical_cast&) {
			outstream_ << TNETSTRING_EMPTY;
		}
	}

	/** Double netstring encoding */
	void operator()(const double& double_val) const {
		try {
			const std::string msg = boost::lexical_cast<std::string>(double_val);
			outstream_tnetstring(msg, TNETSTRING_TAG_FLOAT);
		} catch (boost::bad_lexical_cast&) {
			outstream_ << TNETSTRING_EMPTY;
		}
	}

	/** Boolean netstring encoding */
	void operator()(const bool& bool_val) const {
		std::string msg;
		bool_val ? msg = "true" : msg = "false";
		outstream_tnetstring(msg, TNETSTRING_TAG_BOOLEAN);
	}

	/** List netstring encoding */
	void operator()(const TNetstring_list& list_val) const {
		std::ostringstream os;

		for (auto i: list_val) {
			boost::apply_visitor(Encoder(os), i);
		}

		outstream_tnetstring(os.str(), TNETSTRING_TAG_LIST);
	}

	/** Dict netstring encoding */
	void operator()(const TNetstring_dict& dict_val) const {
		std::ostringstream os;

		for (auto i: dict_val) {
			TNetstring_value key = i.first;
			boost::apply_visitor(Encoder(os), key);
			boost::apply_visitor(Encoder(os), i.second);
		}

		outstream_tnetstring(os.str(), TNETSTRING_TAG_DICT);
	}

private:
	/** Output stream */
	std::ostream& outstream_;

	/** Outputs a Netstring encoded message to outstream */
	void outstream_tnetstring(const std::string& msg, const char tag) const {
		try {
			// get length of the msg
			int len = msg.size();

			// limit to TNETSTRING_MAXSIZE (cut data after MAXSIZE)
			if (TNETSTRING_DATA_MAXLEN < len) {
				// limit length
				len = TNETSTRING_DATA_MAXLEN;
			}

			// cast actual length to string
			std::string length = boost::lexical_cast<std::string>(len);

			// write netstring to output stream
			outstream_ << length << TNETSTRING_SIZE_DELIM << msg.substr(0,len) << tag;

		} catch (boost::bad_lexical_cast&) {
			// return nothing (not NULL)
			outstream_ << TNETSTRING_NULL;
		}
	}
};

} // ::tnetstring

