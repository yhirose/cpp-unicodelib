#include "unicodelib.h"
#include <unordered_map>
#include <vector>

namespace unicode {

struct NormalizationProperties {
  int combining_class;
  const char *compat_format;
  const char32_t *codes;
};

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

extern const GeneralCategory _general_category_properties[];
extern const Block _block_properties[];
extern const Script _script_properties[];
extern const int _script_extension_ids[];
extern const std::vector<std::vector<Script>> _script_extension_properties_for_id;
extern const NormalizationProperties _normalization_properties[];
extern const std::unordered_map<std::u32string, char32_t> _normalization_composition;
extern const GraphemeBreak _grapheme_break_properties[];

} // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
