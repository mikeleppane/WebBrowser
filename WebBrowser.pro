#-------------------------------------------------
#
# Project created by QtCreator 2014-03-24T19:31:09
#
#-------------------------------------------------

QT       += core gui webkitwidgets webkit sql

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebBrowser
TEMPLATE = app

LIBS += -L/usr/lib/x86_64-linux-gnu
#INCLUDEPATH += /usr/lib/i386-linux-gnu
INCLUDEPATH += /usr/lib/x86_64-linux-gnu

SOURCES += main.cpp\
        mainwindow.cpp \
    bookmarkdb.cpp \
    webhistory.cpp

HEADERS  += mainwindow.h \
    bookmarkdb.h \
    webhistory.h

FORMS    += mainwindow.ui
