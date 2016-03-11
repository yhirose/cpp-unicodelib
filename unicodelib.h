//
//  unicodelib.h
//
//  Copyright (c) 2016 Yuji Hirose. All rights reserved.
//  MIT License
//

#ifndef _CPPFSTLIB_UNICODELIB_H_
#define _CPPFSTLIB_UNICODELIB_H_

namespace unicode {

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const char32_t ErrorCode = U'\U0000FFFD';
const char32_t MaxCode = U'\U0010FFFF';

//-----------------------------------------------------------------------------
// Unicode properties
//-----------------------------------------------------------------------------

enum GeneralCategory {
    Lu, Ll, Lt, Lm, Lo,
    Mn, Mc, Me,
    Nd, Nl, No,
    Pc, Pd, Ps, Pe, Pi, Pf, Po,
    Sm, Sc, Sk, So,
    Zs, Zl, Zp,
    Cc, Cf, Cs, Co, Cn,
};

struct UCDEntry {
    GeneralCategory gc;
};

static UCDEntry ucd_entries_[] = {
#include "_unicode_data_entries.h"
};

inline size_t ucd_entry_size() {
    return sizeof(ucd_entries_) / sizeof(ucd_entries_[0]);
}

inline GeneralCategory general_category(char32_t cp) {
    return ucd_entries_[cp].gc;
}

inline bool is_cased_letter(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Lu:
        case Ll:
        case Lt:
            return true;
    }
    return false;
}

inline bool is_letter(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Lu:
        case Ll:
        case Lt:
        case Lm:
        case Lo:
            return true;
    }
    return false;
}

inline bool is_mark(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Mn:
        case Mc:
        case Me:
            return true;
    }
    return false;
}

inline bool is_number(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Nd:
        case Nl:
        case No:
            return true;
    }
    return false;
}

inline bool is_punctuation(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Pc:
        case Pd:
        case Ps:
        case Pe:
        case Pi:
        case Pf:
        case Po:
            return true;
    }
    return false;
}

inline bool is_symbol(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Sm:
        case Sc:
        case Sk:
        case So:
            return true;
    }
    return false;
}

inline bool is_separator(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Zs:
        case Zl:
        case Zp:
            return true;
    }
    return false;
}

