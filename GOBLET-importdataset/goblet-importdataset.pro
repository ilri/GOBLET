#-------------------------------------------------
#
# Project created by QtCreator 2012-01-13T15:08:40
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-importdataset
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp gridtocsv.cpp

HEADERS += gridtocsv.h





