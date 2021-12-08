QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += include

SOURCES += \
    ccddb.cpp \
    ccddbentriesdialog.cpp \
    ccditemmodel.cpp \
    ccdtableview.cpp \
    ccliprocess.cpp \
    cjacktheripper.cpp \
    cmdtreemodel.cpp \
    cmdtreeview.cpp \
    cnetmd.cpp \
    cxenc.cpp \
    main.cpp \
    mainwindow.cpp \
    utils.cpp

HEADERS += \
    ccddb.h \
    ccddbentriesdialog.h \
    ccditemmodel.h \
    ccdtableview.h \
    ccliprocess.h \
    cjacktheripper.h \
    cmdtreemodel.h \
    cmdtreeview.h \
    cnetmd.h \
    cxenc.h \
    include/json.hpp \
    mainwindow.h \
    utils.h

FORMS += \
    ccddbentriesdialog.ui \
    mainwindow.ui

TRANSLATIONS += \
    QCD2NetMD_de_AT.ts

LIBS += -static-libgcc -lcdio -lcdio_cdda -lcdio_paranoia

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32: RC_FILE = program.rc
