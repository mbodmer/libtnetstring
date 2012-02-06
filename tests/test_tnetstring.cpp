
//          Copyright Marc Bodmer 2011-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "tnetstring/all.hpp"

//#define DISABLE_LONGRUN_TESTS

namespace tnetstring {

/**
 * FIXTURE for Test_tnetstring_value
 */
class Test_tnetstring_value : public testing::Test {
protected:

	// outputstream to "send" the encoded netstrings to (test output)
	std::ostringstream os_;

	// input stringstream to "receive" netstrings to decode (test input)
	std::istringstream is_;

	// TNetstring value variant, Object under test
	TNetstring_value tns_var_;

	virtual void SetUp() {
		//empty
	}

	virtual void TearDown() {
		//empty
	}
};



/**
 * TEST TNetstring_value encoding, using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Encoder) {
	try {

	// put sample data into variant (int)
	// apply encoding visitor to variant and stream result to buffer
	// check resulting buffer content and clear buffer for next test
	tns_var_ = 12345;
	os_ << tns_var_;
	EXPECT_EQ("5:12345#",os_.str()) << "Encoded integer netstring is incorrect";
	os_.str("");

	// put sample data into variant (double), reapply visitor, check
	tns_var_ = 12.345;
	os_ << tns_var_;
	EXPECT_EQ("18:12.345000",os_.str().substr(0,12)) << "Encoded double netstring is incorrect";
	EXPECT_EQ("^",os_.str().substr(21,1)) << "Encoded double netstring has wrong type tag";
	os_.str("");

	// put sample data into variant (float), reapply visitor, check
	tns_var_ = 12.345f;
	os_ << tns_var_;
	EXPECT_EQ("18:12.345000",os_.str().substr(0,12)) << "Encoded float netstring is incorrect";
	EXPECT_EQ("^",os_.str().substr(21,1)) << "Encoded float netstring has wrong type tag";
	os_.str("");

	// put sample data into variant (bool), reapply visitor, check
	tns_var_ = false;
	os_ << tns_var_;
	EXPECT_EQ("5:false!",os_.str()) << "Encoded boolean netstring is incorrect";
	os_.str("");

	// put sample data into variant (null), reapply visitor, check
	tns_var_ = nullptr;
	os_ << tns_var_;
	EXPECT_EQ("0:~",os_.str()) << "Encoded cstring netstring is incorrect";
	os_.str("");

	// put sample data into variant (empty string), reapply visitor, check
	tns_var_ = "";
	os_ << tns_var_;
	EXPECT_EQ("0:,",os_.str()) << "Encoded cstring netstring is incorrect";
	os_.str("");

	// put sample data into variant cstring, reapply visitor, check
	tns_var_ = "HelloDuHelloDuHelloDu";
	os_ << tns_var_;
	EXPECT_EQ("21:HelloDuHelloDuHelloDu,",os_.str()) << "Encoded cstring netstring is incorrect";
	os_.str("");

	// put sample data into variant (char[21]), reapply visitor, check
	tns_var_ = std::string("HelloDuHelloDuHelloDu");
	os_ << tns_var_;
	EXPECT_EQ("21:HelloDuHelloDuHelloDu,",os_.str()) << "Encoded string netstring is incorrect";
	os_.str("");

	// put sample data into variant (list), reapply visitor, check
	TNetstring_list list {"Hello", 123, 1.23, true};
	tns_var_ = list;
	os_ << tns_var_;
	EXPECT_EQ("28:5:Hello,3:123#4:1.23^4:true!]",os_.str()) << "Encoded list netstring is incorrect";
	os_.str("");

	// put sample data into variant (dict), reapply visitor, check
	TNetstring_dict dict {{"key1","Hello"}, {"key2",123}, {"key3",1.23}, {"key4",true}};
	tns_var_ = dict;
	os_ << tns_var_;
	EXPECT_EQ("56:4:key2,3:123#4:key1,5:Hello,4:key4,4:true!4:key3,4:1.23^}",os_.str()) << "Encoded dictionary netstring is incorrect";
	os_.str("");

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}

/**
 * TEST TNetstring_value encoding, using fixture Test_tnetstring_value, long running tests
 */
#ifdef DISABLE_LONGRUN_TESTS
TEST_F(Test_tnetstring_value, DISABLED_TNetstring_encoder_longrun) {
#else
TEST_F(Test_tnetstring_value, TNetstring_encoder_longrun) {
#endif
	try {

	// create string greater than 999999999 characters, reapply visior, check
	std::string append_str = "abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij";
	std::string str(append_str);
	for (int i=0;i<tnetstring::TNETSTRING_DATA_MAXLEN/100;i++) {
		// append 100 characters
		str.append(append_str);
	} // 100 * 9999999 + 100 = 999999999 + 1
	tns_var_ = str;
	ASSERT_EQ(tnetstring::TNETSTRING_DATA_MAXLEN+1, str.size()) << "Test input string has incorrect length";
	os_ << tns_var_;
	// 9 length digits + ':' + ',' = additional 11 characters for encoding
	EXPECT_EQ(tnetstring::TNETSTRING_DATA_MAXLEN+11, os_.str().size()) << "Too long string is not truncated";
	os_.str("");

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}


/**
 * TNetstring size decoding, using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Decoder_decode_size) {
	try {
	// initialize decoder
	tnetstring::Decoder decoder(is_);

	// single character size
	is_.str("5:12345#asdf");
	decoder.decode_size();
	EXPECT_EQ(5, decoder.current_size_) << "parsed size does not match";

	// medium length size
	is_.str("543:12345#asdf");
	decoder.decode_size();
	EXPECT_EQ(543,decoder.current_size_) << "parsed size does not match";

	// maximum length size
	is_.str("123456789:12345#asdf");
	decoder.decode_size();
	EXPECT_EQ(123456789,decoder.current_size_) << "parsed size does not match";

	// maximum+1 length size
	is_.str("1234567890:12345#asdf");
	EXPECT_THROW(decoder.decode_size(), Parse_exception) << "invalid netstring not recognized";

	// long string without colon
	is_.str("1234567890123445");
	EXPECT_THROW(decoder.decode_size(), Parse_exception) << "invalid netstring not recognized";

	// premature end of tnetstring
	is_.str("1");
	EXPECT_THROW(decoder.decode_size(), Parse_exception) << "invalid netstring not recognized";

	// no int
	is_.str("a:12345#asdf");
	EXPECT_THROW(decoder.decode_size(), Parse_exception) << "invalid netstring not recognized";

	// zero size
	is_.str("0:12345#asdf");
	decoder.decode_size();
	EXPECT_EQ(0, decoder.current_size_) << "parsed size does not match";

	// empty netstring
	// zero size
	is_.str("");
	EXPECT_THROW(decoder.decode_size(), Parse_exception) << "invalid netstring not recognized";

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}



/**
 * TNetstring payload type decoding, using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Decoder_decode_type) {
	try {
	// initialize decoder
	tnetstring::Decoder decoder(is_);

	// basic functionality
	is_.str("12345#asdf");
	decoder.current_size_ = 5; decoder.current_size_digits_ = 1;
	decoder.decode_type();
	EXPECT_EQ('#', decoder.current_type_) << "decoded type does not match";

	// premature end of stream
	is_.str("123");
	decoder.current_size_ = 5; decoder.current_size_digits_ = 1;
	EXPECT_THROW(decoder.decode_type() , Parse_exception) << "invalid netstring not recognized";

	// empty stream
	is_.str("");
	decoder.current_size_ = 0; decoder.current_size_digits_ = 1;
	EXPECT_THROW(decoder.decode_type() , Parse_exception) << "invalid netstring not recognized";

	// unsupported type
	is_.str("12345?asdf");
	decoder.current_size_ = 5; decoder.current_size_digits_ = 1;
	EXPECT_THROW(decoder.decode_type() , Parse_exception) << "invalid netstring not recognized";

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}



/**
 * TNetstring payload decoding, using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Decoder_decode_value) {
	try {
	// initialize decoder
	tnetstring::Decoder decoder(is_);
	std::string remains;

	// int
	is_.str("12345#asdf");
	decoder.current_size_ = 5; decoder.current_size_digits_ = 1; decoder.current_type_ = '#';
	decoder.decode_value(tns_var_);
	ASSERT_EQ(typeid(int), tns_var_.type()) << "Decoded netstring value is not an integer";
	EXPECT_EQ(12345, boost::get<int>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// float (double)
	is_.str("12.3450^asdf");
	decoder.current_size_ = 7; decoder.current_size_digits_ = 1; decoder.current_type_ = '^';
	decoder.decode_value(tns_var_);
	ASSERT_EQ(typeid(double), tns_var_.type()) << "Decoded netstring value is not a double";
	EXPECT_EQ(12.3450, boost::get<double>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// string
	is_.str("abcdefghijklmnopqrstuvwxyz,asdf");
	decoder.current_size_ = 26; decoder.current_size_digits_ = 2; decoder.current_type_ = ',';
	decoder.decode_value(tns_var_);
	ASSERT_EQ(typeid(std::string), tns_var_.type()) << "Decoded netstring value is not a string";
	EXPECT_EQ("abcdefghijklmnopqrstuvwxyz", boost::get<std::string>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// bool
	is_.str("true!asdf");
	decoder.current_size_ = 4; decoder.current_size_digits_ = 1; decoder.current_type_ = '!';
	decoder.decode_value(tns_var_);
	ASSERT_EQ(typeid(bool), tns_var_.type()) << "Decoded netstring value is not a bool";
	EXPECT_EQ(true, boost::get<bool>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// blank
	is_.str("~asdf");
	decoder.current_size_ = 0; decoder.current_size_digits_ = 1; decoder.current_type_ = '~';
	decoder.decode_value(tns_var_);
	ASSERT_EQ(typeid(const char*), tns_var_.type()) << "Decoded netstring value is not a blank";
	EXPECT_EQ(nullptr, boost::get<const char*>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// empty string
	is_.str(",asdf");
	decoder.current_size_ = 0; decoder.current_size_digits_ = 1; decoder.current_type_ = ',';
	decoder.decode_value(tns_var_);
	ASSERT_EQ(typeid(std::string), tns_var_.type()) << "Decoded netstring value is not a string";
	EXPECT_EQ("", boost::get<std::string>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// premature end
	is_.str("ab");
	decoder.current_size_ = 3; decoder.current_size_digits_ = 1; decoder.current_type_ = ',';
	EXPECT_THROW(decoder.decode_value(tns_var_) , Parse_exception) << "invalid netstring not recognized";

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}



/**
 * TNetstring list decoding, using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Decoder_decode_list) {
	try {
	// initializations
	Decoder decoder(is_);
	TNetstring_list result_list;
	TNetstring_list result_list2;
	TNetstring_list result_list3;
	std::string remains;

	// simple list
	// -----------------------------

	// prepare expected string for roundtrip
	TNetstring_list expected_list {"Hello", 123, 1.23, true};
	std::ostringstream expected_buffer;
	expected_buffer << expected_list;

	// decode test netstring (without leading size field)
	is_.str("5:Hello,3:123#4:1.23^4:true!]asdf");
	decoder.current_size_ = 26; decoder.current_size_digits_ = 2;
	decoder.decode_list(result_list);
	tns_var_ = result_list;

	// prepare result string for comparison
	std::ostringstream result_buffer;
	result_buffer << tns_var_;
	EXPECT_EQ(expected_buffer.str(), result_buffer.str()) << "decoded payload does not match";

	// check remainings
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// nested list
	// -----------------------------
	// prepare multiple times nested list
	expected_buffer.str("");
	expected_list = {"Hello2", 123, 1.23, true, expected_list};
	expected_list = {"Hello3", 123, 1.23, true, expected_list};
	expected_buffer << expected_list;

	// decode test netstring (without leading size field)
	is_.str("6:Hello3,3:123#4:1.23^4:true!61:6:Hello2,3:123#4:1.23^4:true!28:5:Hello,3:123#4:1.23^4:true!]]]asdf");
	decoder.current_size_ = 94; decoder.current_size_digits_ = 2;
	decoder.decode_list(result_list2);
	tns_var_ = result_list2;

	// prepare result string for comparison
	result_buffer.str("");
	result_buffer << tns_var_;
	EXPECT_EQ(expected_buffer.str(), result_buffer.str()) << "decoded payload does not match";

	// check remainings
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// empty list
	// -----------------------------

	// prepare expected string for roundtrip
	TNetstring_list expected_emptylist {};
	expected_buffer.str("");
	expected_buffer << expected_emptylist;

	// decode test netstring (without leading size field)
	is_.str("]asdf");
	decoder.current_size_ = 0; decoder.current_size_digits_ = 1;
	decoder.decode_list(result_list3);
	tns_var_ = result_list3;

	// prepare result string for comparison
	result_buffer.str("");
	result_buffer << tns_var_;
	EXPECT_EQ(expected_buffer.str(), result_buffer.str()) << "decoded payload does not match";

	// check remainings
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}



/**
 * TNetstring dict decoding, using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Decoder_decode_dict) {
	try {
	// initializations
	Decoder decoder(is_);
	TNetstring_dict result_dict;
	TNetstring_dict result_dict2;
	std::string remains;

	// simple dict
	// -----------------------------

	// prepare expected dictionnary
	TNetstring_dict expected_dict {{"key1","Hello"}, {"key2",123}, {"key3",1.23}, {"key4",true}};

	// decode test netstring (without leading size field)
	is_.str("4:key3,4:1.23^4:key2,3:123#4:key4,4:true!4:key1,5:Hello,}asdf");
	decoder.current_size_ = 56; decoder.current_size_digits_ = 2;
	decoder.decode_dict(result_dict);

	// prepare result string for comparison
	std::ostringstream expected, result;
	expected << expected_dict["key1"] << expected_dict["key2"] << expected_dict["key3"] << expected_dict["key4"];
	result   << result_dict["key1"] << result_dict["key2"] << result_dict["key3"] << result_dict["key4"];
	EXPECT_EQ(expected.str(), result.str()) << "decoded payload does not match";

	// check remainings
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// nested dict
	// -----------------------------

	TNetstring_dict expected_dict2 {{"key5","Hello"}, {"key6",123}, {"key7",1.23}, {"key8",true}, {"key9", expected_dict}};

	// decode test netstring (without leading size field)
	is_.str("4:key7,4:1.23^4:key6,3:123#4:key8,4:true!4:key5,5:Hello,4:key9,56:4:key3,4:1.23^4:key2,3:123#4:key4,4:true!4:key1,5:Hello,}}asdf");
	decoder.current_size_ = 123; decoder.current_size_digits_ = 3;
	decoder.decode_dict(result_dict2);

	// prepare result string for comparison
	expected.str(""); result.str("");
	TNetstring_dict expected_key9, result_key9;

	expected_key9 = boost::get<TNetstring_dict>(expected_dict2["key9"]);
	result_key9  =  boost::get<TNetstring_dict>(result_dict2["key9"]);

	expected << expected_dict2["key5"] << expected_dict2["key6"] << expected_dict2["key7"] << expected_dict2["key8"] <<
			expected_key9["key1"] << expected_key9["key2"] << expected_key9["key3"] << expected_key9["key4"];

	result  <<  result_dict2["key5"] << result_dict2["key6"] << result_dict2["key7"] << result_dict2["key8"] <<
			result_key9["key1"] << result_key9["key2"] << result_key9["key3"] << result_key9["key4"];

	EXPECT_EQ(expected.str(), result.str()) << "decoded payload does not match";

	// check remainings
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}



/**
 * TNetstring_value decoding (size), using fixture Test_tnetstring_value
 */
TEST_F(Test_tnetstring_value, Decoder) {
	try {
	std::string remains;

	// int
	is_.str("5:12345#asdf");
	is_ >> tns_var_;
	ASSERT_EQ(typeid(int), tns_var_.type()) << "Decoded netstring value is not an integer";
	EXPECT_EQ(12345, boost::get<int>(tns_var_)) << "Decoded netstring integer is incorrect";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// float (double)
	is_.str("7:12.3450^asdf");
	is_ >> tns_var_;
	ASSERT_EQ(typeid(double), tns_var_.type()) << "Decoded netstring value is not a double";
	EXPECT_EQ(12.345, boost::get<double>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// string
	is_.str("26:abcdefghijklmnopqrstuvwxyz,asdf");
	is_ >> tns_var_;
	ASSERT_EQ(typeid(std::string), tns_var_.type()) << "Decoded netstring value is not a string";
	EXPECT_EQ("abcdefghijklmnopqrstuvwxyz", boost::get<std::string>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// bool
	is_.str("4:true!asdf");
	is_ >> tns_var_;
	ASSERT_EQ(typeid(bool), tns_var_.type()) << "Decoded netstring value is not a bool";
	EXPECT_EQ(true, boost::get<bool>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// blank
	is_.str("0:~asdf");
	is_ >> tns_var_;
	ASSERT_EQ(typeid(const char*), tns_var_.type()) << "Decoded netstring value is not a blank";
	EXPECT_EQ(nullptr, boost::get<const char*>(tns_var_)) << "decoded payload does not match";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// list (nested)
	TNetstring_list expected_list {"Hello", 123, 1.23, true};
	expected_list = {"Hello2", 123, 1.23, true, expected_list};
	expected_list = {"Hello3", 123, 1.23, true, expected_list};
	std::ostringstream expected;
	expected << expected_list;

	is_.str("94:6:Hello3,3:123#4:1.23^4:true!61:6:Hello2,3:123#4:1.23^4:true!28:5:Hello,3:123#4:1.23^4:true!]]]asdf");
	is_ >> tns_var_;
	std::ostringstream result;
	result << tns_var_;

	ASSERT_EQ(typeid(TNetstring_list), tns_var_.type()) << "Decoded netstring value is not a blank";
	EXPECT_EQ(expected.str(), result.str()) << "decoded payload does not match";

	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	// dict(nested)
	TNetstring_dict expected_dict {{"key1","Hello"}, {"key2",123}, {"key3",1.23}, {"key4",true}};
	expected_dict = {{"key5","Hello"}, {"key6",123}, {"key7",1.23}, {"key8",true}, {"key9", expected_dict}};

	is_.str("123:4:key7,4:1.23^4:key6,3:123#4:key8,4:true!4:key5,5:Hello,4:key9,56:4:key3,4:1.23^4:key2,3:123#4:key4,4:true!4:key1,5:Hello,}}asdf");
	is_ >> tns_var_;

	expected.str(""); result.str("");
	TNetstring_dict expected_key9, result_key9;

	TNetstring_dict result_dict = boost::get<TNetstring_dict>(tns_var_);

	expected_key9 = boost::get<TNetstring_dict>(expected_dict["key9"]);
	result_key9  =  boost::get<TNetstring_dict>(result_dict["key9"]);

	expected << expected_dict["key5"] << expected_dict["key6"] << expected_dict["key7"] << expected_dict["key8"] <<
			expected_key9["key1"] << expected_key9["key2"] << expected_key9["key3"] << expected_key9["key4"];

	result  <<  result_dict["key5"] << result_dict["key6"] << result_dict["key7"] << result_dict["key8"] <<
			result_key9["key1"] << result_key9["key2"] << result_key9["key3"] << result_key9["key4"];

	EXPECT_EQ(expected.str(), result.str()) << "decoded payload does not match";

	// check remainings
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

    } catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}


/**
 * TNetstring payload decoding, using fixture Test_tnetstring_value, long running tests
 */
#ifdef DISABLE_LONGRUN_TESTS
TEST_F(Test_tnetstring_value, DISABLED_Decoder_longrun) {
#else
TEST_F(Test_tnetstring_value, Decoder_longrun) {
#endif
	try {
	std::string remains;

	// create maxlen string with 999999999 characters
	std::string append_str = "abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghi,";
	std::string str(append_str);
	for (int i=0;i<tnetstring::TNETSTRING_DATA_MAXLEN/100;i++) {
		// append 100 characters
		str.append(append_str);
	} // 100 * 9999999 + 99 + ","
	std::string teststr = "999999999:";
	teststr.append(str);
	teststr.append("asdf");
	is_.str(teststr);
	is_ >> tns_var_;
	ASSERT_EQ(typeid(std::string), tns_var_.type()) << "Decoded netstring value is not a string";
	EXPECT_EQ(999999999, boost::get<std::string>(tns_var_).size()) << "decoded payload has wrong size";
	is_ >> remains;
	EXPECT_EQ("asdf", remains) << "Stream remainings are wrong";
	is_.clear(); // ... we read until eos

	} catch (...) {
		std::cerr << boost::current_exception_diagnostic_information();
		FAIL();
	}
}



/**
 * TNetstring_value performance, using fixture Test_tnetstring_value
 */
#ifdef DISABLE_LONGRUN_TESTS
TEST_F(Test_tnetstring_value, DISABLED_TNetstring_performance_longrun) {
#else
TEST_F(Test_tnetstring_value, TNetstring_performance_longrun) {
#endif

	std::string test_str = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	std::string test_netstr = "78:abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz,";

	// repeat a typical string encoding for 1 million times and measure time
	for (int i=0; i<1000000; i++) {
		tns_var_ = test_str;
		os_ << tns_var_;
		EXPECT_EQ(test_netstr,os_.str());
		os_.str("");
	}

	// repeat a typical string decoding for 1 million times and measure time
	for (int i=0; i<1000000; i++) {
		is_.str(test_netstr);
		is_ >> tns_var_;
		EXPECT_EQ(test_str, boost::get<std::string>(tns_var_));
	}
}


} // ::tnetstring


