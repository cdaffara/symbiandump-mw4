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
* Description: Logs events related to the SIT tasks.
*
*/

#include <ipsecpolapi.h>

#include "sit.h"
#include "eventlogger.h"
#include "log.h"
#include "eventmediatorapi.h"
#include "vpnconnstarter.h"


/**
 * Used to log events related to the SIT tasks. 
 */
CEventLogger* CEventLogger::NewL(REventMediator& aEventMediator, TUint32 aVpnIapId, TUint32 aRealIapId)
    {
    LOG(Log::Printf(_L("CEventLogger::NewL - begin\n")));
    CEventLogger* self = new (ELeave) CEventLogger(aEventMediator, aVpnIapId, aRealIapId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOG(Log::Printf(_L("CEventLogger::NewL - end\n")));
    return self;
    }

CEventLogger::CEventLogger(REventMediator& aEventMediator, TUint32 aVpnIapId, TUint32 aRealIapId)
    : iEventMediator(aEventMediator), iVpnIapId(aVpnIapId), iRealIapId(aRealIapId)
    {
    }

void CEventLogger::ConstructL()
    {
    // Set default access point names just
    // in case the real name fetching fails
    iVpnApName.Copy(KUnknownVpnAp);
    iRealApName.Copy(KUnknownIap);

    EventViewer::GetIapNames(iVpnIapId, iVpnApName, iRealIapId, iRealApName);
    }
    
CEventLogger::~CEventLogger()
    {
    LOG(Log::Printf(_L("CEventLogger::~CEventLogger\n")));
    }

void CEventLogger::LogEvent(TUint aMsgId, TAny* aAnyPtr, TInt aStatusCode, TInt aReasonCode)
    {
    DoLogEvent( aMsgId, aAnyPtr, aStatusCode, aReasonCode );
    }

TInt CEventLogger::DoLogEvent(TUint aMsgId, TAny* aAnyPtr, TInt aStatusCode, TInt aReasonCode)
    {
    TBuf<KMaxIpAddressLength> ifAddr;
    TBuf8<KMaxIpAddressLength> ifAddr8;

    TPckgBuf<TInt> statusCodeDes(aStatusCode);

    TUid sourceUid = (TUid)(KUidSit);
    
    TInt ret = KErrNone;
                
    switch (aMsgId)
        {
        case R_VPN_MSG_VPN_IAP_ACTIVATED:
            if (aStatusCode == KErrNone)
                {
                static_cast<TInetAddr*>(aAnyPtr)->Output(ifAddr);
                ifAddr8.Copy(ifAddr);
                
                ret = iEventMediator.ReportLogEvent(sourceUid, EInfo, aMsgId, 2,
                                                    &iVpnApName, &ifAddr8);
                }
            break;
            
        case R_VPN_MSG_VPN_IAP_ACT_FAILED:
            if (aStatusCode == KKmdIkeNoCertFoundErr)
                {
                ret = iEventMediator.ReportLogEvent(sourceUid, EError,
                                                    R_VPN_MSG_VPN_IAP_ACT_FAILED_CERT_EXP_MISS,
                                                    1, &iVpnApName);
                }
            else if (aStatusCode != KErrNone && aStatusCode != KErrCancel)
                {
                ret = iEventMediator.ReportLogEvent(sourceUid, EError, aMsgId, 2,
                                                    &iVpnApName, &statusCodeDes);
                }
            break;
            
        case R_VPN_MSG_REAL_IAP_ACT_FAILED:
            if (aStatusCode != KErrNone && aStatusCode != KErrCancel)
                {
                ret = iEventMediator.ReportLogEvent(sourceUid, EError, aMsgId, 3,
                                                    &iRealApName, &iVpnApName, &statusCodeDes);
                }
            break;
            
        case R_VPN_MSG_VPN_IAP_DEACT:
            if (aStatusCode == KErrNone ||
                aStatusCode == EUnknownPolicyHandle)
                {
                if (aReasonCode == EDeactivateForced)
                    {
                    ret = iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                                        R_VPN_MSG_VPN_IAP_DEACT_REAL_IAP_TERMINATED, 2,
                                                        &iVpnApName, &iRealApName);
                    }
                else
                    {
                    ret = iEventMediator.ReportLogEvent(sourceUid, EInfo, aMsgId, 1,
                                                        &iVpnApName);
                    }
                }
            break;
            
        case R_VPN_MSG_REAL_IAP_REACTIVATED:
            if (aStatusCode == KErrNone)
                {
                ret = iEventMediator.ReportLogEvent(sourceUid, EInfo, aMsgId, 2,
                                                    &iRealApName, &iVpnApName);
                }
            else
                {
                ret = iEventMediator.ReportLogEvent(sourceUid, EError, R_VPN_MSG_REAL_IAP_ACT_FAILED, 3,
                                                    &iRealApName, &iVpnApName, &statusCodeDes);
                }
            break;

        case R_VPN_MSG_VPN_IAP_ACT_START:
            // NSSM removal
            break;
            
        case R_VPN_MSG_VPN_IAP_ACT_CANCEL:
            break;

        case R_VPN_MSG_VPN_IAP_ACT_END:
            break;
            
        default:
            return ret;
        }

    LOG(Log::Printf(_L("Logged event %d, logging status = %d\n"), aMsgId, ret));
    return ret;
    }
