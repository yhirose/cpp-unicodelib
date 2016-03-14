#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <unicodelib.h>
#include <sstream>

using namespace std;

std::string u8text = u8"日本語もOKです。";
std::u32string u32text = U"日本語もOKです。";

std::string str1(u8"a");
std::string str2(u8"À");
std::string str3(u8"あ");
std::string str4(u8"𠀋");

TEST_CASE("encode_byte_length", "[utf8]") {
    REQUIRE(unicode::encode_byte_length(U'a') == 1);
    REQUIRE(unicode::encode_byte_length(U'À') == 2);
    REQUIRE(unicode::encode_byte_length(U'あ') == 3);
    REQUIRE(unicode::encode_byte_length(U'𠀋') == 4);
}

TEST_CASE("encode 1", "[utf8]") {
    std::string out1, out2, out3, out4;
    REQUIRE(unicode::encode(U'a', out1) == 1);
    REQUIRE(unicode::encode(U'À', out2) == 2);
    REQUIRE(unicode::encode(U'あ', out3) == 3);
    REQUIRE(unicode::encode(U'𠀋', out4) == 4);
    REQUIRE(out1 == u8"a");
    REQUIRE(out2 == u8"À");
    REQUIRE(out3 == u8"あ");
    REQUIRE(out4 == u8"𠀋");
}

TEST_CASE("encode 2", "[utf8]") {
    std::string out;
    unicode::encode(u32text.data(), u32text.length(), out);
    REQUIRE(out == u8"日本語もOKです。");
}

TEST_CASE("encode 3", "[utf8]") {
    REQUIRE(unicode::encode(u32text.data(), u32text.length()) == u8"日本語もOKです。");
}

TEST_CASE("decode_byte_length", "[utf8]") {
    REQUIRE(unicode::decode_byte_length(str1) == 1);
    REQUIRE(unicode::decode_byte_length(str1.data(), str1.length()) == 1);
    REQUIRE(unicode::decode_byte_length(str2.data(), str2.length()) == 2);
    REQUIRE(unicode::decode_byte_length(str3.data(), str3.length()) == 3);
    REQUIRE(unicode::decode_byte_length(str4.data(), str4.length()) == 4);
}

TEST_CASE("codepoint_count", "[utf8]") {
    REQUIRE(unicode::codepoint_count(u8text.data(), u8text.length()) == 9);
}

TEST_CASE("decode 1", "[utf8]") {
    char32_t out, out1, out2, out3, out4;
    REQUIRE(unicode::decode(str1, out) == 1);
    REQUIRE(unicode::decode(str1.data(), str1.length(), out1) == 1);
    REQUIRE(unicode::decode(str2.data(), str2.length(), out2) == 2);
    REQUIRE(unicode::decode(str3.data(), str3.length(), out3) == 3);
    REQUIRE(unicode::decode(str4.data(), str4.length(), out4) == 4);
}

TEST_CASE("decode 2", "[utf8]") {
    std::u32string out1;
    unicode::decode(u8text, out1);
    REQUIRE(out1 == u32text);

    std::u32string out2;
    unicode::decode(u8text.data(), u8text.length(), out2);
    REQUIRE(out2 == u32text);
}

TEST_CASE("decode 3", "[utf8]") {
    REQUIRE(unicode::decode(u8text) == u32text);
    REQUIRE(unicode::decode(u8text.data(), u8text.length()) == u32text);
}

TEST_CASE("General category", "[property]") {
    REQUIRE(unicode::general_category(0x0000) == unicode::GeneralCategory::Cc);
    REQUIRE(unicode::general_category(0x0370) == unicode::GeneralCategory::Lu);
    REQUIRE(unicode::general_category(0x0371) == unicode::GeneralCategory::Ll);
    REQUIRE(unicode::general_category(0x0483) == unicode::GeneralCategory::Mn);
    REQUIRE(unicode::general_category(unicode::MaxCode) == unicode::GeneralCategory::Unassigned);
}

TEST_CASE("General category predicate functions", "[property]") {
    REQUIRE(unicode::is_letter(U'a') == true);
    REQUIRE(unicode::is_letter(U'あ') == true);
    REQUIRE(unicode::is_mark(0x0303) == true);
    REQUIRE(unicode::is_number(U'1') == true);
    REQUIRE(unicode::is_number(U'¼') == true);
    REQUIRE(unicode::is_punctuation(U'-') == true);
    REQUIRE(unicode::is_separator(0x2028) == true);
    REQUIRE(unicode::is_symbol(U'€') == true);
    REQUIRE(unicode::is_other(0x0000) == true);
    REQUIRE(unicode::is_other(0x00AD) == true); // Soft hyphen
    REQUIRE(unicode::is_other(0xD800) == true); // Surrogate
    REQUIRE(unicode::is_other(0xE000) == true); // Private Use
    REQUIRE(unicode::is_other(0x0378) == true); // Unassigned
}

TEST_CASE("Grapheme cluster segmentations", "[text segmentation]") {
    ifstream fs("../../UCD/auxiliary/GraphemeBreakTest.txt");
    REQUIRE(fs);

    std::string line;
    while (std::getline(fs, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        line.erase(line.find('\t'));

        std::u32string s32;
        std::vector<bool> boundary;
        size_t expected_count = 0;

        std::stringstream ss(line);
        std::string ope;
        ss >> ope;
        boundary.push_back(ope == "÷");
        while (!ss.eof()) {
            int val;
            ss >> std::hex >> val;
            s32 += (char32_t)val;
            ss >> ope;
            auto is_boundary = (ope == "÷");
            boundary.push_back(is_boundary);
            expected_count += (is_boundary ? 1 : 0);
        }

        for (auto i = 0u; i < boundary.size(); i++) {
            REQUIRE(boundary[i] == unicode::is_grapheme_boundary(s32.data(), s32.length(), i));
        }

        size_t actual_count = unicode::grapheme_count(s32.data(), s32.length());
        REQUIRE(expected_count == actual_count);
    }
}
