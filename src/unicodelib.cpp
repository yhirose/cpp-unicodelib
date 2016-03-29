#include "unicodelib.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include "unicodelib_data.h"

namespace unicode {

const char32_t ZERO_WIDTH_JOINER = 0x200D;
const char32_t ZERO_WIDTH_NON_JOINER = 0x200C;

namespace hangul {

//-----------------------------------------------------------------------------
// Hangul Algorithm
//-----------------------------------------------------------------------------

// Common Constants
const char32_t SBase = 0xAC00;
const char32_t LBase = 0x1100;
const char32_t VBase = 0x1161;
const char32_t TBase = 0x11A7;
const size_t LCount = 19;
const size_t VCount = 21;
const size_t TCount = 28;
const size_t NCount = VCount * TCount;  // 588
const size_t SCount = LCount * NCount;  // 11172

static bool is_precomposed_syllable(char32_t cp) {
  return SBase <= cp && cp < SBase + SCount;
}

static bool is_decomposed_syllable(const char32_t *source, size_t len) {
  if (len < 2) {
    return false;
  }

  auto first = source[0];
  auto second = source[1];

  if (LBase <= first && first < LBase + LCount) {
    if (VBase <= second && second < VBase + VCount) {
      return true;
    }
  }

  if (SBase <= first && first < SBase + SCount &&
      ((first - SBase) % TCount) == 0) {
    if (TBase <= second && second < TBase + TCount) {
      return true;
    }
  }

  return false;
}

static void decompose_hangul(char32_t cp, std::u32string &out) {
  int SIndex = cp - SBase;
  char32_t L = LBase + SIndex / NCount;
  char32_t V = VBase + (SIndex % NCount) / TCount;
  char32_t T = TBase + SIndex % TCount;
  out += L;
  out += V;
  if (T != TBase) {
    out += T;
  }
}

static size_t compose_hangul(const char32_t *source, size_t len,
                             std::u32string &out) {
  auto last = source[0];  // copy first char
  out += last;

  size_t i = 1;
  for (; i < len; i++) {
    auto ch = source[i];

    // 1. check to see if two current characters are L and V
    int LIndex = last - LBase;
    if (0 <= LIndex && LIndex < (int)LCount) {
      int VIndex = ch - VBase;
      if (0 <= VIndex && VIndex < (int)VCount) {
        // make syllable of form LV
        last = (char32_t)(SBase + (LIndex * VCount + VIndex) * TCount);
        out.back() = last;  // reset last
        continue;           // discard ch
      }
    }

    // 2. check to see if two current characters are LV and T
    int SIndex = last - SBase;
    if (0 <= SIndex && SIndex < (int)SCount && (SIndex % TCount) == 0) {
      int TIndex = ch - TBase;
      if (0 <= TIndex && TIndex <= (int)TCount) {
        // make syllable of form LVT
        last += TIndex;
        out.back() = last;  // reset last
        continue;           // discard ch
      }
    }

    // if neither case was true, break
    break;
  }

  return i;
}

}  // namespace hangul

//-----------------------------------------------------------------------------
// General Category
//-----------------------------------------------------------------------------

GeneralCategory general_category(char32_t cp) {
  return _general_category_properties[cp];
}

bool is_cased_letter_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Lu:
    case GeneralCategory::Ll:
    case GeneralCategory::Lt:
      return true;
    default:
      return false;
  }
}

bool is_letter_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Lu:
    case GeneralCategory::Ll:
    case GeneralCategory::Lt:
    case GeneralCategory::Lm:
    case GeneralCategory::Lo:
      return true;
    default:
      return false;
  }
}

bool is_mark_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Mn:
    case GeneralCategory::Mc:
    case GeneralCategory::Me:
      return true;
    default:
      return false;
  }
}

bool is_number_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Nd:
    case GeneralCategory::Nl:
    case GeneralCategory::No:
      return true;
    default:
      return false;
  }
}

bool is_punctuation_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Pc:
    case GeneralCategory::Pd:
    case GeneralCategory::Ps:
    case GeneralCategory::Pe:
    case GeneralCategory::Pi:
    case GeneralCategory::Pf:
    case GeneralCategory::Po:
      return true;
    default:
      return false;
  }
}

bool is_symbol_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Sm:
    case GeneralCategory::Sc:
    case GeneralCategory::Sk:
    case GeneralCategory::So:
      return true;
    default:
      return false;
  }
}

bool is_separator_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Zs:
    case GeneralCategory::Zl:
    case GeneralCategory::Zp:
      return true;
    default:
      return false;
  }
}

bool is_other_category(GeneralCategory gc) {
  switch (gc) {
    case GeneralCategory::Cc:
    case GeneralCategory::Cf:
    case GeneralCategory::Cs:
    case GeneralCategory::Co:
    case GeneralCategory::Cn:
      return true;
    default:
      return false;
  }
}

bool is_cased_letter(char32_t cp) {
  return is_cased_letter_category(general_category(cp));
}

bool is_letter(char32_t cp) { return is_letter_category(general_category(cp)); }

bool is_mark(char32_t cp) { return is_mark_category(general_category(cp)); }

bool is_number(char32_t cp) { return is_number_category(general_category(cp)); }

bool is_punctuation(char32_t cp) {
  return is_punctuation_category(general_category(cp));
}

bool is_symbol(char32_t cp) { return is_symbol_category(general_category(cp)); }

bool is_separator(char32_t cp) {
  return is_separator_category(general_category(cp));
}

bool is_other(char32_t cp) { return is_other_category(general_category(cp)); }

//-----------------------------------------------------------------------------
// Property
//-----------------------------------------------------------------------------

const uint32_t Property_White_Space = 0b00000000000000000000000000000001;
const uint32_t Property_Bidi_Control = 0b00000000000000000000000000000010;
const uint32_t Property_Join_Control = 0b00000000000000000000000000000100;
const uint32_t Property_Dash = 0b00000000000000000000000000001000;
const uint32_t Property_Hyphen = 0b00000000000000000000000000010000;
const uint32_t Property_Quotation_Mark = 0b00000000000000000000000000100000;
const uint32_t Property_Terminal_Punctuation =
    0b00000000000000000000000001000000;
const uint32_t Property_Other_Math = 0b00000000000000000000000010000000;
const uint32_t Property_Hex_Digit = 0b00000000000000000000000100000000;
const uint32_t Property_ASCII_Hex_Digit = 0b00000000000000000000001000000000;
const uint32_t Property_Other_Alphabetic = 0b00000000000000000000010000000000;
const uint32_t Property_Ideographic = 0b00000000000000000000100000000000;
const uint32_t Property_Diacritic = 0b00000000000000000001000000000000;
const uint32_t Property_Extender = 0b00000000000000000010000000000000;
const uint32_t Property_Other_Lowercase = 0b00000000000000000100000000000000;
const uint32_t Property_Other_Uppercase = 0b00000000000000001000000000000000;
const uint32_t Property_Noncharacter_Code_Point =
    0b00000000000000010000000000000000;
const uint32_t Property_Other_Grapheme_Extend =
    0b00000000000000100000000000000000;
