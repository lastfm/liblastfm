cmake_minimum_required(VERSION 3.6)
project(liblastfm)

# general settings
set(LASTFM_SOVERSION 1)
set(LASTFM_VERSION "0x00010100")

set(LASTFM_MAJOR_VERSION "1")
set(LASTFM_MINOR_VERSION "1")
set(LASTFM_PATCH_VERSION "0")
set(LASTFM_VERSION_STRING "${LASTFM_MAJOR_VERSION}.${LASTFM_MINOR_VERSION}.${LASTFM_PATCH_VERSION}")

# options
option(BUILD_FINGERPRINT "Build the lastfm-fingerprint library" ON)
option(BUILD_DEMOS "Build the lastfm example programs" OFF)
option(BUILD_TESTS "Build liblastfm tests" ON)


# installation dirs
include(GNUInstallDirs)
include(FeatureSummary)
include(FindPkgConfig)

# setup qt stuff
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Network Xml)

if(BUILD_FINGERPRINT)
    find_package(Qt5Sql CONFIG REQUIRED)

    pkg_check_modules(LibSamplerate REQUIRED IMPORTED_TARGET samplerate)
    add_feature_info(LibSamplerate LibSamplerate_FOUND "Required for lastfm-fingerprint library")

    pkg_check_modules(FFTW3F REQUIRED IMPORTED_TARGET fftw3f>=3.0)
    add_feature_info(FFTW3F FFTW3F_FOUND "Required for lastfm-fingerprint library")
endif()

if(BUILD_TESTS)
    find_package(Qt5Test CONFIG REQUIRED)
endif()

set(LASTFM_LIB_VERSION_SUFFIX 5)
if(UNIX AND NOT APPLE)
    find_package(Qt5DBus CONFIG REQUIRED)
endif()

# macro(qt_wrap_ui)
#     qt5_wrap_ui(${ARGN})
# endmacro()
#
# macro(qt_add_resources)
#     qt5_add_resources(${ARGN})
# endmacro()
#
# find_package(Qt5LinguistTools REQUIRED)
# macro(qt_add_translation)
#     qt5_add_translation(${ARGN})
# endmacro()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-fno-operator-names -fvisibility-inlines-hidden -fvisibility=hidden")
endif()
if(UNIX AND NOT APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
endif(MSVC)

set(LASTFM_LIB_TARGET_NAME lastfm${LASTFM_LIB_VERSION_SUFFIX} CACHE
    INTERNAL "Target name of liblastfm" FORCE)

set(FINGERPRINT_LIB_TARGET_NAME lastfm_fingerprint${LASTFM_LIB_VERSION_SUFFIX} CACHE
    INTERNAL "Target name of liblastfm_fingerprint" FORCE)

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

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
