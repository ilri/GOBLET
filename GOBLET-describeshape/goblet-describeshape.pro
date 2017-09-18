#-------------------------------------------------
#
# Project created by QtCreator 2012-05-01T16:20:38
#
#-------------------------------------------------

QT       += core sql xml

QT       -= gui

TARGET = goblet-describeshape
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
