
#find_package(Qt4 COMPONENTS QtCore QtSql REQUIRED)
#FIXME: find modules are horrible, port them to find_package_handle_standard_args
find_package(LibSamplerate REQUIRED)
find_package(LibFFTW3 REQUIRED)

include_directories(${LIBFFTW3_INCLUDE_DIR})
include_directories(${LIBSAMPLERATE_INCLUDE_DIR})
include_directories(${QT_INCLUDES})
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

set(lastfm_fingerprint_SOURCES
  Collection.cpp
  Fingerprint.cpp
  Sha256.cpp
  fplib/Filter.cpp
  fplib/FingerprintExtractor.cpp
  fplib/OptFFT.cpp
)

set(lastfm_fingerprint_HEADERS
    Fingerprint.h
    FingerprintableSource.h
)

add_library(${FINGERPRINT_LIB_TARGET_NAME} SHARED ${lastfm_fingerprint_SOURCES})

set_target_properties(${FINGERPRINT_LIB_TARGET_NAME} PROPERTIES
    COMPILE_DEFINITIONS LASTFM_FINGERPRINT_LIB
    VERSION ${LASTFM_VERSION_STRING}
    SOVERSION ${LASTFM_SOVERSION}
)

target_link_libraries(${FINGERPRINT_LIB_TARGET_NAME}
    ${LASTFM_LIB_TARGET_NAME}
    ${LIBSAMPLERATE_LIBRARY}
    ${LIBFFTW3_LIBRARY}
)

if(Qt5Core_DIR)
    target_link_libraries(${FINGERPRINT_LIB_TARGET_NAME} Qt5::Network Qt5::Sql Qt5::Xml)
else()
    target_link_libraries(${FINGERPRINT_LIB_TARGET_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTSQL_LIBRARY})
endif()

install(TARGETS ${FINGERPRINT_LIB_TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${lastfm_fingerprint_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lastfm${LASTFM_LIB_VERSION_SUFFIX}/)
