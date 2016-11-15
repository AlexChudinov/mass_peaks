#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T11:53:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = mass_peaks
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    graphics/qcustomplot/qcustomplot.cpp \
    graphics/zoom_plot.cpp \
    app_data/data_export.cpp \
    app_data_handler/app_data_handler.cpp

HEADERS  += mainwindow.h \
    graphics/qcustomplot/qcustomplot.h \
    graphics/zoom_plot.h \
    app_data/app_data.h \
    app_data/data_export.h \
    app_data_handler/app_data_handler.h \
    app_data/math/solvers.h \
    app_data/math/spline.h \
    app_data/math/sym_diag_matrix.h

FORMS    += mainwindow.ui

DISTFILES += \
    graphics/icons/hzoom_icon.png \
    graphics/icons/vzoom_icon.png \
    graphics/icons/zoom_out_icon.png \
    graphics/qcustomplot/changelog.txt \
    graphics/qcustomplot/GPL.txt \
    test_data_files/205.txt \
    test_data_files/206.txt

RESOURCES += \
    graphics/graphics.qrc \
    resources.qrc
