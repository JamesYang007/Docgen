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

add_executable(exceptions_unittests
               ${CMAKE_CURRENT_SOURCE_DIR}/exceptions/exceptions_unittest.cpp
               )

create_test("exceptions_unittests" exceptions_unittests)

######################################################
# File IO Unit Tests
######################################################

add_executable(file_io_unittests
               ${CMAKE_CURRENT_SOURCE_DIR}/file_io/file_reader_unittest.cpp
               )

create_test("file_io_unittests" file_io_unittests)

# copy data directory into where file_io_unittests executable ends up
add_custom_command(
        TARGET file_io_unittests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/file_io/file_io_data/
                $<TARGET_FILE_DIR:file_io_unittests>/file_io_data)
