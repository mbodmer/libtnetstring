#pragma once

#ifdef GTEST
#include <gtest/gtest_prod.h>
#endif


namespace tnetstring {


class Decoder {
public:

	/** CTOR */
	Decoder(std::istream& in)
		: in_(in), current_size_(0), current_size_digits_(0), current_type_('\0') {};

	/** DTOR */
	virtual ~Decoder() {};

	/**
	 * Decodes the netstring in the stream into the given value future
	 *
	 * @throw tnetstring::Parse_exception, boost::bad_lexical_cast
	 * @param value future decoded TNetstring_value
	 */
	void decode(TNetstring_value& value){
		decode_size();
		decode_type();
		decode_value(value);
	}

private:

	/** The input stream all the methods read from */
	std::istream& in_;

	/** Holds the last successfully parsed payload size value */
	int current_size_;

	/** Holds the last successfully parsed payload size digit count */
	int current_size_digits_;

	/** Holds the last successfully parsed payload type character */
	char current_type_;

	/**
	 * Decodes the next size field
	 * Successfully parsed size data will be streamed off the stream.
	 *
	 * @throw tnetstring::Parse_exception, boost::bad_lexical_cast
	 */
	void decode_size() {
		// length of the size field
		int size_length = 0;
		char c;

		// save stream pointer position
		std::streampos streampos = in_.tellg();

		// determine size_length
		for (size_length = 0; size_length < (TNETSTRING_SIZE_MAXLEN+1); size_length++) {
			// position read pointer
			in_.seekg(size_length + streampos);

			// peek the next character
			c = in_.peek();

			// reset to saved pos
			in_.seekg(streampos);

			if (c == TNETSTRING_SIZE_DELIM) {
				// if size delimiter (colon) found
				break;

			} else {
				// check for maximum size length
				if (size_length == TNETSTRING_SIZE_MAXLEN) {
					Parse_exception e =
							create_parse_exception("TNetstring size field is too large");
					e << Parse_pos_info(size_length);
					BOOST_THROW_EXCEPTION(e);

				// check for premature end of stream
				} else if (in_.eof()) {
					in_.clear();
					Parse_exception e =
							create_parse_exception("Premature end of TNetstring");
					e << Parse_pos_info(size_length);
					BOOST_THROW_EXCEPTION(e);

				// check whether character is numeric
				} else if (!std::isdigit(c)) {
					Parse_exception e =
							create_parse_exception("TNetstring size field is not a digit");
					e << Parse_pos_info(size_length);
					e << Parse_char_info(c);
					BOOST_THROW_EXCEPTION(e);
				}
			}
		}

		// stream the size field
		in_.width(size_length);
		std::string size_str;
		in_ >> size_str;

		// extract and discard the size field including the delimiter
		in_.ignore(1);

		// convert to int and save successfully parsed size and digits to class members
		try {
			current_size_ = boost::lexical_cast<int>(size_str);
		} catch (boost::exception& e) {
			e << Error_msg_info("TNetstring size field is not an integer.");
			throw;
		}
		current_size_digits_ = size_length;
	}
#ifdef GTEST
	FRIEND_TEST(Test_tnetstring_value, Decoder_decode_size);
#endif

	/**
	 * Reads and checks the next type character
	 *
	 * @throw tnetstring::Parse_exception
	 */
	void decode_type() {
		// position read pointer to type character position
		// then peek the type and reset the read pointer
		std::streampos streampos = in_.tellg();
		in_.seekg(current_size_ + streampos);
		const int c = in_.peek();
		in_.seekg(streampos);

		// check for premature end of stream
		if (in_.eof()) {
			Parse_exception e = create_parse_exception("Premature end of TNetstring");
			BOOST_THROW_EXCEPTION(e);

		// check peeked character
		} else if (
				// if NOT one of the supported types
				!(c == TNETSTRING_TAG_STRING  || c == TNETSTRING_TAG_INT     ||
				  c == TNETSTRING_TAG_FLOAT   || c == TNETSTRING_TAG_BOOLEAN ||
				  c == TNETSTRING_TAG_NULL    || c == TNETSTRING_TAG_DICT    ||
				  c == TNETSTRING_TAG_LIST))
		{
			// -> illegal netstring.
			Parse_exception e =
					create_parse_exception("Illegal or unsupported TNetstring payload type");
			e << Parse_char_info(c);
			BOOST_THROW_EXCEPTION(e);
		}

		// finally save successfully parsed type
		current_type_ = (char)c;
	}
#ifdef GTEST
	FRIEND_TEST(Test_tnetstring_value, Decoder_decode_type);
#endif

