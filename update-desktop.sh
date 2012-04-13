#!/bin/sh

FILE="/usr/share/applications/battery-icon.desktop"
TMP="/tmp/battery-icon.tmp"

cat $FILE > $TMP
cat $TMP > $FILE
