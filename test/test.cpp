#include <catch2/catch_test_macros.hpp>

#include <unicodelib.h>
#include <unicodelib_encodings.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace unicode;

template <class Fn>
void split(const char *b, const char *e, char d, Fn fn) {
  int i = 0;
  int beg = 0;

  while (e ? (b + i != e) : (b[i] != '\0')) {
    if (b[i] == d) {
      fn(&b[beg], &b[i]);
      beg = i + 1;
    }
    i++;
  }

  if (i) {
    fn(&b[beg], &b[i]);
  }
}

//-----------------------------------------------------------------------------
// General Category
//-----------------------------------------------------------------------------

TEST_CASE("General category", "[general category]") {
  REQUIRE(general_category(0x0000) == GeneralCategory::Cc);
  REQUIRE(general_category(0x0370) == GeneralCategory::Lu);
  REQUIRE(general_category(0x0371) == GeneralCategory::Ll);
  REQUIRE(general_category(0x0483) == GeneralCategory::Mn);
  REQUIRE(general_category(0x10FFFF) == GeneralCategory::Unassigned);
}

TEST_CASE("General category predicate functions", "[general category]") {
  REQUIRE(is_letter(U'a') == true);
  REQUIRE(is_cased_letter(U'A') == true);
  REQUIRE(is_letter(U'あ') == true);
  REQUIRE(is_mark(0x0303) == true);
  REQUIRE(is_number(U'1') == true);
  REQUIRE(is_number(U'¼') == true);
  REQUIRE(is_punctuation(U'-') == true);
  REQUIRE(is_separator(0x2028) == true);
  REQUIRE(is_symbol(U'€') == true);
  REQUIRE(is_other(0x0000) == true);
  REQUIRE(is_other(0x00AD) == true);  // Soft hyphen
  REQUIRE(is_other(0xD800) == true);  // Surrogate
  REQUIRE(is_other(0xE000) == true);  // Private Use
  REQUIRE(is_other(0x0378) == true);  // Unassigned
}

//-----------------------------------------------------------------------------
// Property
//-----------------------------------------------------------------------------

TEST_CASE("Property", "[property]") {
  REQUIRE(is_white_space(U'a') == false);
  REQUIRE(is_white_space(U' ') == true);
}

//-----------------------------------------------------------------------------
// Derived Property
//-----------------------------------------------------------------------------

TEST_CASE("Derived property", "[derived property]") {
  REQUIRE(is_math(U'a') == false);
  REQUIRE(is_math(U' ') == false);
  REQUIRE(is_math(U'+') == true);
  REQUIRE(is_alphabetic(U'a') == true);
  REQUIRE(is_alphabetic(U' ') == false);
  REQUIRE(is_alphabetic(U'+') == false);
}

//-----------------------------------------------------------------------------
// Case
//-----------------------------------------------------------------------------

TEST_CASE("Case property", "[case]") {
  REQUIRE(general_category(U'h') == GeneralCategory::Ll);
  REQUIRE(is_lowercase(U'h') == true);
  REQUIRE(is_uppercase(U'h') == false);

  REQUIRE(general_category(U'H') == GeneralCategory::Lu);
  REQUIRE(is_lowercase(U'H') == false);
  REQUIRE(is_uppercase(U'H') == true);

  REQUIRE(general_category(0x24D7) == GeneralCategory::So);
  REQUIRE(is_lowercase(0x24D7) == true);
  REQUIRE(is_uppercase(0x24D7) == false);

  REQUIRE(general_category(0x24BD) == GeneralCategory::So);
  REQUIRE(is_lowercase(0x24BD) == false);
  REQUIRE(is_uppercase(0x24BD) == true);

  REQUIRE(general_category(0x02B0) == GeneralCategory::Lm);
  REQUIRE(is_lowercase(0x02B0) == true);
  REQUIRE(is_uppercase(0x02B0) == false);

  REQUIRE(general_category(0x1D34) == GeneralCategory::Lm);
  REQUIRE(is_lowercase(0x1D34) == true);
  REQUIRE(is_uppercase(0x1D34) == false);

  REQUIRE(general_category(0x02BD) == GeneralCategory::Lm);
  REQUIRE(is_lowercase(0x02BD) == false);
  REQUIRE(is_uppercase(0x02BD) == false);
}

