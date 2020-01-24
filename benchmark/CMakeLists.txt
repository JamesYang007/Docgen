add_executable(lexer_legacy_benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lexer_legacy_benchmark.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lexer_legacy_2_benchmark.cpp
    # Source dependency
    $<TARGET_OBJECTS:LEXER_LEGACY_2_LIB_OBJECTS>
    )
target_compile_features(lexer_legacy_benchmark PRIVATE cxx_std_17)
target_include_directories(lexer_legacy_benchmark PRIVATE
    ${GBENCH_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${ETERNAL_DIR}/include
    )
target_link_libraries(lexer_legacy_benchmark PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    pthread
    nlohmann_json::nlohmann_json
    )

add_executable(lexer_benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/lexer_benchmark.cpp
    $<TARGET_OBJECTS:LEXER_LIB_OBJECTS>
    )

target_compile_features(lexer_benchmark PRIVATE cxx_std_17)
target_include_directories(lexer_benchmark PRIVATE
    ${GBENCH_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${ETERNAL_DIR}/include
    )
target_link_libraries(lexer_benchmark PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    pthread
    nlohmann_json::nlohmann_json
    )

# copy data directory into where lexer_benchmark executable ends up
add_custom_command(
        TARGET lexer_benchmark POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/core/lex/data/
                $<TARGET_FILE_DIR:lexer_benchmark>/data)
