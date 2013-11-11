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
* WLAN Qt Utilities WLAN IAP settings handling.
*/

// System includes

#include <QScopedPointer>
#include <QSharedPointer>
#include <QVariant>

#include <cmmanager_shim.h>
#include <cmdestination_shim.h>
#include <cmconnectionmethod_shim.h>

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsap.h"
#include "wlanqtutilsiapsettings.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutilsiapsettingsTraces.h"
#endif

/*!
    \class WlanQtUtilsIapSettings
    \brief WLAN IAP related settings handler.

    Provides functionality to manipulate WLAN IAPs via the CM Manager Shim
    interface.
    
    NOTE: Shim functions may throw exceptions that need to be catched in
    this class.
*/

// External function prototypes

// Local constants

//! IAP trace types
#define WLANQTUTILS_IAP_TRACE_FETCH     1
#define WLANQTUTILS_IAP_TRACE_CREATE    2
#define WLANQTUTILS_IAP_TRACE_UPDATE    3

// WEP key lengths used to determine key format
static const int WepHex64BitMaxLength = 10;
static const int WepHex128BitMaxLength = 26;
static const int WepAscii64BitMaxLength = 5;
static const int WepAscii128BitMaxLength = 13;

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] parent Parent object.
*/

WlanQtUtilsIapSettings::WlanQtUtilsIapSettings(QObject *parent) :
    QObject(parent),
    mCmManager(new CmManagerShim())
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_WLANQTUTILSIAPSETTINGS_ENTRY);
    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_WLANQTUTILSIAPSETTINGS_EXIT);
}

/*!
    Destructor.
*/

WlanQtUtilsIapSettings::~WlanQtUtilsIapSettings()
{
    OstTraceFunctionEntry0(DUP1_WLANQTUTILSIAPSETTINGS_WLANQTUTILSIAPSETTINGS_ENTRY);
    
    delete mCmManager;
    
    OstTraceFunctionExit0(DUP1_WLANQTUTILSIAPSETTINGS_WLANQTUTILSIAPSETTINGS_EXIT);
}

/*!
    Fetch all WLAN IAPs.
    
    @param [out] iapList List of WLAN IAPs.
*/

void WlanQtUtilsIapSettings::fetchIaps(
    QList< QSharedPointer<WlanQtUtilsAp> > &iapList) const
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_FETCHIAPS_ENTRY);
    
    // Clear the list content first for safety
    iapList.clear();
    
    QList<uint> iapIds;
    try {
        mCmManager->connectionMethod(iapIds, false);
    } catch (const std::exception &ex) {
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_FETCHIAPS_EXCEPTION,
            "WlanQtUtilsIapSettings::fetchIaps exception;error=%d",
            error);
    }

#ifdef OST_TRACE_COMPILER_IN_USE
    int iapCount = iapIds.count();
    OstTrace1(
        TRACE_NORMAL,
        WLANQTUTILSIAPSETTINGS_FETCHIAPS_COUNT,
        "WlanQtUtilsIapSettings::fetchIaps;iapCount=%d",
        iapCount);
#endif

    foreach (uint iapId, iapIds) {
        QSharedPointer<WlanQtUtilsAp> wlanIap = fetchIap(iapId);
        if (wlanIap) {
            iapList.append(wlanIap);
        }
    }
    
    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_FETCHIAPS_EXIT);
}

/*!
    Fetch WLAN IAP with the given ID
    
    @param [in] iapId ID of IAP to fetch.
    
    @return Found IAP, NULL if not found.
*/

