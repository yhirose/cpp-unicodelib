#include <unicodelib.h>
#include <unicodelib_encodings.h>

#include <catch2/catch_test_macros.hpp>
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
// Unicode Scalar Value
//-----------------------------------------------------------------------------

TEST_CASE("is_scalar_value", "[scalar value]") {
  REQUIRE(is_scalar_value(U'a') == true);
  REQUIRE(is_scalar_value(0xD7FF) == true);
  REQUIRE(is_scalar_value(0xD800) == false);  // Surrogate
  REQUIRE(is_scalar_value(0xDFFF) == false);  // Surrogate
  REQUIRE(is_scalar_value(0xE000) == true);
  REQUIRE(is_scalar_value(0x10FFFF) == true);
  REQUIRE(is_scalar_value(0x110000) == false);  // Beyond U+10FFFF
}

TEST_CASE("Property lookup beyond U+10FFFF", "[scalar value]") {
  REQUIRE(general_category(0x110000) == GeneralCategory::Unassigned);
  REQUIRE(general_category(0xFFFFFFFF) == GeneralCategory::Unassigned);
  REQUIRE(is_letter(0x110000) == false);
  REQUIRE(is_white_space(0xFFFFFFFF) == false);
  REQUIRE(is_alphabetic(0x110000) == false);
  REQUIRE(block(0x110000) == Block::Unassigned);
  REQUIRE(script(0x110000) == Script::Unassigned);
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

  // Characters with only a full (F) folding have no simple folding and
  // must fold to themselves, not to 0.
  REQUIRE(simple_case_folding(U'ß') == U'ß');
  REQUIRE(simple_case_folding(U'İ') == U'İ');
  REQUIRE(simple_case_folding(U'ﬀ') == U'ﬀ');
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

  // German: by default, ß (U+00DF) uppercases to "SS"
  REQUIRE(to_uppercase(U"Maße") == U"MASSE");

  // German capital sharp s tailoring (opt-in): ß -> ẞ (U+1E9E)
  REQUIRE(to_uppercase(U"Maße", CaseTailoring::GermanCapitalSharpS) ==
          U"MAẞE");
  REQUIRE(to_uppercase(U"straße", CaseTailoring::GermanCapitalSharpS) ==
          U"STRAẞE");
  REQUIRE(to_uppercase(U"ß", CaseTailoring::GermanCapitalSharpS) == U"ẞ");
  // It can be combined with a locale and does not affect lowercasing.
  REQUIRE(to_uppercase(U"Maße",
                       Locale{"de"} | CaseTailoring::GermanCapitalSharpS) ==
          U"MAẞE");
  REQUIRE(to_lowercase(U"ẞ", CaseTailoring::GermanCapitalSharpS) == U"ß");

  // Title case
  REQUIRE(to_titlecase(U"hello WORLD. A, a.") == U"Hello World. A, A.");
  REQUIRE(to_titlecase(U"ΧΑΟΣ χαος Σ σ") == U"Χαος Χαος Σ Σ");
  REQUIRE(to_titlecase(U"Ǳabc ǳabc ǲabc") == U"ǲabc ǲabc ǲabc");

  // Dutch IJ titlecasing
  REQUIRE(to_titlecase(U"ijsje", "nl") == U"IJsje");
  REQUIRE(to_titlecase(U"IJssel", "nl") == U"IJssel");
  REQUIRE(to_titlecase(U"ik hou van ijsje", "nl") == U"Ik Hou Van IJsje");
  REQUIRE(to_titlecase(U"ijmuiden", "nl") == U"IJmuiden");
  REQUIRE(to_titlecase(U"IJMUIDEN", "nl") == U"IJmuiden");
  // The locale is matched by its primary subtag, case-insensitively.
  REQUIRE(to_titlecase(U"ijsje", "nl-BE") == U"IJsje");
  REQUIRE(to_titlecase(U"ijsje", "nl_NL") == U"IJsje");
  REQUIRE(to_titlecase(U"ijsje", "NL") == U"IJsje");
  REQUIRE(to_titlecase(U"ijsje", Locale{"nl-NL"}) == U"IJsje");
  // Without Dutch locale, IJ should not be special-cased
  REQUIRE(to_titlecase(U"ijsje") == U"Ijsje");
  REQUIRE(to_titlecase(U"ijsje", "en") == U"Ijsje");
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
  REQUIRE(caseless_match(U"Istanbul", U"ıstanbul",
                         CaseTailoring::TurkicCaseFold) == true);
  REQUIRE(caseless_match(U"İstanbul", U"istanbul",
                         CaseTailoring::TurkicCaseFold) == true);
  REQUIRE(caseless_match(U"İstanbul", U"ıstanbul",
                         CaseTailoring::TurkicCaseFold) == false);
  REQUIRE(caseless_match(U"İstanbul", U"Istanbul",
                         CaseTailoring::TurkicCaseFold) == false);
  REQUIRE(caseless_match(U"Istanbul", U"istanbul",
                         CaseTailoring::TurkicCaseFold) == false);

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

  // D56: The sequence <0030, FE00, 20E3> represents a variant form of the digit
  // zero, followed by an enclosing keycap.
  std::u32string enclosing_keycap = U"\u0030\uFE00\u20E3\u0030\uFE00\u20E3";
  REQUIRE(combining_character_sequence_length(enclosing_keycap.data(),
                                              enclosing_keycap.length()) == 3);
  REQUIRE(combining_character_sequence_count(enclosing_keycap.data(),
                                             enclosing_keycap.length()) == 2);
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
      path, [](const auto &s32, const auto &boundary, auto expected_count,
               auto /*ln*/) {
        for (auto i = 0u; i < boundary.size(); i++) {
          auto actual = is_grapheme_boundary(s32.data(), s32.length(), i);
          CHECK(boundary[i] == actual);
        }

        CHECK(expected_count == grapheme_count(s32));
      });
}

