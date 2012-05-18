QT       += core

TARGET = battery-icon-app
CONFIG   += console qmsystem2 meegotouch
CONFIG   -= app_bundle
CONFIG += link_pkgconfig
PKGCONFIG += gq-gconf

TEMPLATE = app

MOC_DIR = .moc
OBJECTS_DIR = .obj

SOURCES += main.cpp batteryiconapp.cpp
HEADERS += batteryiconapp.h


contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/battery-icon/bin
    INSTALLS += target
}

