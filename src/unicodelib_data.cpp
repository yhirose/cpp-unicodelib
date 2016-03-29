#include "unicodelib.h"
#include "unicodelib_data.h"

namespace unicode {

#include "_general_category_properties.cpp"
#include "_properties.cpp"
#include "_derived_core_properties.cpp"
#include "_simple_case_mappings.cpp"
#include "_special_case_mappings.cpp"
#include "_special_case_mappings_default.cpp"
#include "_case_foldings.cpp"
#include "_block_properties.cpp"
#include "_script_properties.cpp"
#include "_script_extension_ids.cpp"
#include "_script_extension_properties_for_id.cpp"
#include "_normalization_properties.cpp"
#include "_normalization_composition.cpp"
#include "_grapheme_break_properties.cpp"
#include "_word_break_properties.cpp"
#include "_sentence_break_properties.cpp"

} // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
