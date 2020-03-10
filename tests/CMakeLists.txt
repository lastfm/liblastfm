include(lastfm_add_test.cmake)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)

if(NOT BUILD_WITH_QT4)
    find_package(Qt5Test REQUIRED)
endif()

lastfm_add_test(UrlBuilder)
lastfm_add_test(Track)
