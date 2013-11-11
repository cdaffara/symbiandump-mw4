/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* WLAN Sniffer application engine. 
*/

#ifndef WLANSNIFFERENGINE_H
#define WLANSNIFFERENGINE_H

// System includes

#include <QObject>
#include <QVariant>
#include <QString>
#include <xqsettingskey.h>

// User includes

#include "wlanqtutils.h"

// Forward declarations

class WlanSnifferService;
class XQSettingsManager;

// External data types

// Constants

// Class declaration

class WlanSnifferEngine : public QObject
{
    Q_OBJECT
    
public:

    // Data types

    explicit WlanSnifferEngine(QObject *parent = 0);
    
    ~WlanSnifferEngine();

    WlanQtUtils *wlanQtUtils() const;

    bool isEmbedded() const;

    void startWlanScanning();

    void stopWlanScanning();

    bool masterWlan() const;

    void setMasterWlan(bool enabled);
    
    bool forceDisableWlan() const;

    bool offlineMode() const;

signals:

    /*!
        Signal indicating that WLAN scan results are available. 
    */
    void wlanScanReady();

    /*!
        Signal to inform that List View is requested.
        
        @param [in] title Window title.
    */
    void toListView(const QString &title);
    
    /*!
        Signal to inform that application exit was requested.
    */
    void exitTriggered();

    /*!
        Signal indicating that the WLAN master status has changed.
        
        @param [in] enabled Master WLAN status: true if enabled.
    */
    void masterWlanStatus(bool enabled);
    
    /*!
        Signal indicating that the force disable WLAN status has changed.
        
        @param [in] disabled Force disable WLAN status: true if disabled.
    */
    void forceDisableWlanStatus(bool disabled);

public slots:

    void completeService();

protected:

    void timerEvent(QTimerEvent *event);

protected slots:

private:

    Q_DISABLE_COPY(WlanSnifferEngine)

private slots:

    void updateSetting(const XQSettingsKey &key, const QVariant &value);
    
    void handleWlanScanReady(int status);

private: // data

    // Not owned data

    //! WLAN Sniffer service
    WlanSnifferService *mService;               
    
    //! Settings manager for platform settings reading, writing & status changes.
    XQSettingsManager *mSettingsManager;

    // Owned data

    //! Scan interval is 10 seconds
    static const int scanTimerInterval = 10000;

    //! WLAN scan timer ID
    int mScanTimerId;                           
    
    //! TRUE if scanning has been enabled
    bool mScanEnabled;
    
    //! TRUE if embedded
    bool mEmbedded;                             
    
    //! WLAN Qt Utils instance
    QSharedPointer<WlanQtUtils> mWlanQtUtils;

    // Friend classes
};

#endif // WLANSNIFFERENGINE_H
