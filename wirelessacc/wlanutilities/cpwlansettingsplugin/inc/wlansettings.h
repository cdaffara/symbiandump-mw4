/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/

#ifndef WLANSETTINGS_H_
#define WLANSETTINGS_H_

// System includes

#include <QObject>

// User includes

// Forward declarations

class CWlanSettingsPrivate;
class XQSettingsManager;

// External data types

// Constants

// Class declaration

class WlanSettings : public QObject
{
     Q_OBJECT
     
public:
     
     // Data types

     enum ScanNetworkType {
         EScanNetworkAuto = 0,
         EScanNetworkUserDefined        
     };
     
     static const uint ScanNetworkAuto = 0xFFFFFFFF;
     static const uint ScanNetworkMin = 1;
     static const uint ScanNetworkMax = 30;
     
     WlanSettings();

     ~WlanSettings();
     
     int init();
     
     int loadSettings();
     
     ScanNetworkType scanNetworkType();
     
     uint scanInterval();
     
     int joinWlanMode() const;
     
     int setJoinWlanMode(int mode);
     
     bool isWlanPowerSavingEnabled() const;
     
     bool isDevicePowerSavingEnabled();
     
     int setWlanPowerSaving(int powerSavingOption);
     
     int setWlanScanInterval(uint scanInterval);

signals:

    void devicePowerSavingUpdated();

public slots:

protected:

protected slots:

private:

     Q_DISABLE_COPY(WlanSettings)

     void readDevicePowerSavingKey();

private slots:

    void devicePowerSavingKeyChanged();
    
private: // data

     //! Private Symbian specific implementation.
     CWlanSettingsPrivate *d_ptr;
     
     //! Settings manager for platform settings reading & status changes.
     XQSettingsManager *mSettingsManager;
     
     //! Device Power Saving Mode.
     int mDevicePowerSavingMode;
     
     //! Is mDevicePowerSavingMode up to date, or do we need to read it again?
     bool mDevicePowerSavingModeUpToDate;
};

#endif /* WLANSETTINGS_H_ */
