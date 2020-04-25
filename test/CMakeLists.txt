cmake_minimum_required(VERSION 3.0)
project("cpp-unicodelib")

include_directories(..)

add_definitions("-std=c++1y -Werror -Wmost -Wnon-virtual-dtor -Woverloaded-virtual -Wfuture-compat -Wunused-lambda-capture -Wsometimes-uninitialized -Winconsistent-missing-destructor-override -Wshadow-uncaptured-local -Wold-style-cast -Wrange-loop-analysis -Wunused-macros")

add_executable(
    test-main test.cpp
    ../src/unicodelib.cpp
    ../src/data_block_properties.cpp
    ../src/data_case_foldings.cpp
    ../src/data_derived_core_properties.cpp
    ../src/data_general_category_properties.cpp
    ../src/data_grapheme_break_properties.cpp
    ../src/data_normalization_composition.cpp
    ../src/data_normalization_properties.cpp
    ../src/data_properties.cpp
    ../src/data_script_extension_ids.cpp
    ../src/data_script_extension_properties_for_id.cpp
    ../src/data_script_properties.cpp
    ../src/data_sentence_break_properties.cpp
    ../src/data_simple_case_mappings.cpp
    ../src/data_special_case_mappings.cpp
    ../src/data_special_case_mappings_default.cpp
    ../src/data_word_break_properties.cpp
    ../src/data_emoji_properties.cpp)
