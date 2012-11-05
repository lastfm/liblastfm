cmake_minimum_required(VERSION 2.8.6)
project(liblastfm)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# general settings
set(LASTFM_SOVERSION 1)
set(LASTFM_VERSION "0x00010003")

set(LASTFM_MAJOR_VERSION "1")
set(LASTFM_MINOR_VERSION "0")
set(LASTFM_PATCH_VERSION "3")
set(LASTFM_VERSION_STRING "${LASTFM_MAJOR_VERSION}.${LASTFM_MINOR_VERSION}.${LASTFM_PATCH_VERSION}")

# options
option(BUILD_FINGERPRINT "Build the lastfm-fingerprint library" ON)
option(BUILD_DEMOS "Build the lastfm example programs" OFF)
option(BUILD_TESTS "Build liblastfm tests" ON)


# installation dirs
include(GNUInstallDirs)

#cmake module path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake/Modules")

# setup qt stuff
find_package(Qt4 COMPONENTS QtCore QtNetwork QtXml REQUIRED)
set(CMAKE_AUTOMOC TRUE)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-fno-operator-names -fvisibility-inlines-hidden -fvisibility=hidden")
endif()
if(UNIX AND NOT APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
endif(MSVC)

# main library
add_subdirectory(src)

# lastfm_fingerprint library
if(BUILD_FINGERPRINT)
    add_subdirectory(src/fingerprint)
endif()

# demos
if(BUILD_DEMOS)
    add_subdirectory(demos)
endif()

# tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
