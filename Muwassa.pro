#-------------------------------------------------
#
# Project created by QtCreator 2015-01-05T12:03:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Muwassa
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    databasemanager.cpp \
    printingservice.cpp \
    emailingservice.cpp \
    addmembersdialog.cpp


HEADERS  += mainwindow.h \
    databasemanager.h \
    printingservice.h \
    emailingservice.h \
    addmembersdialog.h



FORMS    += mainwindow.ui \
    addmembersdialog.ui

QT += printsupport
QT += sql

RC_FILE = muwassa.rc

