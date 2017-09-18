#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T14:28:48
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-extractmetadata
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty


SOURCES += main.cpp
