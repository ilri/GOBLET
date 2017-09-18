#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T15:02:29
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-gendatasetstats
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp

