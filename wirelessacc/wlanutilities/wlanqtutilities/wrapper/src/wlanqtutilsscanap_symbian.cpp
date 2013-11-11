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
* Symbian platform specific implementation of WLAN AP scanning.
*/

// System includes

#include <QList>
#include <cmmanagerdefines_shim.h>
#include <utf.h>

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsap.h"
#include "wlanqtutilsscan.h"
#include "wlanqtutilsscanap_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutilsscanap_symbianTraces.h"
#endif


/*!
    \class WlanQtUtilsScanApPrivate
    \brief Symbian platform specific implementation of WLAN AP scanning.

    Symbian platform specific implementation of WLAN AP scanning. 
*/


// External function prototypes

// Local constants

// Information Element ID for SSID as specified in 802.11.
static const TUint8 KWlan802Dot11SsidIE = 0;

// Bit mask for Capability info field to get type (Infra/AdHoc).
static const TUint8 KWlan802Dot11CapabilityEssMask = 0x0001;

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Static factory constructor.
    
    @param [in,out] wrapper Public implementation class reference.
    
    @return New instance of the class.
*/

WlanQtUtilsScanApPrivate *WlanQtUtilsScanApPrivate::NewL(WlanQtUtilsScan *wrapper)
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_NEWL_ENTRY);
    
    WlanQtUtilsScanApPrivate *self = new (ELeave) WlanQtUtilsScanApPrivate(wrapper);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_NEWL_EXIT);
    return self;
}

/*!
    Destructor.
*/

WlanQtUtilsScanApPrivate::~WlanQtUtilsScanApPrivate()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_WLANQTUTILSSCANAPPRIVATE_ENTRY);
    
    Cancel();
    delete mWlanMgmtClient;
    mWlanMgmtClient = NULL;
    delete mResults;
    mResults = NULL;
    
    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_WLANQTUTILSSCANAPPRIVATE_EXIT);
}

/*!
    Starts a broadcast scan of available access points.
*/

void WlanQtUtilsScanApPrivate::Scan()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_SCAN_ENTRY);
    
    // Scanning while previous scan is not complete is not supported
    Q_ASSERT(!IsActive());
    
    // Make the scan
    mWlanMgmtClient->GetScanResults(iStatus, *mResults);
    SetActive();
    
    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_SCAN_EXIT);
}

/*!
    Starts a direct scan of available access points having given SSID.
    
    @param ssid SSID to scan.
*/

void WlanQtUtilsScanApPrivate::Scan(const QString &ssid)
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_SCAN_SSID_ENTRY);
    
    // Scanning while previous scan is not complete is not supported
    Q_ASSERT(!IsActive());
    
    // Convert from QString to TBuf8
    TBuf<KWlanMaxSsidLength> buffer(ssid.utf16());
    TInt error = CnvUtfConverter::ConvertFromUnicodeToUtf8(mWlanSsid, buffer);
    qt_symbian_throwIfError(error);

    OstTraceExt1(
        TRACE_NORMAL,
        WLANQTUTILSSCANAPPRIVATE_SCAN_SSID,
        "WlanQtUtilsScanApPrivate::Scan;mWlanSsid=%s",
        mWlanSsid);

    // Make the scan
    mWlanMgmtClient->GetScanResults(mWlanSsid, iStatus, *mResults);
    SetActive();

    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_SCAN_SSID_EXIT);
}

/*!
    Stops an ongoing scan.
*/

void WlanQtUtilsScanApPrivate::StopScan()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_STOPSCAN_ENTRY);
    
    Cancel();
    
    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_STOPSCAN_EXIT);
}

/*!
    Scan results handler.
*/

void WlanQtUtilsScanApPrivate::RunL()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_RUNL_ENTRY);
    
    QT_TRYCATCH_LEAVING(
        if (iStatus != KErrNone) {
            // Scan failed
            emit q_ptr->scanFailed(WlanQtUtils::ScanStatusError);
        } else {
            // Scan succeeded
            QList< QSharedPointer<WlanQtUtilsAp> > scanResults;
            ExtractScanResults(scanResults);
            
            // Inform about the results 
            emit q_ptr->availableWlanAps(scanResults);
        }
    );
    
    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_RUNL_EXIT);
}

/*!
    Cancels an outstanding request.
*/

void WlanQtUtilsScanApPrivate::DoCancel()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_DOCANCEL_ENTRY);
    
    mWlanMgmtClient->CancelGetScanResults();

    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_DOCANCEL_EXIT);
}

/*!
    Constructor.
    
    @param [in,out] wrapper Public implementation class reference.
*/

