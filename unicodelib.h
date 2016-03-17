//
//  unicodelib.h
//
//  Copyright (c) 2016 Yuji Hirose. All rights reserved.
//  MIT License
//

#ifndef _CPPFSTLIB_UNICODELIB_H_
#define _CPPFSTLIB_UNICODELIB_H_

#include <cstdlib>
#include <string>

namespace unicode {

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const char32_t ErrorCode = 0x0000FFFD;
const char32_t MaxCode = 0x0010FFFF;

//-----------------------------------------------------------------------------
// General Category
//-----------------------------------------------------------------------------

enum class GeneralCategory {
  Lu,
  Uppercase_Letter = Lu,
  Ll,
  Lowercase_Letter = Ll,
  Lt,
  Titlecase_Letter = Lt,
  LC,
  Cased_Letter = LC,
  Lm,
  Modifier_Letter = Lm,
  Lo,
  Other_Letter = Lo,
  L,
  Letter = L,
  Mn,
  Nonspacing_Mark = Mn,
  Mc,
  Spacing_Mark = Mc,
  Me,
  Enclosing_Mark = Me,
  M,
  Mark = M,
  Nd,
  Decimal_Number = Nd,
  Nl,
  Letter_Number = Nl,
  No,
  Other_Number = No,
  N,
  Number = N,
  Pc,
  Connector_Punctuation = Pc,
  Pd,
  Dash_Punctuation = Pd,
  Ps,
  Open_Punctuation = Ps,
  Pe,
  Close_Punctuation = Pe,
  Pi,
  Initial_Punctuation = Pi,
  Pf,
  Final_Punctuation = Pf,
  Po,
  Other_Punctuation = Po,
  P,
  Punctuation = P,
  Sm,
  Math_Symbol = Sm,
  Sc,
  Currency_Symbol = Sc,
  Sk,
  Modifier_Symbol = Sk,
  So,
  Other_Symbol = So,
  S,
  Symbol = S,
  Zs,
  Space_Separator = Zs,
  Zl,
  Line_Separator = Zl,
  Zp,
  Paragraph_Separator = Zp,
  Z,
  Separator = Z,
  Cc,
  Control = Cc,
  Cf,
  Format = Cf,
  Cs,
  Surrogate = Cs,
  Co,
  Private_Use = Co,
  Cn,
  Unassigned = Cn,
  C,
  Other = C,
};

extern GeneralCategory general_category(char32_t cp);

extern bool is_general_category(GeneralCategory gc, char32_t cp);

extern bool is_cased_letter(char32_t cp);
extern bool is_letter(char32_t cp);
extern bool is_mark(char32_t cp);
extern bool is_number(char32_t cp);
extern bool is_punctuation(char32_t cp);
extern bool is_symbol(char32_t cp);
extern bool is_separator(char32_t cp);
extern bool is_other(char32_t cp);

//-----------------------------------------------------------------------------
// Block
//-----------------------------------------------------------------------------

// This is generated from 'Blocks.txt' in Unicode database.
// `python scripts/gen_blocks.py < UCD/Blocks.txt`
enum class Block {
  BasicLatin,
  Latin1Supplement,
  LatinExtendedA,
  LatinExtendedB,
  IPAExtensions,
  SpacingModifierLetters,
  CombiningDiacriticalMarks,
  GreekAndCoptic,
  Cyrillic,
  CyrillicSupplement,
  Armenian,
  Hebrew,
  Arabic,
  Syriac,
  ArabicSupplement,
  Thaana,
  NKo,
  Samaritan,
  Mandaic,
  ArabicExtendedA,
  Devanagari,
  Bengali,
  Gurmukhi,
  Gujarati,
  Oriya,
  Tamil,
  Telugu,
  Kannada,
  Malayalam,
  Sinhala,
  Thai,
  Lao,
  Tibetan,
  Myanmar,
  Georgian,
  HangulJamo,
  Ethiopic,
  EthiopicSupplement,
  Cherokee,
  UnifiedCanadianAboriginalSyllabics,
  Ogham,
  Runic,
  Tagalog,
  Hanunoo,
  Buhid,
  Tagbanwa,
  Khmer,
  Mongolian,
  UnifiedCanadianAboriginalSyllabicsExtended,
  Limbu,
  TaiLe,
  NewTaiLue,
  KhmerSymbols,
  Buginese,
  TaiTham,
  CombiningDiacriticalMarksExtended,
  Balinese,
  Sundanese,
  Batak,
  Lepcha,
  OlChiki,
  SundaneseSupplement,
  VedicExtensions,
  PhoneticExtensions,
  PhoneticExtensionsSupplement,
  CombiningDiacriticalMarksSupplement,
  LatinExtendedAdditional,
  GreekExtended,
  GeneralPunctuation,
  SuperscriptsAndSubscripts,
  CurrencySymbols,
  CombiningDiacriticalMarksForSymbols,
  LetterlikeSymbols,
  NumberForms,
  Arrows,
  MathematicalOperators,
  MiscellaneousTechnical,
  ControlPictures,
  OpticalCharacterRecognition,
  EnclosedAlphanumerics,
  BoxDrawing,
  BlockElements,
  GeometricShapes,
  MiscellaneousSymbols,
  Dingbats,
  MiscellaneousMathematicalSymbolsA,
  SupplementalArrowsA,
  BraillePatterns,
  SupplementalArrowsB,
  MiscellaneousMathematicalSymbolsB,
  SupplementalMathematicalOperators,
  MiscellaneousSymbolsAndArrows,
  Glagolitic,
  LatinExtendedC,
  Coptic,
  GeorgianSupplement,
  Tifinagh,
  EthiopicExtended,
  CyrillicExtendedA,
  SupplementalPunctuation,
  CJKRadicalsSupplement,
  KangxiRadicals,
  IdeographicDescriptionCharacters,
  CJKSymbolsAndPunctuation,
  Hiragana,
  Katakana,
  Bopomofo,
  HangulCompatibilityJamo,
  Kanbun,
  BopomofoExtended,
  CJKStrokes,
  KatakanaPhoneticExtensions,
  EnclosedCJKLettersAndMonths,
  CJKCompatibility,
  CJKUnifiedIdeographsExtensionA,
  YijingHexagramSymbols,
  CJKUnifiedIdeographs,
  YiSyllables,
  YiRadicals,
  Lisu,
  Vai,
  CyrillicExtendedB,
  Bamum,
  ModifierToneLetters,
  LatinExtendedD,
  SylotiNagri,
  CommonIndicNumberForms,
  PhagsPa,
  Saurashtra,
  DevanagariExtended,
  KayahLi,
  Rejang,
  HangulJamoExtendedA,
  Javanese,
  MyanmarExtendedB,
  Cham,
  MyanmarExtendedA,
  TaiViet,
  MeeteiMayekExtensions,
  EthiopicExtendedA,
  LatinExtendedE,
  CherokeeSupplement,
  MeeteiMayek,
  HangulSyllables,
  HangulJamoExtendedB,
  HighSurrogates,
  HighPrivateUseSurrogates,
  LowSurrogates,
  PrivateUseArea,
  CJKCompatibilityIdeographs,
  AlphabeticPresentationForms,
  ArabicPresentationFormsA,
  VariationSelectors,
  VerticalForms,
  CombiningHalfMarks,
  CJKCompatibilityForms,
  SmallFormVariants,
  ArabicPresentationFormsB,
  HalfwidthAndFullwidthForms,
  Specials,
  LinearBSyllabary,
  LinearBIdeograms,
  AegeanNumbers,
  AncientGreekNumbers,
  AncientSymbols,
  PhaistosDisc,
  Lycian,
  Carian,
  CopticEpactNumbers,
  OldItalic,
  Gothic,
  OldPermic,
  Ugaritic,
  OldPersian,
  Deseret,
  Shavian,
  Osmanya,
  Elbasan,
  CaucasianAlbanian,
  LinearA,
  CypriotSyllabary,
  ImperialAramaic,
  Palmyrene,
  Nabataean,
  Hatran,
  Phoenician,
  Lydian,
  MeroiticHieroglyphs,
  MeroiticCursive,
  Kharoshthi,
  OldSouthArabian,
  OldNorthArabian,
  Manichaean,
  Avestan,
  InscriptionalParthian,
  InscriptionalPahlavi,
  PsalterPahlavi,
  OldTurkic,
  OldHungarian,
  RumiNumeralSymbols,
  Brahmi,
  Kaithi,
  SoraSompeng,
  Chakma,
  Mahajani,
  Sharada,
  SinhalaArchaicNumbers,
  Khojki,
  Multani,
  Khudawadi,
  Grantha,
  Tirhuta,
  Siddham,
  Modi,
  Takri,
  Ahom,
  WarangCiti,
  PauCinHau,
  Cuneiform,
  CuneiformNumbersAndPunctuation,
  EarlyDynasticCuneiform,
  EgyptianHieroglyphs,
  AnatolianHieroglyphs,
  BamumSupplement,
  Mro,
  BassaVah,
  PahawhHmong,
  Miao,
  KanaSupplement,
  Duployan,
  ShorthandFormatControls,
  ByzantineMusicalSymbols,
  MusicalSymbols,
  AncientGreekMusicalNotation,
  TaiXuanJingSymbols,
  CountingRodNumerals,
  MathematicalAlphanumericSymbols,
  SuttonSignWriting,
  MendeKikakui,
  ArabicMathematicalAlphabeticSymbols,
  MahjongTiles,
  DominoTiles,
  PlayingCards,
  EnclosedAlphanumericSupplement,
  EnclosedIdeographicSupplement,
  MiscellaneousSymbolsAndPictographs,
  Emoticons,
  OrnamentalDingbats,
  TransportAndMapSymbols,
  AlchemicalSymbols,
  GeometricShapesExtended,
  SupplementalArrowsC,
  SupplementalSymbolsAndPictographs,
  CJKUnifiedIdeographsExtensionB,
  CJKUnifiedIdeographsExtensionC,
  CJKUnifiedIdeographsExtensionD,
  CJKUnifiedIdeographsExtensionE,
  CJKCompatibilityIdeographsSupplement,
  Tags,
  VariationSelectorsSupplement,
  SupplementaryPrivateUseAreaA,
  SupplementaryPrivateUseAreaB,
  Unassigned
};

extern Block block(char32_t cp);

//-----------------------------------------------------------------------------
// Script
//-----------------------------------------------------------------------------

enum class Script {
  Caucasian_Albanian,
  Ahom,
  Arabic,
  Imperial_Aramaic,
  Armenian,
  Avestan,
  Balinese,
  Bamum,
  Bassa_Vah,
  Batak,
  Bengali,
  Bopomofo,
  Brahmi,
  Braille,
  Buginese,
  Buhid,
  Chakma,
  Canadian_Aboriginal,
  Carian,
  Cham,
  Cherokee,
  Coptic,
  Cypriot,
  Cyrillic,
  Devanagari,
  Deseret,
  Duployan,
  Egyptian_Hieroglyphs,
  Elbasan,
  Ethiopic,
  Georgian,
  Glagolitic,
  Gothic,
  Grantha,
  Greek,
  Gujarati,
  Gurmukhi,
  Hangul,
  Han,
  Hanunoo,
  Hatran,
  Hebrew,
  Hiragana,
  Anatolian_Hieroglyphs,
  Pahawh_Hmong,
  Katakana_Or_Hiragana,
  Old_Hungarian,
  Old_Italic,
  Javanese,
  Kayah_Li,
  Katakana,
  Kharoshthi,
  Khmer,
  Khojki,
  Kannada,
  Kaithi,
  Tai_Tham,
  Lao,
  Latin,
  Lepcha,
  Limbu,
  Linear_A,
  Linear_B,
  Lisu,
  Lycian,
  Lydian,
  Mahajani,
  Mandaic,
  Manichaean,
  Mende_Kikakui,
  Meroitic_Cursive,
  Meroitic_Hieroglyphs,
  Malayalam,
  Modi,
  Mongolian,
  Mro,
  Meetei_Mayek,
  Multani,
  Myanmar,
  Old_North_Arabian,
  Nabataean,
  Nko,
  Ogham,
  Ol_Chiki,
  Old_Turkic,
  Oriya,
  Osmanya,
  Palmyrene,
  Pau_Cin_Hau,
  Old_Permic,
  Phags_Pa,
  Inscriptional_Pahlavi,
  Psalter_Pahlavi,
  Phoenician,
  Miao,
  Inscriptional_Parthian,
  Rejang,
  Runic,
  Samaritan,
  Old_South_Arabian,
  Saurashtra,
  SignWriting,
  Shavian,
  Sharada,
  Siddham,
  Khudawadi,
  Sinhala,
  Sora_Sompeng,
  Sundanese,
  Syloti_Nagri,
  Syriac,
  Tagbanwa,
  Takri,
  Tai_Le,
  New_Tai_Lue,
  Tamil,
  Tai_Viet,
  Telugu,
  Tifinagh,
  Tagalog,
  Thaana,
  Thai,
  Tibetan,
  Tirhuta,
  Ugaritic,
  Vai,
  Warang_Citi,
  Old_Persian,
  Cuneiform,
  Yi,
  Inherited,
  Common,
  Unknown,
  Unassigned
};

extern Script script(char32_t cp);

extern bool is_script(Script sc, char32_t cp);

//-----------------------------------------------------------------------------
// Text segmentation
//-----------------------------------------------------------------------------

extern bool is_grapheme_boundary(const char32_t *s32, size_t l, size_t i);

extern size_t grapheme_length(const char32_t *s32, size_t l);

extern size_t grapheme_count(const char32_t *s32, size_t l);

//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

enum class Normalization {
  NFC,
  NFD,
  NFKC,
  NFKD,
};

extern std::u32string to_nfc(const char32_t *s32, size_t l);
extern std::u32string to_nfd(const char32_t *s32, size_t l);
extern std::u32string to_nfkc(const char32_t *s32, size_t l);
extern std::u32string to_nfkd(const char32_t *s32, size_t l);

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
  if (cp < 0x0080) {
    return 1;
  } else if (cp < 0x0800) {
    return 2;
  } else if (cp < 0xD800) {
    return 3;
  } else if (cp < 0xe000) {
    return 0;
  } // D800 - DFFF is invalid...
  else if (cp < 0x10000) {
    return 3;
  } else if (cp < 0x110000) {
    return 4;
  }
  return 0;
}

