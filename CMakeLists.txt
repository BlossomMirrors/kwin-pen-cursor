cmake_minimum_required(VERSION 3.16)

project(kwin-pencursor-effect)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui DBus Svg)

# Find individual KF6 components
find_package(KF6CoreAddons REQUIRED)
find_package(KF6Config REQUIRED)
find_package(KF6ConfigWidgets REQUIRED)
find_package(KF6KCMUtils REQUIRED)
find_package(KF6KIO REQUIRED)

# Find libinput and libudev
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBINPUT REQUIRED IMPORTED_TARGET libinput)
pkg_check_modules(LIBUDEV REQUIRED IMPORTED_TARGET libudev)

# Find KWin 6 headers
find_path(KWIN_EFFECT_INCLUDE_DIR
    NAMES effect/effect.h
    PATHS
        /usr/include/kwin
        /usr/local/include/kwin
    NO_DEFAULT_PATH
)

if(NOT KWIN_EFFECT_INCLUDE_DIR)
    message(FATAL_ERROR "KWin 6 effect headers not found. Install with: sudo pacman -S kwin")
endif()

message(STATUS "Found KWin headers: ${KWIN_EFFECT_INCLUDE_DIR}")

# Find D-Bus interfaces
find_file(KWIN_EFFECTS_INTERFACE
    NAMES org.kde.kwin.Effects.xml
    PATHS /usr/share/dbus-1/interfaces
    NO_DEFAULT_PATH
)

# Generate D-Bus interface for config module
qt_add_dbus_interface(kwin_effects_interface_SRCS
    ${KWIN_EFFECTS_INTERFACE}
    kwineffects_interface
)

# Build the effect
add_library(kwin_pencursor MODULE
    src/main.cpp
    src/pen_event_handlers.cpp
    src/overlay_visibility.cpp
    src/pen_input_listener.cpp
    src/overlay_renderer.cpp
)

target_include_directories(kwin_pencursor PRIVATE
    ${KWIN_EFFECT_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(kwin_pencursor
    Qt6::Core
    Qt6::Gui
    Qt6::Svg
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    PkgConfig::LIBINPUT
    PkgConfig::LIBUDEV
)

# Build config module
add_library(kwin_pencursor_config MODULE
    src/pencursor_kcm.cpp
    ${kwin_effects_interface_SRCS}
)

set_target_properties(kwin_pencursor_config PROPERTIES PREFIX "")

target_include_directories(kwin_pencursor_config PRIVATE
    ${KWIN_EFFECT_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(kwin_pencursor_config
    Qt6::Core
    Qt6::Widgets
    Qt6::DBus
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigWidgets
    KF6::KCMUtils
    KF6::KIOWidgets
)

# Install paths
if(NOT PLUGIN_INSTALL_DIR)
    set(PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/qt6/plugins")
endif()

if(NOT DATA_INSTALL_DIR)
    set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share")
endif()

# Install plugin
install(TARGETS kwin_pencursor
    DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins
)

install(TARGETS kwin_pencursor_config
    DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/configs
)

# Install metadata
install(FILES src/pencursor.json
    DESTINATION ${DATA_INSTALL_DIR}/kwin/effects
)

# Install SVG cursor
install(FILES pen-point-cursor.svg
    DESTINATION ${DATA_INSTALL_DIR}/kwin/effects
)

message(STATUS "Building Pen Cursor Effect for KWin 6")
message(STATUS "  Plugin install dir: ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins")
message(STATUS "  Data install dir: ${DATA_INSTALL_DIR}/kwin/effects")