WlanQtUtilsScanApPrivate::WlanQtUtilsScanApPrivate(WlanQtUtilsScan *wrapper) :
    CActive(EPriorityStandard),
    q_ptr(wrapper),
    mWlanMgmtClient(NULL),
    mResults(NULL)
{
    OstTraceFunctionEntry0(DUP1_WLANQTUTILSSCANAPPRIVATE_WLANQTUTILSSCANAPPRIVATE_ENTRY);
    OstTraceFunctionExit0(DUP1_WLANQTUTILSSCANAPPRIVATE_WLANQTUTILSSCANAPPRIVATE_EXIT);
}

/*!
    Second phase constructor.
*/

void WlanQtUtilsScanApPrivate::ConstructL()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANAPPRIVATE_CONSTRUCTL_ENTRY);
    
    CActiveScheduler::Add(this);
    mWlanMgmtClient = CWlanMgmtClient::NewL();
    mResults = CWlanScanInfo::NewL();
    
    OstTraceFunctionExit0(WLANQTUTILSSCANAPPRIVATE_CONSTRUCTL_EXIT);
}

/*
    Extracts scan results and stores them into the given AP list.
    
    @param [out] scanResults Scan result list.
*/

void WlanQtUtilsScanApPrivate::ExtractScanResults(
    QList< QSharedPointer<WlanQtUtilsAp> > &scanResults)
{
    for (mResults->First(); !mResults->IsDone(); mResults->Next()) {
        // Create an access point for each result
        QSharedPointer<WlanQtUtilsAp> ap(new WlanQtUtilsAp());
        
        // SSID
        QByteArray ssid;
        QString name;
        ExtractSsid(ssid, name);

        // Skip over hidden networks. There is no spec for what
        // their names will contain, but at least names with only
        // null characters and whitespace characters are seen
        // in real life.
        if (name.trimmed().isEmpty()) {
            continue;
        }
        ap->setValue(WlanQtUtilsAp::ConfIdSsid, ssid);
        ap->setValue(WlanQtUtilsAp::ConfIdName, name);

        // BSSID
        QByteArray bssid = ExtractBssid();
        ap->setValue(WlanQtUtilsAp::ConfIdBssid, bssid);
        
        // Signal strength
        ap->setValue(
            WlanQtUtilsAp::ConfIdSignalStrength,
            (int)mResults->RXLevel());

        // Connection Mode
        CMManagerShim::WlanConnMode connMode;
        if (mResults->Capability() & KWlan802Dot11CapabilityEssMask) {
            connMode = CMManagerShim::Infra;
        } else {
            connMode = CMManagerShim::Adhoc;
        }
        ap->setValue(WlanQtUtilsAp::ConfIdConnectionMode, (int)connMode);

        // Security Mode parameters
        StoreSecMode(ap, mResults->ExtendedSecurityMode());
        
        // Hidden attribute
        // These scan results do not tell if the AP is hidden or not
        ap->setValue(WlanQtUtilsAp::ConfIdHidden, false);
        ap->setValue(WlanQtUtilsAp::ConfIdWlanScanSSID, false);
        
        // WPS support
        TBool wpsSupported = mResults->IsProtectedSetupSupported();
        ap->setValue(
            WlanQtUtilsAp::ConfIdWpsSupported,
            (wpsSupported == EFalse) ? false : true);

        // Append the AP to scan result list
        scanResults.append(ap);
    }
}

/*!
    Extracts and cleans up the WLAN SSID from current scan result element.
    
    @param [out] ssid SSID in raw byte form (to be used in connect).
    @param [out] name SSID in decoded form (to be used in UI).
*/