const uint32_t Property_IDS_Binary_Operator =
    0b00000000000001000000000000000000;
const uint32_t Property_Radical = 0b00000000000010000000000000000000;
const uint32_t Property_Unified_Ideograph = 0b00000000000100000000000000000000;
const uint32_t Property_Other_Default_Ignorable_Code_Point =
    0b00000000001000000000000000000000;
const uint32_t Property_Deprecated = 0b00000000010000000000000000000000;
const uint32_t Property_Soft_Dotted = 0b00000000100000000000000000000000;
const uint32_t Property_Logical_Order_Exception =
    0b00000001000000000000000000000000;
const uint32_t Property_Other_ID_Start = 0b00000010000000000000000000000000;
const uint32_t Property_Other_ID_Continue = 0b00000100000000000000000000000000;
const uint32_t Property_STerm = 0b00001000000000000000000000000000;
const uint32_t Property_Variation_Selector = 0b00010000000000000000000000000000;
const uint32_t Property_Pattern_White_Space =
    0b00100000000000000000000000000000;
const uint32_t Property_Pattern_Syntax = 0b01000000000000000000000000000000;

bool is_white_space(char32_t cp) {
  return _properties[cp] & Property_White_Space;
}

bool is_bidi_control(char32_t cp) {
  return _properties[cp] & Property_Bidi_Control;
}

bool is_join_control(char32_t cp) {
  return _properties[cp] & Property_Join_Control;
}

bool is_dash(char32_t cp) { return _properties[cp] & Property_Dash; }

bool is_hyphen(char32_t cp) { return _properties[cp] & Property_Hyphen; }

bool is_quotation_mark(char32_t cp) {
  return _properties[cp] & Property_Quotation_Mark;
}

bool is_terminal_punctuation(char32_t cp) {
  return _properties[cp] & Property_Terminal_Punctuation;
}

bool is_other_math(char32_t cp) {
  return _properties[cp] & Property_Other_Math;
}

bool is_hex_digit(char32_t cp) { return _properties[cp] & Property_Hex_Digit; }

bool is_ascii_hex_digit(char32_t cp) {
  return _properties[cp] & Property_ASCII_Hex_Digit;
}

bool is_other_alphabetic(char32_t cp) {
  return _properties[cp] & Property_Other_Alphabetic;
}

bool is_ideographic(char32_t cp) {
  return _properties[cp] & Property_Ideographic;
}

bool is_diacritic(char32_t cp) { return _properties[cp] & Property_Diacritic; }

bool is_extender(char32_t cp) { return _properties[cp] & Property_Extender; }

bool is_other_lowercase(char32_t cp) {
  return _properties[cp] & Property_Other_Lowercase;
}

bool is_other_uppercase(char32_t cp) {
  return _properties[cp] & Property_Other_Uppercase;
}

bool is_noncharacter_code_point(char32_t cp) {
  return _properties[cp] & Property_Noncharacter_Code_Point;
}

bool is_other_grapheme_extend(char32_t cp) {
  return _properties[cp] & Property_Other_Grapheme_Extend;
}

bool is_ids_binary_operator(char32_t cp) {
  return _properties[cp] & Property_IDS_Binary_Operator;
}

bool is_radical(char32_t cp) { return _properties[cp] & Property_Radical; }

bool is_unified_ideograph(char32_t cp) {
  return _properties[cp] & Property_Unified_Ideograph;
}

bool is_other_default_ignorable_code_point(char32_t cp) {
  return _properties[cp] & Property_Other_Default_Ignorable_Code_Point;
}

bool is_deprecated(char32_t cp) {
  return _properties[cp] & Property_Deprecated;
}

bool is_soft_dotted(char32_t cp) {
  return _properties[cp] & Property_Soft_Dotted;
}

bool is_logical_order_exception(char32_t cp) {
  return _properties[cp] & Property_Logical_Order_Exception;
}

bool is_other_id_start(char32_t cp) {
  return _properties[cp] & Property_Other_ID_Start;
}

bool is_other_id_continue(char32_t cp) {
  return _properties[cp] & Property_Other_ID_Continue;
}

bool is_sterm(char32_t cp) { return _properties[cp] & Property_STerm; }

bool is_variation_selector(char32_t cp) {
  return _properties[cp] & Property_Variation_Selector;
}

bool is_pattern_white_space(char32_t cp) {
  return _properties[cp] & Property_Pattern_White_Space;
}

bool is_pattern_syntax(char32_t cp) {
  return _properties[cp] & Property_Pattern_Syntax;
}

//-----------------------------------------------------------------------------
// Derived Property
//-----------------------------------------------------------------------------

const uint32_t DerivedProperty_Math = 0b00000000000000000000000000000001;
const uint32_t DerivedProperty_Alphabetic = 0b00000000000000000000000000000010;
const uint32_t DerivedProperty_Lowercase = 0b00000000000000000000000000000100;
const uint32_t DerivedProperty_Uppercase = 0b00000000000000000000000000001000;
const uint32_t DerivedProperty_Cased = 0b00000000000000000000000000010000;
const uint32_t DerivedProperty_Case_Ignorable =
    0b00000000000000000000000000100000;
const uint32_t DerivedProperty_Changes_When_Lowercased =
    0b00000000000000000000000001000000;
const uint32_t DerivedProperty_Changes_When_Uppercased =
    0b00000000000000000000000010000000;
const uint32_t DerivedProperty_Changes_When_Titlecased =
    0b00000000000000000000000100000000;
const uint32_t DerivedProperty_Changes_When_Casefolded =
    0b00000000000000000000001000000000;
const uint32_t DerivedProperty_Changes_When_Casemapped =
    0b00000000000000000000010000000000;
const uint32_t DerivedProperty_ID_Start = 0b00000000000000000000100000000000;
const uint32_t DerivedProperty_ID_Continue = 0b00000000000000000001000000000000;
const uint32_t DerivedProperty_XID_Start = 0b00000000000000000010000000000000;
const uint32_t DerivedProperty_XID_Continue =
    0b00000000000000000100000000000000;
const uint32_t DerivedProperty_Default_Ignorable_Code_Point =
    0b00000000000000001000000000000000;
const uint32_t DerivedProperty_Grapheme_Extend =
    0b00000000000000010000000000000000;
const uint32_t DerivedProperty_Grapheme_Base =
    0b00000000000000100000000000000000;
const uint32_t DerivedProperty_Grapheme_Link =
    0b00000000000001000000000000000000;

bool is_math(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Math;
}

bool is_alphabetic(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Alphabetic;
}

bool is_lowercase(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Lowercase;
}

bool is_uppercase(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Uppercase;
}

bool is_cased(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Cased;
}

bool is_case_ignorable(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Case_Ignorable;
}

bool is_changes_when_lowercased(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Changes_When_Lowercased;
}

bool is_changes_when_uppercased(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Changes_When_Uppercased;
}

bool is_changes_when_titlecased(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Changes_When_Titlecased;
}

bool is_changes_when_casefolded(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Changes_When_Casefolded;
}

bool is_changes_when_casemapped(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Changes_When_Casemapped;
}

bool is_id_start(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_ID_Start;
}

bool is_id_continue(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_ID_Continue;
}

