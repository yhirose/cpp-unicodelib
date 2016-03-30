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

enum class GraphemeBreak {
  Unassigned,
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
};

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
};

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

extern const GeneralCategory _general_category_properties[];
extern const uint32_t _properties[];
extern const uint32_t _derived_core_properties[];
extern const std::unordered_map<char32_t, const char32_t *>
    _simple_case_mappings;
extern const std::unordered_multimap<char32_t, SpecialCasing>
    _special_case_mappings;
extern const std::unordered_multimap<char32_t, SpecialCasing>
    _special_case_mappings_default;
extern const std::unordered_map<char32_t, CaseFolding> _case_foldings;
extern const Block _block_properties[];
extern const Script _script_properties[];
extern const int _script_extension_ids[];
extern const std::vector<std::vector<Script>>
    _script_extension_properties_for_id;
extern const NormalizationProperties _normalization_properties[];
extern const std::unordered_map<std::u32string, char32_t>
    _normalization_composition;
extern const GraphemeBreak _grapheme_break_properties[];
extern const WordBreak _word_break_properties[];
extern const SentenceBreak _sentence_break_properties[];

}  // namespace unicode

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
