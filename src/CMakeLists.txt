configure_file(${CMAKE_CURRENT_SOURCE_DIR}/global.h.in ${CMAKE_CURRENT_BINARY_DIR}/global.h)


add_definitions(${QT_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
set(liblastfm_LIBRARIES
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTXML_LIBRARY}
)

set(liblastfm_SOURCES
        ws.cpp
        NetworkConnectionMonitor.cpp
        NetworkAccessManager.cpp
        InternetConnectionMonitor.cpp
        Xspf.cpp
        User.cpp
        Track.cpp
        Tasteometer.cpp
        Tag.cpp
        Playlist.cpp
        Mbid.cpp
        FingerprintId.cpp
        Artist.cpp
        Album.cpp
        ScrobbleCache.cpp
        ScrobblePoint.cpp
        Audioscrobbler.cpp
        RadioTuner.cpp
        RadioStation.cpp
        XmlQuery.cpp
        UrlBuilder.cpp
        misc.cpp
        Chart.cpp
        Auth.cpp
        Library.cpp
)

if(WIN32)
    add_definitions("-DWIN32_LEAN_AND_MEAN")

    if(NOT MINGW)
        add_definitions("-D_ATL_DLL -D_CRT_SECURE_NO_WARNINGS")

        list(APPEND liblastfm_SOURCES
            win/WNetworkConnectionMonitor_win.cpp
            win/WmiSink.cpp
            win/Pac.cpp
            win/NdisEvents.cpp
        )
        list(APPEND liblastfm_LIBRARIES
            winhttp
            wbemuuid
        )
    endif()
endif()

if(APPLE)
    #FIXME: enable this when deploying
    #set(CMAKE_OSX_ARCHITECTURES "i386;ppc")
    #set(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)
    #set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX${CMAKE_OSX_DEPLOYMENT_TARGET}.sdk")

    list(APPEND liblastfm_SOURCES
        mac/MNetworkConnectionMonitor_mac.cpp
    )

    find_library(SYSTEMCONFIGURATION_LIBRARY SystemConfiguration)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND liblastfm_LIBRARIES
        ${COREFOUNDATION_LIBRARY}
        ${SYSTEMCONFIGURATION_LIBRARY}
    )
endif()

if(UNIX AND NOT APPLE)
    list(APPEND liblastfm_SOURCES
        linux/LNetworkConnectionMonitor_linux.cpp
    )

    find_package(Qt4 COMPONENTS QtDbus REQUIRED)
    list(APPEND liblastfm_LIBRARIES ${QT_QTDBUS_LIBRARY})
endif()

add_library(lastfm SHARED ${liblastfm_SOURCES})
target_link_libraries(lastfm ${liblastfm_LIBRARIES})
set_target_properties(lastfm PROPERTIES
    VERSION ${LASTFM_VERSION_STRING}
    SOVERSION ${LASTFM_SOVERSION}
    COMPILE_DEFINITIONS LASTFM_LIB
)

install(TARGETS lastfm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

file(GLOB liblastfm_HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.h)
list(APPEND liblastfm_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/global.h)
install(FILES ${liblastfm_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lastfm/)
