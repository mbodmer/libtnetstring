# libtnetstring

This library is a C++ implementation of the TNetString specification.
TNetstring details can be found on:

-   http://tnetstrings.org

## Installation

This is a header-only library, make sure the source is in the build path.

## Dependencies

-   Boost headers
-   google-test (to build tests only)

## Build tests

To build the tests you need google-test and cmake

    cd build
    cmake ..
    make all

## Examples

See test code, Especially tests named Encoder & Decoder.

Typically we have a "Variant", which holds any of the supported TNetString types. We can encode
this variant to a stream or decode a stream to variants and remainings.

Supported TNetstring_value types:

*   const char*
*   int
*   std::string
*   double
*   bool
*   TNetstring_list
*   TNetstring_dict

While TNetstring_list and TNetstring_dict are std::vector and std::unordered_map of
TNetstring_value. 

### Encoding

    std::ostream os;
    TNetstring_value tns_var;

    tns_var = 12345;                    // write int to TNetstring_value
    os << tns_var_;                     // encode to TNetsting stream

### Decoding

    std::istream is;
    TNetstring_value tns_var;
    
    is >> tns_var_;                      // decode from TNetstring stream
    int foo = boost::get<int>(tns_var);  // access int (assuming an integer) 
    
## License

This software is licensed under the Boost License, see LICENSE_1_0.txt
