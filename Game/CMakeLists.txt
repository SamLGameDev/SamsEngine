cmake_minimum_required(VERSION 3.3)

# see https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html#visual-studio if using visual studio
# On Linux-based distrubtions, use your package manager for dependeices

# VCPKG integration for Windows. See my guide on getting that setup
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

message(STATUS "VCPKG_ROOT = $ENV{VCPKG_ROOT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")


# Use C++ 17
set(CMAKE_CXX_STANDARD 23)

set(Contents "${CMAKE_SOURCE_DIR}/Contents")

project(SamsEngine
	VERSION 0.1
	DESCRIPTION "A brief description of your project here"
	LANGUAGES CXX
	)

# Option support (faster build times by disabling features)
option(FAL_TESTS "Should we enable unit test support?" ON)
option(FAL_DOCS "Should we attempt to build documentation?" ON)


# integration for command-line users
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})#

message(STATUS "FAL_TESTS = ${FAL_TESTS}")

add_subdirectory( src )

# Documentation support
if (FAL_DOCS)
	find_package(Doxygen)
	if (Doxygen_FOUND)
		add_subdirectory( docs )
	else()
		message(STATUS "Doxygen not found, no docs will be built")
	endif()
endif()

target_compile_definitions(SamsEngine PRIVATE DEBUG=0)

target_compile_definitions(SamsEngine PRIVATE RELEASE=0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(SamsEngine PRIVATE DEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(SamsEngine PRIVATE RELEASE=1)
endif()

target_compile_definitions(SamsEngine PUBLIC CONTENTS_DIR="${Contents}")
message(STATUS "Contents = ${Contents}")