bool is_xid_start(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_XID_Start;
}

bool is_xid_continue(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_XID_Continue;
}

bool is_default_ignorable_code_point(char32_t cp) {
  return _derived_core_properties[cp] &
         DerivedProperty_Default_Ignorable_Code_Point;
}

bool is_grapheme_extend(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Grapheme_Extend;
}

bool is_grapheme_base(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Grapheme_Base;
}

bool is_grapheme_link(char32_t cp) {
  return _derived_core_properties[cp] & DerivedProperty_Grapheme_Link;
}

//-----------------------------------------------------------------------------
// Case
//-----------------------------------------------------------------------------

static char32_t simple_case_mapping(char32_t cp, CaseMappingType type) {
  using namespace std;
  auto it = _simple_case_mappings.find(cp);
  if (it != _simple_case_mappings.end()) {
    return it->second[(size_t)type];
  }
  return cp;
}

char32_t simple_uppercase_mapping(char32_t cp) {
  return simple_case_mapping(cp, CaseMappingType::Upper);
}

char32_t simple_lowercase_mapping(char32_t cp) {
  return simple_case_mapping(cp, CaseMappingType::Lower);
}

char32_t simple_titlecase_mapping(char32_t cp) {
  return simple_case_mapping(cp, CaseMappingType::Title);
}

char32_t simple_case_folding(char32_t cp) {
  auto it = _case_foldings.find(cp);
  if (it != _case_foldings.end()) {
    const auto &cf = it->second;
    if (cf.S) {
      return cf.S;
    }
    return cf.C;
  }
  return cp;
}

static bool is_language_qualified(const char *user_lang,
                                  const char *spec_lang) {
  return !spec_lang || (user_lang && !strcmp(user_lang, spec_lang));
}

static bool is_final_sigma(const char32_t *s32, size_t l, size_t i) {
  //  C is preceded by a sequence consisting of a cased letter and
  //  then zero or more case-ignorable characters, and C is not
  //  followed by a sequence consisting of zero or more case-ignorable
  //  characters and then a cased letter

  //  Before C: \p{cased} (\p{case-ignorable})*
  auto pos = (int)i - 1;
  while (pos >= 0 && is_case_ignorable(s32[pos])) {
    pos--;
  }
  if (pos < 0 || !is_cased(s32[pos])) {
    return false;
  }

  //  After C: !((\p{case-ignorable})* \p{cased})
  pos = (int)i + 1;
  while (pos < (int)l && is_case_ignorable(s32[pos])) {
    pos++;
  }
  if (pos < (int)l && is_cased(s32[pos])) {
    return false;
  }

  return true;
}

static void full_case_mapping(const char32_t *s32, size_t l, size_t i,
                              const char *lang, CaseMappingType type,
                              std::u32string &out) {
  assert(i < l);
  auto cp = s32[i];
  auto count = _special_case_mappings.count(cp);
  if (count != 0) {
    auto r = _special_case_mappings.equal_range(cp);
    for (auto it = r.first; it != r.second; ++it) {
      const auto &sc = it->second;
      if (is_language_qualified(lang, sc.language)) {
        switch (sc.context) {
          case SpecialCasingContext::Final_Sigma:
            if (is_final_sigma(s32, l, i)) {
              out += sc.case_mapping_codes(type);
              return;
            }
            break;
          case SpecialCasingContext::Not_Final_Sigma:
            if (!is_final_sigma(s32, l, i)) {
              out += sc.case_mapping_codes(type);
              return;
            }
            break;
          case SpecialCasingContext::After_Soft_Dotted:
            // TODO:
            break;
          case SpecialCasingContext::More_Above:
            // TODO:
            break;
          case SpecialCasingContext::Before_Dot:
          case SpecialCasingContext::Not_Before_Dot:
            // TODO:
            break;
          case SpecialCasingContext::After_I:
            // TODO:
            break;
          default:
            // NOTREACHED
            break;
        }
      }
    }
  }

  auto it = _special_case_mappings_default.find(cp);
  if (it != _special_case_mappings_default.end()) {
    out += it->second.case_mapping_codes(type);
  } else {
    out += simple_case_mapping(s32[i], type);
  }
}

void uppercase_mapping(const char32_t *s32, size_t l, size_t i,
                       const char *lang, std::u32string &out) {
  full_case_mapping(s32, l, i, lang, CaseMappingType::Upper, out);
}

static void lowercase_mapping(const char32_t *s32, size_t l, size_t i,
                              const char *lang, std::u32string &out) {
  full_case_mapping(s32, l, i, lang, CaseMappingType::Lower, out);
}

void titlecase_mapping(const char32_t *s32, size_t l, size_t i,
                       const char *lang, std::u32string &out) {
  full_case_mapping(s32, l, i, lang, CaseMappingType::Title, out);
}

std::u32string to_uppercase(const char32_t *s32, size_t l, const char *lang) {
  // R1 toUppercase(X): Map each character C in X to Uppercase_Mapping(C)
  std::u32string out;
  for (size_t i = 0; i < l; i++) {
    uppercase_mapping(s32, l, i, lang, out);
  }
  return out;
}

std::u32string to_lowercase(const char32_t *s32, size_t l, const char *lang) {
  // R2 toLowercase(X): Map each character C in X to Lowercase_Mapping(C)
  std::u32string out;
  for (size_t i = 0; i < l; i++) {
    lowercase_mapping(s32, l, i, lang, out);
  }
  return out;
}

std::u32string to_titlecase(const char32_t *s32, size_t l, const char *lang) {
  // R3 toTitlecase(X): Find the word boundaries in X according to Unicode
  // Standard Annex #29, “Unicode Text Segmentation.” For each word boundary,
  // find the first cased character F following the word boundary. If F exists,
  // map F to Titlecase_Mapping(F); then map all characters C between F and the
  // following word boundary to Lowercase_Mapping(C)
  std::u32string out;
  size_t i = 0;
  while (i < l) {
    while (i < l && !is_cased(s32[i])) {
      out += s32[i];
      i++;
    }

    if (i == l) {
      break;
    }

    titlecase_mapping(s32, l, i, lang, out);
    i++;

    if (i == l) {
      break;
    }

    while (i < l && !is_word_boundary(s32, l, i)) {
      lowercase_mapping(s32, l, i, lang, out);
      i++;
    }
  }
  return out;
}

static void case_folding(
    char32_t cp, bool special_case_for_uppercase_I_and_dotted_uppercase_I,
    std::u32string &out) {
  auto it = _case_foldings.find(cp);
  if (it != _case_foldings.end()) {
    const auto &cf = it->second;
    if (special_case_for_uppercase_I_and_dotted_uppercase_I && cf.T) {
      out += cf.T;
      return;
    } else if (cf.F) {
      out += cf.F;
      return;
    } else if (cf.S) {
      out += cf.S;
      return;
    } else if (cf.C) {
      out += cf.C;
      return;
    }
  }
  out += cp;
}

std::u32string to_case_fold(
    const char32_t *s32, size_t l,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I) {
  // R4 toCasefold(X): Map each character C in X to Case_Folding(C)
  std::u32string out;
  for (size_t i = 0; i < l; i++) {
    case_folding(s32[i], special_case_for_uppercase_I_and_dotted_uppercase_I,
                 out);
  }
  return out;
}