QSharedPointer<WlanQtUtilsAp> WlanQtUtilsIapSettings::fetchIap(uint iapId) const
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_FETCHIAP_ENTRY);

    QSharedPointer<WlanQtUtilsAp> wlanIap;
    try {
        QScopedPointer<CmConnectionMethodShim> iap(
            mCmManager->connectionMethod(iapId));
        if (iap && iap->getIntAttribute(CMManagerShim::CmBearerType) ==
            KUidWlanBearerType) {

            // Get WLAN IAP parameters
            int netId = iap->getIntAttribute(CMManagerShim::CmNetworkId);
            QString name = iap->getStringAttribute(CMManagerShim::CmName);
            QString ssid = iap->getStringAttribute(CMManagerShim::WlanSSID);
            int connMode = iap->getIntAttribute(CMManagerShim::WlanConnectionMode);
            int secMode = iap->getIntAttribute(
                CMManagerShim::WlanSecurityMode);
            bool wpaPskUse = iap->getBoolAttribute(
                CMManagerShim::WlanEnableWpaPsk);
    
            // Create a WLAN Qt Utils IAP
            wlanIap = QSharedPointer<WlanQtUtilsAp>(new WlanQtUtilsAp());
            wlanIap->setValue(WlanQtUtilsAp::ConfIdIapId, iapId);
            wlanIap->setValue(WlanQtUtilsAp::ConfIdNetworkId, netId);
            wlanIap->setValue(WlanQtUtilsAp::ConfIdName, name);
            wlanIap->setValue(WlanQtUtilsAp::ConfIdSsid, ssid);
            wlanIap->setValue(WlanQtUtilsAp::ConfIdConnectionMode, connMode);
            wlanIap->setValue(WlanQtUtilsAp::ConfIdSecurityMode, secMode);
            wlanIap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, wpaPskUse);
            
            // Trace the fetched IAP
            traceIap(
                wlanIap.data(),
                WLANQTUTILS_IAP_TRACE_FETCH,
                iapId);
        }
    } catch (const std::exception &ex) {
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_FETCHIAP_EXCEPTION,
            "WlanQtUtilsIapSettings::fetchIap exception;error=%d",
            error);
    }
    
    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_FETCHIAP_EXIT);
    return wlanIap;
}

/*!
    Create a new WLAN IAP as an uncategorized IAP.
    
    @param [in] wlanAp Information about the WLAN AP.
    
    @return ID of the created IAP, IapIdNone in error cases.
*/

int WlanQtUtilsIapSettings::createIap(const WlanQtUtilsAp *wlanAp)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_CREATEIAP_ENTRY);
    
    int iapId = WlanQtUtils::IapIdNone;
    
    try {
        // Create the new IAP
        QScopedPointer<CmConnectionMethodShim> iap(
            mCmManager->createConnectionMethod(KUidWlanBearerType));
        storeSettings(wlanAp, iap.data());
        iapId = iap->getIntAttribute(CMManagerShim::CmIapId);
        
        // Trace the created IAP
        traceIap(
            wlanAp,
            WLANQTUTILS_IAP_TRACE_CREATE,
            iapId);
    } catch (const std::exception &ex) {
        // Trace error cause and return failure (default value) to caller.
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_CREATEIAP_EXCEPTION,
            "WlanQtUtilsIapSettings::createIap exception;error=%d",
            error);
    }
    
    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_CREATEIAP_EXIT);
    return iapId;
}

/*!
    Update the WLAN IAP given as parameter. All settings are stored to
    database (again) without checking whether they have actually changed
    or not.
    
    @param [in] iapId ID of IAP to update.
    @param [in] wlanAp Information about the WLAN AP.
    
    @return Was update succesful or not?
*/

bool WlanQtUtilsIapSettings::updateIap(
    int iapId,
    const WlanQtUtilsAp *wlanAp)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_UPDATEIAP_ENTRY);
    
    bool success = false;
    
    try {
        QScopedPointer<CmConnectionMethodShim> iap(
            mCmManager->connectionMethod(iapId));
        storeSettings(wlanAp, iap.data());
        // Trace the updated IAP
        traceIap(
            wlanAp,
            WLANQTUTILS_IAP_TRACE_UPDATE,
            iapId);
        success = true;
    } catch (const std::exception &ex) {
        // Trace error cause and return failure (default value) to caller.
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_UPDATEIAP_EXCEPTION,
            "WlanQtUtilsIapSettings::updateIap exception;error=%d",
            error);
    }
    
    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_UPDATEIAP_EXIT);
    return success;
}

