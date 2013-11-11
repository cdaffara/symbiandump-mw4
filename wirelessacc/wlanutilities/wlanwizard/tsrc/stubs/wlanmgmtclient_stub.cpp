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

#include <QTest>

#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include "wlanmgmtclient.h"
#include "testwlanwizardcontext.h"

// ---------------------------------------------------------
// CWlanScanRequest::NewL
// ---------------------------------------------------------
//
CWlanMgmtClient* CWlanMgmtClient::NewL()
    {
    CWlanMgmtClient* self = new CWlanMgmtClient;

    TestWlanWizardContext::setWlanMgmtClientObject(self);
    return self;
    }
    
// ---------------------------------------------------------
// CWlanScanRequest::~CWlanMgmtClient
// ---------------------------------------------------------
//
CWlanMgmtClient::~CWlanMgmtClient()
    {
    delete iResults;
    }

// ---------------------------------------------------------
// CWlanScanRequest::CWlanMgmtClient
// ---------------------------------------------------------
//
CWlanMgmtClient::CWlanMgmtClient()
    {
    iResults = new (ELeave) CArrayFixSeg<TWlanProtectedSetupCredentialAttribute> (4);
    }

// ---------------------------------------------------------
// CWlanScanRequest::RunProtectedSetup
// ---------------------------------------------------------
//
void CWlanMgmtClient::RunProtectedSetup(
    TRequestStatus& aStatus,
    const TWlanSsid& aSsid,
    const TWlanWpsPin& aWpsPin,
    CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials )
{
    Q_UNUSED(aWpsPin);
    QCOMPARE(aSsid, iSsid);
    for (int i = 0 ; i < iResults->Count() ; ++i){
        aCredentials.AppendL(iResults->At(i));
    }
    
    TWlanWpsPin pinForPushButtonMode;
    pinForPushButtonMode.AppendFill('0', 8);
    QCOMPARE( pinForPushButtonMode == aWpsPin, iPushButtonMode ); 
    
    // Complete the request immediately
    TRequestStatus *status = &aStatus;
    User::RequestComplete(status, iCompletionCode);
}

// ---------------------------------------------------------
// CWlanScanRequest::CancelProtectedSetup
// ---------------------------------------------------------
//
void CWlanMgmtClient::CancelProtectedSetup()
    {
    // TODO
    //User::RequestComplete(iStubRequestStatus, KErrCancel);
    //iStubRequestStatus = 0;
    }
