#pragma once

namespace tnetstring {

/**
 * Output stream operator for TNetstring_value
 * Encodes TNetstring_value(s) as Netstring into an output stream
 */
std::ostream& operator<<(std::ostream& outstream, const tnetstring::TNetstring_value& value) {
	tnetstring::Encoder encoder(outstream);
	boost::apply_visitor(encoder, value);
	return outstream;
}

/**
 * Input stream operator for TNetstring_value
 * Decodes an input stream into TNetstring_value(s), parsed netstrings will be removed of the stream.
 *
 * @throw Parse_exception when there is no TNetstring detected at the head of the stream or any
 *        other error occured while parsing the stream.
 */
std::istream& operator>>(std::istream& instream, tnetstring::TNetstring_value& value) {
	tnetstring::Decoder decoder(instream);
	decoder.decode(value);
	return instream;
}

}  // ::tnetstring
