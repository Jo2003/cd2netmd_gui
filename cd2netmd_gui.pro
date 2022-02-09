QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 debug_and_release

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
           QT_MESSAGELOGCONTEXT \
           NO_PROGRAM \
           TAGLIB_STATIC

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += include \
               externals/netmd/libnetmd \
               externals/netmd/netmdcli
			   
mac{
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libiso9660 libudf libcue json-c
    INCLUDEPATH += /usr/local/Cellar/libusb/1.0.24/include \
                   /usr/local/Cellar/libgcrypt/1.9.4_1/include \
                   /usr/local/Cellar/libgpg-error/1.43/include \
                   /usr/local/include
    LIBS += -L/usr/local/lib -lgcrypt -lusb-1.0 -lgpg-error
}

win32{
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libiso9660 libudf libcue json-c taglib
    INCLUDEPATH += /mingw64/lib/gcc/x86_64-w64-mingw32/10.3.0/include
    RC_FILE = program.rc
    LIBS += -lws2_32 -lgcrypt -lusb-1.0 -lgpg-error -static-libgcc
}

linux{
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/7/include
	LIBS += -lcue -lcdio -lcdio_cdda -lcdio_paranoia -ljson-c -lgcrypt -lusb-1.0 -lgpg-error -static-libgcc
}

SOURCES += \
    externals/netmd/libnetmd/CMDiscHeader.cpp \
    externals/netmd/libnetmd/common.c \
    externals/netmd/libnetmd/error.c \
    externals/netmd/libnetmd/libnetmd.c \
    externals/netmd/libnetmd/log.c \
    externals/netmd/libnetmd/netmd_dev.c \
    externals/netmd/libnetmd/playercontrol.c \
    externals/netmd/libnetmd/secure.c \
    externals/netmd/libnetmd/trackinformation.c \
    externals/netmd/libnetmd/utils.c \
    externals/netmd/netmdcli/netmdcli.c \
    caboutdialog.cpp \
    ccddb.cpp \
    ccddbentriesdialog.cpp \
    ccditemmodel.cpp \
    ccdtableview.cpp \
    ccliprocess.cpp \
    cjacktheripper.cpp \
    cmdtreemodel.cpp \
    cmdtreeview.cpp \
    cnamingdialog.cpp \
    cnetmd.cpp \
    cxenc.cpp \
    cffmpeg.cpp \
    main.cpp \
    mainwindow.cpp \
    helpers.cpp \
    mdtitle.cpp \
    settingsdlg.cpp \
    audio.cpp

HEADERS += \
    externals/netmd/libnetmd/CMDiscHeader.h \
    externals/netmd/libnetmd/const.h \
    externals/netmd/libnetmd/common.h \
    externals/netmd/libnetmd/error.h \
    externals/netmd/libnetmd/libnetmd.h \
    externals/netmd/libnetmd/log.h \
    externals/netmd/libnetmd/netmd_dev.h \
    externals/netmd/libnetmd/playercontrol.h \
    externals/netmd/libnetmd/secure.h \
    externals/netmd/libnetmd/trackinformation.h \
    externals/netmd/libnetmd/utils.h \
    externals/netmd/netmdcli/netmdcli.h \
    caboutdialog.h \
    ccddb.h \
    ccddbentriesdialog.h \
    ccditemmodel.h \
    ccdtableview.h \
    ccliprocess.h \
    cjacktheripper.h \
    cmdtreemodel.h \
    cmdtreeview.h \
    cnamingdialog.h \
    cnetmd.h \
    cxenc.h \
    cffmpeg.h \
    defines.h \
    include/json.hpp \
    mainwindow.h \
    helpers.h \
    mdtitle.h \
    settingsdlg.h \
    audio.h

FORMS += \
    caboutdialog.ui \
    ccddbentriesdialog.ui \
    cnamingdialog.ui \
    mainwindow.ui \
    settingsdlg.ui

TRANSLATIONS += \
    QCD2NetMD_de_AT.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    qdarkstyle/dark/style_dk.qrc \
    qdarkstyle/light/style_lt.qrc

!win32 {
    CONFIG(debug, debug|release):DESTDIR = debug
    else:DESTDIR = release
}
