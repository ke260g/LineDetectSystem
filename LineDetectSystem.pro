#-------------------------------------------------
#
# Project created by QtCreator 2018-04-17T11:28:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LineDetectSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    qtreemethod.cpp \
    imagthread.cpp \
    qjsonmethod.cpp \
    common_detect.cpp \
    dbscan.cpp

HEADERS  += mainwindow.h \
    qtreemethod.h \
    imagthread.h \
    qjsonmethod.h \
    common_detect.h \
    dbscan.h

FORMS    += mainwindow.ui

LIBS += -lmvsdk_iface \
        `pkg-config --libs opencv`

QMAKE_CXXFLAGS += `pkg-config --cflags opencv` \
