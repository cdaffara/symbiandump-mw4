/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CEventLogger
*
*/

#include <utf.h>

#include "eventlogger.h"
#include "vpnlogger.h"
#include "eventviewer.h"

#include "dmadadapterimplconst.h"
#include <vpnlogmessages.rsg>

_LIT(KUnknownPolicy, "unknown");
_LIT8(KUnknownVpnAp, "unknown");
_LIT8(KUnknownIap, "unknown");


CEventLogger* CEventLogger::NewL()
    {
    CEventLogger* self = new (ELeave) CEventLogger();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CEventLogger::CEventLogger()
    {
    }
    
void CEventLogger::ConstructL()
    {
    User::LeaveIfError(iEventMediator.Connect());
    }

CEventLogger::~CEventLogger()
    {
    iEventMediator.Close();
    }

void CEventLogger::LogEvent(RVpnServ& vpnApi, TUint aMsgId, const TDesC* aDes1, TInt aInt1, TInt aInt2)
    {
    TRACE("CEventLogger::LogEvent");
    
    TVpnPolicyDetails policyDetails;

    if (aMsgId == R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_AP ||
        aMsgId == R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_SNAP)
        {
        // Descriptor parameter 1 is a policy ID
        if (aDes1 && aDes1->Length() > 0)
            {
            TInt err = KErrNone;
            err = vpnApi.GetPolicyDetails(*aDes1, policyDetails);
            
            // If we cannot find the policy name...
            if (err)
                {
                // ...use the policy ID as the name
                policyDetails.iName.Copy(*aDes1);
                }
            }
        else
            {
            // Completely unknown policy reference
            policyDetails.iName.Copy(KUnknownPolicy);
            }
        }

    // Make a 8 bit copy of the policy name
    
    HBufC8* nameCopy = HBufC8::New(policyDetails.iName.Length());   
    if (!nameCopy)
        {
        return;
        }

    TPtr8 nameCopyPtr = nameCopy->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(nameCopyPtr, policyDetails.iName);
    LogEvent(aMsgId, nameCopy, aInt1, aInt2);

    delete nameCopy;
    }
        
void CEventLogger::LogEvent(TUint aMsgId, const TDesC8* aDes1, TInt aInt1, TInt aInt2)
  	{
  	TRACE("CEventLogger::LogEvent 2");
  	
    TUid sourceUid = TUid::Uid(KDmAdDllUid);    

    TIapName vpnApName(KUnknownVpnAp);
    TIapName realConnectionName(KUnknownIap);
                
    switch (aMsgId)
        {
        case R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_AP:
            EventViewer::GetIapNames(aInt1, vpnApName, aInt2, realConnectionName);

            //The return value is ignored
            iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                          R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_AP,
                                          3, &vpnApName, aDes1, &realConnectionName);
            break;
        case R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_SNAP:
            EventViewer::GetIapName(aInt1, vpnApName);
            EventViewer::GetSnapName(aInt2, realConnectionName);

            iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                          R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_SNAP,
                                          3, &vpnApName, aDes1, &realConnectionName);
            
            break;
        default:
            return;
        }
    }