bool is_uppercase(const char32_t *s32, size_t l) {
  // D140 isUppercase(X): isUppercase(X) is true when toUppercase(Y) = Y
  for (size_t i = 0; i < l; i++) {
    if (is_changes_when_uppercased(s32[i])) {
      return false;
    }
  }
  return true;
}

bool is_lowercase(const char32_t *s32, size_t l) {
  // D139 isLowercase(X): isLowercase(X) is true when toLowercase(Y) = Y
  for (size_t i = 0; i < l; i++) {
    if (is_changes_when_lowercased(s32[i])) {
      return false;
    }
  }
  return true;
}

bool is_titlecase(const char32_t *s32, size_t l) {
  // D141 isTitlecase(X): isTitlecase(X) is true when toTitlecase(Y) = Y
  size_t i = 0;
  while (i < l) {
    while (i < l && !is_cased(s32[i])) {
      if (is_changes_when_lowercased(s32[i])) {
        return false;
      }
      i++;
    }

    if (i == l) {
      break;
    }

    if (is_changes_when_titlecased(s32[i])) {
      return false;
    }
    i++;

    if (i == l) {
      break;
    }

    while (i < l && !is_word_boundary(s32, l, i)) {
      if (is_changes_when_lowercased(s32[i])) {
        return false;
      }
      i++;
    }
  }

  return true;
}

bool is_case_fold(const char32_t *s32, size_t l) {
  // D142 isCasefolded(X): isCasefolded(X) is true when toCasefold(Y) = Y
  for (size_t i = 0; i < l; i++) {
    if (is_changes_when_casefolded(s32[i])) {
      return false;
    }
  }
  return true;
}