void WlanQtUtilsScanApPrivate::ExtractSsid(QByteArray &ssid, QString &name)
{
    // Get the SSID in raw data format
    TUint8 ieLen;
    const TUint8 *ieData;
    TInt ret = mResults->InformationElement(KWlan802Dot11SsidIE, ieLen, &ieData);

    // Default to empty
    name.clear();
    ssid.clear();

    if (ret == KErrNone && ieLen > 0) {
        // Store the SSID exactly like we get it from the scanning.
        // This is to make sure the connecting works even if our character
        // conversion changes how the SSID looks like in UI.
        ssid = QByteArray((char *)ieData, (int)ieLen);

        // Trace the buffer as data to ease low level debugging
        OstTraceData(
            TRACE_DUMP,
            WLANQTUTILSSCANAPPRIVATE_EXTRACTSSID_DATA,
            "WlanQtUtilsScanApPrivate::ExtractSsid data 0x%{hex8[]}",
            ssid.data(),
            ssid.length());

#ifdef OST_TRACE_COMPILER_IN_USE
        // Additional trace since one byte hex string tracing has problems
        // TODO: To be removed when the hex above tracing works OK
        QString ssidHex(ssid.toHex());
        TPtrC16 hexString(ssidHex.utf16(), ssidHex.length());
    
        OstTraceExt1(
            TRACE_DUMP,
            WLANQTUTILSSCANAPPRIVATE_EXTRACTSSID_HEX,
            "WlanQtUtilsScanApPrivate::ExtractSsid;ssid hex=%S",
            hexString);
#endif

        // The IEEE 802.11-2007 section 7.3.2.1 only specifies that
        // the SSID is 0-32 octets, leaving the format of the octets
        // completely open. This means that there are bound to be WLAN
        // SSID data handling implementations that can not be supported 
        // all at the same time. 

        // To support a bit wider character set than 7-bit ASCII, we
        // first try to treat the raw SSID bytes as UTF-8.
        TPtrC8 ssid_data(ieData, (TInt)ieLen);
        TBuf16<CMManagerShim::WlanSSIDLength> ssid_utf8;
        TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(
            ssid_utf8,
            ssid_data);
        if (err == KErrNone) {
            // UTF-8 conversion succeeded, using it.
            name = QString(
                reinterpret_cast<const QChar *>(ssid_utf8.Ptr()),
                ssid_utf8.Length());
        } else {
            // If UTF-8 conversion fails, we fall back to treating each SSID
            // data byte as the lowest octet of Unicode (UCS-2).
            for (int i = 0; i < ieLen; i++) {
                name.append(QChar((uint)ieData[i]));
            }
        }

        // Remove nul characters
        name.remove(QChar());

#ifdef OST_TRACE_COMPILER_IN_USE
        TPtrC16 string(name.utf16(), name.length());
        OstTraceExt1(
            TRACE_DUMP,
            WLANQTUTILSSCANAPPRIVATE_EXTRACTSSID_STRING,
            "WlanQtUtilsScanApPrivate::ExtractSsid string;ssid=%S",
            string);
#endif
    }
}

/*!
    Extracts the WLAN BSSID from current scan result element.
    
    @return BSSID array.
*/

QByteArray WlanQtUtilsScanApPrivate::ExtractBssid()
{
    TWlanBssid wlanBssid;
    mResults->Bssid(wlanBssid);
    QByteArray bssid(
        reinterpret_cast<const char *>(wlanBssid.Ptr()),
        wlanBssid.Length());

    OstTraceData(
        TRACE_DUMP,
        WLANQTUTILSSCANAPPRIVATE_EXTRACTBSSID_DATA,
        "WlanQtUtilsScanApPrivate::ExtractBssid data 0x%{hex8[]}",
        bssid.data(),
        bssid.length());

    return bssid;
}

/*!
    Stores Security mode parameters to \a ap class.

    @param [in,out] ap Access Point object to where the configuration is stored.
    @param [in] secMode Security mode to store.
*/

void WlanQtUtilsScanApPrivate::StoreSecMode(
    QSharedPointer<WlanQtUtilsAp> ap,
    TUint wlanSecMode)
{
    CMManagerShim::WlanSecMode secMode = CMManagerShim::WlanSecModeOpen;
    ap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, false);
    
    switch (wlanSecMode) {
    case EWlanConnectionExtentedSecurityModeWepOpen:
    case EWlanConnectionExtentedSecurityModeWepShared:
        secMode = CMManagerShim::WlanSecModeWep;
        break;
    
    case EWlanConnectionExtentedSecurityMode802d1x:
        secMode = CMManagerShim::WlanSecMode802_1x;
        break;
        
    case EWlanConnectionExtentedSecurityModeWpa:
        secMode = CMManagerShim::WlanSecModeWpa;
        break;
        
    case EWlanConnectionExtentedSecurityModeWpaPsk:
        secMode = CMManagerShim::WlanSecModeWpa;
        ap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
        break;
        
    case EWlanConnectionExtentedSecurityModeWpa2:
        secMode = CMManagerShim::WlanSecModeWpa2;
        break;
        
    case EWlanConnectionExtentedSecurityModeWpa2Psk:
        secMode = CMManagerShim::WlanSecModeWpa2;
        ap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
        break;

    case EWlanConnectionExtentedSecurityModeWapi:
    case EWlanConnectionExtentedSecurityModeWapiPsk:
        secMode = CMManagerShim::WlanSecModeWapi;
        break;

    case EWlanConnectionExtentedSecurityModeOpen:
        secMode = CMManagerShim::WlanSecModeOpen;
        break;
        
#ifndef QT_NO_DEBUG
    default:
        // Invalid security mode detected
        Q_ASSERT(0);
        break;
#endif        
    }
    
    ap->setValue(WlanQtUtilsAp::ConfIdSecurityMode, secMode);
}
