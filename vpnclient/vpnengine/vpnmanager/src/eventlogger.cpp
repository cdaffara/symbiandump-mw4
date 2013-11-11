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
* Description: Event logging.
*
*/

#include <utf.h>

#include "vpnmanagerserverdefs.h"
#include "eventlogger.h"
#include "log_r6.h"
#include "vpnapiservant.h"
#include "policystore.h"
#include "pkiutil.h"
#include "eventviewer.h"

_LIT(KUnknownPolicy, "unknown");
_LIT8(KUnknownVpnAp, "unknown");
_LIT8(KUnknownIap, "unknown");


CEventLogger* CEventLogger::NewL(CPolicyStore& aPolicyStore)
    {
    CEventLogger* self = new (ELeave) CEventLogger(aPolicyStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CEventLogger::CEventLogger(CPolicyStore& aPolicyStore)
    : iPolicyStore(aPolicyStore)
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

void CEventLogger::LogEvent(TUint aMsgId, const TDesC* aDes1, const TDesC8* aDes2, TInt aInt1, TInt aInt2)
    {
    //Logging is an error diagnostic tool.
    //If the logging fails. There is much we can do. So we just ignore the error.
    TRAP_IGNORE(LogEventL(aMsgId, aDes1, aDes2, aInt1, aInt2));
    }
    
    
void CEventLogger::LogEventL(TUint aMsgId, const TDesC* aDes1, const TDesC8* aDes2, TInt aInt1, TInt aInt2)
    {                
    TVpnPolicyInfo* policyInfo = new (ELeave) TVpnPolicyInfo;
    CleanupDeletePushL(policyInfo);
    
    
    // Special handling for event messages that have policy name in them
    __ASSERT_DEBUG((aMsgId == R_VPN_MSG_INSTALLED_POLICY_SERVER ||
                    aMsgId == R_VPN_MSG_INSTALLED_POLICY_FILE ||
                    aMsgId == R_VPN_MSG_POLICY_INSTALL_FAIL ||
                    aMsgId == R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_AP ||
                    aMsgId == R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_SNAP), User::Invariant());
                    
    // Descriptor parameter 1 is a policy ID
    if (aDes1 && aDes1->Length() > 0)
        {                                    
        if (iPolicyStore.GetPolicyInfo(*aDes1, *policyInfo) != KErrNone)
            {
            // ...use the policy ID as the name
            policyInfo->iName = *aDes1;
            }
        }
    else
        {
        // Completely unknown policy reference
        policyInfo->iName = KUnknownPolicy;
        }

    // Make a 8 bit copy of the policy name         
    HBufC8* nameCopy = CnvUtfConverter::ConvertFromUnicodeToUtf8L(policyInfo->iName);
    
    LogEvent(aMsgId, nameCopy, aDes2, aInt1, aInt2);   
    
    delete nameCopy;    
    nameCopy = NULL;
    
    CleanupStack::PopAndDestroy(); //policyInfo
    }
    
void CEventLogger::LogEvent(TUint aMsgId, const TDesC8* aDes1, const TDesC8* aDes2, TInt aInt1, TInt aInt2)
  	{
    TPckgBuf<TInt> int1Des(aInt1);

    TUid sourceUid = (TUid)(KVpnManagerUid3);

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
        case R_VPN_MSG_INSTALLED_ENROLLMENT_SERVICE:
            if (aInt1) // if update...
                {
                iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                              R_VPN_MSG_UPDATED_ENROLLMENT_SERVICE,
                                              2, aDes1, aDes2);
                }
            else // install
                {
                iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                              R_VPN_MSG_INSTALLED_ENROLLMENT_SERVICE,
                                              2, aDes1, aDes2);
                }
            break;
            
        case R_VPN_MSG_DELETED_ENROLLMENT_SERVICE:
            iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                          R_VPN_MSG_DELETED_ENROLLMENT_SERVICE,
                                          1, aDes1);
            break;
            
        case R_VPN_MSG_INSTALLED_POLICY_SERVER:
            LOG(Log::Printf(_L("Logging event R_VPN_MSG_INSTALLED_POLICY_SERVER, aInt2 = %d\n"), aInt2));
            if (aInt2) // if update...
                {
                if (aInt1 == KErrNone)
                    {
                    LOG(Log::Printf(_L("Logging event R_VPN_MSG_UPDATED_POLICY\n")));
                    iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                                  R_VPN_MSG_UPDATED_POLICY,
                                                  2, aDes1, aDes2);
                    }
                else
                    {
                    LOG(Log::Printf(_L("Logging event R_VPN_MSG_POLICY_UPDATE_FAIL\n")));
                    iEventMediator.ReportLogEvent(sourceUid, EError,
                                                  R_VPN_MSG_POLICY_UPDATE_FAIL,
                                                  2, aDes1, &int1Des);
                    }
                }
            else // install
                {
                if (aInt1 == KErrNone)
                    {
                    LOG(Log::Printf(_L("Logging event R_VPN_MSG_INSTALLED_POLICY_SERVER\n")));
                    iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                                  R_VPN_MSG_INSTALLED_POLICY_SERVER,
                                                  2, aDes1, aDes2);
                    }
                else
                    {
                    LOG(Log::Printf(_L("Logging event R_VPN_MSG_POLICY_INSTALL_FAIL\n")));
                    iEventMediator.ReportLogEvent(sourceUid, EError,
                                                  R_VPN_MSG_POLICY_INSTALL_FAIL,
                                                  2, aDes1, &int1Des);
                    }
                }
            break;
            
        case R_VPN_MSG_INSTALLED_POLICY_FILE:
            if (!aInt2) // if installation from file (to avoid event logging when installed from server)
                {
                iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                              R_VPN_MSG_INSTALLED_POLICY_FILE,
                                              1, aDes1);
                }
            break;
            
        case R_VPN_MSG_POLICY_INSTALL_FAIL:
            if (!aInt2) // if installation from file (failure in installation from server was covered above)
                {
                iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                              R_VPN_MSG_POLICY_INSTALL_FAIL,
                                              2, aDes1, &int1Des);
                }
            break;
                      
        case R_VPN_MSG_DELETED_POLICY:
            iEventMediator.ReportLogEvent(sourceUid, EInfo,
                                          R_VPN_MSG_DELETED_POLICY,
                                          1, aDes1);
            break;
            
        default:
            return;
        }

    LOG(Log::Printf(_L("Logged event %d\n"), aMsgId));
    }

HBufC8* CEventLogger::CertSubjectNameL(const TDesC8& aCertData)
    {
    HBufC* certSubjectName = PkiUtil::CertSubjectNameL(aCertData);
    CleanupStack::PushL(certSubjectName);
    
    HBufC8* certSubjectName8 = HBufC8::NewL(certSubjectName->Length());
    certSubjectName8->Des().Copy(*certSubjectName);

    CleanupStack::PopAndDestroy(); // certSubjectName

    return certSubjectName8;
    }