/*!
    Delete the WLAN IAP given as parameter.
    
    @param [in] iapId ID of IAP to delete.
*/

void WlanQtUtilsIapSettings::deleteIap(int iapId)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_DELETEIAP_ENTRY);
    
    try {
        QScopedPointer<CmConnectionMethodShim> iap(
            mCmManager->connectionMethod(iapId));
        iap->deleteConnectionMethod();
    } catch (const std::exception &ex) {
        // Just trace error cause. It is not fatal, if we are not able to
        // delete the IAP. No need to retry, since errors should be very
        // rare and user can delete the IAP later from Control Panel, if
        // needed.
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_DELETEIAP_EXCEPTION,
            "WlanQtUtilsIapSettings::deleteIap exception;error=%d",
            error);
    }

    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_DELETEIAP_EXIT);
}

/*!
    Move IAP to the Internet SNAP.
    
    @param [in] iapId ID of the IAP to move.
*/

void WlanQtUtilsIapSettings::moveIapToInternetSnap(int iapId)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_MOVEIAPTOINTERNETSNAP_ENTRY);
    
    // Read all destination (SNAP) IDs
    QList<uint> destinations;
    try {
        mCmManager->allDestinations(destinations);
        foreach (int destId, destinations) {
            QScopedPointer<CmDestinationShim> destination(
                mCmManager->destination(destId));
        
            // Internet destination will always exist in the system. It has 
            // SnapPurposeInternet set in its metadata.
            if (destination->metadata(CMManagerShim::SnapMetadataPurpose)
                == CMManagerShim::SnapPurposeInternet) {
                QScopedPointer<CmConnectionMethodShim> iap(
                    mCmManager->connectionMethod(iapId));
                destination->addConnectionMethod(iap.data());
                destination->update();
                break;
            }
        }
    } catch (const std::exception &ex) {
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_MOVEIAPTOINTERNETSNAP_EXCEPTION,
            "WlanQtUtilsIapSettings::moveIapToInternetSnap exception;error=%d",
            error);
    }

    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_MOVEIAPTOINTERNETSNAP_EXIT);
}

/*!
    Set Hotspot metadata to the Hotspot IAP given as parameter.
    
    @param [in] iapId ID of IAP to set.
*/

void WlanQtUtilsIapSettings::setAsHotspotIap(int iapId)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_SETASHOTSPOTIAP_ENTRY);
        
    try {
        QScopedPointer<CmConnectionMethodShim> iap(
            mCmManager->connectionMethod(iapId));
        iap->setBoolAttribute(CMManagerShim::CmMetaHotSpot, true);
        iap->update();
    } catch (const std::exception &ex) {
        // Just trace error cause. It is not fatal, if we are not able to
        // set IAP as hotspot IAP. No need to retry, since errors should be very
        // rare and it does not prevent connection opening of the hotspot IAP.
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_SETASHOTSPOTIAP_EXCEPTION,
            "WlanQtUtilsIapSettings::setAsHotspotIap exception;error=%d",
            error);
    }
                 
    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_SETASHOTSPOTIAP_EXIT);
}

/*!
    Stores the given Wlan AP settings to database using CM Manager Shim.
   
    @param [in] wlanAp WLAN AP settings to store.
    @param [in] iap WLAN IAP to store to.
*/

