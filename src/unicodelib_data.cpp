#include "unicodelib.h"
#include "unicodelib_data.h"

namespace unicode {

#include "_general_category_properties.cpp"
#include "_block_properties.cpp"
#include "_script_properties.cpp"
#include "_script_extension_ids.cpp"
#include "_script_extension_properties_for_id.cpp"
#include "_normalization_properties.cpp"
#include "_normalization_composition.cpp"
#include "_grapheme_break_properties.cpp"

} // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
