#-------------------------------------------------
#
# Project created by QtCreator 2014-05-05T21:48:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DBGenerator
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    TableAttribute.cpp

HEADERS  += MainWindow.h \
    TableAttribute.h

FORMS    += MainWindow.ui

RESOURCES += \
    Images.qrc

RC_FILE = ApplicationIcon.rc

OTHER_FILES += \
    DBGeneratorIcon.png \
    DBGeneratorIcon.ico \
    ApplicationIcon.rc
