//
//  unicodelib.cppm
//
//  Copyright (c) 2025 Yuji Hirose. All rights reserved.
//  MIT License
//

module;

#include "../unicodelib.h"
#include "../unicodelib_encodings.h"

export module unicodelib;

export import :names;

export namespace unicode {
    // UTF-8 Encoding
    namespace utf8 {
        using unicode::utf8::codepoint_length;
        using unicode::utf8::codepoint_count;
        using unicode::utf8::encode_codepoint;
        using unicode::utf8::encode;
        using unicode::utf8::decode_codepoint;
        using unicode::utf8::decode;
        using unicode::utf8::for_each;
    }

    // UTF-16 Encoding
    namespace utf16 {
        using unicode::utf16::codepoint_length;
        using unicode::utf16::codepoint_count;
        using unicode::utf16::encode_codepoint;
        using unicode::utf16::encode;
        using unicode::utf16::decode_codepoint;
        using unicode::utf16::decode;
        using unicode::utf8::for_each;
    }

    // Unicode Version
    using unicode::Unicode_Version;

    // General Category
    using unicode::GeneralCategory;
    using unicode::general_category;

    using unicode::is_cased_letter_category;
    using unicode::is_letter_category;
    using unicode::is_mark_category;
    using unicode::is_number_category;
    using unicode::is_punctuation_category;
    using unicode::is_symbol_category;
    using unicode::is_separator_category;
    using unicode::is_other_category;

    using unicode::is_cased_letter;
    using unicode::is_letter;
    using unicode::is_mark;
    using unicode::is_number;
    using unicode::is_punctuation;
    using unicode::is_symbol;
    using unicode::is_separator;
    using unicode::is_other;

    // Property
    using unicode::is_white_space;
    using unicode::is_bidi_control;
    using unicode::is_join_control;
    using unicode::is_dash;
    using unicode::is_hyphen;
    using unicode::is_quotation_mark;
    using unicode::is_terminal_punctuation;
    using unicode::is_other_math;
    using unicode::is_hex_digit;
    using unicode::is_ascii_hex_digit;
    using unicode::is_other_alphabetic;
    using unicode::is_ideographic;
    using unicode::is_diacritic;
    using unicode::is_extender;
    using unicode::is_other_lowercase;
    using unicode::is_other_uppercase;
    using unicode::is_noncharacter_code_point;
    using unicode::is_other_grapheme_extend;
    using unicode::is_ids_binary_operator;
    using unicode::is_ids_trinary_operator;
    using unicode::is_radical;
    using unicode::is_unified_ideograph;
    using unicode::is_other_default_ignorable_code_point;
    using unicode::is_deprecated;
    using unicode::is_soft_dotted;
    using unicode::is_logical_order_exception;
    using unicode::is_other_id_start;
    using unicode::is_other_id_continue;
    using unicode::is_sentence_terminal;
    using unicode::is_variation_selector;
    using unicode::is_pattern_white_space;
    using unicode::is_pattern_syntax;
    using unicode::is_prepended_concatenation_mark;

    // Derived Property
    using unicode::is_math;
    using unicode::is_alphabetic;
    using unicode::is_lowercase;
    using unicode::is_uppercase;
    using unicode::is_cased;
    using unicode::is_case_ignorable;
    using unicode::is_changes_when_lowercased;
    using unicode::is_changes_when_uppercased;
    using unicode::is_changes_when_titlecased;
    using unicode::is_changes_when_casefolded;
    using unicode::is_changes_when_casemapped;
    using unicode::is_id_start;
    using unicode::is_id_continue;
    using unicode::is_xid_start;
    using unicode::is_xid_continue;
    using unicode::is_default_ignorable_code_point;
    using unicode::is_grapheme_extend;
    using unicode::is_grapheme_base;
    using unicode::is_grapheme_link;
    using unicode::is_indic_conjunct_break_linker;
    using unicode::is_indic_conjunct_break_consonant;
    using unicode::is_indic_conjunct_break_extend;

    // Case
    using unicode::simple_uppercase_mapping;
    using unicode::simple_lowercase_mapping;
    using unicode::simple_titlecase_mapping;
    using unicode::simple_case_folding;

    using unicode::to_uppercase;
    using unicode::to_lowercase;
    using unicode::to_titlecase;
    using unicode::to_case_fold;

    using unicode::is_uppercase;
    using unicode::is_lowercase;
    using unicode::is_titlecase;
    using unicode::is_case_fold;

    using unicode::caseless_match;
    using unicode::canonical_caseless_match;
    using unicode::compatibility_caseless_match;

    // Text Segmentation
    using unicode::is_graphic_character;
    using unicode::is_base_character;
    using unicode::is_combining_character;

    using unicode::combining_character_sequence_length;
    using unicode::combining_character_sequence_count;

    using unicode::extended_combining_character_sequence_length;
    using unicode::extended_combining_character_sequence_count;

    using unicode::is_grapheme_boundary;
    using unicode::grapheme_length;
    using unicode::grapheme_count;

    using unicode::is_word_boundary;

    using unicode::is_sentence_boundary;

    // Block
    using unicode::Block;
    using unicode::block;

    // Script
    using unicode::Script;
    using unicode::script;
    using unicode::is_script;

    // Normalization
    using unicode::to_nfc;
    using unicode::to_nfd;
    using unicode::to_nfkc;
    using unicode::to_nfkd;

    // String Conversion
    using unicode::to_wstring;
    using unicode::to_utf8;
    using unicode::to_utf16;
    using unicode::to_utf32;
}
