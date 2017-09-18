#-------------------------------------------------
#
# Project created by QtCreator 2012-04-05T00:12:50
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-classifydataset
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