TEST_CASE("Simple case mapping", "[case]") {
  REQUIRE(simple_lowercase_mapping(U'A') == U'a');
  REQUIRE(simple_lowercase_mapping(U'a') == U'a');
  REQUIRE(simple_lowercase_mapping(U'あ') == U'あ');
  REQUIRE(simple_lowercase_mapping(U',') == U',');
  REQUIRE(simple_lowercase_mapping(0x118DF) == 0x118DF);
  REQUIRE(simple_lowercase_mapping(0x118BF) == 0x118DF);

  REQUIRE(simple_uppercase_mapping(U'A') == U'A');
  REQUIRE(simple_uppercase_mapping(U'a') == U'A');
  REQUIRE(simple_uppercase_mapping(U'あ') == U'あ');
  REQUIRE(simple_uppercase_mapping(U',') == U',');
  REQUIRE(simple_uppercase_mapping(0x118DF) == 0x118BF);
  REQUIRE(simple_uppercase_mapping(0x118BF) == 0x118BF);

  REQUIRE(simple_lowercase_mapping(U'Ǳ') == U'ǳ');
  REQUIRE(simple_lowercase_mapping(U'ǲ') == U'ǳ');
  REQUIRE(simple_lowercase_mapping(U'ǳ') == U'ǳ');
  REQUIRE(simple_uppercase_mapping(U'Ǳ') == U'Ǳ');
  REQUIRE(simple_uppercase_mapping(U'ǲ') == U'Ǳ');
  REQUIRE(simple_uppercase_mapping(U'ǳ') == U'Ǳ');
  REQUIRE(simple_titlecase_mapping(U'Ǳ') == U'ǲ');
  REQUIRE(simple_titlecase_mapping(U'ǲ') == U'ǲ');
  REQUIRE(simple_titlecase_mapping(U'ǳ') == U'ǲ');
}

TEST_CASE("Simple case folding", "[case]") {
  REQUIRE(simple_case_folding(U'A') == U'a');
  REQUIRE(simple_case_folding(U'a') == U'a');
  REQUIRE(simple_case_folding(U'あ') == U'あ');
  REQUIRE(simple_case_folding(U',') == U',');
  REQUIRE(simple_case_folding(0x118DF) == 0x118DF);
  REQUIRE(simple_case_folding(0x118BF) == 0x118DF);
  REQUIRE(simple_case_folding(U'Ǳ') == U'ǳ');
  REQUIRE(simple_case_folding(U'ǲ') == U'ǳ');
  REQUIRE(simple_case_folding(U'ǳ') == U'ǳ');
}

TEST_CASE("Full case mapping", "[case]") {
  // Sigma
  REQUIRE(to_lowercase(U"Σ") == U"σ");
  REQUIRE(to_lowercase(U"ΧΑΟΣ") == U"χαος");
  REQUIRE(to_lowercase(U"ΧΑΟΣΣ") == U"χαοσς");
  REQUIRE(to_lowercase(U"ΧΑΟΣ Σ") == U"χαος σ");

  REQUIRE(to_uppercase(U"σ") == U"Σ");
  REQUIRE(to_uppercase(U"χαος") == U"ΧΑΟΣ");
  REQUIRE(to_uppercase(U"χαοσς") == U"ΧΑΟΣΣ");
  REQUIRE(to_uppercase(U"χαος σ") == U"ΧΑΟΣ Σ");

  // German
  REQUIRE(to_uppercase(U"Maße") == U"MASSE");

  // Title case
  REQUIRE(to_titlecase(U"hello WORLD. A, a.") == U"Hello World. A, A.");
  REQUIRE(to_titlecase(U"ΧΑΟΣ χαος Σ σ") == U"Χαος Χαος Σ Σ");
  REQUIRE(to_titlecase(U"Ǳabc ǳabc ǲabc") == U"ǲabc ǲabc ǲabc");
}

TEST_CASE("Full case folding", "[case]") {
  REQUIRE(to_case_fold(U"heiss") == to_case_fold(U"heiß"));
}