void WlanQtUtilsIapSettings::storeSettings(
    const WlanQtUtilsAp *wlanAp,
    CmConnectionMethodShim *iap)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_STORESETTINGS_ENTRY);

    // Store general settings
    iap->setStringAttribute(
        CMManagerShim::CmName,
        wlanAp->value(WlanQtUtilsAp::ConfIdName).toString());
    iap->setStringAttribute(
        CMManagerShim::WlanSSID,
        wlanAp->value(WlanQtUtilsAp::ConfIdSsid).toString());
    iap->setIntAttribute(
        CMManagerShim::WlanSecurityMode,
        wlanAp->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt());
    iap->setIntAttribute(
        CMManagerShim::WlanConnectionMode, 
        wlanAp->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt());
    iap->setBoolAttribute(
        CMManagerShim::CmHidden,
        wlanAp->value(WlanQtUtilsAp::ConfIdHidden).toBool());
    iap->setBoolAttribute(
        CMManagerShim::WlanScanSSID,
        wlanAp->value(WlanQtUtilsAp::ConfIdWlanScanSSID).toBool());

    // Store the WEP settings
    storeWepKey(
        wlanAp->value(WlanQtUtilsAp::ConfIdWepKey1).toString(),
        1,
        iap);
    storeWepKey(
        wlanAp->value(WlanQtUtilsAp::ConfIdWepKey2).toString(),
        2,
        iap);
    storeWepKey(
        wlanAp->value(WlanQtUtilsAp::ConfIdWepKey3).toString(),
        3,
        iap);
    storeWepKey(
        wlanAp->value(WlanQtUtilsAp::ConfIdWepKey4).toString(),
        4,
        iap);
    iap->setIntAttribute(
        CMManagerShim::WlanWepKeyIndex,
        wlanAp->value(WlanQtUtilsAp::ConfIdWepDefaultIndex).toInt());

    // Store WPA PSK settings
    bool usePsk = wlanAp->value(WlanQtUtilsAp::ConfIdWpaPskUse).toBool();
    iap->setBoolAttribute(CMManagerShim::WlanEnableWpaPsk, usePsk);
    QString wpaKey(wlanAp->value(WlanQtUtilsAp::ConfIdWpaPsk).toString());
    iap->setString8Attribute(CMManagerShim::WlanWpaPreSharedKey, wpaKey);

    // Write the settings.
    iap->update();

    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_STORESETTINGS_EXIT);
}

/*!
    Stores the given valid WEP key to database using CM Manager Shim. If key
    is not used an empty key must be provided.
  
    @note This method MUST not be called for invalid WEP Keys and/or indexes.
          Wlanwizard validates keys, before accepting user input.
          
    @param [in] key Key to write.
    @param [in] index Key index. Valid range is [0,4].
    @param [in] iap WLAN IAP to store the key into.
*/

void WlanQtUtilsIapSettings::storeWepKey(
    const QString &key,
    int index,
    CmConnectionMethodShim *iap)
{
    OstTraceFunctionEntry0(WLANQTUTILSIAPSETTINGS_STOREWEPKEY_ENTRY);
    
    int length = key.length();
    if (length == WepHex64BitMaxLength || length == WepHex128BitMaxLength) {
        // HEX
        iap->setString8Attribute(mapWepKeyIndexHex(index), key);
    } else if (length == WepAscii64BitMaxLength || length == WepAscii128BitMaxLength) {
        // ASCII
        iap->setString8Attribute(mapWepKeyIndexAscii(index), key);
    } else {
        // Length must always be a valid one or zero
        Q_ASSERT(length == 0);
        
        // Write default value. Note that the key is stored in the same data
        // field regardless of the format so writing only one key is enough.
        iap->setString8Attribute(mapWepKeyIndexHex(index), key);
    }

    OstTraceFunctionExit0(WLANQTUTILSIAPSETTINGS_STOREWEPKEY_EXIT);
}

/*!
    Maps given Hex WEP key index into the corresponding CM Manager Connection
    Method attribute.
    
    @param [in] index Hex WEP key index [1,4].
    
    @return Connection Method attribute. 
*/

