#-------------------------------------------------
#
# Project created by QtCreator 2012-06-08T17:43:57
#
#-------------------------------------------------

QT       += core sql xml

QT       -= gui

TARGET = goblet-listdatasets
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)
unix:INCLUDEPATH += ../3rdParty
SOURCES += main.cpp