TEST_CASE("Casless match", "[case]") {
  REQUIRE(caseless_match(U"résumé", U"RéSUMé") == true);

  // German
  REQUIRE(caseless_match(U"MASSE", U"Maße") == true);
  REQUIRE(caseless_match(U"Dürst", U"DüRST") == true);

  // Turkish dot I
  REQUIRE(caseless_match(U"Istanbul", U"ıstanbul") == false);
  REQUIRE(caseless_match(U"İstanbul", U"istanbul") == false);
  REQUIRE(caseless_match(U"İstanbul", U"ıstanbul") == false);
  REQUIRE(caseless_match(U"İstanbul", U"Istanbul") == false);
  REQUIRE(caseless_match(U"Istanbul", U"istanbul") == true);
  REQUIRE(caseless_match(U"Istanbul", U"ıstanbul", true) == true);
  REQUIRE(caseless_match(U"İstanbul", U"istanbul", true) == true);
  REQUIRE(caseless_match(U"İstanbul", U"ıstanbul", true) == false);
  REQUIRE(caseless_match(U"İstanbul", U"Istanbul", true) == false);
  REQUIRE(caseless_match(U"Istanbul", U"istanbul", true) == false);

  // Sigma
  REQUIRE(caseless_match(U"όσος", U"ΌΣΟΣ") == true);

  // French (ignore diacritics)
  // REQUIRE(caseless_match(U"côte", U"côté") == true);
}

TEST_CASE("case detection", "[case]") {
  REQUIRE(is_uppercase(U"ΌΣΟΣ HELLO") == true);
  REQUIRE(is_uppercase(U"όσος hello") == false);
  REQUIRE(is_lowercase(U"ΌΣΟΣ HELLO") == false);
  REQUIRE(is_lowercase(U"όσος hello") == true);
  REQUIRE(is_titlecase(U"ΌΣΟΣ HELLO") == false);
  REQUIRE(is_titlecase(U"όσος hello") == false);
  REQUIRE(is_titlecase(U"Όσος Hello") == true);
  REQUIRE(is_case_fold(U"heiss") == true);
  REQUIRE(is_case_fold(U"heiß") == false);
}

//-----------------------------------------------------------------------------
// Text Segmentation
//-----------------------------------------------------------------------------

TEST_CASE("Combining character sequence", "[segmentation]") {
  REQUIRE(is_graphic_character(U'あ'));
  REQUIRE(is_graphic_character(0x0001) == false);
  REQUIRE(is_base_character(U'A'));
  REQUIRE(is_base_character(0x0300) == false);
  REQUIRE(is_combining_character(U'A') == false);
  REQUIRE(is_combining_character(0x0300));

  std::u32string base = U"\u0061\u0062";
  REQUIRE(combining_character_sequence_length(base.data(), base.length()) == 1);

  std::u32string valid = U"\u0061\u0301\u0302\u0062";
  REQUIRE(combining_character_sequence_length(valid.data(), valid.length()) ==
          3);

  std::u32string invalid = U"\u0301\u0302\u0062";
  REQUIRE(combining_character_sequence_length(invalid.data(),
                                              invalid.length()) == 2);

  std::u32string zwj = U"\u0061\u200D\u0062";  // TODO: Need better examples
  REQUIRE(combining_character_sequence_length(zwj.data(), zwj.length()) == 2);

  std::u32string zwnj = U"\u0061\u200C\u0062";  // TODO: Need better examples
  REQUIRE(combining_character_sequence_length(zwnj.data(), zwnj.length()) == 2);

  std::u32string count = U"\u0061\u0301\u0302\u0062\u0301\u0063";
  REQUIRE(combining_character_sequence_count(count.data(), count.length()) ==
          3);

  std::u32string korean = U"\uD4DB\u1111\u1171\u11B6";
  REQUIRE(combining_character_sequence_count(korean.data(), korean.length()) ==
          4);
  REQUIRE(extended_combining_character_sequence_count(korean.data(),
                                                      korean.length()) == 2);
}

template <typename T>
void read_text_segmentation_test_file(const char *path, T callback) {
  ifstream fs(path);
  REQUIRE(fs);

  size_t ln = 0;
  std::string line;
  while (std::getline(fs, line)) {
    ln++;
    if (line.empty() || line[0] == '#') {
      continue;
    }
    line.erase(line.find('\t'));

    std::u32string s32;
    std::vector<bool> boundary;
    size_t expected_count = 0;

    stringstream ss(line);
    string ope;
    char32_t ope_cp;
    ss >> ope;
    utf8::decode_codepoint(ope.data(), ope.length(), ope_cp);
    boundary.push_back(ope_cp == U'÷');
    while (!ss.eof()) {
      int val;
      ss >> hex >> val;
      s32 += static_cast<char32_t>(val);
      ss >> ope;
      utf8::decode_codepoint(ope.data(), ope.length(), ope_cp);
      auto is_boundary = (ope_cp == U'÷');
      boundary.push_back(is_boundary);
      expected_count += (is_boundary ? 1 : 0);
    }

    callback(s32, boundary, expected_count, ln);
  }
}