bool caseless_match(const char32_t *s1, size_t l1, const char32_t *s2,
                    size_t l2,
                    bool special_case_for_uppercase_I_and_dotted_uppercase_I) {
  // D144 A string X is a caseless match for a string Y if and only if
  // toCasefold(X) = toCasefold(Y)
  return to_case_fold(s1, l1,
                      special_case_for_uppercase_I_and_dotted_uppercase_I) ==
         to_case_fold(s2, l2,
                      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

bool canonical_caseless_match(
    const char32_t *s1, size_t l1, const char32_t *s2, size_t l2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I) {
  // D145 A string X is a canonical caseless match for a string Y if and only if
  // NFD(toCasefold(NFD(X))) = NFD(toCasefold(NFD(Y)))
  return to_nfd(to_case_fold(
             to_nfd(s1, l1),
             special_case_for_uppercase_I_and_dotted_uppercase_I)) ==
         to_nfd(
             to_case_fold(to_nfd(s2, l2),
                          special_case_for_uppercase_I_and_dotted_uppercase_I));
}

bool compatibility_caseless_match(
    const char32_t *s1, size_t l1, const char32_t *s2, size_t l2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I) {
  // D146 A string X is a compatibility caseless match for a string Y if and
  // only if NFKD(toCasefold(NFKD(toCasefold(NFD(X))))) =
  // NFKD(toCasefold(NFKD(toCasefold(NFD(Y)))))
  return to_nfkd(to_case_fold(
             to_nfkd(to_case_fold(
                 to_nfd(s1, l1),
                 special_case_for_uppercase_I_and_dotted_uppercase_I)),
             special_case_for_uppercase_I_and_dotted_uppercase_I)) ==
         to_nfkd(to_case_fold(
             to_nfkd(to_case_fold(
                 to_nfd(s2, l2),
                 special_case_for_uppercase_I_and_dotted_uppercase_I)),
             special_case_for_uppercase_I_and_dotted_uppercase_I));
}

//-----------------------------------------------------------------------------
// Combining Character Sequence
//-----------------------------------------------------------------------------

bool is_graphic_character(char32_t cp) {
  // D50 Graphic character: A character with the General Category of Letter (L),
  // Combining Mark (M), Number (N), Punctuation (P), Symbol (S), or Space
  // Separator (Zs).
  return is_base_character(cp) || is_mark(cp);
}

bool is_base_character(char32_t cp) {
  // D51 Base character: Any graphic character except for those with the General
  // Category of  Combining Mark (M).
  auto gc = general_category(cp);
  switch (gc) {
    case GeneralCategory::Zs:
      return true;
    default:
      return is_letter_category(gc) || is_number_category(gc) ||
             is_punctuation_category(gc) || is_symbol_category(gc);
  }
}

static bool is_standard_korean_syllable_block(const char32_t *s32, size_t l,
                                              size_t &length) {
  // D134 Standard Korean syllable block: A sequence of one or more L followed
  // by a sequence of one or more V and a sequence of zero or more T, or any
  // other sequence that is canonically equivalent.
  size_t i = 0;
  if (i == l || _grapheme_break_properties[s32[i]] != GraphemeBreak::L) {
    return false;
  }
  i++;
  while (i < l && _grapheme_break_properties[s32[i]] == GraphemeBreak::L) {
    i++;
  }
  if (i == l || _grapheme_break_properties[s32[i]] != GraphemeBreak::V) {
    return false;
  }
  i++;
  while (i < l && _grapheme_break_properties[s32[i]] == GraphemeBreak::V) {
    i++;
  }
  while (i < l || _grapheme_break_properties[s32[i]] == GraphemeBreak::T) {
    i++;
  }
  length = i;
  return true;
}

static bool is_extended_base(const char32_t *s32, size_t l, size_t &length) {
  // D51a Extended base: Any base character, or any standard Korean syllable
  // block.
  if (l > 0) {
    if (is_standard_korean_syllable_block(s32, l, length)) {
      return true;
    } else if (is_base_character(s32[0])) {
      length = 1;
      return true;
    }
  }
  return false;
}

bool is_combining_character(char32_t cp) {
  // D52 Combining character: A character with the General Category of Combining
  // Mark (M)
  return is_mark(cp);
}

size_t combining_character_sequence_length(const char32_t *s32, size_t l) {
  // D56 Combining character sequence: A maximal character sequence consisting
  // of either a base character followed by a sequence of one or more characters
  // where each is a combining character, zero width joiner, or zero width
  // non-joiner; or a sequence of one or more characters where each is a
  // combining character, zero width joiner, or zero width non-joiner.
  size_t i = 0;
  if (l) {
    if (is_base_character(s32[i])) {
      i++;
    }
    while (i < l) {
      auto cp = s32[i];
      if (is_combining_character(cp) || cp == ZERO_WIDTH_JOINER ||
          cp == ZERO_WIDTH_NON_JOINER) {
        i++;
      } else {
        break;
      }
    }
  }
  return i;
}

size_t extended_combining_character_sequence_length(const char32_t *s32,
                                                    size_t l) {
  // D56a Extended combining character sequence: A maximal character sequence
  // consisting of either an extended base followed by a sequence of one or more
  // characters where each is a combining character, zero width joiner, or zero
  // width non-joiner; or a sequence of one or more characters where each is a
  // combining character, zero width joiner, or zero width non-joiner.
  size_t i = 0;
  if (l) {
    size_t length;
    if (is_extended_base(s32, l, length)) {
      i += length;
    }
    while (i < l) {
      auto cp = s32[i];
      if (is_combining_character(cp) || cp == ZERO_WIDTH_JOINER ||
          cp == ZERO_WIDTH_NON_JOINER) {
        i++;
      } else {
        break;
      }
    }
  }
  return i;
}

size_t combining_character_sequence_count(const char32_t *s32, size_t l) {
  size_t count = 0;
  size_t i = 0;
  while (i < l) {
    count++;
    i += combining_character_sequence_length(s32 + i, l - i);
  }
  return count;
}

size_t extended_combining_character_sequence_count(const char32_t *s32,
                                                   size_t l) {
  size_t count = 0;
  size_t i = 0;
  while (i < l) {
    count++;
    i += extended_combining_character_sequence_length(s32 + i, l - i);
  }
  return count;
}

//-----------------------------------------------------------------------------
// Grapheme Cluster Segmentation
//-----------------------------------------------------------------------------

bool is_grapheme_boundary(const char32_t *s32, size_t l, size_t i) {
  // GB1: sot ÷
  if (i == 0) {
    return true;
  }

  // GB2: ÷ eot
  if (i == l) {
    return true;
  }

  auto lp = _grapheme_break_properties[s32[i - 1]];
  auto rp = _grapheme_break_properties[s32[i]];

  // GB3: CR × LF
  if ((lp == GraphemeBreak::CR) && (rp == GraphemeBreak::LF)) {
    return false;
  }

  // GB4: (Control|CR|LF) ÷
  if ((lp == GraphemeBreak::Control || lp == GraphemeBreak::CR ||
       lp == GraphemeBreak::LF)) {
    return true;
  }

  // GB5: ÷ (Control|CR|LF)
  if ((rp == GraphemeBreak::Control || rp == GraphemeBreak::CR ||
       rp == GraphemeBreak::LF)) {
    return true;
  }

  // GB6: L × (L|V|LV|LVT)
  if ((lp == GraphemeBreak::L) &&
      (rp == GraphemeBreak::L || rp == GraphemeBreak::V ||
       rp == GraphemeBreak::LV || rp == GraphemeBreak::LVT)) {
    return false;
  }

  // GB7: (LV|V) × (V|T)
  if ((lp == GraphemeBreak::LV || lp == GraphemeBreak::V) &&
      (rp == GraphemeBreak::V || rp == GraphemeBreak::T)) {
    return false;
  }

  // GB8: (LVT|T) × T
  if ((lp == GraphemeBreak::LVT || lp == GraphemeBreak::T) &&
      (rp == GraphemeBreak::T)) {
    return false;
  }

  // GB8a: Regional_Indicator × Regional_Indicator
  if ((lp == GraphemeBreak::Regional_Indicator) &&
      (rp == GraphemeBreak::Regional_Indicator)) {
    return false;
  }

  // GB9: × Extend
  if (rp == GraphemeBreak::Extend) {
    return false;
  }

  // GB9a: × SpacingMark
  if (rp == GraphemeBreak::SpacingMark) {
    return false;
  }

  // GB9b: Prepend ×
  if (lp == GraphemeBreak::Prepend) {
    return false;
  }

  // GB10: Any ÷ Any
  return true;
}

size_t grapheme_length(const char32_t *s32, size_t l) {
  size_t i = 1;
  for (; i < l; i++) {
    if (is_grapheme_boundary(s32, l, i)) {
      return i;
    }
  }
  return i;
}

size_t grapheme_count(const char32_t *s32, size_t l) {
  size_t count = 0;
  size_t i = 0;
  while (i < l) {
    count++;
    i += grapheme_length(s32 + i, l - i);
  }
  return count;
}

//-----------------------------------------------------------------------------
// Word Segmentation
//-----------------------------------------------------------------------------

inline bool AHLetter(WordBreak p) {
  return p == WordBreak::ALetter || p == WordBreak::Hebrew_Letter;
}

inline bool MidNumLetQ(WordBreak p) {
  return p == WordBreak::MidNumLet || p == WordBreak::Single_Quote;
}

static int previous_word_break_property_position(const char32_t *s32,
                                                 size_t i) {
  auto prop = WordBreak::Unassigned;
  auto pos = (int)i - 1;
  while (pos >= 0) {
    prop = _word_break_properties[s32[pos]];
    if (prop != WordBreak::Extend && prop != WordBreak::Format) {
      break;
    }
    pos--;
  }
  return pos;
}

static size_t next_word_break_property_position(const char32_t *s32, size_t l,
                                                size_t i) {
  auto prop = WordBreak::Unassigned;
  auto pos = i + 1;
  while (pos < l) {
    prop = _word_break_properties[s32[pos]];
    if (prop != WordBreak::Extend && prop != WordBreak::Format) {
      break;
    }
    pos++;
  }
  return pos;
}

bool is_word_boundary(const char32_t *s32, size_t l, size_t i) {
  // WB1: sot ÷
  if (i == 0) {
    return true;
  }

  // WB2: ÷ eot
  if (i == l) {
    return true;
  }

  auto lp = _word_break_properties[s32[i - 1]];
  auto rp = _word_break_properties[s32[i]];

  // WB3: CR × LF
  if ((lp == WordBreak::CR) && (rp == WordBreak::LF)) {
    return false;
  }

  // WB3a: (Newline|CR|LF) ÷
  if ((lp == WordBreak::Newline || lp == WordBreak::CR ||
       lp == WordBreak::LF)) {
    return true;
  }

  // WB3b: ÷ (Newline|CR|LF)
  if ((rp == WordBreak::Newline || rp == WordBreak::CR ||
       rp == WordBreak::LF)) {
    return true;
  }

  // WB4: X (Extend | Format)* → X

  // Ignore Format and Extend characters, except when they appear at the
  // beginning of a region of text
  if ((rp == WordBreak::Extend || rp == WordBreak::Format)) {
    return false;
  }

  // Find left property
  lp = WordBreak::Unassigned;
  auto lpos = previous_word_break_property_position(s32, i);
  if (lpos >= 0) {
    lp = _word_break_properties[s32[lpos]];
  }

  // WB5: AHLetter × AHLetter
  if (AHLetter(lp) && AHLetter(rp)) {
    return false;
  }

  WordBreak rp1 = WordBreak::Unassigned;
  auto rpos = next_word_break_property_position(s32, l, i);
  if (rpos < l) {
    rp1 = _word_break_properties[s32[rpos]];
  }

  // WB6: AHLetter × (MidLetter | MidNumLetQ) AHLetter
  if ((AHLetter(lp)) &&
      ((rp == WordBreak::MidLetter || MidNumLetQ(rp)) && AHLetter(rp1))) {
    return false;
  }

  auto lp1 = WordBreak::Unassigned;
  lpos = previous_word_break_property_position(s32, lpos);
  if (lpos >= 0) {
    lp1 = _word_break_properties[s32[lpos]];
  }

  // WB7: AHLetter (MidLetter | MidNumLetQ) × AHLetter
  if ((AHLetter(lp1) && (lp == WordBreak::MidLetter || MidNumLetQ(lp))) &&
      (AHLetter(rp))) {
    return false;
  }

  // WB7a: Hebrew_Letter × Single_Quote
  if ((lp == WordBreak::Hebrew_Letter) && (rp == WordBreak::Single_Quote)) {
    return false;
  }

  // WB7b: Hebrew_Letter × Double_Quote Hebrew_Letter
  if ((lp == WordBreak::Hebrew_Letter) &&
      (rp == WordBreak::Double_Quote && rp1 == WordBreak::Hebrew_Letter)) {
    return false;
  }

  // WB7c: Hebrew_Letter Double_Quote × Hebrew_Letter
  if ((lp1 == WordBreak::Hebrew_Letter && lp == WordBreak::Double_Quote) &&
      (rp == WordBreak::Hebrew_Letter)) {
    return false;
  }

  // WB8: Numeric × Numeric
  if ((lp == WordBreak::Numeric) && (rp == WordBreak::Numeric)) {
    return false;
  }

  // WB9: AHLetter × Numeric
  if ((AHLetter(lp)) && (rp == WordBreak::Numeric)) {
    return false;
  }

  // WB10: Numeric × AHLetter
  if ((lp == WordBreak::Numeric) && (AHLetter(rp))) {
    return false;
  }

  // WB11: Numeric (MidNum | MidNumLetQ) × Numeric
  if ((lp1 == WordBreak::Numeric &&
       (lp == WordBreak::MidNum || MidNumLetQ(lp))) &&
      (rp == WordBreak::Numeric)) {
    return false;
  }

  // WB12: Numeric × (MidNum | MidNumLetQ) Numeric
  if ((lp == WordBreak::Numeric) &&
      ((rp == WordBreak::MidNum || MidNumLetQ(rp)) &&
       rp1 == WordBreak::Numeric)) {
    return false;
  }

  // WB13: Katakana × Katakana
  if ((lp == WordBreak::Katakana) && (rp == WordBreak::Katakana)) {
    return false;
  }

  // WB13a: (AHLetter | Numeric | Katakana | ExtendNumLet) × ExtendNumLet
  if ((AHLetter(lp) || lp == WordBreak::Katakana || lp == WordBreak::Numeric ||
       lp == WordBreak::Katakana || lp == WordBreak::ExtendNumLet) &&
      (rp == WordBreak::ExtendNumLet)) {
    return false;
  }

  // WB13b: ExtendNumLet × (AHLetter | Numeric | Katakana)
  if ((lp == WordBreak::ExtendNumLet) &&
      (AHLetter(rp) || rp == WordBreak::Numeric || rp == WordBreak::Katakana)) {
    return false;
  }

  // WB13c: Regional_Indicator × Regional_Indicator
  if ((lp == WordBreak::Regional_Indicator) &&
      (rp == WordBreak::Regional_Indicator)) {
    return false;
  }

  // WB14: Any ÷ Any
  return true;
}

//-----------------------------------------------------------------------------
// Sentence Segmentation
//-----------------------------------------------------------------------------

inline bool ParaSep(SentenceBreak p) {
  return p == SentenceBreak::Sep || p == SentenceBreak::CR ||
         p == SentenceBreak::LF;
}

inline bool SATerm(SentenceBreak p) {
  return p == SentenceBreak::STerm || p == SentenceBreak::ATerm;
}

static int previous_sentence_break_property_position(const char32_t *s32,
                                                     size_t i) {
  auto prop = SentenceBreak::Unassigned;
  auto pos = (int)i - 1;
  while (pos >= 0) {
    prop = _sentence_break_properties[s32[pos]];
    if (prop != SentenceBreak::Extend && prop != SentenceBreak::Format) {
      break;
    }
    pos--;
  }
  return pos;
}

static size_t next_sentence_break_property_position(const char32_t *s32,
                                                    size_t l, size_t i) {
  auto pos = i + 1;
  while (pos < l) {
    auto prop = _sentence_break_properties[s32[pos]];
    if (prop != SentenceBreak::Extend && prop != SentenceBreak::Format) {
      break;
    }
    pos++;
  }
  return pos;
}

bool is_sentence_boundary(const char32_t *s32, size_t l, size_t i) {
  // SB1: sot ÷
  if (i == 0) {
    return true;
  }

  // SB2: ÷ eot
  if (i == l) {
    return true;
  }

  auto lp = _sentence_break_properties[s32[i - 1]];
  auto rp = _sentence_break_properties[s32[i]];

  // SB3: CR × LF
  if ((lp == SentenceBreak::CR) && (rp == SentenceBreak::LF)) {
    return false;
  }

  // SB4: ParaSep ÷
  if (ParaSep(lp)) {
    return true;
  }

  // SB5: X (Extend | Format)* → X

  // Ignore Format and Extend characters, except when they appear at the
  // beginning of a region of text
  if ((rp == SentenceBreak::Extend || rp == SentenceBreak::Format)) {
    return false;
  }

  // Find left property
  lp = SentenceBreak::Unassigned;
  auto lpos = previous_sentence_break_property_position(s32, i);
  if (lpos >= 0) {
    lp = _sentence_break_properties[s32[lpos]];
  }

  // SB6: ATerm × Numeric
  if ((lp == SentenceBreak::ATerm) && (rp == SentenceBreak::Numeric)) {
    return false;
  }

  auto lp1 = SentenceBreak::Unassigned;
  if (lpos > 0) {
    lpos = previous_sentence_break_property_position(s32, lpos);
    if (lpos >= 0) {
      lp1 = _sentence_break_properties[s32[lpos]];
    }
  }

  // SB7: (Upper | Lower) ATerm × Upper
  if (((lp1 == SentenceBreak::Upper || lp1 == SentenceBreak::Lower) &&
       (lp == SentenceBreak::ATerm)) &&
      (rp == SentenceBreak::Upper)) {
    return false;
  }

  SentenceBreak lp2 = SentenceBreak::Unassigned;
  {
    lp2 = SentenceBreak::Unassigned;
    auto pos = previous_sentence_break_property_position(s32, i);
    while (pos >= 0) {
      lp2 = _sentence_break_properties[s32[pos]];
      if (lp2 != SentenceBreak::Sp) {
        break;
      }
      pos = previous_sentence_break_property_position(s32, pos);
    }
    while (pos >= 0) {
      lp2 = _sentence_break_properties[s32[pos]];
      if (lp2 != SentenceBreak::Close) {
        break;
      }
      pos = previous_sentence_break_property_position(s32, pos);
    }
  }

  SentenceBreak rp2 = SentenceBreak::Unassigned;
  {
    size_t pos = i;
    while (pos < l) {
      rp2 = _sentence_break_properties[s32[pos]];
      if (ParaSep(rp2) || SATerm(rp2) || rp2 == SentenceBreak::OLetter ||
          rp2 == SentenceBreak::Upper || rp2 == SentenceBreak::Lower) {
        break;
      }
      pos = next_sentence_break_property_position(s32, l, pos);
    }
  }

  // SB8: ATerm Close* Sp* × (¬(OLetter | Upper | Lower | ParaSep | SATerm))*
  // Lower
  if ((lp2 == SentenceBreak::ATerm) && (rp2 == SentenceBreak::Lower)) {
    return false;
  }

  // SB8a: SATerm Close* Sp* × (SContinue | SATerm)
  if ((SATerm(lp2)) && (rp == SentenceBreak::SContinue || SATerm(rp))) {
    return false;
  }

  SentenceBreak lp3 = SentenceBreak::Unassigned;
  {
    auto pos = previous_sentence_break_property_position(s32, i);
    while (pos >= 0) {
      lp3 = _sentence_break_properties[s32[pos]];
      if (lp3 != SentenceBreak::Close) {
        break;
      }
      pos = previous_sentence_break_property_position(s32, pos);
    }
  }

  // SB9: SATerm Close* × (Close | Sp | ParaSep)
  if ((SATerm(lp3)) &&
      (rp == SentenceBreak::Close || rp == SentenceBreak::Sp || ParaSep(rp))) {
    return false;
  }

  // SB10: SATerm Close* Sp* × (Sp | ParaSep)
  if ((SATerm(lp2)) && (rp == SentenceBreak::Sp || ParaSep(rp))) {
    return false;
  }

  // SB11: SATerm Close* Sp* ParaSep? ÷
  // NOTE: 'ParaSep?' has been handled in SB4.
  if (SATerm(lp2)) {
    return true;
  }

  // SB12: Any × Any
  return false;
}

//-----------------------------------------------------------------------------
// Block
//-----------------------------------------------------------------------------

Block block(char32_t cp) { return _block_properties[cp]; }

//-----------------------------------------------------------------------------
// Script
//-----------------------------------------------------------------------------

Script script(char32_t cp) { return _script_properties[cp]; }

bool is_script(Script sc, char32_t cp) {
  auto val = script(cp);
  switch (val) {
    case Script::Common:
    case Script::Inherited: {
      auto id = _script_extension_ids[cp];
      const auto &props = _script_extension_properties_for_id[id];
      return std::find(props.begin(), props.end(), sc) != props.end();
    }
    default:
      return sc == val;
  }
}

//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

enum class Normalization {
  NFC,
  NFD,
  NFKC,
  NFKD,
};

static void decompose_code(const char32_t cp, std::u32string &out,
                           Normalization norm) {
  if (hangul::is_precomposed_syllable(cp)) {
    hangul::decompose_hangul(cp, out);
  } else {
    const auto &prop = _normalization_properties[cp];
    if (prop.codes && (!prop.compat_format || norm == Normalization::NFKC ||
                       norm == Normalization::NFKD)) {
      size_t i = 0;
      while (prop.codes[i]) {
        decompose_code(prop.codes[i], out, norm);
        i++;
      }
    } else {
      out += cp;
    }
  }
}

static std::u32string decompose(const char32_t *s32, size_t l,
                                Normalization norm) {
  std::u32string out;

  // Decompose
  for (size_t i = 0; i < l; i++) {
    decompose_code(s32[i], out, norm);
  }

  // Reorder combining marks with 'Canonical Ordering Algorithm'.
  for (size_t i = 0; i < out.length(); i++) {
    const auto &prop = _normalization_properties[out[i]];
    if (prop.combining_class > 0) {
      for (size_t j = i; j > 0; j--) {
        auto prev = out[j - 1];
        auto curr = out[j];
        const auto &prop_prev = _normalization_properties[prev];
        const auto &prop_curr = _normalization_properties[curr];
        if (prop_prev.combining_class <= prop_curr.combining_class) {
          break;
        }
        std::swap(out[j - 1], out[j]);
      }
    }
  }

  return out;
}

static bool compose_pair(char32_t cp0, char32_t cp1, char32_t &cp) {
  std::u32string key = {cp0, cp1};
  auto it = _normalization_composition.find(key);
  if (it != _normalization_composition.end()) {
    cp = it->second;
    return true;
  }
  return false;
}

static size_t compose_codes(const char32_t *s32, size_t l,
                            std::u32string &out) {
  auto starter = s32[0];
  std::vector<bool> cheched(l, false);

  // Compose successively
  bool handled = true;
  while (handled) {
    handled = false;
    int max_class = -1;
    size_t i = 1;
    for (; i < l; i++) {
      if (!cheched[i]) {
        auto klass = _normalization_properties[s32[i]].combining_class;
        if (max_class < klass) {
          if (compose_pair(starter, s32[i], starter)) {
            handled = true;
            cheched[i] = true;
            break;
          }
        }
        if (klass == 0) {
          break;
        }
        max_class = std::max(max_class, klass);
      }
    }
  }

  // Output
  out += starter;
  size_t i = 1;
  for (; i < l; i++) {
    if (!cheched[i]) {
      if (_normalization_properties[s32[i]].combining_class == 0) {
        break;
      }
      out += s32[i];
    }
  }

  return i;
}

static std::u32string compose(const std::u32string &s32) {
  std::u32string out;
  size_t i = 0;
  while (i < s32.length()) {
    if (hangul::is_decomposed_syllable(s32.data() + i, s32.length() - i)) {
      i += hangul::compose_hangul(s32.data() + i, s32.length() - i, out);
    } else {
      i += compose_codes(s32.data() + i, s32.length() - i, out);
    }
  }
  return out;
}

std::u32string to_nfc(const char32_t *s32, size_t l) {
  return compose(decompose(s32, l, Normalization::NFC));
}

std::u32string to_nfd(const char32_t *s32, size_t l) {
  return decompose(s32, l, Normalization::NFD);
}

std::u32string to_nfkc(const char32_t *s32, size_t l) {
  return compose(decompose(s32, l, Normalization::NFKC));
}

std::u32string to_nfkd(const char32_t *s32, size_t l) {
  return decompose(s32, l, Normalization::NFKD);
}

//-----------------------------------------------------------------------------
// UTF8 encoding
//-----------------------------------------------------------------------------

namespace utf8 {

size_t codepoint_length(char32_t cp) {
  if (cp < 0x0080) {
    return 1;
  } else if (cp < 0x0800) {
    return 2;
  } else if (cp < 0xD800) {
    return 3;
  } else if (cp < 0xe000) {
    // D800 - DFFF is invalid...
    return 0;
  } else if (cp < 0x10000) {
    return 3;
  } else if (cp < 0x110000) {
    return 4;
  }
  return 0;
}

size_t codepoint_length(const char *s8, size_t l) {
  if (l) {
    uint8_t b = s8[0];
    if ((b & 0x80) == 0) {
      return 1;
    } else if ((b & 0xE0) == 0xC0) {
      return 2;
    } else if ((b & 0xF0) == 0xE0) {
      return 3;
    } else if ((b & 0xF8) == 0xF0) {
      return 4;
    }
  }
  return 0;
}

size_t codepoint_count(const char *s8, size_t l) {
  size_t count = 0;
  for (size_t i = 0; i < l; i += codepoint_length(s8 + i, l - i)) {
    count++;
  }
  return count;
}

static size_t encode_codepoint(char32_t cp, char *buff) {
  if (cp < 0x0080) {
    buff[0] = (uint8_t)(cp & 0x7F);
    return 1;
  } else if (cp < 0x0800) {
    buff[0] = (uint8_t)(0xC0 | ((cp >> 6) & 0x1F));
    buff[1] = (uint8_t)(0x80 | (cp & 0x3F));
    return 2;
  } else if (cp < 0xD800) {
    buff[0] = (uint8_t)(0xE0 | ((cp >> 12) & 0xF));
    buff[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
    buff[2] = (uint8_t)(0x80 | (cp & 0x3F));
    return 3;
  } else if (cp < 0xE000) {
    // D800 - DFFF is invalid...
    return 0;
  } else if (cp < 0x10000) {
    buff[0] = (uint8_t)(0xE0 | ((cp >> 12) & 0xF));
    buff[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
    buff[2] = (uint8_t)(0x80 | (cp & 0x3F));
    return 3;
  } else if (cp < 0x110000) {
    buff[0] = (uint8_t)(0xF0 | ((cp >> 18) & 0x7));
    buff[1] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
    buff[2] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
    buff[3] = (uint8_t)(0x80 | (cp & 0x3F));
    return 4;
  }
  return 0;
}

size_t encode_codepoint(char32_t cp, std::string &out) {
  char buff[4];
  auto l = encode_codepoint(cp, buff);
  out.append(buff, l);
  return l;
}

void encode(const char32_t *s32, size_t l, std::string &out) {
  for (size_t i = 0; i < l; i++) {
    encode_codepoint(s32[i], out);
  }
}

static bool decode_codepoint(const char *s8, size_t l, size_t &bytes,
                             char32_t &cp) {
  if (l) {
    uint8_t b = s8[0];
    if ((b & 0x80) == 0) {
      bytes = 1;
      cp = b;
      return true;
    } else if ((b & 0xE0) == 0xC0) {
      if (l >= 2) {
        bytes = 2;
        cp = (((char32_t)(s8[0] & 0x1F)) << 6) | ((char32_t)(s8[1] & 0x3F));
        return true;
      }
    } else if ((b & 0xF0) == 0xE0) {
      if (l >= 3) {
        bytes = 3;
        cp = (((char32_t)(s8[0] & 0x0F)) << 12) |
             (((char32_t)(s8[1] & 0x3F)) << 6) | ((char32_t)(s8[2] & 0x3F));
        return true;
      }
    } else if ((b & 0xF8) == 0xF0) {
      if (l >= 4) {
        bytes = 4;
        cp = (((char32_t)(s8[0] & 0x07)) << 18) |
             (((char32_t)(s8[1] & 0x3F)) << 12) |
             (((char32_t)(s8[2] & 0x3F)) << 6) | ((char32_t)(s8[3] & 0x3F));
        return true;
      }
    }
  }
  return false;
}

size_t decode_codepoint(const char *s8, size_t l, char32_t &out) {
  size_t bytes;
  if (decode_codepoint(s8, l, bytes, out)) {
    return bytes;
  }
  return 0;
}

template <typename T>
inline void for_each(const char *s8, size_t l, T callback) {
  size_t id = 0;
  size_t i = 0;
  while (i < l) {
    auto beg = i++;
    while (i < l && (s8[i] & 0xc0) == 0x80) {
      i++;
    }
    callback(s8, l, beg, i, id++);
  }
}

void decode(const char *s8, size_t l, std::u32string &out) {
  for_each(s8, l,
           [&](const char *s, size_t l, size_t beg, size_t end, size_t i) {
             size_t bytes;
             char32_t cp;
             decode_codepoint(&s[beg], (end - beg), bytes, cp);
             out += cp;
           });
}

}  // namespace utf8

//-----------------------------------------------------------------------------
// UTF16 encoding
//-----------------------------------------------------------------------------

namespace utf16 {

inline bool is_surrogate_pair(const char16_t *s16, size_t l) {
  if (l > 0) {
    auto first = s16[0];
    if (0xD800 <= first && first < 0xDC00) {
      auto second = s16[1];
      if (0xDC00 <= second && second < 0xE000) {
        return true;
      }
    }
  }
  return false;
}

size_t codepoint_length(char32_t cp) { return cp <= 0xFFFF ? 1 : 2; }

size_t codepoint_length(const char16_t *s16, size_t l) {
  if (l > 0) {
    if (is_surrogate_pair(s16, l)) {
      return 2;
    }
    return 1;
  }
  return 0;
}

size_t codepoint_count(const char16_t *s16, size_t l) {
  size_t count = 0;
  for (size_t i = 0; i < l; i += codepoint_length(s16 + i, l - i)) {
    count++;
  }
  return count;
}

static size_t encode_codepoint(char32_t cp, char16_t *buff) {
  if (cp < 0xD800) {
    buff[0] = (char16_t)cp;
    return 1;
  } else if (cp < 0xE000) {
    // D800 - DFFF is invalid...
    return 0;
  } else if (cp < 0x10000) {
    buff[0] = (char16_t)cp;
    return 1;
  } else if (cp < 0x110000) {
    // high surrogate
    buff[0] = (char16_t)(0xD800 + (((cp - 0x10000) >> 10) & 0x3FF));
    // low surrogate
    buff[1] = (char16_t)(0xDC00 + ((cp - 0x10000) & 0x3FF));
    return 2;
  }
  return 0;
}

size_t encode_codepoint(char32_t cp, std::u16string &out) {
  char16_t buff[2];
  auto l = encode_codepoint(cp, buff);
  out.append(buff, l);
  return l;
}

void encode(const char32_t *s32, size_t l, std::u16string &out) {
  for (size_t i = 0; i < l; i++) {
    encode_codepoint(s32[i], out);
  }
}

static bool decode_codepoint(const char16_t *s16, size_t l, size_t &length,
                             char32_t &cp) {
  if (l) {
    // Surrogate
    auto first = s16[0];
    if (0xD800 <= first && first < 0xDC00) {
      if (l >= 2) {
        auto second = s16[1];
        if (0xDC00 <= second && second < 0xE000) {
          cp = (((first - 0xD800) << 10) | (second - 0xDC00)) + 0x10000;
          length = 2;
          return true;
        }
      }
    }

    // Non surrogate
    else {
      cp = first;
      length = 1;
      return true;
    }
  }

  return false;
}

size_t decode_codepoint(const char16_t *s16, size_t l, char32_t &out) {
  size_t length;
  if (decode_codepoint(s16, l, length, out)) {
    return length;
  }
  return 0;
}

template <typename T>
inline void for_each(const char16_t *s16, size_t l, T callback) {
  size_t id = 0;
  size_t i = 0;
  while (i < l) {
    auto beg = i++;
    if (is_surrogate_pair(&s16[beg], l - beg)) {
      i++;
    }
    callback(s16, l, beg, i, id++);
  }
}

void decode(const char16_t *s16, size_t l, std::u32string &out) {
  for_each(s16, l,
           [&](const char16_t *s, size_t l, size_t beg, size_t end, size_t i) {
             size_t length;
             char32_t cp;
             decode_codepoint(&s[beg], (end - beg), length, cp);
             out += cp;
           });
}

}  // namespace utf16

}  // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
