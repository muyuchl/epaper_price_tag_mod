#-------------------------------------------------
#
# Project created by QtCreator 2019-06-05T09:54:39
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_pc_tool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    uarthandler.cpp \
    dialogportconfig.cpp \
    formled.cpp \
    formtraffic.cpp \
    a7105pingtest.cpp \
    forma7105pingtest.cpp \
    a7105uploader.cpp \
    forma7105uploader.cpp \
    a7105downloader.cpp \
    forma7105download.cpp \
    monoimagelabel.cpp \
    formtest.cpp \
    dialogtexttoimage.cpp \
    texttobingenerator.cpp

HEADERS += \
        mainwindow.h \
    uarthandler.h \
    command.h \
    dialogportconfig.h \
    formled.h \
    formtraffic.h \
    a7105pingtest.h \
    forma7105pingtest.h \
    a7105uploader.h \
    forma7105uploader.h \
    a7105downloader.h \
    forma7105download.h \
    monoimagelabel.h \
    formtest.h \
    dialogtexttoimage.h \
    texttobingenerator.h

FORMS += \
        mainwindow.ui \
    dialogportconfig.ui \
    formled.ui \
    formspiflash.ui \
    formtraffic.ui \
    forma7105.ui \
    formepaperimage.ui \
    forma7105pingtest.ui \
    forma7105uploader.ui \
    forma7105download.ui \
    formtest.ui \
    dialogtexttoimage.ui

# Instructions to compile the icon.
RC_FILE = icon/simulator.rc

RESOURCES += \
    resource.qrc