TEST_CASE("Grapheme cluster segmentation", "[segmentation]") {
  auto path = "../UCD/auxiliary/GraphemeBreakTest.txt";
  read_text_segmentation_test_file(
      path,
      [](const auto &s32, const auto &boundary, auto expected_count, auto ln) {
        for (auto i = 0u; i < boundary.size(); i++) {
          auto actual = is_grapheme_boundary(s32.data(), s32.length(), i);
          REQUIRE(boundary[i] == actual);
        }

        REQUIRE(expected_count == grapheme_count(s32));
      });
}

TEST_CASE("Word segmentation", "[segmentation]") {
  auto path = "../UCD/auxiliary/WordBreakTest.txt";
  read_text_segmentation_test_file(
      path,
      [](const auto &s32, const auto &boundary, auto expected_count, auto ln) {
        for (auto i = 0u; i < boundary.size(); i++) {
          auto actual = is_word_boundary(s32.data(), s32.length(), i);
          REQUIRE(boundary[i] == actual);
        }
      });
}

TEST_CASE("Sentence segmentation", "[segmentation]") {
  auto path = "../UCD/auxiliary/SentenceBreakTest.txt";
  read_text_segmentation_test_file(
      path,
      [](const auto &s32, const auto &boundary, auto expected_count, auto ln) {
        for (auto i = 0u; i < boundary.size(); i++) {
          auto actual = is_sentence_boundary(s32.data(), s32.length(), i);
          REQUIRE(boundary[i] == actual);
        }
      });
}

//-----------------------------------------------------------------------------
// Block
//-----------------------------------------------------------------------------

TEST_CASE("Block", "[block]") {
  REQUIRE(block(U'a') == Block::BasicLatin);
  REQUIRE(block(U'あ') == Block::Hiragana);
}

//-----------------------------------------------------------------------------
// Script
//-----------------------------------------------------------------------------

TEST_CASE("Script", "[script]") {
  REQUIRE(script(U'a') == Script::Latin);
  REQUIRE(script(U'あ') == Script::Hiragana);
  REQUIRE(script(U'ー') == Script::Common);
}

TEST_CASE("Script extension", "[script]") {
  REQUIRE(is_script(Script::Hiragana, U'ー'));
  REQUIRE(is_script(Script::Katakana, U'ー'));
}

//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

TEST_CASE("Normalization", "[normalization]") {
  ifstream fs("../UCD/NormalizationTest.txt");
  REQUIRE(fs);

  std::string line;
  while (std::getline(fs, line)) {
    if (line.empty() || line[0] == '#' || line[0] == '@') {
      continue;
    }
    line.erase(line.find("; #"));

    vector<u32string> fields;
    split(line.data(), line.data() + line.length(), ';', [&](auto b, auto e) {
      u32string codes;
      split(b, e, ' ', [&](auto b, auto e) {
        char32_t cp = stoi(string(b, e), nullptr, 16);
        codes += cp;
      });
      fields.push_back(codes);
    });

    const auto &c1 = fields[0];
    const auto &c2 = fields[1];
    const auto &c3 = fields[2];
    const auto &c4 = fields[3];
    const auto &c5 = fields[4];

    // NFC
    //   c2 == toNFC(c1) == toNFC(c2) == toNFC(c3)
    //   c4 == toNFC(c4) == toNFC(c5)
    REQUIRE(c2 == to_nfc(c1));
    REQUIRE(c2 == to_nfc(c2));
    REQUIRE(c2 == to_nfc(c3));
    REQUIRE(c4 == to_nfc(c4));
    REQUIRE(c4 == to_nfc(c5));

    // NFD
    //   c3 == toNFD(c1) == toNFD(c2) == toNFD(c3)
    //   c5 == toNFD(c4) == toNFD(c5)
    REQUIRE(c3 == to_nfd(c1));
    REQUIRE(c3 == to_nfd(c2));
    REQUIRE(c3 == to_nfd(c3));
    REQUIRE(c5 == to_nfd(c4));
    REQUIRE(c5 == to_nfd(c5));

    // NFKC
    //   c4 == toNFKC(c1) == toNFKC(c2) == toNFKC(c3) == toNFKC(c4) ==
    //   toNFKC(c5)
    REQUIRE(c4 == to_nfkc(c1));
    REQUIRE(c4 == to_nfkc(c2));
    REQUIRE(c4 == to_nfkc(c3));
    REQUIRE(c4 == to_nfkc(c4));
    REQUIRE(c4 == to_nfkc(c5));

    // NFKD
    //   c5 == toNFKD(c1) == toNFKD(c2) == toNFKD(c3) == toNFKD(c4) ==
    //   toNFKD(c5)
    REQUIRE(c5 == to_nfkd(c1));
    REQUIRE(c5 == to_nfkd(c2));
    REQUIRE(c5 == to_nfkd(c3));
    REQUIRE(c5 == to_nfkd(c4));
    REQUIRE(c5 == to_nfkd(c5));
  }
}

