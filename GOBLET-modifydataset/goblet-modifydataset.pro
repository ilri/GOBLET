#-------------------------------------------------
#
# Project created by QtCreator 2012-06-11T10:33:18
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-modifydataset
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
