#-------------------------------------------------
#
# Project created by QtCreator 2012-06-04T10:07:58
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-datasetcalc
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
