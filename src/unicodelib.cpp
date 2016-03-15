#include "unicodelib.h"
#include <algorithm>
#include <vector>

namespace unicode {

//-----------------------------------------------------------------------------
// Unicode properties
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
// Blocks
//-----------------------------------------------------------------------------

static const Block _block_properties[] = {
#include "_block_properties.cpp"
};

Block block(char32_t cp) { return _block_properties[cp]; }

//-----------------------------------------------------------------------------
// Scripts
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
// Unicode text segmentation
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
