# Adds strict debug compile options,
# includes directories, and links to googletest.
# TEST_TARGET is added as a test and an executable named ${TEST_NAME} will be created.
function(create_test TEST_NAME TEST_TARGET)
    target_compile_options(${TEST_TARGET} PRIVATE -g -Wall -Werror -Wextra)
    target_include_directories(${TEST_TARGET}
        PRIVATE ${GTEST_DIR}/include ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${TEST_TARGET}
        PRIVATE gtest_main pthread nlohmann_json::nlohmann_json)
    add_test(${TEST_NAME} ${TEST_TARGET})
endfunction(create_test)

######################################################
# Exception Unit Tests
######################################################

add_executable(exceptions_unittest
               ${CMAKE_CURRENT_SOURCE_DIR}/exceptions/exceptions_unittest.cpp
               )

create_test("exceptions_unittests" exceptions_unittest)

######################################################
# Parse Unit Tests
######################################################

add_executable(parse_unittest
               ${CMAKE_CURRENT_SOURCE_DIR}/parse/core/symbol_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/parse/core/read_routine_unittest.cpp
               )

create_test("parse_unittests" parse_unittest)
