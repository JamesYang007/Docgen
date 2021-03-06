cmake_minimum_required(VERSION 3.0)

project("Docgen" VERSION 1.0.0
                 DESCRIPTION "A C++ documentation generator.")

# This will perform memcheck
include(CTest)

# enables testing
enable_testing()

# This is to make this library portable to other machines.
# This will be used for install.
include(GNUInstallDirs)

# Set this such that dependency installation through conan can be found
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/libs)
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# Create DocgenConfigVersion.cmake which contains current project version
# This is supposed to help with (major) version compatibility.
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Automate the choosing of config
# if CMAKE_BUILD_TYPE not defined
if (NOT CMAKE_BUILD_TYPE)
    # if binary directory ends with "release", use release mode
    if (${PROJECT_BINARY_DIR} MATCHES "release$")
        set(CMAKE_BUILD_TYPE RELEASE)
    # otherwise, use debug mode
    else()
        set(CMAKE_BUILD_TYPE DEBUG)
    endif()
endif()
message(STATUS "Compiling in ${CMAKE_BUILD_TYPE} mode")

# Points to the root of Google Test
set(GTEST_DIR ${PROJECT_SOURCE_DIR}/libs/benchmark/googletest/googletest)
set(GBENCH_DIR ${PROJECT_SOURCE_DIR}/libs/benchmark)
set(ETERNAL_DIR ${PROJECT_SOURCE_DIR}/libs/eternal)

# find json library
find_package(nlohmann_json 3.2.0 REQUIRED)

# find google benchmark
find_package(benchmark REQUIRED PATHS ${GBENCH_DIR}/build)

# add libs subdirectory
add_subdirectory(${PROJECT_SOURCE_DIR}/libs ${PROJECT_BINARY_DIR}/libs)

# add src subdirectory
add_subdirectory(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

# add benchmark subdirectory
add_subdirectory(${PROJECT_SOURCE_DIR}/benchmark ${PROJECT_BINARY_DIR}/benchmark)

# add test subdirectory
add_subdirectory(${PROJECT_SOURCE_DIR}/test ${PROJECT_BINARY_DIR}/test)
