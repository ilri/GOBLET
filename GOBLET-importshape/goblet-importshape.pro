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

unix:INCLUDEPATH += /usr/include/mysql ../common ../3rdParty ../3rdParty/shapelib
unix:LIBS += -L/lib64 -L/usr/lib64 \
    -lcrypt \
    -laio \
    -lmysqld \
    -ldl \
    -lrt \
    ../3rdParty/shapelib/libshp.a \
    -lz

win32:INCLUDEPATH += C:\Qt\mysql\include ../common ../3rdParty ../3rdParty/shapelib
win32:LIBS += C:\Qt\mysql\lib\libmysqld.lib ../3rdParty/shapelib/shapelib.dll

SOURCES += main.cpp \
    ../common/embdriver.cpp \
    ../common/mydbconn.cpp \
    insertshape.cpp \
    LineEdge.cpp \
    shapeToGrid.cpp

HEADERS += \
    ../common/embdriver.h \
    ../common/mydbconn.h \
    insertshape.h \
    LineEdge.h \
    shapeToGrid.h




