#-------------------------------------------------
#
# Project created by QtCreator 2016-10-21T13:33:42
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shadows
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    scene.cpp \
    loader.cpp

HEADERS  += mainwindow.h \
    scene.h \
    loader.h

FORMS    += mainwindow.ui

RESOURCES += \
    shaders.qrc
