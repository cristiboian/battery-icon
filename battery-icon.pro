#-------------------------------------------------
#
# Project created by QtCreator 2012-02-04T13:43:09
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = battery-icon
CONFIG   += console mobility qmsystem2
CONFIG   -= app_bundle
CONFIG += link_pkgconfig
PKGCONFIG += gq-gconf

TEMPLATE = app

MOC_DIR = moc
OBJECTS_DIR = obj

SOURCES += main.cpp

OTHER_FILES += \
    debian/rules \
    debian/README \
    debian/manifest.aegis \
    debian/copyright \
    debian/control \
    debian/compat \
    debian/changelog

desktop.files = "battery-icon.desktop"
desktop.path = /usr/share/applications

icons.files = icons/*
icons.path = /opt/battery-icon/icons

scripts.files = "update-desktop.sh" "start-battery-icon.sh"
scripts.path = /opt/battery-icon/bin

init.files = "battery-icon.conf"
init.path = /etc/init/apps

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/battery-icon/bin
    INSTALLS += target desktop icons scripts init
}

