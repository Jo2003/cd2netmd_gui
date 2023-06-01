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
INCLUDEPATH += include

TARGET = netmd_wizard

mac{
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libiso9660 libudf taglib libnetmd++
    INCLUDEPATH += /usr/local/Cellar/libusb/1.0.24/include \
                   /usr/local/Cellar/libgcrypt/1.9.4_1/include \
                   /usr/local/Cellar/libgpg-error/1.43/include \
                   /usr/local/include
    LIBS += -L/usr/local/lib

    # since libcdio doesn't support CD-Text on Mac, use drutil
    QT += xml
    SOURCES += cdrutil.cpp
    HEADERS += cdrutil.h
}

win32{
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libiso9660 libudf taglib libnetmd++
    # INCLUDEPATH += /mingw64/lib/gcc/x86_64-w64-mingw32/10.3.0/include
    RC_FILE = program.rc
    LIBS += -lws2_32
}

linux{
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/7/include
        LIBS += -lcdio -lcdio_cdda -lcdio_paranoia -ljson-c -lgcrypt -lusb-1.0 -lgpg-error -static-libgcc
}

SOURCES += \
    cdaoconfdlg.cpp \
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
    ctocmanip.cpp \
    cueparser.cpp \
    cxenc.cpp \
    cffmpeg.cpp \
    main.cpp \
    mainwindow.cpp \
    helpers.cpp \
    mdtitle.cpp \
    settingsdlg.cpp \
    audio.cpp \
    statuswidget.cpp

HEADERS += \
    cdaoconfdlg.h \
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
    ctocmanip.h \
    cueparser.h \
    cxenc.h \
    cffmpeg.h \
    defines.h \
    include/json.hpp \
    mainwindow.h \
    helpers.h \
    mdtitle.h \
    settingsdlg.h \
    audio.h \
    statuswidget.h

FORMS += \
    caboutdialog.ui \
    ccddbentriesdialog.ui \
    cdaoconfdlg.ui \
    cnamingdialog.ui \
    mainwindow.ui \
    settingsdlg.ui \
    statuswidget.ui

TRANSLATIONS += \
    QCD2NetMD_de_AT.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    qdarkstyle/dark/darkstyle.qrc \
    qdarkstyle/light/lightstyle.qrc

!win32 {
    CONFIG(debug, debug|release):DESTDIR = debug
    else:DESTDIR = release
}
