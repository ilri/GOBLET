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

unix:INCLUDEPATH += /usr/include/mysql \
    ../common \
    ../3rdParty \
    /usr/local/gnome/include/quazip
unix:LIBS += -L/lib64 -L/usr/lib64 -L/usr/local/gnome/lib \
    -lcrypt \
    -laio \
    -lmysqld \
    -ldl \
    -lrt \
    -lquazip \
    -lz
win32:INCLUDEPATH += C:\Qt\mysql\include \
    ../common \
    ../3rdParty \
    ../3rdParty/quazip-0.4.4/quazip ../3rdParty/zlib-1.2.5
win32:LIBS += C:\Qt\mysql\lib\libmysqld.lib \
    ../3rdParty/quazip-0.4.4/quazip/release/quazip.dll
SOURCES += main.cpp \
    ../common/embdriver.cpp \
    ../common/mydbconn.cpp
HEADERS += ../common/embdriver.h \
    ../common/mydbconn.h
RESOURCES += mysqlFiles.qrc
