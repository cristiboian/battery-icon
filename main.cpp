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

#define ICON_BASE_SB "/opt/battery-icon/icons/base-sb-icon.png"
#define ICON_SB "/opt/battery-icon/icons/sb-icon.png"

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

    // a hack to keep in tune with the system
    if (batState == MeeGo::QmBattery::StateFull)
        pct = 100;

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

    // another hack to keep in tune with the system
    if (batState == MeeGo::QmBattery::StateFull && newPct != 100)
        return;

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
    QString pctStr = QString("%1%").arg(pct);

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
    int fillWidth = pct * 70 / 100; // we only have 70 pixeles to fill
    if (batState == MeeGo::QmBattery::StateLow) {
        fillColor.setRgb(0xe00000); // red
        fillWidth = 4; // keep a min of 4 pixels so that the red color is visible
    }
    QPainter painter(&icon);
    QPen pen(QColor("white"));
    painter.setPen(pen);
    painter.fillRect(2, 22, fillWidth, 38, fillColor);

    painter.setFont(QFont("Arial", 20, QFont::Bold)); // for 100% font 20
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
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(3, 15, 70, 50, Qt::AlignCenter, pctStr);
    painter.end();

    if (!sbIcon.save(ICON_SB)) {
        qDebug() << "Failed to save image to" << ICON_SB;
        mutex.unlock();
        return;
    }

    GConfItem opLogo("/desktop/meego/screen_lock/low_power_mode/operator_logo");
    opLogo.unset();
    opLogo.set(QVariant(ICON_SB));


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
