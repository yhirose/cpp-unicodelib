#include "unicodelib.h"
#include "unicodelib_data.h"
#include <algorithm>

namespace unicode {

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
// Combination
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

bool is_combining_character(char32_t cp) {
  // D52 Combining character: A character with the General Category of Combining Mark
  // (M)
  return is_mark(cp);
}

size_t combining_character_sequence_length(const char32_t *s32, size_t l) {
  // D56 Combining character sequence: A maximal character sequence consisting of either a
  // base character followed by a sequence of one or more characters where each is a
  // combining character, zero width joiner, or zero width non-joiner; or a
  // sequence of one or more characters where each is a combining character, zero
  // width joiner, or zero width non-joiner.

  const char32_t ZERO_WIDTH_JOINER = 0x200D;
  const char32_t ZERO_WIDTH_NON_JOINER = 0x200C;

  size_t i = 0;
  if (l) {
    if (is_base_character(s32[i])) {
      i++;
    }
    while (i < l) {
      auto cp = s32[i];
      if (is_combining_character(cp) ||
          cp == ZERO_WIDTH_JOINER ||
          cp == ZERO_WIDTH_NON_JOINER) {
        i++;
      } else {
        break;
      }
    }
  }
  return i;
}

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

// Implementation is based on
// http://unicode.org/reports/tr15/tr15-18.html#Hangul
namespace hangul {

// Common Constants
const char32_t SBase = 0xAC00, LBase = 0x1100, VBase = 0x1161, TBase = 0x11A7,
               LCount = 19, VCount = 21, TCount = 28,
               NCount = VCount * TCount, // 588
    SCount = LCount * NCount;            // 11172

static bool is_hangul(char32_t cp) {
  return SBase <= cp && cp < SBase + SCount;
}

static bool is_hangul_jamo(const char32_t *source, size_t len) {
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

// Hangul Decomposition
static void decompose_hangul(char32_t cp, std::u32string &out) {
  auto SIndex = cp - SBase;
  auto L = LBase + SIndex / NCount;
  auto V = VBase + (SIndex % NCount) / TCount;
  auto T = TBase + SIndex % TCount;
  out += L;
  out += V;
  if (T != TBase) {
    out += T;
  }
}

// Hangul Composition
static size_t compose_hangul(const char32_t *source, size_t len,
                             std::u32string &out) {
  auto last = source[0]; // copy first char
  out += last;

  size_t i = 1;
  for (; i < len; i++) {
    auto ch = source[i];

    // 1. check to see if two current characters are L and V
    int LIndex = last - LBase;
    if (0 <= LIndex && LIndex < LCount) {
      int VIndex = ch - VBase;
      if (0 <= VIndex && VIndex < VCount) {

        // make syllable of form LV
        last = (char32_t)(SBase + (LIndex * VCount + VIndex) * TCount);
        out.back() = last; // reset last
        continue;          // discard ch
      }
    }

    // 2. check to see if two current characters are LV and T
    int SIndex = last - SBase;
    if (0 <= SIndex && SIndex < SCount && (SIndex % TCount) == 0) {
      int TIndex = ch - TBase;
      if (0 <= TIndex && TIndex <= TCount) {

        // make syllable of form LVT
        last += TIndex;
        out.back() = last; // reset last
        continue;          // discard ch
      }
    }

    // if neither case was true, break
    break;
  }

  return i;
}

} // namespace hangul

static void decompose_code(const char32_t cp, std::u32string &out,
                           Normalization norm) {
  if (hangul::is_hangul(cp)) {
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
    if (hangul::is_hangul_jamo(s32.data() + i, s32.length() - i)) {
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
// UTF8 encode/decode
//-----------------------------------------------------------------------------

/* -- UTF-8 Specification --
   U+000000-U+00007F = 0xxxxxxx
   U+000080-U+0007FF = 110xxxxx 10yyyyyy
   U+000800-U+00FFFF = 1110xxxx 10yyyyyy 10zzzzzz
   U+010000-U+10FFFF = 11110www 10xxxxxx 10yyyyyy 10zzzzzz
*/

const size_t UTF8MaxByteLen = 4;

size_t encode_byte_length(char32_t cp) {
  if (cp < 0x0080) {
    return 1;
  } else if (cp < 0x0800) {
    return 2;
  } else if (cp < 0xD800) {
    return 3;
  } else if (cp < 0xe000) {
    return 0;
  } // D800 - DFFF is invalid...
  else if (cp < 0x10000) {
    return 3;
  } else if (cp < 0x110000) {
    return 4;
  }
  return 0;
}

size_t encode(char32_t cp, char *buff, size_t buff_len) {
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
  } else if (cp < 0xE000) { // D800 - DFFF is invalid...
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

size_t encode(char32_t cp, std::string &out) {
  char b[UTF8MaxByteLen];
  auto l = encode(cp, b, UTF8MaxByteLen);
  out.append(b, l);
  return l;
}

std::string encode(char32_t cp) {
  std::string out;
  encode(cp, out);
  return out;
}

void encode(const char32_t *s32, size_t l, std::string &out) {
  for (size_t i = 0; i < l; i++) {
    encode(s32[i], out);
  }
}

std::string encode(const char32_t *s32, size_t l) {
  std::string out;
  encode(s32, l, out);
  return out;
}

size_t decode_byte_length(const char *s8, size_t l) {
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

bool decode(const char *s8, size_t l, size_t &bytes, char32_t &cp) {
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

size_t decode(const char *s8, size_t l, char32_t &out) {
  size_t bytes;
  if (decode(s8, l, bytes, out)) {
    return bytes;
  }
  return 0;
}

template <typename T>
inline void for_each(const char *s, size_t l, T callback) {
  size_t id = 0;
  size_t i = 0;
  while (i < l) {
    auto beg = i++;
    while (i < l && (s[i] & 0xc0) == 0x80) {
      i++;
    }
    callback(s, l, beg, i, id++);
  }
}

void decode(const char *s8, size_t l, std::u32string &out) {
  for_each(s8, l,
           [&](const char *s, size_t l, size_t beg, size_t end, size_t i) {
             size_t bytes;
             char32_t cp;
             decode(&s[beg], (end - beg), bytes, cp);
             out += cp;
           });
}

std::u32string decode(const char *s8, size_t l) {
  std::u32string out;
  decode(s8, l, out);
  return out;
}

size_t codepoint_count(const char *s8, size_t l) {
  size_t count = 0;
  for (size_t i = 0; i < l; i += decode_byte_length(s8 + i, l - i)) {
    count++;
  }
  return count;
}

} // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
