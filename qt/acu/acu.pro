#-------------------------------------------------
#
# Project created by QtCreator 2014-10-20T23:48:08
#
#-------------------------------------------------

QT       += core gui

TARGET = acu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DEFINES = SIMULATOR
INCLUDEPATH += /home/mikekang/Project_il
LIBS = -L/home/mikekang/Project_il -L/home/mikekang/Project_il/tools -lfid -ltool -L/home/mikekang/Project_il/inih_r29/arch/x86_64 -linih -lpthread -lrt

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
