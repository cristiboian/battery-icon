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


#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QProcess>
#include <QMutex>
#include <QDebug>

#include <gconfitem.h>

#include <qmbattery.h>

//#define ICON_FILE "/tmp/bat.png"
#define ICON_FILE "/opt/battery-icon/icons/battery-icon.png"
#define ICON_BASE "/opt/battery-icon/icons/base-icon.png"
#define ICON_BASE_CHARGING "/opt/battery-icon/icons/base-charging-icon.png"
#define ICON_BASE_EMPTY "/opt/battery-icon/icons/base-empty-icon.png"
// stand-by screen icons
#define ICON_BASE_SB "/opt/battery-icon/icons/base-sb-icon.png"
#define ICON_SB "/opt/battery-icon/icons/sb-icon.png"

unsigned int bat_fill_color_strip[37] = {
    0x67B808,
    0x66B708,
    0x66B608,
    0x65B508,
    0x64B308,
    0x63B207,
    0x62B007,
    0x61AF07,
    0x60AD07,
    0x5EAB07,
    0x5DA907,
    0x5CA706,
    0x5AA506,
    0x59A306,
    0x58A106,
    0x569E06,
    0x559C05,
    0x539A05,
    0x529805,
    0x509505,
    0x4F9305,
    0x4D9104,
    0x4C8F04,
    0x4A8C04,
    0x498A04,
    0x478804,
    0x468603,
    0x458403,
    0x438203,
    0x428003,
    0x417E03,
    0x407D03,
    0x3F7B02,
    0x3E7A02,
    0x3D7802,
    0x3C7702,
    0x3B7602
    
/*    0x69C200,
    0x68C000,
    0x67BF00,
    0x66BD00,
    0x64BA00,
    0x63B800,
    0x62B500,
    0x60B200,
    0x5FAF00,
    0x5DAC00,
    0x5BA900,
    0x59A600,
    0x58A200,
    0x569F00,
    0x549B00,
    0x529800,
    0x509400,
    0x4E9000,
    0x4C8D00,
    0x4A8900,
    0x488600,
    0x468200,
    0x457F00,
    0x437C00,
    0x417900,
    0x407600,
    0x3E7300,
    0x3D7000,
    0x3B6E00,
    0x3A6B00,
    0x396900,
    0x386800
    */
};

class IconGenerator : public QObject
{
    Q_OBJECT
public:
    IconGenerator();

    void drawCurrentState();

public slots:
    void capacityChanged(int pct, int bars = 0);
    void batteryStateChanged(MeeGo::QmBattery::BatteryState);
    void chargingChanged(MeeGo::QmBattery::ChargingState);

private:
    MeeGo::QmBattery bat;
    int pct;
    MeeGo::QmBattery::BatteryState batState;
    MeeGo::QmBattery::ChargingState chState;

    QMutex mutex;
};


IconGenerator::IconGenerator()
{

    pct = bat.getRemainingCapacityPct();
    batState = bat.getBatteryState();
    chState = bat.getChargingState();

    qDebug() << Q_FUNC_INFO << pct << "%" << "bat:" << batState << "ch:" << chState;

    connect(&bat, SIGNAL(batteryRemainingCapacityChanged(int, int)),
            this, SLOT(capacityChanged(int, int)));
    connect(&bat, SIGNAL(batteryStateChanged(MeeGo::QmBattery::BatteryState)),
            this, SLOT(batteryStateChanged(MeeGo::QmBattery::BatteryState)));
    connect(&bat, SIGNAL(chargingStateChanged(MeeGo::QmBattery::ChargingState)),
            this, SLOT(chargingChanged(MeeGo::QmBattery::ChargingState)));

}

void IconGenerator::capacityChanged(int newPct, int bars)
{
    qDebug() << Q_FUNC_INFO << newPct;

    if(pct != newPct) {
        pct = newPct;
        drawCurrentState();
    }
}

