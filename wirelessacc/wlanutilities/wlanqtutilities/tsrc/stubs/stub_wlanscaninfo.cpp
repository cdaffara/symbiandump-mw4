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
* WLAN Management API stubbing for WLAN Qt Utilities.
*/

#include <e32base.h>
#include "wlanscaninfo.h"

#include <cmmanager_shim.h>

#include "wlanqtutilstestcontext.h"
#include "wlanqtutilsap.h"

extern WlanQtUtilsTestContext testContext;

// Information Element ID for SSID as specified in 802.11.
static const TUint8 KWlan802Dot11SsidIE = 0;

// Bit mask for Capability info field to get type (Infra/AdHoc).
static const TUint8 KWlan802Dot11CapabilityEssMask = 0x0001;

// ---------------------------------------------------------
// CWlanScanInfo::NewL
// ---------------------------------------------------------
//
CWlanScanInfo* CWlanScanInfo::NewL()
    {
    CWlanScanInfo* self = new CWlanScanInfo;
    return self;
    }
    
// ---------------------------------------------------------
// CWlanScanInfo::~CWlanScanInfo
// ---------------------------------------------------------
//
CWlanScanInfo::~CWlanScanInfo()
    {
    }

// ---------------------------------------------------------
// CWlanScanInfo::~First
// ---------------------------------------------------------
//
const TWlanScanInfoFrame* CWlanScanInfo::First()
    {
    // Rewind to the beginning of scan result list
    testContext.mScan.mScanResultIterator = 0;
    
    return (TWlanScanInfoFrame*)1;   // Info frame is not supported
    }

// ---------------------------------------------------------
// CWlanScanInfo::~Next
// ---------------------------------------------------------
//
const TWlanScanInfoFrame* CWlanScanInfo::Next()
    {
    // Go to next AP in scan results
    testContext.mScan.mScanResultIterator++;
    
    return (TWlanScanInfoFrame*)1;   // Info frame is not supported
    }

// ---------------------------------------------------------
// CWlanScanInfo::~IsDone
// ---------------------------------------------------------
//
TBool CWlanScanInfo::IsDone() const
    {
    TBool result = EFalse;
    
    // Iterating is complete when iterator is one past the last item
    if (testContext.mScan.mScanResultIterator >=
        testContext.mScan.mWlanScanApResultList.count()) {
        result = ETrue;
    }
    
    return result;
    }

// ---------------------------------------------------------
// CWlanScanInfo::RXLevel
// ---------------------------------------------------------
//
TUint8 CWlanScanInfo::RXLevel() const
    {
    QSharedPointer<WlanQtUtilsAp> ap(GetCurrentAp());
    int rxlevel = ap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt();
    
    return (TUint8)rxlevel;
    }

// ---------------------------------------------------------
// CWlanScanInfo::Bssid
// ---------------------------------------------------------
//
void CWlanScanInfo::Bssid(
    TWlanBssid& aBssid ) const
    {
    QSharedPointer<WlanQtUtilsAp> ap(GetCurrentAp());
    QByteArray bssid = ap->value(WlanQtUtilsAp::ConfIdBssid).toByteArray();

    // Length must match, since BSSID is always the same length
    Q_ASSERT(bssid.length() == KWlanMaxBssidLength);
    
    // Copy the BSSID
    aBssid.Copy((const unsigned char *)bssid.constData(), bssid.length());
    }

// ---------------------------------------------------------
// CWlanScanInfo::Capability
// ---------------------------------------------------------
//
TUint16 CWlanScanInfo::Capability() const
    {
    QSharedPointer<WlanQtUtilsAp> ap(GetCurrentAp());
    
    TUint16 capabilities = 0;
    
    // Only connection mode cabability is supported 
    int connMode = ap->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt();
    if (connMode == CMManagerShim::Infra) {
        capabilities |= KWlan802Dot11CapabilityEssMask; 
    }
    
    return capabilities;
    }

