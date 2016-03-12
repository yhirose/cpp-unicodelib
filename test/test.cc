#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <unicodelib.h>

using namespace std;
namespace ucl = unicode;

std::string u8text = u8"日本語もOKです。";
std::u32string u32text = U"日本語もOKです。";

std::string str1(u8"a");
std::string str2(u8"À");
std::string str3(u8"あ");
std::string str4(u8"𠀋");

TEST_CASE("encode_byte_length", "[utf8]") {
    REQUIRE(ucl::encode_byte_length(U'a') == 1);
    REQUIRE(ucl::encode_byte_length(U'À') == 2);
    REQUIRE(ucl::encode_byte_length(U'あ') == 3);
    REQUIRE(ucl::encode_byte_length(U'𠀋') == 4);
}

TEST_CASE("encode 1", "[utf8]") {
    std::string out1, out2, out3, out4;
    REQUIRE(ucl::encode(U'a', out1) == 1);
    REQUIRE(ucl::encode(U'À', out2) == 2);
    REQUIRE(ucl::encode(U'あ', out3) == 3);
    REQUIRE(ucl::encode(U'𠀋', out4) == 4);
    REQUIRE(out1 == u8"a");
    REQUIRE(out2 == u8"À");
    REQUIRE(out3 == u8"あ");
    REQUIRE(out4 == u8"𠀋");
}

TEST_CASE("encode 2", "[utf8]") {
    std::string out;
    ucl::encode(u32text.data(), u32text.length(), out);
    REQUIRE(out == u8"日本語もOKです。");
}

TEST_CASE("encode 3", "[utf8]") {
    REQUIRE(ucl::encode(u32text.data(), u32text.length()) == u8"日本語もOKです。");
}

TEST_CASE("decode_byte_length", "[utf8]") {
    REQUIRE(ucl::decode_byte_length(str1) == 1);
    REQUIRE(ucl::decode_byte_length(str1.data(), str1.length()) == 1);
    REQUIRE(ucl::decode_byte_length(str2.data(), str2.length()) == 2);
    REQUIRE(ucl::decode_byte_length(str3.data(), str3.length()) == 3);
    REQUIRE(ucl::decode_byte_length(str4.data(), str4.length()) == 4);
}

TEST_CASE("codepoint_count", "[utf8]") {
    REQUIRE(ucl::codepoint_count(u8text.data(), u8text.length()) == 9);
}

TEST_CASE("decode 1", "[utf8]") {
    char32_t out, out1, out2, out3, out4;
    REQUIRE(ucl::decode(str1, out) == 1);
    REQUIRE(ucl::decode(str1.data(), str1.length(), out1) == 1);
    REQUIRE(ucl::decode(str2.data(), str2.length(), out2) == 2);
    REQUIRE(ucl::decode(str3.data(), str3.length(), out3) == 3);
    REQUIRE(ucl::decode(str4.data(), str4.length(), out4) == 4);
}

TEST_CASE("decode 2", "[utf8]") {
    std::u32string out1;
    ucl::decode(u8text, out1);
    REQUIRE(out1 == u32text);

    std::u32string out2;
    ucl::decode(u8text.data(), u8text.length(), out2);
    REQUIRE(out2 == u32text);
}

TEST_CASE("decode 3", "[utf8]") {
    REQUIRE(ucl::decode(u8text) == u32text);
    REQUIRE(ucl::decode(u8text.data(), u8text.length()) == u32text);
}

TEST_CASE("General category", "[property]") {
    REQUIRE(ucl::general_category(0x0000) == ucl::Cc);
    REQUIRE(ucl::general_category(0x0370) == ucl::Lu);
    REQUIRE(ucl::general_category(0x0371) == ucl::Ll);
    REQUIRE(ucl::general_category(0x0483) == ucl::Mn);
    REQUIRE(ucl::general_category(ucl::MaxCode) == ucl::Unassigned);
}

TEST_CASE("General category predicate functions", "[property]") {
    REQUIRE(ucl::is_letter(U'a') == true);
    REQUIRE(ucl::is_letter(U'あ') == true);
    REQUIRE(ucl::is_mark(0x0303) == true);
    REQUIRE(ucl::is_number(U'1') == true);
    REQUIRE(ucl::is_number(U'¼') == true);
    REQUIRE(ucl::is_punctuation(U'-') == true);
    REQUIRE(ucl::is_separator(0x2028) == true);
    REQUIRE(ucl::is_symbol(U'€') == true);
    REQUIRE(ucl::is_other(0x0000) == true);
    REQUIRE(ucl::is_other(0x00AD) == true); // Soft hyphen
    REQUIRE(ucl::is_other(0xD800) == true); // Surrogate
    REQUIRE(ucl::is_other(0xE000) == true); // Private Use
    REQUIRE(ucl::is_other(0x0378) == true); // Unassigned
}
