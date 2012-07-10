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

#include <QGraphicsLinearLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

#include <MSeparator>
#include <MDialog>

#include "batteryiconapp.h"

#define ICON_SB "/opt/battery-icon/icons/sb-icon.png"
#define GCONF_OP_LOGO "/desktop/meego/screen_lock/low_power_mode/operator_logo"

BatteryIconApp::BatteryIconApp() :
    MApplicationPage(0), layout(0), about(0), opLogo(0)
{
    pct = bat.getRemainingCapacityPct();
    batState = bat.getBatteryState();
    chState = bat.getChargingState();

    // signals setup
    connect(&bat, SIGNAL(batteryRemainingCapacityChanged(int, int)),
            this, SLOT(capacityChanged(int, int)));
    connect(&bat, SIGNAL(batteryStateChanged(MeeGo::QmBattery::BatteryState)),
            this, SLOT(batteryStateChanged(MeeGo::QmBattery::BatteryState)));
    connect(&bat, SIGNAL(chargingStateChanged(MeeGo::QmBattery::ChargingState)),
            this, SLOT(chargingChanged(MeeGo::QmBattery::ChargingState)));

    connect(&psSwitch, SIGNAL(clicked(bool)), this, SLOT(powerSaveClicked(bool)));
    connect(&sbSwitch, SIGNAL(toggled(bool)), this, SLOT(standByClicked(bool)));

    opLogo = new GConfItem(GCONF_OP_LOGO);
    connect(opLogo, SIGNAL(valueChanged()), this, SLOT(opLogoChanged()));

    // power save
    if (devMode.getPSMState() == MeeGo::QmDeviceMode::PSMStateOn)
        powerMode = MeeGo::QmBattery::PowersaveMode;
    else
        powerMode = MeeGo::QmBattery::NormalMode;

    connect(&devMode, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)),
            this, SLOT(powerSaveChanged(MeeGo::QmDeviceMode::PSMState)));
}

void BatteryIconApp::createContent()
{
    layout = new QGraphicsLinearLayout(Qt::Vertical);
    centralWidget()->setLayout(layout);
    layout->setSpacing(10);
    setStyleName("CommonApplicationPageInverted");

    // Labels
    MLabel *title =  new MLabel("Battery");
    title->setStyleName("CommonHeaderInverted");

    status.setStyleName("CommonBodyTextInverted");

    MLabel *remaingTitle = new MLabel("<b>Remaining times</b>");
    remaingTitle->setStyleName("CommonBodyTextInverted");

    chgTime.setStyleName("CommonBodyTextInverted");
    sbTime.setStyleName("CommonBodyTextInverted");
    callTime.setStyleName("CommonBodyTextInverted");
    activeTime.setStyleName("CommonBodyTextInverted");


    // Switches
    psSwitch.setViewType(MButton::switchType);
    psSwitch.setCheckable(true);
    psSwitch.setChecked(powerMode == MeeGo::QmBattery::PowersaveMode);
    psSwitch.setStyleName("CommonRightSwitchInverted");
    MLabel *psLabel = new MLabel("Power saving mode");
    psLabel->setStyleName("CommonBodyTextInverted");
    QGraphicsLinearLayout *psLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    psLayout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    psLayout->addItem(psLabel);
    psLayout->addItem(&psSwitch);

    sbSwitch.setViewType(MButton::switchType);
    sbSwitch.setCheckable(true);
    sbSwitch.setChecked(opLogo->value().toString() == ICON_SB);
    sbSwitch.setStyleName("CommonRightSwitchInverted");
    MLabel *sbLabel = new MLabel("Stand-by screen battery indicator");
    sbLabel->setStyleName("CommonBodyTextInverted");
    QGraphicsLinearLayout *sbLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    sbLayout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sbLayout->addItem(sbLabel);
    sbLayout->addItem(&sbSwitch);

    // Layout
    layout->addItem(title);
    layout->addItem(&status);
    layout->addItem(new MSeparator()); //TODO make it CommonHorizontalSeparatorInverted
    layout->addItem(remaingTitle);
    layout->addItem(&callTime);
    layout->addItem(&activeTime);
    layout->addItem(&sbTime);
    layout->addItem(&chgTime);
    layout->addItem(new MSeparator());
    layout->addItem(psLayout);
    layout->addItem(sbLayout);

    layout->addStretch(1);

    updateLabels();

    // About
    about = new MAction("About", centralWidget());
    about->setLocation(MAction::ApplicationMenuLocation);
    addAction(about);
    connect(about, SIGNAL(triggered()), this, SLOT(aboutTriggered()));

}

