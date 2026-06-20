cpp-unicodelib
==============

[![](https://github.com/yhirose/cpp-unicodelib/workflows/CMake/badge.svg)](https://github.com/yhirose/cpp-unicodelib/actions)

A C++17 single-file header-only Unicode library. (Unicode 17.0.0)

API
---

## Functions

### Unicode Scalar Value

```cpp
bool is_scalar_value(char32_t cp); // false for surrogates and code points beyond U+10FFFF
```

### Unicode Property

#### General Category

```cpp
GeneralCategory general_category(char32_t cp);

bool is_cased_letter_category(GeneralCategory gc);
bool is_letter_category(GeneralCategory gc);
bool is_mark_category(GeneralCategory gc);
bool is_number_category(GeneralCategory gc);
bool is_punctuation_category(GeneralCategory gc);
bool is_symbol_category(GeneralCategory gc);
bool is_separator_category(GeneralCategory gc);
bool is_other_category(GeneralCategory gc);

bool is_cased_letter(char32_t cp);
bool is_letter(char32_t cp);
bool is_mark(char32_t cp);
bool is_number(char32_t cp);
bool is_punctuation(char32_t cp);
bool is_symbol(char32_t cp);
bool is_separator(char32_t cp);
bool is_other(char32_t cp);
```

#### Property

```cpp
bool is_white_space(char32_t cp);
bool is_bidi_control(char32_t cp);
bool is_join_control(char32_t cp);
bool is_dash(char32_t cp);
bool is_hyphen(char32_t cp);
bool is_quotation_mark(char32_t cp);
bool is_terminal_punctuation(char32_t cp);
bool is_other_math(char32_t cp);
bool is_hex_digit(char32_t cp);
bool is_ascii_hex_digit(char32_t cp);
bool is_other_alphabetic(char32_t cp);
bool is_ideographic(char32_t cp);
bool is_diacritic(char32_t cp);
bool is_extender(char32_t cp);
bool is_other_lowercase(char32_t cp);
bool is_other_uppercase(char32_t cp);
bool is_noncharacter_code_point(char32_t cp);
bool is_other_grapheme_extend(char32_t cp);
bool is_ids_binary_operator(char32_t cp);
bool is_radical(char32_t cp);
bool is_unified_ideograph(char32_t cp);
bool is_other_default_ignorable_code_point(char32_t cp);
bool is_deprecated(char32_t cp);
bool is_soft_dotted(char32_t cp);
bool is_logical_order_exception(char32_t cp);
bool is_other_id_start(char32_t cp);
bool is_other_id_continue(char32_t cp);
bool is_sterm(char32_t cp);
bool is_variation_selector(char32_t cp);
bool is_pattern_white_space(char32_t cp);
bool is_pattern_syntax(char32_t cp);
```

#### Derived Property

```cpp
bool is_math(char32_t cp);
bool is_alphabetic(char32_t cp);
bool is_lowercase(char32_t cp);
bool is_uppercase(char32_t cp);
bool is_cased(char32_t cp);
bool is_case_ignorable(char32_t cp);
bool is_changes_when_lowercased(char32_t cp);
bool is_changes_when_uppercased(char32_t cp);
bool is_changes_when_titlecased(char32_t cp);
bool is_changes_when_casefolded(char32_t cp);
bool is_changes_when_casemapped(char32_t cp);
bool is_id_start(char32_t cp);
bool is_id_continue(char32_t cp);
bool is_xid_start(char32_t cp);
bool is_xid_continue(char32_t cp);
bool is_default_ignorable_code_point(char32_t cp);
bool is_grapheme_extend(char32_t cp);
bool is_grapheme_base(char32_t cp);
bool is_grapheme_link(char32_t cp);
bool is_indic_conjunct_break_linker(char32_t cp);
bool is_indic_conjunct_break_consonant(char32_t cp);
bool is_indic_conjunct_break_extend(char32_t cp);
```

### Case

```cpp
char32_t simple_uppercase_mapping(char32_t cp);
char32_t simple_lowercase_mapping(char32_t cp);
char32_t simple_titlecase_mapping(char32_t cp);
char32_t simple_case_folding(char32_t cp);

std::u32string to_uppercase(const char32_t *s32, size_t l, const CaseOptions &options = {});
std::u32string to_lowercase(const char32_t *s32, size_t l, const CaseOptions &options = {});
std::u32string to_titlecase(const char32_t *s32, size_t l, const CaseOptions &options = {});
std::u32string to_case_fold(const char32_t *s32, size_t l, const CaseOptions &options = {});

bool is_uppercase(const char32_t *s32, size_t l);
bool is_lowercase(const char32_t *s32, size_t l);
bool is_titlecase(const char32_t *s32, size_t l);
bool is_case_fold(const char32_t *s32, size_t l);

bool caseless_match(const char32_t *s1, size_t l1, const char32_t *s2, size_t l2, const CaseOptions &options = {});
bool canonical_caseless_match(const char32_t *s1, size_t l1, const char32_t *s2, size_t l2, const CaseOptions &options = {});
bool compatibility_caseless_match(const char32_t *s1, size_t l1, const char32_t *s2, size_t l2, const CaseOptions &options = {});
```

#### Locale and tailorings

The case operations take a `CaseOptions` value that bundles a locale and a set of
opt-in tailorings. It is implicitly constructible from a locale tag, a `Locale`,
or a `CaseTailoring`, so most call sites stay terse:

```cpp
struct CaseOptions {
  Locale locale;
  CaseTailoring tailoring = CaseTailoring::None;
};

enum class CaseTailoring : unsigned {
  None = 0,
  TurkicCaseFold = 1u << 0,       // optional "T" mappings in CaseFolding.txt
  GermanCapitalSharpS = 1u << 1,  // uppercase ß (U+00DF) as ẞ (U+1E9E)
};
```

`Locale` is matched by its primary language subtag, case-insensitively, so
`"tr"`, `"tr-TR"`, `"TR"` and `"tr_TR"` are all treated as Turkish. The locale
drives the language-sensitive mappings — Turkish/Azeri/Lithuanian (from
`SpecialCasing.txt`) and the Dutch `IJ` titlecasing tailoring:

```cpp
to_titlecase(U"ijsje", "nl");          // "IJsje"
to_titlecase(U"ijsje", "nl-BE");       // "IJsje"
```

`CaseTailoring` holds opt-in behaviors that are *not* implied by the locale.
Flags combine with `operator|`, and a locale can be combined with flags too:

```cpp
to_uppercase(U"Maße");                                       // "MASSE" (default)
to_uppercase(U"Maße", CaseTailoring::GermanCapitalSharpS);   // "MAẞE"
to_uppercase(U"Straße", Locale{"de"} | CaseTailoring::GermanCapitalSharpS);
to_case_fold(U"İ", CaseTailoring::TurkicCaseFold);
```

### Code Block

```cpp
Block block(char32_t cp)
```

### Script

```cpp
Script script(char32_t cp);
bool is_script(Script sc, char32_t cp); // Script Extension support
```

### Normalization

```cpp
std::u32string to_nfc(const char32_t *s32, size_t l);
std::u32string to_nfd(const char32_t *s32, size_t l);
std::u32string to_nfkc(const char32_t *s32, size_t l);
std::u32string to_nfkd(const char32_t *s32, size_t l);
```

### Combining Character Sequence

```cpp
bool is_graphic_character(char32_t cp);
bool is_base_character(char32_t cp);
bool is_combining_character(char32_t cp);

size_t combining_character_sequence_length(const char32_t* s32, size_t l);
size_t combining_character_sequence_count(const char32_t* s32, size_t l);

size_t extended_combining_character_sequence_length(const char32_t* s32, size_t l);
size_t extended_combining_character_sequence_count(const char32_t* s32, size_t l);
```

### Text Segmentation

```cpp
bool is_grapheme_boundary(const char32_t* s32, size_t l, size_t i);
size_t grapheme_length(const char32_t* s32, size_t l);
size_t grapheme_count(const char32_t* s32, size_t l);

bool is_word_boundary(const char32_t *s32, size_t l, size_t i);

bool is_sentence_boundary(const char32_t *s32, size_t l, size_t i);
```

### East Asian Width / Display Width

```cpp
// East_Asian_Width property (UAX #11)
enum class EastAsianWidth { Neutral, Ambiguous, Halfwidth, Wide, Fullwidth, Narrow };
EastAsianWidth east_asian_width(char32_t cp);

// How Ambiguous-width characters are counted as terminal columns
enum class AmbiguousWidth { Narrow, Wide };

// Display width (terminal columns) of a single code point: 0 / 1 / 2
int codepoint_width(char32_t cp, AmbiguousWidth amb = AmbiguousWidth::Narrow);

// Display width of a string, measured per extended grapheme cluster so that
// emoji ZWJ sequences, flags, keycaps and base+variation-selector sequences
// each count as one cell group.
int width(const char32_t* s32, size_t l, AmbiguousWidth amb = AmbiguousWidth::Narrow);
int width(const char32_t* s32, AmbiguousWidth amb = AmbiguousWidth::Narrow);
int width(std::u32string_view s32, AmbiguousWidth amb = AmbiguousWidth::Narrow);
```

### Encoding

#### UTF8 Encoding

```cpp
namespace utf8 {

size_t codepoint_length(char32_t uc);
size_t codepoint_length(const char* s8, size_t l);
size_t codepoint_count(const char* s8, size_t l);

size_t encode_codepoint(char32_t uc, std::string& out);
void encode(const char32_t* s32, size_t l, std::string& out);

size_t decode_codepoint(const char* s8, size_t l, char32_t& out);
void decode(const char* s8, size_t l, std::u32string& out);

}
```

#### UTF16 Encoding

```cpp
namespace utf16 {

size_t codepoint_length(char32_t uc);
size_t codepoint_length(const char16_t* s16, size_t l);
size_t codepoint_count(const char16_t* s16, size_t l);

size_t encode_codepoint(char32_t uc, std::u16string& out);
void encode(const char32_t* s32, size_t l, std::u16string& out);

size_t decode_codepoint(const char16_t* s16, size_t l, char32_t& out);
void decode(const char16_t* s16, size_t l, std::u32string& out);

}
```

#### std::wstring Conversion

```cpp
std::wstring to_wstring(const char *s8, size_t l);
std::wstring to_wstring(const char *s16, size_t l);
std::wstring to_wstring(const char32_t *s32, size_t l);
std::string to_utf8(const wchar_t *sw, size_t l);
std::u16string to_utf16(const wchar_t *sw, size_t l);
std::u32string to_utf32(const wchar_t *sw, size_t l);
```

License
-------

MIT license (© 2025 Yuji Hirose)
