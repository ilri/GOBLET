#-------------------------------------------------
#
# Project created by QtCreator 2012-08-03T12:49:44
#
#-------------------------------------------------

QT       += core sql xml

QT       -= gui

TARGET = goblet-executesql
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
