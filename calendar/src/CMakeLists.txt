project(plasma-calendar)

cmake_minimum_required(VERSION 2.6)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(calendar_SRCS plasma-calendar.cpp)
 
kde4_add_plugin(plasma_applet_calendar ${calendar_SRCS})
target_link_libraries(plasma_applet_calendar
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_calendar
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-calendar.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
 
