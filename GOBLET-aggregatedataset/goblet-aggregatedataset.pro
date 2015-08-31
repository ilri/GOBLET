#-------------------------------------------------
#
# Project created by QtCreator 2012-04-18T12:46:37
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-aggregatedataset
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

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

