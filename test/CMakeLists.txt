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
# File IO Unit Tests
######################################################

#add_executable(io_unittests
#               ${CMAKE_CURRENT_SOURCE_DIR}/io/file_reader_unittest.cpp
#               )
#
#create_test("io_unittests" io_unittests)
#
## copy data directory into where io_unittests executable ends up
#add_custom_command(
#        TARGET io_unittests POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#                ${CMAKE_CURRENT_SOURCE_DIR}/io/io_data/
#                $<TARGET_FILE_DIR:io_unittests>/io_data)

######################################################
# Core Unit Tests
######################################################

add_executable(core_unittests
               ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/trie_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lexer_unittest.cpp
               # Source dependency
               $<TARGET_OBJECTS:LEXER_LIB_OBJECTS>
               )

create_test("core_unittests" core_unittests)


