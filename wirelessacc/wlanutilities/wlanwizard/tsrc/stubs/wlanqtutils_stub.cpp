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

#include "wlanqtutilsap.h"
#include "wlanqtutils.h"
#include <QDebug>

WlanQtUtils::WlanQtUtils() : mWlanAp(NULL), mEmitScanApReady(true),
mScanApStatus(ScanStatusOk), mScanDirectStatus(ScanStatusOk)
{
    mWlanNetworkActiveIapId = -1;
    mCalledMethods.append("WlanQtUtils");
}

WlanQtUtils::~WlanQtUtils()
{
    mCalledMethods.append("~WlanQtUtils");
    delete mWlanAp;
}

void WlanQtUtils::availableWlanAps(QList<QSharedPointer<WlanQtUtilsAp> > &wlanApList)
{
    wlanApList = *mScanResult;
    mCalledMethods.append("availableWlanAps");
}

int WlanQtUtils::createIap(const WlanQtUtilsAp * wlanAp)
{
    mCalledMethods.append("createIap");
    delete mWlanAp;
    mWlanAp = new WlanQtUtilsAp(*wlanAp);
    return mCreateWlanIapReturn;
}

bool WlanQtUtils::updateIap(int iapId, const WlanQtUtilsAp * wlanAp)
{
    mCalledMethods.append("updateIap"); 
    qDebug("updateWlanIap iapId = %d", iapId);
    qDebug("mCreateWlanIapReturn = %d", mCreateWlanIapReturn);
    delete mWlanAp;
    mWlanAp = new WlanQtUtilsAp(*wlanAp);
    QCOMPARE(iapId, mCreateWlanIapReturn);
    return mUpdateWlanIapReturn;
}

void WlanQtUtils::deleteIap(int iapId)
{
    mCalledMethods.append("deleteIap");
    QCOMPARE(iapId, mCreateWlanIapReturn);
}

int WlanQtUtils::connectedWlanId()
{
    mCalledMethods.append("connectedWlanId");
    return mCreateWlanIapReturn;
}

void WlanQtUtils::connectIap(int iapId, bool runIct)
{
    qDebug("connectIap iapId = %d", iapId);
    qDebug("mCreateWlanIapReturn = %d", mCreateWlanIapReturn);
    mCalledMethods.append("connectIap");
    QCOMPARE(iapId, mCreateWlanIapReturn);
    QCOMPARE(runIct, true);
    
    if (mConnectionSuccess) {
        mWlanNetworkActiveIapId = iapId;
        emit wlanNetworkOpened(mWlanNetworkOpenedIapId);
        emit ictResult(mIctResultIapId - 1, false);
        emit ictResult(mIctResultIapId, mIctResultResults);
    } else {
        emit wlanNetworkClosed(
            mWlanNetworkClosedIapId -1,
            mWlanNetworkClosedReason );
        
        emit wlanNetworkClosed(
            mWlanNetworkClosedIapId,
            mWlanNetworkClosedReason );
    }
}

void WlanQtUtils::disconnectIap(int iapId)
{
    qDebug("disconnectIap iapId = %d", iapId);
    qDebug("mActiveWlanIapReturn = %d", mActiveWlanIapReturn);
    mCalledMethods.append("disconnectIap");
    QCOMPARE(iapId, mActiveWlanIapReturn);
}

void WlanQtUtils::moveIapToInternetSnap(int iapId)
{
    qDebug("moveIapToInternetSnap iapId = %d", iapId);
    //mCalledMethods.append("moveIapToInternetSnap");
}

int WlanQtUtils::activeIap()
{
    mCalledMethods.append("activeIap");
    return mActiveWlanIapReturn;
}

void WlanQtUtils::scanWlanAps()
{
    mCalledMethods.append("scanWlanAps");
    mScanResult = &mScanWlanAps;
    
    if(mEmitScanApReady) {
        emit wlanScanApReady(mScanApStatus);
    }
}

void WlanQtUtils::scanWlanDirect(const QString &ssid)
{
    mCalledMethods.append("scanWlanDirect");
    QCOMPARE(ssid, mScanWlanDirectSsid);
    mScanResult = &mScanWlanDirect;
    
    emit wlanScanDirectReady(mScanDirectStatus);
}

void WlanQtUtils::stopWlanScan()
{
    mCalledMethods.append("stopWlanScan");
}

void WlanQtUtils::emitScanApsReady()
{
    emit wlanScanApReady(mScanApStatus);
}


