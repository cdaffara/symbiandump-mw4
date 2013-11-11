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
* WLAN Qt Utilities private implementation interface.
*/

#ifndef WLANQTUTILS_P_H
#define WLANQTUTILS_P_H

// System includes

#include <QObject>
#include <QSharedPointer>
#include <QList>

// User includes

#include "wlanqtutils.h"
#include "ictswlanlogininterface.h"

// Forward declarations

class WlanQtUtils;
class WlanQtUtilsAp;
class WlanQtUtilsConnection;
class WlanQtUtilsIapSettings;
class WlanQtUtilsConMonWrapper;
class WlanQtUtilsEsockWrapper;
class WlanQtUtilsScan;

// External data types

// Constants

// Class declaration

class WlanQtUtilsPrivate : public QObject
{
    Q_OBJECT

public:

    // Data types

    explicit WlanQtUtilsPrivate(WlanQtUtils *q_ptr);

    virtual ~WlanQtUtilsPrivate();
    
    void scanWlans();
    
    void scanWlanAps();
    
    void scanWlanDirect(const QString &ssid);

    void stopWlanScan();

    void availableWlans(
        QList< QSharedPointer<WlanQtUtilsAp> > &wlanIapList,
        QList< QSharedPointer<WlanQtUtilsAp> > &wlanApList) const;

    void availableWlanAps(
        QList< QSharedPointer<WlanQtUtilsAp> > &wlanApList) const;  

    int createIap(const WlanQtUtilsAp *wlanAp);

    bool updateIap(int iapId, const WlanQtUtilsAp *wlanAp);

    void deleteIap(int iapId);

    void connectIap(int iapId, bool runIct);

    void disconnectIap(int iapId);
    
    void moveIapToInternetSnap(int iapId);

    WlanQtUtils::ConnStatus connectionStatus() const;

    int activeIap() const;

    QString iapName(int iapId) const;

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanQtUtilsPrivate)

    bool wlanIapExists(
        const QList< QSharedPointer<WlanQtUtilsAp> > list,
        const WlanQtUtilsAp *ap) const;
    
    void reportScanResult(int status, int mode);
    
    void traceIapsAndAps(
        const QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
        const QList< QSharedPointer<WlanQtUtilsAp> > &aps) const;

private slots:

    void updateAvailableWlanIaps(
        QList< QSharedPointer<WlanQtUtilsAp> > &availableIaps);

    void updateAvailableWlanAps(
        QList< QSharedPointer<WlanQtUtilsAp> > &availableWlans);

    void updateScanFailed(int status);
    
    void updateConnectionStatus(bool isOpened);
    
    void addActiveConnection(uint connectionId);
    
    void removeActiveConnection(uint connectionId);
    
    void updateActiveConnection(
        uint connectionId,
        WlanQtUtils::ConnStatus connectionStatus);

    void updateIctResult(int ictsResult);
    
    void updateIctHotspotCase();

private: // data

    //! Current scan mode
    enum ScanMode {
        ScanModeNone = 0,               //!< No scan active
        ScanModeAvailableWlans,         //!< Available AP's & IAPS
        ScanModeAvailableWlanAps,       //!< Available AP's
        ScanModeDirect                  //!< Direct SSID scan
    };
    
    // Not owned data

    //! Pointer to public implementation.
    WlanQtUtils *q_ptr;

    // Owned data

    //! Iap settings handler.
    WlanQtUtilsIapSettings *mSettings;
    
    //! Wrapper object for Connection Monitor and other parts of connmon library.
    WlanQtUtilsConMonWrapper *mConMonWrapper;

    //! Wrapper object for WLAN scanning.
    WlanQtUtilsScan *mScanWrapper;
    
    //! Wrapper object for esock library.
    WlanQtUtilsEsockWrapper *mEsockWrapper;

    //! Instance of Icts Wlan Login Interface.
    QSharedPointer<IctsWlanLoginInterface> mIctService;

    //! Current WLAN scan mode.
    ScanMode mScanMode;

    //! List of available WLAN IAPs according to the latest scan.
    QList< QSharedPointer<WlanQtUtilsAp> > mIapScanList;
    
    //! List of available WLAN APs according to the latest scan.
    QList< QSharedPointer<WlanQtUtilsAp> > mWlanScanList;

    //! ID of IAP requiring ICT running, IapIdNone if not valid.
    int mToBeTestedIapId;
    
    //! ID of IAP being connected by us (requested by UI), IapIdNone if not valid.
    int mConnectingIapId;
    
    //! Information of possible active connection.
    QSharedPointer<WlanQtUtilsConnection> mConnection;

    // Friend classes
    
    // This is defined as a friend class in order to be able to
    // call event handlers of wrappers from test code.
    friend class TestWlanQtUtils;
};

#endif // WLANQTUTILS_P_H
