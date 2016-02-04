//
//  unicodelib.h
//
//  Copyright (c) 2016 Yuji Hirose. All rights reserved.
//  MIT License
//

#ifndef _CPPFSTLIB_UNICODELIB_H_
#define _CPPFSTLIB_UNICODELIB_H_

namespace unicodelib {

/*
   -- UTF-8 Specification --
   U+000000-U+00007F = 0xxxxxxx
   U+000080-U+0007FF = 110xxxxx 10yyyyyy
   U+000800-U+00FFFF = 1110xxxx 10yyyyyy 10zzzzzz
   U+010000-U+10FFFF = 11110www 10xxxxxx 10yyyyyy 10zzzzzz

   -- UTF-16 Specification --
   Modified UniScalar = 0000yyyy yyyyyyxx xxxxxxxx (Sub 0x10000 from UniScalar)
   Hi-Surrogate       = 110110yy yyyyyyyy
   Lo-Surrogate       = 110111xx xxxxxxxx
*/

template <typename T>
inline void enumerate_u8(const char* s, size_t len, T callback) {
    size_t i = 0;
    while (i < len) {
        auto beg = i++;
        while (i < len && (s[i] & 0xc0) == 0x80) {
            i++;
        }
        callback(s, beg, i);
    }
}

template <typename T>
inline void enumerate_u8(const std::string& s, T callback) {
    enumerate_u8(s.data(), s.length(), callback);
}

inline size_t to_u8(char32_t us, char* buff, size_t buff_len) {
    if (us < 0x0080) {
        buff[0] = (uint8_t)(us & 0x7f);
        return 1;
    } else if (us < 0x0800) {
        buff[0] = (uint8_t)(0xc0 | ((us >> 6) & 0x1f));
        buff[1] = (uint8_t)(0x80 | (us & 0x3f));
        return 2;
    } else if (us < 0xd800) {
        buff[0] = (uint8_t)(0xe0 | ((us >> 12) & 0xf));
        buff[1] = (uint8_t)(0x80 | ((us >> 6) & 0x3f));
        buff[2] = (uint8_t)(0x80 | (us & 0x3f));
        return 3;
    } else if (us < 0xe000) { // D800 - DFFF is invalid...
        return 0;
    } else if (us < 0x10000) {
        buff[0] = (uint8_t)(0xe0 | ((us >> 12) & 0xf));
        buff[1] = (uint8_t)(0x80 | ((us >> 6) & 0x3f));
        buff[2] = (uint8_t)(0x80 | (us & 0x3f));
        return 3;
    } else if (us < 0x110000) {
        buff[0] = (uint8_t)(0xf0 | ((us >> 18) & 0x7));
        buff[1] = (uint8_t)(0x80 | ((us >> 12) & 0x3f));
        buff[2] = (uint8_t)(0x80 | ((us >> 6) & 0x3f));
        buff[3] = (uint8_t)(0x80 | (us & 0x3f));
        return 4;
    }
    return 0;
}

inline std::string to_u8(char32_t us) {
    const size_t buff_len = 4;
    char buff[buff_len];
    auto len = to_u8(us, buff, buff_len);
    return std::string{buff, len};
}

inline std::string to_u8string(const std::u32string& s32) {
    std::string s;
    for (char32_t us: s32) {
        s += to_u8(us);
    }
    return s;
}

inline bool to_u32(const char* buff, size_t buff_len, size_t& read_buff_len, char32_t& us) {
    if (buff_len) {
        uint8_t code1 = buff[0];
        if ((code1 & 0x80) == 0) {
            read_buff_len = 1;
            us = code1;
            return true;
        } else if ((code1 & 0xe0) == 0xc0) {
            if (buff_len >= 2) {
                read_buff_len = 2;
                us = (((char32_t)(buff[0] & 0x1f)) << 6) |
                    ((char32_t)(buff[1] & 0x3f));
                return true;
            }
        } else if ((code1 & 0xf0) == 0xe0) {
            if (buff_len >= 3) {
                read_buff_len = 3;
                us = (((char32_t)(buff[0] & 0x0f)) << 12) |
                    (((char32_t)(buff[1] & 0x3f)) << 6) |
                    ((char32_t)(buff[2] & 0x3f));
                return true;
            }
        } else if ((code1 & 0xf8) == 0xf0) {
            if (buff_len >= 4) {
                read_buff_len = 4;
                us = (((char32_t)(buff[0] & 0x07)) << 18) |
                    (((char32_t)(buff[1] & 0x3f)) << 12) |
                    (((char32_t)(buff[2] & 0x3f)) << 6) |
                    ((char32_t)(buff[3] & 0x3f));
                return true;
            }
        }
    }
    return false;
}

inline char32_t to_u32(const std::string& u8, size_t& len) {
    char32_t us;
    to_u32(u8.data(), u8.size(), len, us);
    return us;
}

inline std::u32string to_u32string(const std::string& s) {
    std::u32string s32;
    enumerate_u8(s, [&](const auto& s, auto beg, auto end) {
        size_t len;
        char32_t us;
        to_u32(&s[beg], (end - beg), len, us);
        s32 += us;
    });
    return s32;
}

} // namespace unicodelib

#endif
// vim: et ts=4 sw=4 cin cino={1s ff=unix

