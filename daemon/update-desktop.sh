#!/bin/sh

# Copyright (C) 2012 Cristi Boian
# This file is part of Battery Icon
# License: GPLv2


DESKTOP_FILE="/usr/share/applications/battery-icon.desktop"
TMP_FILE="/usr/share/applications/battery-icon.desktop.tmp"

cp $DESKTOP_FILE $TMP_FILE
mv -f $TMP_FILE $DESKTOP_FILE