void BatteryIconApp::updateLabels()
{

    if (chState == MeeGo::QmBattery::StateCharging &&
            batState == MeeGo::QmBattery::StateFull) {
        status.setText(QString("Level: <b>100%</b> - charging complete"));
        chgTime.setText("");
        psSwitch.setEnabled(false);
    } else if (chState == MeeGo::QmBattery::StateCharging) {
        status.setText(QString("Level: <b>%1%</b> - charging").arg(pct));
        chgTime.setText(QString("Charging time: ") +
            timeFormat(bat.getRemainingChargingTime()));
        psSwitch.setEnabled(false);
    } else {
        status.setText(QString("Level: <b>%1%</b>").arg(pct));
        chgTime.setText("");
        psSwitch.setEnabled(true);
    }

    callTime.setText(QString("Talk time: ") +
            timeFormat(bat.getRemainingTalkTime(powerMode)));
    sbTime.setText(QString("Stand-by time: ") +
            timeFormat(bat.getRemainingIdleTime(powerMode)));
    activeTime.setText(QString("Active usage time: ") +
            timeFormat(bat.getRemainingActiveTime(powerMode)));

}

QString BatteryIconApp::timeFormat(int s)
{
    QString str;

    if (s <= 60)        // 1 min
        return QString("less than 1 minute");
    if (s < 3600)       // 1 h
        return QString("%1 minutes").arg(s/60);
    if (s < 86400)      // 1 day
        return QString("%1 hours %2 minutes").arg(s/3600).arg((s % 3600)/60);
    // more than a day
    return QString("%1 days %2 hours").arg(s/86400).arg((s % 86400) / 3600);
}

void BatteryIconApp::capacityChanged(int newPct, int bars)
{
    qDebug() << Q_FUNC_INFO << newPct;

    if(pct != newPct) {
        pct = newPct;
        updateLabels();
    }
}

void BatteryIconApp::batteryStateChanged(MeeGo::QmBattery::BatteryState newBatState)
{
    qDebug() << Q_FUNC_INFO << newBatState;

    if(batState != newBatState) {
        batState = newBatState;
        updateLabels();
    }
}

void BatteryIconApp::chargingChanged(MeeGo::QmBattery::ChargingState newChState)
{
    qDebug() << Q_FUNC_INFO << newChState;

    if(chState != newChState) {
        chState = newChState;
        updateLabels();
    }
}

void BatteryIconApp::aboutTriggered()
{
    MWidget *centralWidget = new MWidget;
    QGraphicsLinearLayout *dialogLayout = new QGraphicsLinearLayout(Qt::Vertical);
    MDialog *dialog = new MDialog("Battery Icon", M::CloseButton);
    MLabel *label = new MLabel( "Battery Icon is created by <br>"
            "<a href=\"http://cristiboian.com\">Cristi Boian</a>.");
    label->setWrapMode(QTextOption::WordWrap);
    label->setWordWrap(true);
    label->setStyleName("CommonQueryText");

    centralWidget->setLayout(dialogLayout);
    dialogLayout->addItem(label);
    dialog->setCentralWidget(centralWidget);
    connect(label, SIGNAL(linkActivated(QString)),
            this, SLOT(linkActivated(QString)));

    dialog->appear(MSceneWindow::DestroyWhenDone);
}

void BatteryIconApp::linkActivated(QString url)
{
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void BatteryIconApp::standByClicked(bool checked)
{
    qDebug() << Q_FUNC_INFO << checked;

    if (checked) {
        if (opLogo->value().toString() != ICON_SB)
            opLogo->set(QVariant(ICON_SB));
    } else {
        if (opLogo->value().toString() == ICON_SB)
            opLogo->unset();
    }
}

void BatteryIconApp::opLogoChanged()
{
    qDebug() << Q_FUNC_INFO << opLogo->value().toString();
    sbSwitch.setChecked(opLogo->value().toString() == ICON_SB);
}

void BatteryIconApp::powerSaveClicked(bool checked)
{
    qDebug() << Q_FUNC_INFO << checked;

    if (checked && devMode.getPSMState() != MeeGo::QmDeviceMode::PSMStateOn) {
        if (devMode.setPSMState(MeeGo::QmDeviceMode::PSMStateOn)) {
            powerMode = MeeGo::QmBattery::PowersaveMode;
            updateLabels();
            qDebug() << "  OK";
        } else {
            psSwitch.setChecked(false);
            qDebug() << "  FAIL";
        }
    } else if (!checked && devMode.getPSMState() != MeeGo::QmDeviceMode::PSMStateOff) {
        if (devMode.setPSMState(MeeGo::QmDeviceMode::PSMStateOff)) {
            powerMode = MeeGo::QmBattery::NormalMode;
            updateLabels();
            qDebug() << "  OK";
        } else {
            psSwitch.setChecked(true);
            qDebug() << "  FAIL";
        }
    }
}

void BatteryIconApp::powerSaveChanged(MeeGo::QmDeviceMode::PSMState newState)
{
    qDebug() << Q_FUNC_INFO << newState;
    if (newState == MeeGo::QmDeviceMode::PSMStateOn) {
        powerMode = MeeGo::QmBattery::PowersaveMode;
        psSwitch.setChecked(true);
        qDebug() << "  PSM ON";
    } else {
        powerMode = MeeGo::QmBattery::NormalMode;
        psSwitch.setChecked(false);
        qDebug() << "  PSM OFF";
    }
    updateLabels();

}

