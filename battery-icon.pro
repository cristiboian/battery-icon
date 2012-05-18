TEMPLATE = subdirs
SUBDIRS = daemon app

icons.files = icons/*
icons.path = /opt/battery-icon/icons


contains(MEEGO_EDITION,harmattan) {
    INSTALLS += icons
}