void IconGenerator::batteryStateChanged(MeeGo::QmBattery::BatteryState newBatState)
{
    qDebug() << Q_FUNC_INFO << newBatState;

    if(batState != newBatState) {
        batState = newBatState;
        drawCurrentState();
    }
}

void IconGenerator::chargingChanged(MeeGo::QmBattery::ChargingState newChState)
{
    qDebug() << Q_FUNC_INFO << newChState;

    if(chState != newChState) {
        chState = newChState;
        drawCurrentState();
    }
}

void IconGenerator::drawCurrentState()
{
    if(!mutex.tryLock(3000)) { // 3s
        qDebug() << Q_FUNC_INFO << "cannot lock mutex";
        return;
    }
    qDebug() << Q_FUNC_INFO << pct;

    // This is the user visible percentage which is kept at 100% when the
    // battery is in StateFull. This is how the system shows it.
    int pctUser;
    if (batState == MeeGo::QmBattery::StateFull)
        pctUser = 100;
    else
        pctUser = pct;

    QString pctStr = QString("%1%").arg(pctUser);

    QString iconBaseFile(ICON_BASE);

    /* this does not work properly because it takes so much time
     * for the daemon to start after boot (~ 1min)
    if (batState == MeeGo::QmBattery::StateEmpty)
        iconBaseFile = ICON_BASE_EMPTY;
    */

    if (chState == MeeGo::QmBattery::StateCharging)
        iconBaseFile = ICON_BASE_CHARGING;

    // Application icon
    QImage icon(iconBaseFile);
    if (icon.isNull()) {
        qDebug() << "Base icon is null.";
        // TODO probably we should not continue after this
    }

    QColor fillColor(0x00b000);     // green
    int fillWidth = pctUser * 56 / 100; // we only have 49 pixeles to fill
    if (batState == MeeGo::QmBattery::StateLow) {
        fillColor.setRgb(0xe00000); // red
        fillWidth = 4; // keep a min of 4 pixels so that the red color is visible
    }
    QPainter painter(&icon);
    QPen pen(QColor("white"));
    painter.setPen(pen);

    for (int i = 0; i < 37; i++) {
        fillColor.setRgb(bat_fill_color_strip[i]);
        painter.fillRect(10, 21 + i, fillWidth, 1, fillColor);
    }

    painter.setFont(QFont("Arial", 25, QFont::Bold)); // for 100% font 20
    painter.drawText(3, 15, 70, 50, Qt::AlignCenter, pctStr); // for 100% x = 3

    painter.end();

    if (!icon.save(ICON_FILE)) {
        qDebug() << "Failed to save image to" << ICON_FILE;
        mutex.unlock();
        return;
    }

    int ret = QProcess::execute("/opt/battery-icon/bin/update-desktop.sh");
    if(ret) {
        qDebug() << "Failed to update desktop file. Error code:" << ret;
    }

    // Stand-by screen icon
    QImage sbIcon(ICON_BASE_SB);
    if (sbIcon.isNull()) {
        qDebug() << "Stand-by Base icon is null.";
    }
    painter.begin(&sbIcon);
    painter.setPen(pen); // reuse
    painter.setFont(QFont("Arial", 21, QFont::DemiBold));
    painter.drawText(3, 15, 70, 50, Qt::AlignCenter, pctStr);
    painter.end();

    if (!sbIcon.save(ICON_SB)) {
        qDebug() << "Failed to save image to" << ICON_SB;
        mutex.unlock();
        return;
    }

    GConfItem opLogo("/desktop/meego/screen_lock/low_power_mode/operator_logo");
    if (opLogo.value().toString() == ICON_SB) {
        opLogo.unset();
        opLogo.set(QVariant(ICON_SB));
    }


    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    IconGenerator gen;
    gen.drawCurrentState();

    return a.exec();
    //return 0;
}

#include "main.moc"
