#-------------------------------------------------
#
# Project created by QtCreator 2012-04-22T21:47:16
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-combinedatasets
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp

