#-------------------------------------------------
#
# Project created by QtCreator 2012-04-19T11:18:10
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-outputraster
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
