# -------------------------------------------------
# Project created by QtCreator 2011-12-24T02:22:42
# -------------------------------------------------
QT += core \
    sql
QT -= gui
TARGET = goblet-createdb
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

unix:QMAKE_POST_LINK=strip $(TARGET)

INCLUDEPATH += ../3rdParty

SOURCES += main.cpp
