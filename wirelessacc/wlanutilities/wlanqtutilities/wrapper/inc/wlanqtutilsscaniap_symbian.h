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
* Symbian platform specific implementation of WLAN IAP scanning.
*/

#ifndef WLANQTUTILSSCANIAP_SYMBIAN_H
#define WLANQTUTILSSCANIAP_SYMBIAN_H

// System includes

#include <wlanmgmtclient.h>
#include <QSharedPointer>

// User includes

#include "wlanqtutilsap.h"

// Forward declarations

class CWlanMgmtClient;
class WlanQtUtilsScan;

// External data types

// Constants

// Class declaration

class WlanQtUtilsScanIapPrivate : public CActive
{

public:

    // Data types

    static WlanQtUtilsScanIapPrivate *NewL(WlanQtUtilsScan *wrapper);
    
    ~WlanQtUtilsScanIapPrivate();

    void ScanIaps();

    void StopScan();
    
protected:

    void RunL();

    void DoCancel();

private:

    explicit WlanQtUtilsScanIapPrivate(WlanQtUtilsScan *wrapper);

    void ConstructL();

    void ExtractScanResults(
        QList< QSharedPointer<WlanQtUtilsAp> > &scanResults);

private: // data
    
    //! Public implementation reference
    WlanQtUtilsScan *q_ptr;
    
    // Owned data

    //! WLAN Management Client
    CWlanMgmtClient *mWlanMgmtClient;

    //! Accepted cache lifetime
    TInt mCacheLifetime;
    
    //! Accepted maximum delay
    TUint mMaxDelay;
    
    //! Scan results
    RArray<TWlanIapAvailabilityData> mAvailableIaps;

    // Not owned data
    
    // Friend classes
};

#endif // WLANQTUTILSSCANIAP_SYMBIAN_H
