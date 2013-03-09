#-------------------------------------------------
#
# Project created by QtCreator 2013-01-03T03:15:17
#
#-------------------------------------------------

QT       += core gui printsupport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wwwtool-qt
TEMPLATE = app

CONFIG += silent

SOURCES += main.cpp\
    mainwindow.cpp \
    code128.cpp \
    addcommanddialog.cpp \
    gamemodel.cpp \
    questioncountdialog.cpp \
    keypresseater.cpp

HEADERS  += \
    mainwindow.h \
    code128.h \
    addcommanddialog.h \
    gamemodel.h \
    questioncountdialog.h \
    keypresseater.h

FORMS +=
