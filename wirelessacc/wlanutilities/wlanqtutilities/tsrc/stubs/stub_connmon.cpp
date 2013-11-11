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
* This is a source file for connmon library stub functions for UT.
*/

#include <QString>
#include <QObject>
#include <QVariant>
#include <rconnmon.h>
#include <nifvar.h>
#include <cmmanager_shim.h>

#include "wlanqtutilsap.h"

#ifdef __WINS__

#include "wlanqtutilstestcontext.h"

extern WlanQtUtilsTestContext testContext;

// -----------------------------------------------------------------------------
// CConnMonEventBase::CConnMonEventBase
// -----------------------------------------------------------------------------
//
CConnMonEventBase::CConnMonEventBase( const TInt aEventType, const TUint aConnectionId )
    {
    iEventType = aEventType;
    iConnectionId = aConnectionId;
    }

// Destructor
CConnMonEventBase::~CConnMonEventBase()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonEventBase::EventType
// -----------------------------------------------------------------------------
//
TInt CConnMonEventBase::EventType() const
    {
    return iEventType;
    }

// -----------------------------------------------------------------------------
// CConnMonConnectionStatusChange::CConnMonConnectionStatusChange
// -----------------------------------------------------------------------------
//
CConnMonConnectionStatusChange::CConnMonConnectionStatusChange(
    const TUint aConnectionId,
    const TUint aSubConnectionId,
    const TInt aConnectionStatus ) :
    CConnMonEventBase(EConnMonConnectionStatusChange, aConnectionId)
    {
    iSubConnectionId = aSubConnectionId;
    iConnectionStatus = aConnectionStatus;
    }

// Destructor
CConnMonConnectionStatusChange::~CConnMonConnectionStatusChange()
    {
    }

// Request status for canceling stubbed async request
TRequestStatus* iStubRequestStatus = 0;

void RConnectionMonitor::CancelAsyncRequest(
    TInt aReqToCancel)
{
    (void)aReqToCancel;
    User::RequestComplete(iStubRequestStatus, KErrCancel);
    iStubRequestStatus = 0;
}

void RConnectionMonitor::GetConnectionCount(
    TUint& aConnectionCount,
    TRequestStatus& aStatus)
{
    aConnectionCount = testContext.connMon_.activeConnections_.activeConnList_.count();
    User::RequestComplete(&aStatus, KErrNone);
}

TInt RConnectionMonitor::GetConnectionInfo(
        const TUint aIndex,
        TUint& aConnectionId,
        TUint& aSubConnectionCount ) const
{
    aConnectionId = testContext.connMon_.activeConnections_.activeConnList_[aIndex - 1]->connectionId();
    aSubConnectionCount = 0;
    return KErrNone; // TODO: put return value into context.
}

void RConnectionMonitor::GetIntAttribute(
        const TUint aConnectionId,
        const TUint /* aSubConnectionId */,
        const TUint aAttribute,
        TInt& aValue,
        TRequestStatus& aStatus )
{
    WlanQtUtilsCtxActiveConn *activeConn = testContext.connMon_.activeConnections_.findActiveConn(aConnectionId);
    if (aAttribute == KBearer) {
        aValue = activeConn->connMonBearerType_;
    } else if (aAttribute == KConnectionStatus) {
        aValue = activeConn->connMonConnectionStatus_;
    } else {
        Q_ASSERT(false);
    }
    User::RequestComplete(&aStatus, KErrNone); // TODO: Take return value from the context.
}

void RConnectionMonitor::GetUintAttribute(
        const TUint aConnectionId,
        const TUint /* aSubConnectionId */,
        const TUint aAttribute,
        TUint& aValue,
        TRequestStatus& aStatus )
{
    WlanQtUtilsCtxActiveConn *activeConn = testContext.connMon_.activeConnections_.findActiveConn(aConnectionId);
    if (aAttribute == KIAPId) {
        aValue = activeConn->iapId();
    } else {
        Q_ASSERT(false);
    }

    User::RequestComplete(&aStatus, KErrNone); // TODO: Take return value from the context.
}

#endif // __WINS__
