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

//-----------------------------------------------------------------------------
// Property
//-----------------------------------------------------------------------------

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
bool is_sentence_terminal(char32_t cp);
bool is_variation_selector(char32_t cp);
bool is_pattern_white_space(char32_t cp);
bool is_pattern_syntax(char32_t cp);
bool is_prepended_concatenation_mark(char32_t cp);

//-----------------------------------------------------------------------------
// Derived Property
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// Case
//-----------------------------------------------------------------------------

char32_t simple_uppercase_mapping(char32_t cp);
char32_t simple_lowercase_mapping(char32_t cp);
char32_t simple_titlecase_mapping(char32_t cp);
char32_t simple_case_folding(char32_t cp);

std::u32string to_uppercase(const char32_t *s32, size_t l,
                            const char *lang = nullptr);
std::u32string to_lowercase(const char32_t *s32, size_t l,
                            const char *lang = nullptr);
std::u32string to_titlecase(const char32_t *s32, size_t l,
                            const char *lang = nullptr);
std::u32string to_case_fold(
    const char32_t *s32, size_t l,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false);

bool is_uppercase(const char32_t *s32, size_t l);
bool is_lowercase(const char32_t *s32, size_t l);
bool is_titlecase(const char32_t *s32, size_t l);
bool is_case_fold(const char32_t *s32, size_t l);

bool caseless_match(
    const char32_t *s1, size_t l1, const char32_t *s2, size_t l2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false);

bool canonical_caseless_match(
    const char32_t *s1, size_t l1, const char32_t *s2, size_t l2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false);

bool compatibility_caseless_match(
    const char32_t *s1, size_t l1, const char32_t *s2, size_t l2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false);

//-----------------------------------------------------------------------------
// Text Segmentation
//-----------------------------------------------------------------------------

bool is_graphic_character(char32_t cp);
bool is_base_character(char32_t cp);
bool is_combining_character(char32_t cp);

size_t combining_character_sequence_length(const char32_t *s32, size_t l);
size_t combining_character_sequence_count(const char32_t *s32, size_t l);

size_t extended_combining_character_sequence_length(const char32_t *s32,
                                                    size_t l);
size_t extended_combining_character_sequence_count(const char32_t *s32,
                                                   size_t l);

// The following functions support 'extended' grapheme cluster, not 'legacy'
// graphme cluster.
bool is_grapheme_boundary(const char32_t *s32, size_t l, size_t i);
size_t grapheme_length(const char32_t *s32, size_t l);
size_t grapheme_count(const char32_t *s32, size_t l);

bool is_word_boundary(const char32_t *s32, size_t l, size_t i);

bool is_sentence_boundary(const char32_t *s32, size_t l, size_t i);

//-----------------------------------------------------------------------------
// Block
//-----------------------------------------------------------------------------

// This is generated from 'Blocks.txt' in Unicode database.
// `python scripts/gen_property_values.py < UCD/Blocks.txt`
enum class Block {
  Unassigned,
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
  SyriacSupplement,
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
  CyrillicExtendedC,
  GeorgianExtended,
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
  Osage,
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
  HanifiRohingya,
  RumiNumeralSymbols,
  OldSogdian,
  Sogdian,
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
  Newa,
  Tirhuta,
  Siddham,
  Modi,
  MongolianSupplement,
  Takri,
  Ahom,
  Dogra,
  WarangCiti,
  ZanabazarSquare,
  Soyombo,
  PauCinHau,
  Bhaiksuki,
  Marchen,
  MasaramGondi,
  GunjalaGondi,
  Makasar,
  Cuneiform,
  CuneiformNumbersAndPunctuation,
  EarlyDynasticCuneiform,
  EgyptianHieroglyphs,
  AnatolianHieroglyphs,
  BamumSupplement,
  Mro,
  BassaVah,
  PahawhHmong,
  Medefaidrin,
  Miao,
  IdeographicSymbolsAndPunctuation,
  Tangut,
  TangutComponents,
  KanaSupplement,
  KanaExtendedA,
  Nushu,
  Duployan,
  ShorthandFormatControls,
  ByzantineMusicalSymbols,
  MusicalSymbols,
  AncientGreekMusicalNotation,
  MayanNumerals,
  TaiXuanJingSymbols,
  CountingRodNumerals,
  MathematicalAlphanumericSymbols,
  SuttonSignWriting,
  GlagoliticSupplement,
  MendeKikakui,
  Adlam,
  IndicSiyaqNumbers,
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
  ChessSymbols,
  CJKUnifiedIdeographsExtensionB,
  CJKUnifiedIdeographsExtensionC,
  CJKUnifiedIdeographsExtensionD,
  CJKUnifiedIdeographsExtensionE,
  CJKUnifiedIdeographsExtensionF,
  CJKCompatibilityIdeographsSupplement,
  Tags,
  VariationSelectorsSupplement,
  SupplementaryPrivateUseAreaA,
  SupplementaryPrivateUseAreaB,
};

