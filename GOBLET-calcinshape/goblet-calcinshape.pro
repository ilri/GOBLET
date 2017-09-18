#-------------------------------------------------
#
# Project created by QtCreator 2012-06-11T16:56:08
#
#-------------------------------------------------

QT       += core sql xml

QT       -= gui

TARGET = goblet-calcinshape
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty

SOURCES += main.cpp


