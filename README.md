# libtnetstring

This library is a C++ implementation of the TNetString specification.
TNetstring details can be found on:

-   http://tnetstrings.org

## Installation

This is a header-only library, make sure the source is in the build path.

## Dependencies

-   Boost (headers only)
-   GTest (google-test needed to build tests only)

## Build/Run tests

To build the tests you need GTest and CMake. Make sure GTest is built and can be found by FindGTest.cmake:

    cd build
    GTEST_ROOT=/usr/src/gtest/build/ cmake ..
    make

### Install / Build GTest

If you first have to install GTest, here is an example:

    apt-get install libgtest-dev   # equivalent to download source to /usr/src/gtest
    cd /usr/src/gtest
    mkdir build
    cd build
    cmake ..
    make

## Run tests

Tests can be run bei either:

    make test

or

    ./test_tnetstring

Note: Unfortunately the tests are not implemented very robust, so sometimes they can
fail even if the result is actually correct. Problems are mostly due to the unordered
nature of std::unordered_map and my lazyness to not address this yet.

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

## API Documentation

To generate html API documentation use doxygen and the supplied Doxyfile:

    doxygen
    
## License

This software is licensed under the Boost License, see LICENSE_1_0.txt

## Feedback

I hope this library is useful for you and I am happy to receive any feedback.
You can reach me on Github.