Block block(char32_t cp);

//-----------------------------------------------------------------------------
// Script
//-----------------------------------------------------------------------------

// This is generated from 'Scripts.txt' in Unicode database.
// `python scripts/gen_property_values.py < UCD/Scripts.txt`
enum class Script {
  Unassigned,
  Common,
  Latin,
  Greek,
  Cyrillic,
  Armenian,
  Hebrew,
  Arabic,
  Syriac,
  Thaana,
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
  Hangul,
  Ethiopic,
  Cherokee,
  Canadian_Aboriginal,
  Ogham,
  Runic,
  Khmer,
  Mongolian,
  Hiragana,
  Katakana,
  Bopomofo,
  Han,
  Yi,
  Old_Italic,
  Gothic,
  Deseret,
  Inherited,
  Tagalog,
  Hanunoo,
  Buhid,
  Tagbanwa,
  Limbu,
  Tai_Le,
  Linear_B,
  Ugaritic,
  Shavian,
  Osmanya,
  Cypriot,
  Braille,
  Buginese,
  Coptic,
  New_Tai_Lue,
  Glagolitic,
  Tifinagh,
  Syloti_Nagri,
  Old_Persian,
  Kharoshthi,
  Balinese,
  Cuneiform,
  Phoenician,
  Phags_Pa,
  Nko,
  Sundanese,
  Lepcha,
  Ol_Chiki,
  Vai,
  Saurashtra,
  Kayah_Li,
  Rejang,
  Lycian,
  Carian,
  Lydian,
  Cham,
  Tai_Tham,
  Tai_Viet,
  Avestan,
  Egyptian_Hieroglyphs,
  Samaritan,
  Lisu,
  Bamum,
  Javanese,
  Meetei_Mayek,
  Imperial_Aramaic,
  Old_South_Arabian,
  Inscriptional_Parthian,
  Inscriptional_Pahlavi,
  Old_Turkic,
  Kaithi,
  Batak,
  Brahmi,
  Mandaic,
  Chakma,
  Meroitic_Cursive,
  Meroitic_Hieroglyphs,
  Miao,
  Sharada,
  Sora_Sompeng,
  Takri,
  Caucasian_Albanian,
  Bassa_Vah,
  Duployan,
  Elbasan,
  Grantha,
  Pahawh_Hmong,
  Khojki,
  Linear_A,
  Mahajani,
  Manichaean,
  Mende_Kikakui,
  Modi,
  Mro,
  Old_North_Arabian,
  Nabataean,
  Palmyrene,
  Pau_Cin_Hau,
  Old_Permic,
  Psalter_Pahlavi,
  Siddham,
  Khudawadi,
  Tirhuta,
  Warang_Citi,
  Ahom,
  Anatolian_Hieroglyphs,
  Hatran,
  Multani,
  Old_Hungarian,
  SignWriting,
  Adlam,
  Bhaiksuki,
  Marchen,
  Newa,
  Osage,
  Tangut,
  Masaram_Gondi,
  Nushu,
  Soyombo,
  Zanabazar_Square,
  Dogra,
  Gunjala_Gondi,
  Makasar,
  Medefaidrin,
  Hanifi_Rohingya,
  Sogdian,
  Old_Sogdian,
};

Script script(char32_t cp);
bool is_script(Script sc, char32_t cp);

//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

std::u32string to_nfc(const char32_t *s32, size_t l);
std::u32string to_nfd(const char32_t *s32, size_t l);
std::u32string to_nfkc(const char32_t *s32, size_t l);
std::u32string to_nfkd(const char32_t *s32, size_t l);

//-----------------------------------------------------------------------------
// Inline Wrapper functions
//-----------------------------------------------------------------------------

inline std::u32string to_uppercase(const std::u32string &s32,
                                   const char *lang = nullptr) {
  return to_uppercase(s32.data(), s32.length(), lang);
}

inline std::u32string to_uppercase(const char32_t *s32,
                                   const char *lang = nullptr) {
  return to_uppercase(s32, std::char_traits<char32_t>::length(s32), lang);
}

inline std::u32string to_lowercase(const std::u32string &s32,
                                   const char *lang = nullptr) {
  return to_lowercase(s32.data(), s32.length(), lang);
}

inline std::u32string to_lowercase(const char32_t *s32,
                                   const char *lang = nullptr) {
  return to_lowercase(s32, std::char_traits<char32_t>::length(s32), lang);
}

