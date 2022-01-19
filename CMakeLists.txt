cmake_minimum_required(VERSION 3.1.0)

project(cd2netmd_gui VERSION 0.1.7.7 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

find_library(CDIO NAMES cdio libcdio Hint /usr/lib/x86_64-linux-gnu)
find_library(CDIO_CDDA NAMES cdio_cdda libcdio-cdda Hint /usr/lib/x86_64-linux-gnu)
find_library(CDIO_PARANOIA NAMES cdio_paranoia libcdio-paranoia Hint /usr/lib/x86_64-linux-gnu)
find_library(JSON_C NAMES json-c libjson-c Hint /usr/lib/x86_64-linux-gnu)
find_library(GCRYPT NAMES gcrypt libgcrypt Hint /usr/lib/x86_64-linux-gnu)
find_library(LIB_USB NAMES usb-1.0 libusb-1.0 Hint /usr/lib/x86_64-linux-gnu)
find_library(GPG_ERR NAMES gpg-error libgpg-error libusb-1.0 Hint /usr/lib/x86_64-linux-gnu)
find_library(THREAD NAMES pthread libpthread libusb-1.0 Hint /usr/lib/x86_64-linux-gnu)

SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")

add_definitions(-DQT_DEPRECATED_WARNINGS -DQT_MESSAGELOGCONTEXT -DNO_PROGRAM)

include_directories(
    include
    externals/netmd/libnetmd
    externals/netmd/netmdcli
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(cd2netmd_gui
    caboutdialog.ui
    ccddbentriesdialog.ui
    cnamingdialog.ui
    mainwindow.ui
    settingsdlg.ui
    externals/netmd/libnetmd/CMDiscHeader.cpp
    externals/netmd/libnetmd/common.c
    externals/netmd/libnetmd/error.c
    externals/netmd/libnetmd/libnetmd.c
    externals/netmd/libnetmd/log.c
    externals/netmd/libnetmd/netmd_dev.c
    externals/netmd/libnetmd/playercontrol.c
    externals/netmd/libnetmd/secure.c
    externals/netmd/libnetmd/trackinformation.c
    externals/netmd/libnetmd/utils.c
    externals/netmd/netmdcli/netmdcli.c
    caboutdialog.cpp
    ccddb.cpp
    ccddbentriesdialog.cpp
    ccditemmodel.cpp
    ccdtableview.cpp
    ccliprocess.cpp
    cjacktheripper.cpp
    cmdtreemodel.cpp
    cmdtreeview.cpp
    cnamingdialog.cpp
    cnetmd.cpp
    cxenc.cpp
    main.cpp
    mainwindow.cpp
    helpers.cpp
    mdtitle.cpp
    settingsdlg.cpp
    resources.qrc
    qdarkstyle/dark/style_dk.qrc
    qdarkstyle/light/style_lt.qrc
)

target_link_libraries(cd2netmd_gui 
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Network 
    Qt5::Gui 
    ${CDIO} 
    ${CDIO_CDDA} 
    ${CDIO_PARANOIA} 
    ${JSON_C}
    ${LIB_USB}
    ${GCRYPT}
    ${GPG_ERR}
    ${THREAD}
)
