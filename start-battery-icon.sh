#!/bin/sh

export DISPLAY=:0
source /tmp/session_bus_address.user

exec /usr/bin/single-instance /opt/battery-icon/bin/battery-icon
