cpp-unicodelib
==============

C++11 Unicode library.

API
---

## Constants

```cpp
const char32_t ErrorCode = 0x0000FFFD;
const char32_t MaxCode = 0x0010FFFF;
```

## Functions

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

bool is_graphic_character(char32_t cp);
bool is_base_character(char32_t cp);
bool is_combining_character(char32_t cp);
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
```

### Case

```cpp
char32_t simple_uppercase_mapping(char32_t cp);
char32_t simple_lowercase_mapping(char32_t cp);
char32_t simple_titlecase_mapping(char32_t cp);
char32_t simple_case_folding(char32_t cp);

std::u32string to_uppercase(const char32_t *s32, size_t l, const char *lang = nullptr);
std::u32string to_lowercase(const char32_t *s32, size_t l, const char *lang = nullptr);
std::u32string to_titlecase(const char32_t *s32, size_t l, const char *lang = nullptr); // TODO:
std::u32string to_case_fold(const char32_t *s32, size_t l, bool special_case_for_uppercase_I_and_dotted_uppercase_I = false);

bool is_uppercase(const char32_t *s32, size_t l);
bool is_lowercase(const char32_t *s32, size_t l);
bool is_titlecase(const char32_t *s32, size_t l); // TODO:
bool is_case_fold(const char32_t *s32, size_t l); // TODO:

bool caseless_match(const char32_t *s1, size_t l1, const char32_t *s2, size_t l2, bool special_case_for_uppercase_I_and_dotted_uppercase_I = false);
bool canonical_caseless_match(const char32_t *s1, size_t l1, const char32_t *s2, size_t l2); // TODO:
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

### Case

```cpp
bool is_uppsercase(const char32_t *s32, size_t l);
bool is_lowercase(const char32_t *s32, size_t l);
bool is_titlecase(const char32_t *s32, size_t l);

std::u32string to_uppsercase(const char32_t *s32, size_t l);
std::u32string to_lowercase(const char32_t *s32, size_t l);
std::u32string to_titlecase(const char32_t *s32, size_t l);
std::u32string to_case_fold(const char32_t *s32, size_t l);
```

### Normalization

```cpp
std::u32string to_nfc(const char32_t *s32, size_t l);
std::u32string to_nfd(const char32_t *s32, size_t l);
std::u32string to_nfkc(const char32_t *s32, size_t l);
std::u32string to_nfkd(const char32_t *s32, size_t l);
```

### Combination

```cpp
size_t combining_character_sequence_length(const char32_t* s32, size_t l);
size_t combining_character_sequence_count(const char32_t* s32, size_t l);

size_t extended_combining_character_sequence_length(const char32_t* s32, size_t l);
size_t extended_combining_character_sequence_count(const char32_t* s32, size_t l);

bool is_grapheme_boundary(const char32_t* s32, size_t l, size_t i);
size_t grapheme_length(const char32_t* s32, size_t l);
size_t grapheme_count(const char32_t* s32, size_t l);
```

### UTF8 Encode/Decode

```cpp
size_t encode_byte_length(char32_t uc);
size_t encode(char32_t uc, std::string& out);
void encode(const char32_t* s32, size_t l, std::string& out);
std::string encode(const char32_t* s32, size_t l);

size_t decode_byte_length(const char* s8, size_t l);
size_t decode(const char* s8, size_t l, char32_t& out);
void decode(const char* s8, size_t l, std::u32string& out);
std::u32string decode(const char* s8, size_t l);
size_t codepoint_count(const char* s8, size_t l);
```

Tested compilers
----------------

  * Clang 3.5

License
-------

MIT license (© 2016 Yuji Hirose)
