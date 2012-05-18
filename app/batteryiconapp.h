#ifndef BATTERY_ICON_APP__H_
#define BATTERY_ICON_APP__H_

#include <QGraphicsLinearLayout>

#include <MApplicationPage>
#include <MButton>
#include <MLabel>
#include <MAction>

#include <qmbattery.h>
#include <qmdevicemode.h>

#include <gconfitem.h>

class BatteryIconApp : public MApplicationPage
{
    Q_OBJECT
public:
    BatteryIconApp();
    void createContent();

public slots:
    void capacityChanged(int pct, int bars = 0);
    void batteryStateChanged(MeeGo::QmBattery::BatteryState);
    void chargingChanged(MeeGo::QmBattery::ChargingState);
    void opLogoChanged();
    void powerSaveChanged(MeeGo::QmDeviceMode::PSMState);

private:
    // GUI
    MLabel      status;
    MLabel      sbTime;
    MLabel      callTime;
    MLabel      activeTime;
    MLabel      chgTime;
    MButton     psSwitch; // power save
    MButton     sbSwitch; // stand-by screen

    QGraphicsLinearLayout   *layout;
    MAction                 *about;

    // Battery data
    MeeGo::QmBattery        bat;
    int pct;
    MeeGo::QmBattery::BatteryState batState;
    MeeGo::QmBattery::ChargingState chState;
    MeeGo::QmDeviceMode     devMode;
    MeeGo::QmBattery::RemainingTimeMode powerMode;
    GConfItem               *opLogo;

    QString timeFormat(int seconds);
    void updateLabels();

private slots:
    void aboutTriggered();
    void linkActivated(QString);
    void powerSaveClicked(bool);
    void standByClicked(bool);
};



#endif // BATTERY_ICON_APP__H_

