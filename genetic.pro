#-------------------------------------------------
#
# Project created by QtCreator 2015-06-22T17:32:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = genetic
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    tourmanager.cpp \
    geneticengine.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    city.h \
    tourmanager.h \
    tour.h \
    population.h \
    geneticengine.h

FORMS    += mainwindow.ui

RESOURCES += \
    application.qrc

TRANSLATIONS = genetic_ru_RU.ts

# win32:RC_FILE += application.rc
win32:RC_ICONS += 531.ico

# QMAKE_LFLAGS += -static -static-libgcc
