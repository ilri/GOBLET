#-------------------------------------------------
#
# Project created by QtCreator 2012-08-04T06:52:12
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-aggregatetoshape
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
