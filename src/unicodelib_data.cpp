#include "unicodelib.h"
#include "unicodelib_data.h"

namespace unicode {

const GeneralCategory _general_category_properties[] = {
#include "_general_category_properties.cpp"
};

const Block _block_properties[] = {
#include "_block_properties.cpp"
};

const Script _script_properties[] = {
#include "_script_properties.cpp"
};

const int _script_extension_ids[] = {
#include "_script_extension_ids.cpp"
};

const std::vector<std::vector<Script>> _script_extension_properties_for_id = {
#include "_script_extension_properties_for_id.cpp"
};

const NormalizationProperties _normalization_properties[] = {
#include "_normalization_properties.cpp"
};

const std::unordered_map<std::u32string, char32_t> _normalization_composition =
    {
#include "_normalization_composition.cpp"
};

const GraphemeBreak _grapheme_break_properties[] = {
#include "_grapheme_break_properties.cpp"
};

} // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
