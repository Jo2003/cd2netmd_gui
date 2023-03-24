cmake_minimum_required(VERSION 3.1.0)

project(netmd_wizard VERSION 0.1.9.2 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

find_library(GCRYPT NAMES gcrypt libgcrypt Hint /usr/lib/x86_64-linux-gnu)
find_library(GPG_ERR NAMES gpg-error libgpg-error Hint /usr/lib/x86_64-linux-gnu)
find_library(THREAD NAMES pthread libpthread Hint /usr/lib/x86_64-linux-gnu)
find_library(UDEV NAMES udev libudev Hint /usr/lib/x86_64-linux-gnu)

pkg_check_modules(CDIO REQUIRED libcdio)
pkg_check_modules(CDDA REQUIRED libcdio_cdda)
pkg_check_modules(PARANOIA REQUIRED libcdio_paranoia)
pkg_check_modules(CUE REQUIRED libcue)
pkg_check_modules(JSONC REQUIRED json-c)
pkg_check_modules(LIBISO REQUIRED libiso9660)
pkg_check_modules(LIBUDF REQUIRED libudf)
pkg_check_modules(TAGLIB REQUIRED taglib)
pkg_check_modules(NETMD REQUIRED libnetmd)
pkg_check_modules(USB REQUIRED libusb-1.0)

SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")

add_definitions(-DQT_DEPRECATED_WARNINGS -DQT_MESSAGELOGCONTEXT -DNO_PROGRAM)

# include directories
set(IDIRS
    include
    ${CDIO_INCLUDE_DIRS}
	${CDDA_INCLUDE_DIRS}
	${PARANOIA_INCLUDE_DIRS}
	${CUE_INCLUDE_DIRS}
	${JSONC_INCLUDE_DIRS}
	${LIBISO_INCLUDE_DIRS}
	${LIBUDF_INCLUDE_DIRS}
	${TAGLIB_INCLUDE_DIRS}
	${NETMD_INCLUDE_DIRS}
	${USB_INCLUDE_DIRS}
    .
)

list(REMOVE_DUPLICATES "IDIRS")

include_directories(${IDIRS})

# link directories
set(LDIRS
	${CDIO_LIBRARY_DIRS}
	${CDIO_LIBRARY_DIRS}
	${CDDA_LIBRARY_DIRS}
	${PARANOIA_LIBRARY_DIRS}
	${CUE_LIBRARY_DIRS}
	${JSONC_LIBRARY_DIRS}
	${LIBISO_LIBRARY_DIRS}
	${LIBUDF_LIBRARY_DIRS}
	${TAGLIB_LIBRARY_DIRS}
	${NETMD_LIBRARY_DIRS}
	${USB_LIBRARY_DIRS}
)

list(REMOVE_DUPLICATES "LDIRS")

link_directories(${LDIRS})

# static libs
set(SLIBS
    ${CDIO_LIBRARIES}
	${CDDA_LIBRARIES}
	${PARANOIA_LIBRARIES}
	${CUE_LIBRARIES}
	${JSONC_LIBRARIES}
	${LIBISO_LIBRARIES}
	${LIBUDF_LIBRARIES}
	${TAGLIB_LIBRARIES}
	${NETMD_LIBRARIES}
	${USB_LIBRARIES}
)

list(REMOVE_DUPLICATES "SLIBS")

add_executable(netmd_wizard
    caboutdialog.ui
    ccddbentriesdialog.ui
    cdaoconfdlg.ui
    cnamingdialog.ui
    mainwindow.ui
    settingsdlg.ui
    caboutdialog.cpp
    ccddb.cpp
    ccddbentriesdialog.cpp
    ccditemmodel.cpp
    ccdtableview.cpp
    ccliprocess.cpp
    cdaoconfdlg.cpp
    cjacktheripper.cpp
    cmdtreemodel.cpp
    cmdtreeview.cpp
    cnamingdialog.cpp
    cnetmd.cpp
    cxenc.cpp
    cffmpeg.cpp
    audio.cpp
    main.cpp
    mainwindow.cpp
    helpers.cpp
    mdtitle.cpp
    settingsdlg.cpp
    resources.qrc
    qdarkstyle/dark/style_dk.qrc
    qdarkstyle/light/style_lt.qrc
)

set(MYCFLAGS
    -W -Wall -Wformat-overflow=0 
	-Wno-unused-result -Wformat=0
	${CDIO_CFLAGS_OTHER}
	${CDDA_CFLAGS_OTHER}
	${PARANOIA_CFLAGS_OTHER}
	${CUE_CFLAGS_OTHER}
	${JSONC_CFLAGS_OTHER}
	${LIBISO_CFLAGS_OTHER}
	${LIBUDF_CFLAGS_OTHER}
	${TAGLIB_CFLAGS_OTHER}
	${NETMD_CFLAGS_OTHER}
	${USB_CFLAGS_OTHER}
)

list(REMOVE_DUPLICATES "MYCFLAGS")

target_compile_options(netmd_wizard PRIVATE ${MYCFLAGS})

target_link_libraries(netmd_wizard
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Network 
    Qt5::Gui 
    ${SLIBS}
    ${LIB_USB}
    ${GCRYPT}
    ${GPG_ERR}
    ${THREAD}
    ${UDEV}
)
