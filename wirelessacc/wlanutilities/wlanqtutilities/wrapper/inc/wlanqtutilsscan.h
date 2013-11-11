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
* WLAN scan platform specific implementation wrapper.
*/

#ifndef WLANQTUTILSSCAN_H
#define WLANQTUTILSSCAN_H

// System includes

#include <QObject>
#include <QList>
#include <QSharedPointer>

// User includes

#include "wlanqtutilsap.h"

// Forward declarations

class WlanQtUtilsScanIapPrivate;
class WlanQtUtilsScanApPrivate;

// External data types

// Constants

// Class declaration

class WlanQtUtilsScan : public QObject
{
    Q_OBJECT

public:

    // Data types

    explicit WlanQtUtilsScan(QObject *parent = 0);
    
    ~WlanQtUtilsScan();

    void scanWlanIaps();

    void scanWlanAps();

    void scanWlanDirect(const QString &ssid);

    void stopScan();

signals:
    
    /*!
        Signal indicating available WLAN IAP's.
    
        @param [in] availableIaps Available WLAN IAP's found in scan.
    */
    void availableWlanIaps(
        QList< QSharedPointer<WlanQtUtilsAp> > &availableIaps);

    /*!
        Signal indicating available WLAN access points.
    
        @param [in] availableWlans Available WLAN access points found in scan.
    */
    void availableWlanAps(
        QList< QSharedPointer<WlanQtUtilsAp> > &availableWlans);

    /*!
        Signal indicating that scanning failed.
        
        @param [in] status Scan status code (ScanStatus).
    */
    void scanFailed(int status);
    
public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanQtUtilsScan)

private slots:

private: // data

    // Owned data

    //! Private implementation of IAP scan interface
    WlanQtUtilsScanIapPrivate *d_ptrIap;
    
    //! Private implementation of AP scan interface
    WlanQtUtilsScanApPrivate *d_ptrAp;

    // Not owned data
    
    // Friend classes

    // Needed to be able to emit public signals directly from
    // private implementation.
    friend class WlanQtUtilsScanIapPrivate;
    friend class WlanQtUtilsScanApPrivate;
};

#endif // WLANQTUTILSSCAN_H
