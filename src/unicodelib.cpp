#include "unicodelib.h"
#include <algorithm>
#include <vector>

namespace unicode {

//-----------------------------------------------------------------------------
// General Category
//-----------------------------------------------------------------------------

static const GeneralCategory _general_category_properties[] = {
#include "_general_category_properties.cpp"
};

GeneralCategory general_category(char32_t cp) {
  return _general_category_properties[cp];
}

inline bool is_cased_letter(char32_t cp) {
  switch (general_category(cp)) {
  case GeneralCategory::Lu:
  case GeneralCategory::Ll:
  case GeneralCategory::Lt:
    return true;
  default:
    return false;
  }
}

inline bool is_letter(char32_t cp) {
  switch (general_category(cp)) {
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

inline bool is_mark(char32_t cp) {
  switch (general_category(cp)) {
  case GeneralCategory::Mn:
  case GeneralCategory::Mc:
  case GeneralCategory::Me:
    return true;
  default:
    return false;
  }
}

inline bool is_number(char32_t cp) {
  switch (general_category(cp)) {
  case GeneralCategory::Nd:
  case GeneralCategory::Nl:
  case GeneralCategory::No:
    return true;
  default:
    return false;
  }
}

inline bool is_punctuation(char32_t cp) {
  switch (general_category(cp)) {
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

inline bool is_symbol(char32_t cp) {
  switch (general_category(cp)) {
  case GeneralCategory::Sm:
  case GeneralCategory::Sc:
  case GeneralCategory::Sk:
  case GeneralCategory::So:
    return true;
  default:
    return false;
  }
}

inline bool is_separator(char32_t cp) {
  switch (general_category(cp)) {
  case GeneralCategory::Zs:
  case GeneralCategory::Zl:
  case GeneralCategory::Zp:
    return true;
  default:
    return false;
  }
}

inline bool is_other(char32_t cp) {
  switch (general_category(cp)) {
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

bool is_general_category(GeneralCategory gc, char32_t cp) {
  switch (gc) {
  case GeneralCategory::L:
    return is_letter(cp);
  case GeneralCategory::LC:
    return is_cased_letter(cp);
  case GeneralCategory::M:
    return is_mark(cp);
  case GeneralCategory::N:
    return is_number(cp);
  case GeneralCategory::P:
    return is_punctuation(cp);
  case GeneralCategory::S:
    return is_symbol(cp);
  case GeneralCategory::Z:
    return is_separator(cp);
  case GeneralCategory::C:
    return is_other(cp);
  default:
    return general_category(cp) == gc;
  }
}

//-----------------------------------------------------------------------------
// Block
//-----------------------------------------------------------------------------

static const Block _block_properties[] = {
#include "_block_properties.cpp"
};

Block block(char32_t cp) { return _block_properties[cp]; }

//-----------------------------------------------------------------------------
// Script
//-----------------------------------------------------------------------------

static const Script _script_properties[] = {
#include "_script_properties.cpp"
};

static const int _script_extension_ids[] = {
#include "_script_extension_ids.cpp"
};

static const std::vector<std::vector<Script>>
    _script_extension_properties_for_id = {
#include "_script_extension_properties_for_id.cpp"
};

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

struct NormalizationProperties {
  const char32_t cp;
  int combining_class;
  const char *compat_format;
  const char32_t *codes;
};

static const NormalizationProperties _normalization_properties[] = {
#include "_normalization_properties.cpp"
};

// Hangul: // http://unicode.org/reports/tr15/tr15-18.html#Hangul
// Common Constants
const char32_t SBase = 0xAC00, LBase = 0x1100, VBase = 0x1161, TBase = 0x11A7,
               LCount = 19, VCount = 21, TCount = 28,
               NCount = VCount * TCount, // 588
    SCount = LCount * NCount;            // 11172

inline bool is_hangul(char32_t cp) {
  return SBase <= cp && cp < SBase + SCount;
}

// Hangul Decomposition
inline void decompose_hangul(char32_t cp, std::u32string &out) {
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

inline void decompose(const char32_t cp, std::u32string &out,
                           Normalization norm) {
  if (is_hangul(cp)) {
    decompose_hangul(cp, out);
  } else {
    const auto &prop = _normalization_properties[cp];
    if (prop.codes && (!prop.compat_format || norm == Normalization::NFKC ||
                       norm == Normalization::NFKD)) {
      size_t i = 0;
      while (prop.codes[i]) {
        decompose(prop.codes[i], out, norm);
        i++;
      }
    } else {
      out += cp;
    }
  }
}

inline std::u32string normalize(const char32_t *s32, size_t l,
                                Normalization norm) {
  std::u32string out;

  // Decompose
  for (size_t i = 0; i < l; i++) {
    decompose(s32[i], out, norm);
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

std::u32string to_nfc(const char32_t *s32, size_t l) {
  return normalize(s32, l, Normalization::NFC);
}

std::u32string to_nfd(const char32_t *s32, size_t l) {
  return normalize(s32, l, Normalization::NFD);
}

std::u32string to_nfkc(const char32_t *s32, size_t l) {
  return normalize(s32, l, Normalization::NFKC);
}

std::u32string to_nfkd(const char32_t *s32, size_t l) {
  return normalize(s32, l, Normalization::NFKD);
}

//-----------------------------------------------------------------------------
// Text segmentation
//-----------------------------------------------------------------------------

enum class GraphemeBreak {
  CR,
  LF,
  Control,
  Extend,
  Regional_Indicator,
  SpacingMark,
  Prepend,
  L,
  V,
  T,
  LV,
  LVT,
  Unassigned
};

static const GraphemeBreak _grapheme_break_properties[] = {
#include "_grapheme_break_properties.cpp"
};

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

} // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
