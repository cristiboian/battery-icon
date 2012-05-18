#!/bin/sh

DESKTOP_FILE="/usr/share/applications/battery-icon.desktop"
TMP_FILE="/usr/share/applications/battery-icon.desktop.tmp"

cp $DESKTOP_FILE $TMP_FILE
mv -f $TMP_FILE $DESKTOP_FILE