inline std::u32string to_titlecase(const std::u32string &s32,
                                   const char *lang = nullptr) {
  return to_titlecase(s32.data(), s32.length(), lang);
}

inline std::u32string to_titlecase(const char32_t *s32,
                                   const char *lang = nullptr) {
  return to_titlecase(s32, std::char_traits<char32_t>::length(s32), lang);
}

inline std::u32string to_case_fold(
    const std::u32string &s32,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return to_case_fold(s32.data(), s32.length(),
                      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline std::u32string to_case_fold(
    const char32_t *s32,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return to_case_fold(s32, std::char_traits<char32_t>::length(s32),
                      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline bool is_uppercase(const std::u32string &s32) {
  return is_uppercase(s32.data(), s32.length());
}

inline bool is_uppercase(const char32_t *s32) {
  return is_uppercase(s32, std::char_traits<char32_t>::length(s32));
}

inline bool is_lowercase(const std::u32string &s32) {
  return is_lowercase(s32.data(), s32.length());
}

inline bool is_lowercase(const char32_t *s32) {
  return is_lowercase(s32, std::char_traits<char32_t>::length(s32));
}

inline bool is_titlecase(const std::u32string &s32) {
  return is_titlecase(s32.data(), s32.length());
}

inline bool is_titlecase(const char32_t *s32) {
  return is_titlecase(s32, std::char_traits<char32_t>::length(s32));
}

inline bool is_case_fold(const std::u32string &s32) {
  return is_case_fold(s32.data(), s32.length());
}

inline bool is_case_fold(const char32_t *s32) {
  return is_case_fold(s32, std::char_traits<char32_t>::length(s32));
}

inline bool caseless_match(
    const std::u32string &s1, const std::u32string &s2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return caseless_match(s1.data(), s1.length(), s2.data(), s2.length(),
                        special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline bool caseless_match(
    const char32_t *s1, const char32_t *s2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return caseless_match(s1, std::char_traits<char32_t>::length(s1), s2,
                        std::char_traits<char32_t>::length(s2),
                        special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline bool canonical_caseless_match(
    const std::u32string &s1, const std::u32string &s2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return canonical_caseless_match(
      s1.data(), s1.length(), s2.data(), s2.length(),
      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline bool canonical_caseless_match(
    const char32_t *s1, const char32_t *s2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return canonical_caseless_match(
      s1, std::char_traits<char32_t>::length(s1), s2,
      std::char_traits<char32_t>::length(s2),
      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline bool compatibility_caseless_match(
    const std::u32string &s1, const std::u32string &s2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return compatibility_caseless_match(
      s1.data(), s1.length(), s2.data(), s2.length(),
      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline bool compatibility_caseless_match(
    const char32_t *s1, const char32_t *s2,
    bool special_case_for_uppercase_I_and_dotted_uppercase_I = false) {
  return compatibility_caseless_match(
      s1, std::char_traits<char32_t>::length(s1), s2,
      std::char_traits<char32_t>::length(s2),
      special_case_for_uppercase_I_and_dotted_uppercase_I);
}

inline std::u32string to_nfc(const std::u32string &s32) {
  return to_nfc(s32.data(), s32.length());
}

inline std::u32string to_nfc(const char32_t *s32) {
  return to_nfc(s32, std::char_traits<char32_t>::length(s32));
}

inline std::u32string to_nfd(const std::u32string &s32) {
  return to_nfd(s32.data(), s32.length());
}

inline std::u32string to_nfd(const char32_t *s32) {
  return to_nfd(s32, std::char_traits<char32_t>::length(s32));
}

inline std::u32string to_nfkc(const std::u32string &s32) {
  return to_nfkc(s32.data(), s32.length());
}

inline std::u32string to_nfkc(const char32_t *s32) {
  return to_nfkc(s32, std::char_traits<char32_t>::length(s32));
}

inline std::u32string to_nfkd(const std::u32string &s32) {
  return to_nfkd(s32.data(), s32.length());
}

inline std::u32string to_nfkd(const char32_t *s32) {
  return to_nfkd(s32, std::char_traits<char32_t>::length(s32));
}

inline size_t grapheme_count(const std::u32string &s32) {
  return grapheme_count(s32.data(), s32.length());
}

inline size_t grapheme_count(const char32_t *s32) {
  return grapheme_count(s32, std::char_traits<char32_t>::length(s32));
}

inline size_t grapheme_length(const std::u32string &s32) {
  return grapheme_length(s32.data(), s32.length());
}

inline size_t grapheme_length(const char32_t *s32) {
  return grapheme_length(s32, std::char_traits<char32_t>::length(s32));
}

}  // namespace unicode

#endif

// vim: et ts=2 sw=2 cin cino=\:0 ff=unix