CMManagerShim::ConnectionMethodAttribute WlanQtUtilsIapSettings::mapWepKeyIndexHex(
    int index)
{
    CMManagerShim::ConnectionMethodAttribute attribute = 
        CMManagerShim::WlanWepKey1InHex;
    
    switch (index) {
    case 1:
        attribute = CMManagerShim::WlanWepKey1InHex;
        break;
        
    case 2:
        attribute = CMManagerShim::WlanWepKey2InHex;
        break;
        
    case 3:
        attribute = CMManagerShim::WlanWepKey3InHex;
        break;
        
    case 4:
        attribute = CMManagerShim::WlanWepKey4InHex;
        break;
        
#ifndef QT_NO_DEBUG
    default:
        // Invalid key index detected
        Q_ASSERT(0);
        break;
#endif
    }

    return attribute;
}

/*!
    Maps given Ascii WEP key index into the corresponding CM Manager Connection
    Method attribute.
    
    @param [in] index Ascii WEP key index [1,4].
    
    @return Connection Method attribute. 
*/

CMManagerShim::ConnectionMethodAttribute WlanQtUtilsIapSettings::mapWepKeyIndexAscii(
    int index)
{
    CMManagerShim::ConnectionMethodAttribute attribute = 
        CMManagerShim::WlanWepKey1InAscii;

    switch (index) {
    case 1:
        attribute = CMManagerShim::WlanWepKey1InAscii;
        break;
        
    case 2:
        attribute = CMManagerShim::WlanWepKey2InAscii;
        break;
        
    case 3:
        attribute = CMManagerShim::WlanWepKey3InAscii;
        break;

    case 4:
        attribute = CMManagerShim::WlanWepKey4InAscii;
        break;
        
#ifndef QT_NO_DEBUG
    default:
        // Invalid key index detected
        Q_ASSERT(0);
        break;
#endif            
    }

    return attribute;
}

/*
    Traces given IAP (which is taken in as an AP).

    @param [in] ap IAP to trace.
    @param [in] traceType Trace type (WLANQTUTILS_IAP_TRACE_*).
    @param [in] iapId IAP ID.
*/

void WlanQtUtilsIapSettings::traceIap(
    const WlanQtUtilsAp *ap,
    int traceType,
    uint iapId) const
{
#ifndef OST_TRACE_COMPILER_IN_USE
    Q_UNUSED(ap);
    Q_UNUSED(traceType);
    Q_UNUSED(iapId);
#else    
    QString ssid_string(ap->value(WlanQtUtilsAp::ConfIdSsid).toString());
    TPtrC16 ssid(ssid_string.utf16(), ssid_string.length());
    int secMode = ap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt();
    int connMode = ap->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt();    
    bool useWpaPsk = ap->value(WlanQtUtilsAp::ConfIdWpaPskUse).toBool();
    
    switch (traceType) {
    case WLANQTUTILS_IAP_TRACE_FETCH:
        OstTraceExt5(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_TRACEIAP_FETCH,
            "WlanQtUtilsIapSettings::traceIap Fetched;iapId=%u;ssid=%S;secMode=%{WlanSecMode};useWpaPsk=%u;connMode=%{WlanConnMode}",
            iapId,
            ssid,
            secMode,
            useWpaPsk,
            connMode);
        break;

    case WLANQTUTILS_IAP_TRACE_CREATE:
        OstTraceExt5(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_TRACEIAP_CREATE,
            "WlanQtUtilsIapSettings::traceIap Created;iapId=%u;ssid=%S;secMode=%{WlanSecMode};useWpaPsk=%u;connMode=%{WlanConnMode}",
            iapId,
            ssid,
            secMode,
            useWpaPsk,
            connMode);
        break;

    case WLANQTUTILS_IAP_TRACE_UPDATE:
        OstTraceExt5(
            TRACE_NORMAL,
            WLANQTUTILSIAPSETTINGS_TRACEIAP_UPDATE,
            "WlanQtUtilsIapSettings::traceIap Updated;iapId=%u;ssid=%S;secMode=%{WlanSecMode};useWpaPsk=%u;connMode=%{WlanConnMode}",
            iapId,
            ssid,
            secMode,
            useWpaPsk,
            connMode);
        break;
    }
#endif
}
