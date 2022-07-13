#-------------------------------------------------
#
# Project created by QtCreator 2012-04-24T15:26:14
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-outputshape
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty
unix:LIBS += -L/usr/lib64 /usr/lib/libshp.a

SOURCES += main.cpp writeshapefile.cpp

HEADERS += writeshapefile.h


