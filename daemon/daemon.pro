QT       += core

#QT       -= gui

TARGET = battery-icon
CONFIG   += console qmsystem2
CONFIG   -= app_bundle
CONFIG += link_pkgconfig
PKGCONFIG += gq-gconf

TEMPLATE = app

MOC_DIR = .moc
OBJECTS_DIR = .obj

SOURCES += main.cpp

desktop.files = "battery-icon.desktop"
desktop.path = /usr/share/applications

scripts.files = "update-desktop.sh" "start-battery-icon.sh"
scripts.path = /opt/battery-icon/bin

init.files = "battery-icon.conf"
init.path = /etc/init/apps

other.files = "battery-icon.desktop"
other.path = /opt/battery-icon/var

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/battery-icon/bin
    INSTALLS += target desktop scripts init other
}

