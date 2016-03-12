#include "unicodelib.h"

namespace unicode {

static GeneralCategory _general_category_properties[] = {
#include "_general_category_properties.cc"
};

GeneralCategory general_category(char32_t cp) {
    return _general_category_properties[cp];
}

} // namespace unicode