// ---------------------------------------------------------
// CWlanScanInfo::InformationElement
// ---------------------------------------------------------
//
TInt CWlanScanInfo::InformationElement(
    TUint8 aIE, 
    TUint8& aLength, 
    const TUint8** aData )
    {
    // Static SSID buffer
    static TBuf8<255> ssidBuffer;
    
    QSharedPointer<WlanQtUtilsAp> ap(GetCurrentAp());
    
    aLength = 0;
    
    // Only SSID IE is supported
    Q_ASSERT(aIE == KWlan802Dot11SsidIE);
    (void)aIE;
    
    // Cypy the SSID
    ssidBuffer.SetLength(0);
    QString ssid = ap->value(WlanQtUtilsAp::ConfIdSsid).toString();
    for (int i = 0; i < ssid.length(); i++) {
        QChar character = ssid[i];
        ssidBuffer.Append((TUint8)(character.unicode() & 0x000000FF));
    }

    // Give reference to the buffer to the caller
    *aData = ssidBuffer.Ptr();
    aLength = (TUint8)ssidBuffer.Length();
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanScanInfo::IsProtectedSetupSupported
// ---------------------------------------------------------
//
TBool CWlanScanInfo::IsProtectedSetupSupported()
    {
    QSharedPointer<WlanQtUtilsAp> ap(GetCurrentAp());
    
    TBool wpsSupport;
    bool wps = ap->value(WlanQtUtilsAp::ConfIdWpsSupported).toBool();
    if (wps) {
        wpsSupport = ETrue;
    } else {
        wpsSupport = EFalse;
    }
    
    return wpsSupport;
    }

// ---------------------------------------------------------
// CWlanScanInfo::ExtendedSecurityMode
// ---------------------------------------------------------
//
TWlanConnectionExtentedSecurityMode CWlanScanInfo::ExtendedSecurityMode() const
    {
    QSharedPointer<WlanQtUtilsAp> ap(GetCurrentAp());
    int secMode = ap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt();
    bool pskUse = ap->value(WlanQtUtilsAp::ConfIdWpaPskUse).toBool();
    
    TWlanConnectionExtentedSecurityMode ret;
    switch (secMode) {
    case CMManagerShim::WlanSecModeWep:
        // Both WEP security modes are mapped to same WEP sec mode value.
        // We test both of them, and toggle the used value each time.
        static bool wepOpen = true;
        if (wepOpen) {
            ret = EWlanConnectionExtentedSecurityModeWepOpen;
        } else {
            ret = EWlanConnectionExtentedSecurityModeWepShared;
        }
        wepOpen = !wepOpen;
        break;
        
    case CMManagerShim::WlanSecMode802_1x:
        ret = EWlanConnectionExtentedSecurityMode802d1x;        
        break;
        
    case CMManagerShim::WlanSecModeWpa:
        if (pskUse)
            {
            ret = EWlanConnectionExtentedSecurityModeWpaPsk;
            }
        else
            {
            ret = EWlanConnectionExtentedSecurityModeWpa;            
            }
        break;
        
    case CMManagerShim::WlanSecModeWpa2:
        if (pskUse)
            {
            ret = EWlanConnectionExtentedSecurityModeWpa2Psk;
            }
        else
            {
            ret = EWlanConnectionExtentedSecurityModeWpa2;            
            }
        break;
        
    case CMManagerShim::WlanSecModeWapi:
        if (pskUse)
            {
            ret = EWlanConnectionExtentedSecurityModeWapiPsk;
            }
        else
            {
            ret = EWlanConnectionExtentedSecurityModeWapi;            
            }
        break;
        
    default:
        ret = EWlanConnectionExtentedSecurityModeOpen;
        break;
    }
    
    return ret;
    }

// ---------------------------------------------------------
// CWlanScanInfo::CWlanScanInfo
// ---------------------------------------------------------
//
CWlanScanInfo::CWlanScanInfo()
    {
    }

// ---------------------------------------------------------
// CWlanScanInfo::GetCurrentAp
// ---------------------------------------------------------
//
QSharedPointer<WlanQtUtilsAp> CWlanScanInfo::GetCurrentAp() const
    {
    // If this fails, client iterates erroneously
    Q_ASSERT(!IsDone());
    
    // Get the currently iterated AP
    QSharedPointer<WlanQtUtilsAp> ap(
        testContext.mScan.mWlanScanApResultList[
            testContext.mScan.mScanResultIterator]);
    
    return ap;
    }
