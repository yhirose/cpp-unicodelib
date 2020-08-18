#include <unordered_map>
#include <vector>
#include "unicodelib.h"

namespace unicode {

enum class CaseMappingType {
  Upper = 0,
  Lower,
  Title,
};

enum class SpecialCasingContext {
  Unassigned,
  Final_Sigma,
  Not_Final_Sigma,
  After_Soft_Dotted,
  More_Above,
  Before_Dot,
  Not_Before_Dot,
  After_I,
};

struct SpecialCasing {
  const char32_t *lower;
  const char32_t *title;
  const char32_t *upper;
  const char* language;
  SpecialCasingContext context;

  const char32_t* case_mapping_codes(CaseMappingType type) const {
    switch (type) {
    case CaseMappingType::Upper:
      return upper;
    case CaseMappingType::Lower:
      return lower;
    case CaseMappingType::Title:
      return title;
    }
    // NOTREACHED
    return nullptr;
  }
};

struct CaseFolding {
  const char32_t C;
  const char32_t S;
  const char32_t *F;
  const char32_t T;
};

struct NormalizationProperties {
  int combining_class;
  const char *compat_format;
  const char32_t *codes;
};

// This is generated from 'GraphemeBreakProperty.txt' in Unicode database.
// `python scripts/gen_property_values.py < UCD/auxiliary/GraphemeBreakProperty.txt`
enum class GraphemeBreak {
  Unassigned,
  Prepend,
  CR,
  LF,
  Control,
  Extend,
  Regional_Indicator,
  SpacingMark,
  L,
  V,
  T,
  LV,
  LVT,
  ZWJ,
};

// This is generated from 'WordBreakProperty.txt' in Unicode database.
// `python scripts/gen_property_values.py < UCD/auxiliary/WordBreakProperty.txt`
enum class WordBreak {
  Unassigned,
  Double_Quote,
  Single_Quote,
  Hebrew_Letter,
  CR,
  LF,
  Newline,
  Extend,
  Regional_Indicator,
  Format,
  Katakana,
  ALetter,
  MidLetter,
  MidNum,
  MidNumLet,
  Numeric,
  ExtendNumLet,
  ZWJ,
  WSegSpace,
};

// This is generated from 'SentenceBreakProperty.txt' in Unicode database.
// `python scripts/gen_property_values.py < UCD/auxiliary/SentenceBreakProperty.txt`
enum class SentenceBreak {
  Unassigned,
  CR,
  LF,
  Extend,
  Sep,
  Format,
  Sp,
  Lower,
  Upper,
  OLetter,
  Numeric,
  ATerm,
  STerm,
  Close,
  SContinue,
};

// This is generated from 'emoji-data.txt' in Unicode database.
// `python scripts/gen_property_values.py < UCD/emoji/emoji-data.txt`
enum class Emoji {
  Unassigned,
  Emoji,
  Emoji_Presentation,
  Emoji_Modifier,
  Emoji_Modifier_Base,
  Emoji_Component,
  Extended_Pictographic,
};

extern const std::unordered_map<char32_t, const char32_t *> _simple_case_mappings;
extern const std::unordered_multimap<char32_t, SpecialCasing> _special_case_mappings;
extern const std::unordered_multimap<char32_t, SpecialCasing> _special_case_mappings_default;
extern const std::unordered_map<char32_t, CaseFolding> _case_foldings;

extern const std::unordered_map<std::u32string, char32_t> _normalization_composition;
extern const std::vector<std::vector<Script>> _script_extension_properties_for_id;

namespace _block_properties { extern Block get_value(char32_t cp); }
namespace _derived_core_properties { extern uint32_t get_value(char32_t cp); }
namespace _emoji_properties { extern Emoji get_value(char32_t cp); }
namespace _general_category_properties{ extern GeneralCategory get_value(char32_t cp); }
namespace _grapheme_break_properties{ extern GraphemeBreak get_value(char32_t cp); }
namespace _normalization_properties { extern NormalizationProperties get_value(char32_t cp); }
namespace _properties { extern uint64_t get_value(char32_t cp); }
namespace _script_extension_ids { extern int get_value(char32_t cp); }
namespace _script_properties { extern Script get_value(char32_t cp); }
namespace _sentence_break_properties { extern SentenceBreak get_value(char32_t cp); }
namespace _word_break_properties { extern WordBreak get_value(char32_t cp); }

}  // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
