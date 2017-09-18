#-------------------------------------------------
#
# Project created by QtCreator 2012-04-12T15:33:16
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = goblet-importshape
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#unix:QMAKE_POST_LINK=strip $(TARGET)

unix:INCLUDEPATH += ../3rdParty /usr/include
unix:LIBS += -L/usr/lib64 /usr/lib64/libshp.a

SOURCES += main.cpp \
    insertshape.cpp \
    LineEdge.cpp \
    shapeToGrid.cpp

HEADERS += insertshape.h \
    LineEdge.h \
    shapeToGrid.h




