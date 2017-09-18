#-------------------------------------------------
#
# Project created by QtCreator 2012-04-18T12:46:37
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-aggregatedataset
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty
SOURCES += main.cpp

