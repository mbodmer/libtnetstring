#pragma once

namespace tnetstring {

/** Tagged Netsting Tag: String */
const char TNETSTRING_TAG_STRING  = ',';

/** Tagged Netsting Tag: int */
const char TNETSTRING_TAG_INT     = '#';

/** Tagged Netsting Tag: float */
const char TNETSTRING_TAG_FLOAT   = '^';

/** Tagged Netsting Tag: boolean */
const char TNETSTRING_TAG_BOOLEAN = '!';

/** Tagged Netsting Tag: null */
const char TNETSTRING_TAG_NULL    = '~';

/** Tagged Netsting Tag: dict */
const char TNETSTRING_TAG_DICT    = '}';

/** Tagged Netsting Tag: list */
const char TNETSTRING_TAG_LIST    = ']';

/** Tagged Netstring Size Delimiter */
const char TNETSTRING_SIZE_DELIM  = ':';

/** Tagged Netstring NULL value */
const std::string TNETSTRING_NULL = "0:~";

/** Tagged Netstring EMPTY value */
const std::string TNETSTRING_EMPTY = "";

/** Tagged Netstring maximal data length. */
const int TNETSTRING_DATA_MAXLEN = 999999999;

/** Tagged Netstring maximal size length */
const int TNETSTRING_SIZE_MAXLEN = 9;

}  // ::tnetstring

