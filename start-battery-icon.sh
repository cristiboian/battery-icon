#!/bin/sh

export DISPLAY=:0
source /tmp/session_bus_address.user

exec /opt/battery-icon/bin/battery-icon