//-----------------------------------------------------------------------------
// UTF8 encoding
//-----------------------------------------------------------------------------

namespace test_utf8 {

std::string u8text = u8"日本語もOKです。";
std::u32string u32text = U"日本語もOKです。";

std::string str1(u8"a");
std::string str2(u8"À");
std::string str3(u8"あ");
std::string str4(u8"𠀋");

TEST_CASE("codepoint length in char32_t", "[utf8]") {
  REQUIRE(utf8::codepoint_length(U'a') == 1);
  REQUIRE(utf8::codepoint_length(U'À') == 2);
  REQUIRE(utf8::codepoint_length(U'あ') == 3);
  REQUIRE(utf8::codepoint_length(U'𠀋') == 4);
}

TEST_CASE("encode 1", "[utf8]") {
  std::string out1, out2, out3, out4;
  REQUIRE(utf8::encode_codepoint(U'a', out1) == 1);
  REQUIRE(utf8::encode_codepoint(U'À', out2) == 2);
  REQUIRE(utf8::encode_codepoint(U'あ', out3) == 3);
  REQUIRE(utf8::encode_codepoint(U'𠀋', out4) == 4);
  REQUIRE(out1 == u8"a");
  REQUIRE(out2 == u8"À");
  REQUIRE(out3 == u8"あ");
  REQUIRE(out4 == u8"𠀋");
}

TEST_CASE("encode 2", "[utf8]") {
  std::string out;
  utf8::encode(u32text, out);
  REQUIRE(out == u8"日本語もOKです。");
}

TEST_CASE("encode 3", "[utf8]") {
  REQUIRE(utf8::encode(u32text) == u8"日本語もOKです。");
}

TEST_CASE("codepoint length in utf8", "[utf8]") {
  REQUIRE(utf8::codepoint_length(str1) == 1);
  REQUIRE(utf8::codepoint_length(str2) == 2);
  REQUIRE(utf8::codepoint_length(str3) == 3);
  REQUIRE(utf8::codepoint_length(str4) == 4);
}

TEST_CASE("codepoint_count", "[utf8]") {
  REQUIRE(utf8::codepoint_count(u8text) == 9);
}

TEST_CASE("decode 1", "[utf8]") {
  char32_t out1, out2, out3, out4;
  REQUIRE(utf8::decode_codepoint(str1, out1) == 1);
  REQUIRE(utf8::decode_codepoint(str2, out2) == 2);
  REQUIRE(utf8::decode_codepoint(str3, out3) == 3);
  REQUIRE(utf8::decode_codepoint(str4, out4) == 4);

  REQUIRE(utf8::decode_codepoint(str1) == 0x0061);
  REQUIRE(utf8::decode_codepoint(str2) == 0x00C0);
  REQUIRE(utf8::decode_codepoint(str3) == 0x3042);
  REQUIRE(utf8::decode_codepoint(str4) == 0x2000B);
}

TEST_CASE("decode 2", "[utf8]") {
  std::u32string out1;
  utf8::decode(u8text, out1);
  REQUIRE(out1 == u32text);

  std::u32string out2;
  utf8::decode(u8text, out2);
  REQUIRE(out2 == u32text);
}

TEST_CASE("decode 3", "[utf8]") {
  REQUIRE(utf8::decode(u8text) == u32text);
  REQUIRE(utf8::decode(u8text) == u32text);
}

} // namespace test_utf8