TEST_CASE("Word segmentation", "[segmentation]") {
  auto path = "../UCD/auxiliary/WordBreakTest.txt";
  read_text_segmentation_test_file(
      path, [](const auto &s32, const auto &boundary, auto /*expected_count*/,
               auto /*ln*/) {
        for (auto i = 0u; i < boundary.size(); i++) {
          auto actual = is_word_boundary(s32.data(), s32.length(), i);
          CHECK(boundary[i] == actual);
        }
      });
}

TEST_CASE("Sentence segmentation", "[segmentation]") {
  auto path = "../UCD/auxiliary/SentenceBreakTest.txt";
  read_text_segmentation_test_file(
      path, [](const auto &s32, const auto &boundary, auto /*expected_count*/,
               auto /*ln*/) {
        for (auto i = 0u; i < boundary.size(); i++) {
          auto actual = is_sentence_boundary(s32.data(), s32.length(), i);
          CHECK(boundary[i] == actual);
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
  REQUIRE(is_script(Script::Latin, U'ー') == false);
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

TEST_CASE("decode invalid utf8 resync", "[utf8]") {
  // An invalid lead byte must resync at the next byte and must not swallow the
  // following valid characters.
  REQUIRE(utf8::decode("\xE0" "AB") == U"AB");        // truncated 3-byte lead
  REQUIRE(utf8::decode("\xF0" "Hello") == U"Hello");  // truncated 4-byte lead
  REQUIRE(utf8::decode("\xE1\x80" "A") == U"A");       // bad 3rd byte
  REQUIRE(utf8::decode("a\x80" "b") == U"ab");         // lone continuation byte
  REQUIRE(utf8::decode("\xFF\xFF" "x") == U"x");        // never-valid lead bytes
}

TEST_CASE("decode_codepoint rejects bad continuation bytes", "[utf8]") {
  char32_t cp;
  // A continuation byte that is not 0x80-0xBF makes decode fail (0 bytes).
  REQUIRE(utf8::decode_codepoint("\xE0" "AB", 3, cp) == 0);
  REQUIRE(utf8::decode_codepoint("\xF0\x41\x42\x43", 4, cp) == 0);
  // A well-formed sequence is still accepted and reports its length.
  REQUIRE(utf8::decode_codepoint(u8"あ", 3, cp) == 3);
}

TEST_CASE("decode_codepoint rejects ill-formed scalar values", "[utf8]") {
  char32_t cp;
  // Overlong sequences
  REQUIRE(utf8::decode_codepoint("\xC0\xAF", 2, cp) == 0);
  REQUIRE(utf8::decode_codepoint("\xC1\xBF", 2, cp) == 0);
  REQUIRE(utf8::decode_codepoint("\xE0\x80\xAF", 3, cp) == 0);
  REQUIRE(utf8::decode_codepoint("\xF0\x80\x80\xAF", 4, cp) == 0);
  // Surrogates
  REQUIRE(utf8::decode_codepoint("\xED\xA0\x80", 3, cp) == 0);  // U+D800
  REQUIRE(utf8::decode_codepoint("\xED\xBF\xBF", 3, cp) == 0);  // U+DFFF
  // Beyond U+10FFFF
  REQUIRE(utf8::decode_codepoint("\xF4\x90\x80\x80", 4, cp) == 0);  // U+110000
  REQUIRE(utf8::decode_codepoint("\xF5\x80\x80\x80", 4, cp) == 0);

  // Boundaries that must stay valid
  REQUIRE(utf8::decode_codepoint("\xC2\x80", 2, cp) == 2);
  REQUIRE(cp == 0x0080);
  REQUIRE(utf8::decode_codepoint("\xE0\xA0\x80", 3, cp) == 3);
  REQUIRE(cp == 0x0800);
  REQUIRE(utf8::decode_codepoint("\xED\x9F\xBF", 3, cp) == 3);
  REQUIRE(cp == 0xD7FF);
  REQUIRE(utf8::decode_codepoint("\xEE\x80\x80", 3, cp) == 3);
  REQUIRE(cp == 0xE000);
  REQUIRE(utf8::decode_codepoint("\xF4\x8F\xBF\xBF", 4, cp) == 4);
  REQUIRE(cp == 0x10FFFF);
}

TEST_CASE("decode drops ill-formed sequences and resyncs", "[utf8]") {
  REQUIRE(utf8::decode("\xC0\xAF" "A") == U"A");          // overlong '/'
  REQUIRE(utf8::decode("\xED\xA0\x80" "A") == U"A");      // surrogate U+D800
  REQUIRE(utf8::decode("\xF4\x90\x80\x80" "A") == U"A");  // U+110000
}

TEST_CASE("codepoint_length consistent with decode_codepoint", "[utf8]") {
  REQUIRE(utf8::codepoint_length("\xE3\x81", 2) == 0);      // truncated
  REQUIRE(utf8::codepoint_length("\x80", 1) == 0);          // continuation byte
  REQUIRE(utf8::codepoint_length("\xED\xA0\x80", 3) == 0);  // surrogate
  REQUIRE(utf8::codepoint_length("\xE3\x81\x82", 3) == 3);
}

TEST_CASE("codepoint_count with ill-formed input", "[utf8]") {
  // never loops forever, and matches what decode() produces
  REQUIRE(utf8::codepoint_count("a\x80" "b", 3) == 2);
  REQUIRE(utf8::codepoint_count("\xF0" "Hello", 6) == 5);
}

}  // namespace test_utf8

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

TEST_CASE("utf16 decode invalid resync", "[utf16]") {
  // A lone high surrogate must resync and must not swallow the next unit.
  const char16_t lone_high[] = {0xD800, u'A'};
  REQUIRE(utf16::decode(std::u16string_view(lone_high, 2)) == U"A");
}

TEST_CASE("utf16 is_surrogate_pair bounds", "[utf16]") {
  // Must not read s16[1] when only one unit is available.
  const char16_t lone_high[] = {0xD800};
  REQUIRE(utf16::is_surrogate_pair(lone_high, 1) == false);
}

TEST_CASE("utf16 decode_codepoint rejects unpaired surrogates", "[utf16]") {
  char32_t cp;
  const char16_t high[] = {0xD800, u'A'};
  const char16_t low[] = {0xDC00, u'A'};
  REQUIRE(utf16::decode_codepoint(high, 2, cp) == 0);
  REQUIRE(utf16::decode_codepoint(low, 2, cp) == 0);
}

TEST_CASE("utf16 decode drops unpaired surrogates", "[utf16]") {
  const char16_t s[] = {0xDC00, u'A'};
  REQUIRE(utf16::decode(s, 2) == U"A");
}

TEST_CASE("utf16 codepoint_length consistent with encode/decode", "[utf16]") {
  // Invalid scalar values yield 0, as encode_codepoint does.
  REQUIRE(utf16::codepoint_length(0xD800) == 0);
  REQUIRE(utf16::codepoint_length(0x110000) == 0);
  REQUIRE(utf16::codepoint_length(0x10FFFF) == 2);
  // Unpaired surrogates yield 0, as decode_codepoint does.
  const char16_t high[] = {0xD800};
  const char16_t low[] = {0xDC00};
  REQUIRE(utf16::codepoint_length(high, 1) == 0);
  REQUIRE(utf16::codepoint_length(low, 1) == 0);
}

TEST_CASE("utf16 codepoint_count with unpaired surrogates", "[utf16]") {
  const char16_t s[] = {0xD800, u'A', 0xDC00, u'B'};
  REQUIRE(utf16::codepoint_count(s, 4) == 2);
}

}  // namespace test_utf16

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

}  // namespace test_encodeings

//-----------------------------------------------------------------------------
// East Asian Width / Display Width
//-----------------------------------------------------------------------------

TEST_CASE("east_asian_width property", "[width]") {
  REQUIRE(east_asian_width(U'A') == EastAsianWidth::Narrow);       // Na
  REQUIRE(east_asian_width(U' ') == EastAsianWidth::Narrow);       // Na
  REQUIRE(east_asian_width(0x00A1) == EastAsianWidth::Ambiguous);  // ¡ -> A
  REQUIRE(east_asian_width(0x3042) == EastAsianWidth::Wide);       // ぁ -> W
  REQUIRE(east_asian_width(0xFF21) == EastAsianWidth::Fullwidth);  // Ａ -> F
  REQUIRE(east_asian_width(0xFF61) == EastAsianWidth::Halfwidth);  // ｡ -> H

  // Unassigned code points in the CJK / SIP / TIP ranges default to Wide.
  REQUIRE(east_asian_width(0x4E00) == EastAsianWidth::Wide);   // assigned CJK
  REQUIRE(east_asian_width(0x20000) == EastAsianWidth::Wide);  // SIP
  REQUIRE(east_asian_width(0x3FFFD) == EastAsianWidth::Wide);  // TIP boundary
}

TEST_CASE("codepoint_width", "[width]") {
  REQUIRE(codepoint_width(U'A') == 1);
  REQUIRE(codepoint_width(0x3042) == 2);  // ぁ (Wide)
  REQUIRE(codepoint_width(0xFF21) == 2);  // Ａ (Fullwidth)
  REQUIRE(codepoint_width(0xFF61) == 1);  // ｡ (Halfwidth)

  REQUIRE(codepoint_width(U'\0') == 0);    // NUL
  REQUIRE(codepoint_width(U'\t') == 0);    // control
  REQUIRE(codepoint_width(0x0301) == 0);   // combining acute (Mn)
  REQUIRE(codepoint_width(0x200D) == 0);   // ZWJ (Cf)
  REQUIRE(codepoint_width(0x20DD) == 0);   // enclosing circle (Me)
  REQUIRE(codepoint_width(0x1F600) == 2);  // GRINNING FACE (Emoji_Presentation)

  // Ambiguous width follows the policy.
  REQUIRE(codepoint_width(0x00A1, AmbiguousWidth::Narrow) == 1);
  REQUIRE(codepoint_width(0x00A1, AmbiguousWidth::Wide) == 2);
}

TEST_CASE("width of strings", "[width]") {
  REQUIRE(width(U"hello") == 5);
  REQUIRE(width(U"日本語") == 6);
  REQUIRE(width(U"aあb") == 4);

  // A combining mark adds no width: 'e' + U+0301 (COMBINING ACUTE ACCENT).
  REQUIRE(width(U"é") == 1);

}

TEST_CASE("width of emoji clusters", "[width]") {
  // Single emoji.
  REQUIRE(width(U"\U0001F600") == 2);

  // ZWJ sequence: family renders as one wide glyph.
  REQUIRE(width(U"\U0001F468‍\U0001F469‍\U0001F467‍\U0001F466") ==
          2);

  // Flag: two regional indicators -> one wide glyph.
  REQUIRE(width(U"\U0001F1EF\U0001F1F5") == 2);

  // Emoji modifier sequence (thumbs up + skin tone).
  REQUIRE(width(U"\U0001F44D\U0001F3FD") == 2);

  // Variation selectors: VS16 forces wide, VS15 forces narrow.
  REQUIRE(width(U"❤️") == 2);  // heart, emoji presentation
  REQUIRE(width(U"❤︎") == 1);  // heart, text presentation

  // Keycap: '1' + VS16 + COMBINING ENCLOSING KEYCAP.
  REQUIRE(width(U"1️⃣") == 2);
}

TEST_CASE("width consistency with EastAsianWidth.txt", "[width]") {
  std::ifstream fs("../UCD/EastAsianWidth.txt");
  REQUIRE(fs);

  auto to_enum = [](const std::string &s) {
    if (s == "N") return EastAsianWidth::Neutral;
    if (s == "A") return EastAsianWidth::Ambiguous;
    if (s == "H") return EastAsianWidth::Halfwidth;
    if (s == "W") return EastAsianWidth::Wide;
    if (s == "F") return EastAsianWidth::Fullwidth;
    return EastAsianWidth::Narrow;  // Na
  };

  std::string line;
  while (std::getline(fs, line)) {
    auto hash = line.find('#');
    if (hash != std::string::npos) line = line.substr(0, hash);
    auto semi = line.find(';');
    if (semi == std::string::npos) continue;

    std::string range = line.substr(0, semi);
    std::string value = line.substr(semi + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t\r") + 1);
    if (value.empty()) continue;

    auto dotdot = range.find("..");
    char32_t first = std::stoul(range.substr(0, dotdot), nullptr, 16);
    char32_t last = dotdot == std::string::npos
                        ? first
                        : std::stoul(range.substr(dotdot + 2), nullptr, 16);

    auto expected = to_enum(value);
    for (char32_t cp = first; cp <= last; cp++) {
      REQUIRE(east_asian_width(cp) == expected);
    }
  }
}

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
