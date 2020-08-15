# Adds strict debug compile options,
# includes directories, and links to googletest.
# TEST_TARGET is added as a test and an executable named ${TEST_NAME} will be created.
function(create_test TEST_NAME TEST_TARGET)
    target_compile_options(${TEST_TARGET} PRIVATE -g -Wall -Werror -Wextra)
    target_compile_features(${TEST_TARGET} PRIVATE cxx_std_17)
    target_include_directories(${TEST_TARGET}
        PRIVATE
        ${GTEST_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/test
        ${ETERNAL_DIR}/include)
    target_link_libraries(${TEST_TARGET}
        PRIVATE gtest_main pthread nlohmann_json::nlohmann_json)
    add_test(${TEST_NAME} ${TEST_TARGET})
endfunction(create_test)

######################################################
# Exception Unit Tests
######################################################

add_executable(exceptions_unittests
               ${CMAKE_CURRENT_SOURCE_DIR}/exceptions/exceptions_unittest.cpp
               )

create_test("exceptions_unittests" exceptions_unittests)

######################################################
# Core Unit Tests
######################################################

add_executable(core_unittests
               ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lexer_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/core/utils/list_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/core/utils/map_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/core/utils/trie_params_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lextrie_unittest.cpp
               )

create_test("core_unittests" core_unittests)

