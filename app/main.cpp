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

