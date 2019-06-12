#-------------------------------------------------
#
# Project created by QtCreator 2019-06-10T19:14:31
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        my_widget.cpp \
    serialthread.cpp \
    crc16.cpp

HEADERS  += my_widget.h \
    serialthread.h \
    crc16.h

FORMS    += my_widget.ui