	/**
	 * (Recursively) decodes the payload and the type
	 * Successfully parsed (payload & type) data will extracted from the stream.
	 * Nested TNetstrings will be decoded recursively.
	 *
	 * @throw tnetstring::Parse_exception
	 * @param value future decoded TNetstring_value
	 */
	void decode_value(TNetstring_value& value) {

		// save members for hierarchical calls
		char current_type = current_type_;
		int current_size = current_size_;

		std::string payload_str;

		// stream data into string if datatype is NOT a nested one AND not 0
		if ( !(current_type == TNETSTRING_TAG_DICT || current_type == TNETSTRING_TAG_LIST) )
		{
			if (current_size > 0) {
				in_.width(current_size);
				in_ >> payload_str;
			}

			// extract and discard type character
			in_.ignore(1);
		}

		// check for premature end of stream
		if (in_.eof()) {
			in_.clear();
			// -> illegal netstring, at least the type character should still be in the stream.
			Parse_exception e = create_parse_exception("Premature end of TNetstring");
			BOOST_THROW_EXCEPTION(e);
		}

		// lexical casting to native type
		switch (current_type) {
			case TNETSTRING_TAG_STRING: {
				value = payload_str;
			}; break;

			case TNETSTRING_TAG_INT: {
				try {
					value = boost::lexical_cast<int>(payload_str);
				} catch (boost::exception& e) {
					e << Error_msg_info("TNetstring payload cannot be casted to int");
					e << Parse_value_info(payload_str);
					throw;
				}
			}; break;

			case TNETSTRING_TAG_FLOAT: {
				try {
					value = boost::lexical_cast<double>(payload_str);
				} catch (boost::exception& e) {
					e << Error_msg_info("TNetstring payload cannot be casted to double");
					e << Parse_value_info(payload_str);
					throw;
				}
			}; break;

			case TNETSTRING_TAG_BOOLEAN: {
				if (payload_str == "true") {
					value = true;
				} else {
					value = false;
				}
			}; break;

			case TNETSTRING_TAG_NULL: {
				value = nullptr;
			}; break;

			case TNETSTRING_TAG_DICT: {
				TNetstring_dict dict;
				decode_dict(dict);
				value = dict;  // copy dict into variant
			}; break;

			case TNETSTRING_TAG_LIST: {
				TNetstring_list list;
				decode_list(list);
				value = list;  // copy list into variant
			}; break;

			default:
				// -> unsupported type
				Parse_exception e = create_parse_exception("payload type not implemented");
				e << Parse_value_info(payload_str);
				BOOST_THROW_EXCEPTION(e);
		}
	}
#ifdef GTEST
	FRIEND_TEST(Test_tnetstring_value, Decoder_decode_value);
#endif

	/**
	 * (Recursively) decodes the stream into a TNetstring_list
	 *
	 * @throw tnetstring::Parse_exception
	 * @param list future decoded TNetstring_list
	 */
	void decode_list(TNetstring_list& list) {

		// init loop condition
		int remaining_count = current_size_;

		try {

			while (remaining_count > 1) {

				// find out about size of current value
				decode_size();

				// locally save members for hierarchical calls
				int current_size = current_size_;
				int current_size_digits = current_size_digits_;

				// allocate new value which will be added to the list
				TNetstring_value new_value;

				// (recursively) decode the current value and add the new element to the list
				decode_type();
				decode_value(new_value);
				list.push_back(new_value); // add copy to vector

				// count consumed characters and use to calculate the remaining character count
				remaining_count = remaining_count
						- current_size          // payload length
						- current_size_digits   // size field length
						-2;                     // type & colon characters
			}

		} catch (boost::exception& e) {
			e << Count_info(remaining_count);
			throw;
		}

		// extract and discard type character
		in_.ignore(1);

	}
#ifdef GTEST
	FRIEND_TEST(Test_tnetstring_value, Decoder_decode_list);
#endif

	/**
	 * (Recursively) decodes the stream into a TNetstring_dict
	 */
	void decode_dict(TNetstring_dict& dict) {

		// init loop condition
		int remaining_count = current_size_;

		try {

			while (remaining_count > 1) {

				// KEY
				// --------------------------------

				// find out about size of current KEY value
				decode_size();

				// locally save members for hierarchical calls
				int key_current_size = current_size_;
				int key_current_size_digits = current_size_digits_;

				// allocate new value which will be added to the list
				TNetstring_value new_key;

				// decode the current value and add the new element to the dict
				decode_type();
				decode_value(new_key);

				// check for type, only strings are allowed as key
				if (typeid(std::string) != new_key.type()) {
					Parse_exception e = create_parse_exception("dict key must be of type string");
					BOOST_THROW_EXCEPTION(e);
				}

				// VALUE
				// --------------------------------

				// find out about size of current value
				decode_size();

				// locally save members for hierarchical calls
				int value_current_size = current_size_;
				int value_current_size_digits = current_size_digits_;

				// allocate new value which will be added to the list
				TNetstring_value new_value;

				// decode the current value and add the new element to the dict
				decode_type();
				decode_value(new_value);

				// --------------------------------

				// append to dict
				std::string new_key_str = boost::get<std::string>(new_key);
				dict.insert( std::make_pair( new_key_str , new_value ));  // add copies to map

				// count consumed characters and use to calculate the remaining character count
				remaining_count = remaining_count
						- key_current_size          // KEY payload length
						- value_current_size        // VALUE payload length
						- key_current_size_digits   // KEY size field length
						- value_current_size_digits // VALUE size field length
						-4;                         // type & colon characters
			}

		} catch (boost::exception& e) {
			e << Count_info(remaining_count);
			throw;
		}

		// extract and discard type character
		in_.ignore(1);

	}
#ifdef GTEST
	FRIEND_TEST(Test_tnetstring_value, Decoder_decode_dict);
#endif

	/**
	 * Utility methode to create new exceptions and fill them with
	 * default error_info data.
	 */
	tnetstring::Parse_exception create_parse_exception(const std::string& error_msg) {
		Parse_exception e = Parse_exception();
		e << Error_msg_info(error_msg);
		return e;
	}

};



} // ::tnetstring
