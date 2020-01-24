# Create object files for lexer
add_library(LEXER_LIB_OBJECTS OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lexer.cpp
    )
target_compile_features(LEXER_LIB_OBJECTS PRIVATE cxx_std_17)
target_include_directories(LEXER_LIB_OBJECTS PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ETERNAL_DIR}/include
    )

# Create object files for lexer legacy_2
add_library(LEXER_LEGACY_2_LIB_OBJECTS OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/legacy_2/lexer.cpp
    )
target_compile_features(LEXER_LEGACY_2_LIB_OBJECTS PRIVATE cxx_std_17)
target_include_directories(LEXER_LEGACY_2_LIB_OBJECTS  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ETERNAL_DIR}/include
    )
