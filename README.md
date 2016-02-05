cpp-unicodelib
==============

C++11 header-only Unicode library.

API
---

### Constants

```cpp
const char32_t ErrorCode = U'\U0000FFFD';
const char32_t MaxCode = U'\U0010FFFF';
const size_t UTF8MaxByteLen = 4;
```

### Functions

```cpp
size_t encode_bytes(char32_t uc)
size_t encode(char32_t uc, std::string& out)
void encode(const char32_t* s32, size_t l, std::string& out)
std::string encode(const char32_t* s32, size_t l)

size_t decode_bytes(const char* s8, size_t l)
size_t decode(const char* s8, size_t l, char32_t& out)
void decode(const char* s8, size_t l, std::u32string& out)
std::u32string decode(const char* s8, size_t l)
size_t count(const char* s8, size_t l)
void for_each(const char* s, size_t l, fn(const char* s, size_t l, size_t beg, size_t end, size_t i))
```

Tested compilers
----------------

  * Clang 3.5

License
-------

MIT license (© 2016 Yuji Hirose)
