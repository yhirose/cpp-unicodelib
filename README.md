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

### Unicode properties

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

### Unicode code block

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

### Combination

```cpp
size_t combining_character_sequence_length(const char32_t* s32, size_t l);

bool is_grapheme_boundary(const char32_t* s32, size_t l, size_t i);
size_t grapheme_length(const char32_t* s32, size_t l);
size_t grapheme_count(const char32_t* s32, size_t l);
```

### UTF8 encode/decode

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
