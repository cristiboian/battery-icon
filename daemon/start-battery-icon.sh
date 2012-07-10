#!/bin/sh

# Copyright (C) 2012 Cristi Boian
# This file is part of Battery Icon
# License: GPLv2


export DISPLAY=:0
source /tmp/session_bus_address.user

exec /usr/bin/single-instance /opt/battery-icon/bin/battery-icon
