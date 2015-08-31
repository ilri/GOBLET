#-------------------------------------------------
#
# Project created by QtCreator 2012-04-23T17:51:30
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-resetaggregate
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

INCLUDEPATH += /usr/include/mysql ../common

unix:INCLUDEPATH += /usr/include/mysql ../common ../3rdParty
unix:LIBS += -L/lib64 -L/usr/lib64 \
    -lcrypt \
    -laio \
    -lmysqld \
    -ldl \
    -lrt \
    -lz

win32:INCLUDEPATH += C:\Qt\mysql\include ../common ../3rdParty
win32:LIBS += C:\Qt\mysql\lib\libmysqld.lib


SOURCES += main.cpp \
    ../common/embdriver.cpp \
    ../common/mydbconn.cpp

HEADERS += \
    ../common/embdriver.h \
    ../common/mydbconn.h \
