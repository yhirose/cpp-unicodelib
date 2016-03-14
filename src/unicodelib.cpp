#include "unicodelib.h"
#include <iostream>

namespace unicode {

//-----------------------------------------------------------------------------
// Unicode properties
//-----------------------------------------------------------------------------

static GeneralCategory _general_category_properties[] = {
#include "_general_category_properties.cpp"
};

GeneralCategory general_category(char32_t cp) {
    return _general_category_properties[cp];
}

//-----------------------------------------------------------------------------
// Unicode bloks
//-----------------------------------------------------------------------------

static Block _block_values[] = {
#include "_block_values.cpp"
};

Block block(char32_t cp) {
    return _block_values[cp];
}

//-----------------------------------------------------------------------------
// Unicode text segmentation
//-----------------------------------------------------------------------------

enum class GraphemeBreak {
    CR, LF, Control, Extend, Regional_Indicator, SpacingMark, Prepend, L, V, T, LV, LVT, Unassigned
};

static GraphemeBreak _grapheme_break_properties[] = {
#include "_grapheme_break_properties.cpp"
};

bool is_grapheme_boundary(const char32_t* s32, size_t l, size_t i) {
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
    if ((lp == GraphemeBreak::CR) &&
        (rp == GraphemeBreak::LF)) {
        return false;
    }

    // GB4: (Control|CR|LF) ÷
    if ((lp == GraphemeBreak::Control ||
         lp == GraphemeBreak::CR ||
         lp == GraphemeBreak::LF)) {
        return true;
    }

    // GB5: ÷ (Control|CR|LF)
    if ((rp == GraphemeBreak::Control ||
         rp == GraphemeBreak::CR ||
         rp == GraphemeBreak::LF)) {
        return true;
    }

    // GB6: L × (L|V|LV|LVT)
    if ((lp == GraphemeBreak::L) &&
        (rp == GraphemeBreak::L ||
         rp == GraphemeBreak::V ||
         rp == GraphemeBreak::LV ||
         rp == GraphemeBreak::LVT)) {
        return false;
    }

    // GB7: (LV|V) × (V|T)
    if ((lp == GraphemeBreak::LV ||
         lp == GraphemeBreak::V) &&
        (rp == GraphemeBreak::V ||
         rp == GraphemeBreak::T)) {
        return false;
    }

    // GB8: (LVT|T) × T
    if ((lp == GraphemeBreak::LVT ||
         lp == GraphemeBreak::T) &&
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

size_t grapheme_length(const char32_t* s32, size_t l) {
    size_t i = 1;
    for (; i < l; i++) {
        if (is_grapheme_boundary(s32, l, i)) {
            return i;
        }
    }
    return i;
}

size_t grapheme_count(const char32_t* s32, size_t l) {
    size_t count = 0;
    size_t i = 0;
    while (i < l) {
        count++;
        i += grapheme_length(s32 + i, l - i);
    }
    return count;
}

} // namespace unicode
