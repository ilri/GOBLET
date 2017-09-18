#-------------------------------------------------
#
# Project created by QtCreator 2012-04-24T09:00:46
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-combineaggregate
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty


SOURCES += main.cpp