//-----------------------------------------------------------------------------
// UTF16 encoding
//-----------------------------------------------------------------------------

namespace test_utf16 {

std::u16string u16text = u"日本語もOKです。";
std::u32string u32text = U"日本語もOKです。";

std::u16string str1(u"a");
std::u16string str2(u"À");
std::u16string str3(u"あ");
std::u16string str4(u"𠀋");

TEST_CASE("utf16 codepoint length in char32_t", "[utf16]") {
  REQUIRE(utf16::codepoint_length(U'a') == 1);
  REQUIRE(utf16::codepoint_length(U'À') == 1);
  REQUIRE(utf16::codepoint_length(U'あ') == 1);
  REQUIRE(utf16::codepoint_length(U'𠀋') == 2);
}

TEST_CASE("utf16 encode 1", "[utf16]") {
  std::u16string out1, out2, out3, out4;
  REQUIRE(utf16::encode_codepoint(U'a', out1) == 1);
  REQUIRE(utf16::encode_codepoint(U'À', out2) == 1);
  REQUIRE(utf16::encode_codepoint(U'あ', out3) == 1);
  REQUIRE(utf16::encode_codepoint(U'𠀋', out4) == 2);
  REQUIRE(out1 == u"a");
  REQUIRE(out2 == u"À");
  REQUIRE(out3 == u"あ");
  REQUIRE(out4 == u"𠀋");
}

TEST_CASE("utf16 encode 2", "[utf16]") {
  std::u16string out;
  utf16::encode(u32text, out);
  REQUIRE(out == u"日本語もOKです。");
}

TEST_CASE("utf16 encode 3", "[utf16]") {
  REQUIRE(utf16::encode(u32text) == u"日本語もOKです。");
}

TEST_CASE("codepoint length in utf16", "[utf16]") {
  REQUIRE(utf16::codepoint_length(str1) == 1);
  REQUIRE(utf16::codepoint_length(str2) == 1);
  REQUIRE(utf16::codepoint_length(str3) == 1);
  REQUIRE(utf16::codepoint_length(str4) == 2);
}

TEST_CASE("utf16 codepoint_count", "[utf16]") {
  REQUIRE(utf16::codepoint_count(u16text) == 9);
}

TEST_CASE("utf16 decode 1", "[utf16]") {
  char32_t out1, out2, out3, out4;
  REQUIRE(utf16::decode_codepoint(str1, out1) == 1);
  REQUIRE(utf16::decode_codepoint(str2, out2) == 1);
  REQUIRE(utf16::decode_codepoint(str3, out3) == 1);
  REQUIRE(utf16::decode_codepoint(str4, out4) == 2);

  REQUIRE(utf16::decode_codepoint(str1) == 0x0061);
  REQUIRE(utf16::decode_codepoint(str2) == 0x00C0);
  REQUIRE(utf16::decode_codepoint(str3) == 0x3042);
  REQUIRE(utf16::decode_codepoint(str4) == 0x2000B);
}

TEST_CASE("utf16 decode 2", "[utf16]") {
  std::u32string out1;
  utf16::decode(u16text, out1);
  REQUIRE(out1 == u32text);

  std::u32string out2;
  utf16::decode(u16text, out2);
  REQUIRE(out2 == u32text);
}

TEST_CASE("utf16 decode 3", "[utf16]") {
  REQUIRE(utf16::decode(u16text) == u32text);
  REQUIRE(utf16::decode(u16text) == u32text);
}

} // namespace test_utf16

//-----------------------------------------------------------------------------
// Conversion between encodings
//-----------------------------------------------------------------------------

namespace test_encodeings {

TEST_CASE("Conversion text", "[encodings]") {
  std::string u8text = u8"日本語もOKです。";
  std::u16string u16text = u"日本語もOKです。";
  std::u32string u32text = U"日本語もOKです。";
  std::wstring wtext = L"日本語もOKです。";

  REQUIRE(to_utf16(u8text) == u16text);
  REQUIRE(to_utf8(u16text) == u8text);

  REQUIRE(to_wstring(u8text) == wtext);
  REQUIRE(to_wstring(u16text) == wtext);
  REQUIRE(to_wstring(u32text) == wtext);
  REQUIRE(to_utf8(wtext) == u8text);
  REQUIRE(to_utf16(wtext) == u16text);
  REQUIRE(to_utf32(wtext) == u32text);
}

} // namespace test_encodeings

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
