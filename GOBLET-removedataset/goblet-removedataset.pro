#-------------------------------------------------
#
# Project created by QtCreator 2012-07-12T10:46:29
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-removedataset
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
