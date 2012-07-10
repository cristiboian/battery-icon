/*
 * Copyright (C) 2012 Cristi Boian.
 *
 * This file is part of Battery Icon.
 *
 * Author: Cristi Boian <cristi@cristiboian.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QImage>
#include <QPainter>
#include <QProcess>
#include <QMutex>
#include <QDebug>

#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>

#include <gconfitem.h>
#include <qmbattery.h>

#include "batteryiconapp.h"

//#define ICON_FILE "/tmp/bat.png"
#define ICON_FILE "/opt/battery-icon/icons/battery-icon.png"
#define ICON_BASE "/opt/battery-icon/icons/base-icon.png"
#define ICON_BASE_CHARGING "/opt/battery-icon/icons/base-charging-icon.png"
#define ICON_BASE_EMPTY "/opt/battery-icon/icons/base-empty-icon.png"

#define ICON_BASE_SB "/opt/battery-icon/icons/base-sb-icon.png"
#define ICON_SB "/opt/battery-icon/icons/sb-icon.png"



int main(int argc, char *argv[])
{
    MApplication a(argc, argv);
    //qInstallMsgHandler(0);

    MApplicationWindow win;
    win.setStyleName("CommonApplicationWindowInverted");
    win.setOrientationAngle(M::Angle270);
    win.setOrientationAngleLocked(true);
    BatteryIconApp page;
    page.appear(&win);
    win.show();

    return a.exec();
}

