/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Qt Utilities library test stub.
*/

#ifndef STUB_WLANQTUTILS_H
#define STUB_WLANQTUTILS_H

#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QTest>

#include "wlanqtutilscommon.h"
#include "wlanqtutilsap.h"

class WlanQtUtilsIap;
class WlanQtUtilsWlanIap;

class WLANQTUTILITIESDLL_EXPORT WlanQtUtils : public QObject
{
    Q_OBJECT
    friend class WlanQtUtilsContext;
public:
    //! "None" IAP ID value (e.g. for "not found" cases)
    static const int IapIdNone = -1;
    
    /*!
        WLAN connection status.
        Remember to update traces/trace.properties when modifying this enum.
    */
    enum ConnStatus {
        ConnStatusNone = 0,         //!< Reserved.
        ConnStatusConnecting,       //!< Connecting.
        ConnStatusConnected,        //!< Connected.
        ConnStatusDisconnected      //!< Disconnected.
    };
    
    /*!
        WLAN scan status
        Remember to update traces/trace.properties when modifying this enum.
    */
    enum ScanStatus {
        ScanStatusOk = 0,           //!< Scan succeeded.
        ScanStatusCancelled,        //!< Scan was cancelled.
        ScanStatusError             //!< Scan failed.
    };
    
    /*! 
        Internet Connectivity Test status.
        Remember to update traces/trace.properties when modifying this enum.
    */        
    enum IctStatus {
        IctPassed = 0,              //!< Normal ICT passed.
        IctHotspotPassed,           //!< Hotspot ICT passed.
        IctCancelled,               //!< ICT was cancelled.
        IctFailed                   //!< ICT failed.
    }; 

public:
    WlanQtUtils();
    
    ~WlanQtUtils();
   
    void availableWlanAps( QList<QSharedPointer<WlanQtUtilsAp> > &wlanApList);   
    
    int createIap(const WlanQtUtilsAp *wlanAp);

    bool updateIap(int iapId, const WlanQtUtilsAp *wlanAp);

    void deleteIap(int iapId);

    void connectIap(int iapId, bool runIct = false);
    
    int connectedWlanId();

    void disconnectIap(int iapId);

    void moveIapToInternetSnap(int iapId);

    int activeIap();
    
    void scanWlanAps();
   
    void scanWlanDirect(const QString &ssid);

    void stopWlanScan();

signals:
    
    void wlanScanApReady(int scanStatus);
   
    void wlanScanDirectReady(int scanStatus);
    
    void wlanNetworkOpened(int iapId);

    void wlanNetworkClosed(int iapId, int reason);

    void ictResult(int iapId, int result);

private: // Return values for all methods.
    
    void emitScanApsReady();
    
    QList<QSharedPointer<WlanQtUtilsAp> > mScanWlanAps;
    QList<QSharedPointer<WlanQtUtilsAp> > mScanWlanDirect;
    QList<QSharedPointer<WlanQtUtilsAp> > *mScanResult;
    QString mScanWlanDirectSsid;
    int mCreateWlanIapReturn;
    bool mUpdateWlanIapReturn;
    
    int mWlanNetworkOpenedIapId;
    int mWlanNetworkActiveIapId;
    int mWlanNetworkClosedIapId;
    int mWlanNetworkClosedReason;
    
    int mActiveWlanIapReturn;
    
    int mIctResultIapId;
    WlanQtUtils::IctStatus mIctResultResults;

    bool mConnectionSuccess;
    
    QStringList mCalledMethods;
    
    WlanQtUtilsAp *mWlanAp;
    
    bool mEmitScanApReady;
    
    int mScanApStatus;
    int mScanDirectStatus;
};

#endif // WLANQTUTILS_H
