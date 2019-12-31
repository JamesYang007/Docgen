cmake_minimum_required(VERSION 3.7)

project("Docgen" VERSION 1.0.0
                 DESCRIPTION "A fast documentation generator.")

# This will perform memcheck
include(CTest)

# This is to make this library portable to other machines.
# This will be used for install.
include(GNUInstallDirs)

