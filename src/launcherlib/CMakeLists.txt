include(GNUInstallDirs)

set(COMMON ${CMAKE_HOME_DIRECTORY}/src/common)

# Find systemd
find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD "libsystemd" REQUIRED)

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SYSTEMD_INCLUDE_DIRS} ${COMMON})

# Hide all symbols except the ones explicitly exported in the code (like main())
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

# Set sources
set(SRC appdata.cpp booster.cpp connection.cpp daemon.cpp logger.cpp
        singleinstance.cpp socketmanager.cpp)

set(HEADERS appdata.h booster.h connection.h daemon.h logger.h launcherlib.h
    singleinstance.h socketmanager.h ${COMMON}/protocol.h)

# Set libraries to be linked. Shared libraries to be preloaded are not linked in anymore,
# but dlopen():ed and listed in src/launcher/preload.h instead.
link_libraries(${LIBDL} "-L/lib -lcap")

# Set executable
add_library(applauncherd MODULE ${SRC} ${MOC_SRC})
set_target_properties(applauncherd PROPERTIES VERSION 0.1 SOVERSION 0)

target_link_libraries(applauncherd ${SYSTEMD_LIBRARIES})


# Add install rule
install(TARGETS applauncherd DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/applauncherd
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
