//
//  unicodelib.h
//
//  Copyright (c) 2016 Yuji Hirose. All rights reserved.
//  MIT License
//

#ifndef _CPPUNICODELIB_UNICODELIB_H_
#define _CPPUNICODELIB_UNICODELIB_H_

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
  Lm,
  Modifier_Letter = Lm,
  Lo,
  Other_Letter = Lo,
  Mn,
  Nonspacing_Mark = Mn,
  Mc,
  Spacing_Mark = Mc,
  Me,
  Enclosing_Mark = Me,
  Nd,
  Decimal_Number = Nd,
  Nl,
  Letter_Number = Nl,
  No,
  Other_Number = No,
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
  Sm,
  Math_Symbol = Sm,
  Sc,
  Currency_Symbol = Sc,
  Sk,
  Modifier_Symbol = Sk,
  So,
  Other_Symbol = So,
  Zs,
  Space_Separator = Zs,
  Zl,
  Line_Separator = Zl,
  Zp,
  Paragraph_Separator = Zp,
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
};

extern GeneralCategory general_category(char32_t cp);

extern bool is_cased_letter_category(GeneralCategory gc);
extern bool is_letter_category(GeneralCategory gc);
extern bool is_mark_category(GeneralCategory gc);
extern bool is_number_category(GeneralCategory gc);
extern bool is_punctuation_category(GeneralCategory gc);
extern bool is_symbol_category(GeneralCategory gc);
extern bool is_separator_category(GeneralCategory gc);
extern bool is_other_category(GeneralCategory gc);

extern bool is_cased_letter(char32_t cp);
extern bool is_letter(char32_t cp);
extern bool is_mark(char32_t cp);
extern bool is_number(char32_t cp);
extern bool is_punctuation(char32_t cp);
extern bool is_symbol(char32_t cp);
extern bool is_separator(char32_t cp);
extern bool is_other(char32_t cp);

extern bool is_graphic_character(char32_t cp);
extern bool is_base_character(char32_t cp);
extern bool is_combining_character(char32_t cp);

//-----------------------------------------------------------------------------
// Combination
//-----------------------------------------------------------------------------

extern size_t combining_character_sequence_length(const char32_t *s32,
                                                  size_t l);
extern size_t combining_character_sequence_count(const char32_t *s32, size_t l);

extern size_t extended_combining_character_sequence_length(const char32_t *s32,
                                                           size_t l);
extern size_t extended_combining_character_sequence_count(const char32_t *s32,
                                                          size_t l);

// The following functions support 'extended' grapheme cluster, not 'legacy'
// graphme cluster.
extern bool is_grapheme_boundary(const char32_t *s32, size_t l, size_t i);
extern size_t grapheme_length(const char32_t *s32, size_t l);
extern size_t grapheme_count(const char32_t *s32, size_t l);

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

extern size_t encode_byte_length(char32_t cp);

extern size_t encode(char32_t cp, char *buff, size_t buff_len);
extern size_t encode(char32_t cp, std::string &out);
extern std::string encode(char32_t cp);
extern void encode(const char32_t *s32, size_t l, std::string &out);
extern std::string encode(const char32_t *s32, size_t l);
template <typename T> inline void encode(const T &s32, std::string &out) {
  encode(s32.data(), s32.length(), out);
}
template <typename T> inline std::string encode(const T &s32) {
  std::string out;
  encode(s32, out);
  return out;
}

extern size_t decode_byte_length(const char *s8, size_t l);
template <typename T> inline size_t decode_byte_length(const T &s8) {
  return decode_byte_length(s8.data(), s8.length());
}

extern bool decode(const char *s8, size_t l, size_t &bytes, char32_t &cp);
extern size_t decode(const char *s8, size_t l, char32_t &out);
extern void decode(const char *s8, size_t l, std::u32string &out);
extern std::u32string decode(const char *s8, size_t l);
template <typename T> inline size_t decode(const T &s8, char32_t &cp) {
  return decode(s8.data(), s8.length(), cp);
}
template <typename T> inline void decode(const T &s8, std::u32string &out) {
  decode(s8.data(), s8.length(), out);
}
template <typename T> inline std::u32string decode(const T &s8) {
  std::u32string out;
  decode(s8.data(), s8.length(), out);
  return out;
}

extern size_t codepoint_count(const char *s8, size_t l);

} // namespace unicode

#endif

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
