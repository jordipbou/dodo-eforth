cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "-ansi -pedantic")

project(dodo_eforth LANGUAGES C)

set(dodo_eforth_VERSION_MAJOR 0)
set(dodo_eforth_VERSION_MINOR 1)

add_executable (
	dodo_eforth 
	vm/c89/eforth)

target_include_directories (dodo_eforth PRIVATE vm/c89)
