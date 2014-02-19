#-------------------------------------------------
#
# Project created by QtCreator 2013-01-03T03:15:17
#
#-------------------------------------------------

QT       += core gui printsupport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WWWTool
TEMPLATE = app

CONFIG += silent

SOURCES += main.cpp\
    mainwindow.cpp \
    code128.cpp \
    addcommanddialog.cpp \
    gamemodel.cpp \
    questioncountdialog.cpp \
    keypresseater.cpp \
    scannermanager.cpp \
    tablepainterdelegate.cpp

HEADERS  += \
    mainwindow.h \
    code128.h \
    addcommanddialog.h \
    gamemodel.h \
    questioncountdialog.h \
    keypresseater.h \
    scannermanager.h \
    tablepainterdelegate.h

RESOURCES += resources/templates.qrc

win32 {
    RC_FILE = resources/wwwtool.rc
}

mac {
    system(cd $$PWD/resources; ./create_icns.sh)
    ICON = resources/wwwtool-old.icns
    QMAKE_INFO_PLIST = resources/Info.plist.template
    QMAKE_POST_LINK += macdeployqt $(QMAKE_TARGET).app -dmg
}