inline bool is_other(char32_t cp) {
    switch (ucd_entries_[cp].gc) {
        case Cc:
        case Cf:
        case Cs:
        case Co:
        case Cn:
            return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// UTF8 encode/decode
//-----------------------------------------------------------------------------

/* -- UTF-8 Specification --
   U+000000-U+00007F = 0xxxxxxx
   U+000080-U+0007FF = 110xxxxx 10yyyyyy
   U+000800-U+00FFFF = 1110xxxx 10yyyyyy 10zzzzzz
   U+010000-U+10FFFF = 11110www 10xxxxxx 10yyyyyy 10zzzzzz
*/

const size_t UTF8MaxByteLen = 4;

inline size_t encode_byte_length(char32_t cp) {
    if (cp < 0x0080) { return 1; }
    else if (cp < 0x0800) { return 2; }
    else if (cp < 0xD800) { return 3; }
    else if (cp < 0xe000) { return 0; } // D800 - DFFF is invalid...
    else if (cp < 0x10000) { return 3; }
    else if (cp < 0x110000) { return 4; }
    return 0;
}

inline size_t encode(char32_t cp, char* buff, size_t buff_len) {
    if (cp < 0x0080) {
        buff[0] = (uint8_t)(cp & 0x7F);
        return 1;
    } else if (cp < 0x0800) {
        buff[0] = (uint8_t)(0xC0 | ((cp >> 6) & 0x1F));
        buff[1] = (uint8_t)(0x80 | (cp & 0x3F));
        return 2;
    } else if (cp < 0xD800) {
        buff[0] = (uint8_t)(0xE0 | ((cp >> 12) & 0xF));
        buff[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        buff[2] = (uint8_t)(0x80 | (cp & 0x3F));
        return 3;
    } else if (cp < 0xE000) { // D800 - DFFF is invalid...
        return 0;
    } else if (cp < 0x10000) {
        buff[0] = (uint8_t)(0xE0 | ((cp >> 12) & 0xF));
        buff[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        buff[2] = (uint8_t)(0x80 | (cp & 0x3F));
        return 3;
    } else if (cp < 0x110000) {
        buff[0] = (uint8_t)(0xF0 | ((cp >> 18) & 0x7));
        buff[1] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        buff[2] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        buff[3] = (uint8_t)(0x80 | (cp & 0x3F));
        return 4;
    }
    return 0;
}

inline size_t encode(char32_t cp, std::string& out) {
    char b[UTF8MaxByteLen];
    auto l = encode(cp, b, UTF8MaxByteLen);
    out.append(b, l);
    return l;
}

inline std::string encode(char32_t cp) {
    std::string out;
    encode(cp, out);
    return out;
}

inline void encode(const char32_t* s32, size_t l, std::string& out) {
    for (size_t i = 0; i < l; i++) {
        encode(s32[i], out);
    }
}

template<typename T>
inline void encode(const T& s32, std::string& out) {
    encode(s32.data(), s32.length(), out);
}

inline std::string encode(const char32_t* s32, size_t l) {
    std::string out;
    encode(s32, l, out);
    return out;
}

template<typename T>
inline std::string encode(const T& s32) {
    std::string out;
    encode(s32, out);
    return out;
}

inline size_t decode_byte_length(const char* s8, size_t l) {
    if (l) {
        uint8_t b = s8[0];
        if ((b & 0x80) == 0) { return 1; }
        else if ((b & 0xE0) == 0xC0) { return 2; }
        else if ((b & 0xF0) == 0xE0) { return 3; }
        else if ((b & 0xF8) == 0xF0) { return 4; }
    }
    return 0;
}

template<typename T>
inline size_t decode_byte_length(const T& s8) {
    return decode_byte_length(s8.data(), s8.length());
}

inline bool decode(const char* s8, size_t l, size_t& bytes, char32_t& cp) {
    if (l) {
        uint8_t b = s8[0];
        if ((b & 0x80) == 0) {
            bytes = 1;
            cp = b;
            return true;
        } else if ((b & 0xE0) == 0xC0) {
            if (l >= 2) {
                bytes = 2;
                cp = (((char32_t)(s8[0] & 0x1F)) << 6) |
                    ((char32_t)(s8[1] & 0x3F));
                return true;
            }
        } else if ((b & 0xF0) == 0xE0) {
            if (l >= 3) {
                bytes = 3;
                cp = (((char32_t)(s8[0] & 0x0F)) << 12) |
                    (((char32_t)(s8[1] & 0x3F)) << 6) |
                    ((char32_t)(s8[2] & 0x3F));
                return true;
            }
        } else if ((b & 0xF8) == 0xF0) {
            if (l >= 4) {
                bytes = 4;
                cp = (((char32_t)(s8[0] & 0x07)) << 18) |
                    (((char32_t)(s8[1] & 0x3F)) << 12) |
                    (((char32_t)(s8[2] & 0x3F)) << 6) |
                    ((char32_t)(s8[3] & 0x3F));
                return true;
            }
        }
    }
    return false;
}

inline size_t decode(const char* s8, size_t l, char32_t& out) {
    size_t bytes;
    if (decode(s8, l, bytes, out)) {
        return bytes;
    }
    return 0;
}

template<typename T>
inline size_t decode(const T& s8, char32_t& out) {
    return decode(s8.data(), s8.length(), out);
}

template <typename T>
inline void for_each(const char* s, size_t l, T callback) {
    size_t id = 0;
    size_t i = 0;
    while (i < l) {
        auto beg = i++;
        while (i < l && (s[i] & 0xc0) == 0x80) {
            i++;
        }
        callback(s, l, beg, i, id++);
    }
}

inline void decode(const char* s8, size_t l, std::u32string& out) {
    for_each(s8, l, [&](const char* s, size_t l, size_t beg, size_t end, size_t i) {
        size_t bytes;
        char32_t cp;
        decode(&s[beg], (end - beg), bytes, cp);
        out += cp;
    });
}

template<typename T>
inline void decode(const T& s8, std::u32string& out) {
    decode(s8.data(), s8.length(), out);
}

inline std::u32string decode(const char* s8, size_t l) {
    std::u32string out;
    decode(s8, l, out);
    return out;
}

template<typename T>
inline std::u32string decode(const T& s8) {
    std::u32string out;
    decode(s8.data(), s8.length(), out);
    return out;
}

inline size_t codepoint_count(const char* s8, size_t l) {
    size_t c = 0;
    for (size_t i = 0; i < l; i += decode_byte_length(s8 + i, l - i)) {
        c++;
    }
    return c;
}

} // namespace unicode

#endif
// vim: et ts=4 sw=4 cin cino={1s ff=unix