inline size_t encode(char32_t cp, char *buff, size_t buff_len) {
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

inline size_t encode(char32_t cp, std::string &out) {
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

inline void encode(const char32_t *s32, size_t l, std::string &out) {
  for (size_t i = 0; i < l; i++) {
    encode(s32[i], out);
  }
}

template <typename T> inline void encode(const T &s32, std::string &out) {
  encode(s32.data(), s32.length(), out);
}

inline std::string encode(const char32_t *s32, size_t l) {
  std::string out;
  encode(s32, l, out);
  return out;
}

template <typename T> inline std::string encode(const T &s32) {
  std::string out;
  encode(s32, out);
  return out;
}

inline size_t decode_byte_length(const char *s8, size_t l) {
  if (l) {
    uint8_t b = s8[0];
    if ((b & 0x80) == 0) {
      return 1;
    } else if ((b & 0xE0) == 0xC0) {
      return 2;
    } else if ((b & 0xF0) == 0xE0) {
      return 3;
    } else if ((b & 0xF8) == 0xF0) {
      return 4;
    }
  }
  return 0;
}

template <typename T> inline size_t decode_byte_length(const T &s8) {
  return decode_byte_length(s8.data(), s8.length());
}

inline bool decode(const char *s8, size_t l, size_t &bytes, char32_t &cp) {
  if (l) {
    uint8_t b = s8[0];
    if ((b & 0x80) == 0) {
      bytes = 1;
      cp = b;
      return true;
    } else if ((b & 0xE0) == 0xC0) {
      if (l >= 2) {
        bytes = 2;
        cp = (((char32_t)(s8[0] & 0x1F)) << 6) | ((char32_t)(s8[1] & 0x3F));
        return true;
      }
    } else if ((b & 0xF0) == 0xE0) {
      if (l >= 3) {
        bytes = 3;
        cp = (((char32_t)(s8[0] & 0x0F)) << 12) |
             (((char32_t)(s8[1] & 0x3F)) << 6) | ((char32_t)(s8[2] & 0x3F));
        return true;
      }
    } else if ((b & 0xF8) == 0xF0) {
      if (l >= 4) {
        bytes = 4;
        cp = (((char32_t)(s8[0] & 0x07)) << 18) |
             (((char32_t)(s8[1] & 0x3F)) << 12) |
             (((char32_t)(s8[2] & 0x3F)) << 6) | ((char32_t)(s8[3] & 0x3F));
        return true;
      }
    }
  }
  return false;
}

inline size_t decode(const char *s8, size_t l, char32_t &out) {
  size_t bytes;
  if (decode(s8, l, bytes, out)) {
    return bytes;
  }
  return 0;
}

template <typename T> inline size_t decode(const T &s8, char32_t &cp) {
  return decode(s8.data(), s8.length(), cp);
}

template <typename T>
inline void for_each(const char *s, size_t l, T callback) {
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

inline void decode(const char *s8, size_t l, std::u32string &out) {
  for_each(s8, l,
           [&](const char *s, size_t l, size_t beg, size_t end, size_t i) {
             size_t bytes;
             char32_t cp;
             decode(&s[beg], (end - beg), bytes, cp);
             out += cp;
           });
}

template <typename T> inline void decode(const T &s8, std::u32string &out) {
  decode(s8.data(), s8.length(), out);
}

inline std::u32string decode(const char *s8, size_t l) {
  std::u32string out;
  decode(s8, l, out);
  return out;
}

template <typename T> inline std::u32string decode(const T &s8) {
  std::u32string out;
  decode(s8.data(), s8.length(), out);
  return out;
}

inline size_t codepoint_count(const char *s8, size_t l) {
  size_t count = 0;
  for (size_t i = 0; i < l; i += decode_byte_length(s8 + i, l - i)) {
    count++;
  }
  return count;
}

} // namespace unicode

#endif

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
