/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Task handler for getting IP version.
*
*/



#include "protocolversiongetter.h"
#include "ipsecpolparser.h"
#include "log.h"

CProtocolVersionGetter* CProtocolVersionGetter::NewL(MTaskHandlerManager* aManager,
                                                     const TTaskArrivedEventData& aTaskInfo)
    {
    CProtocolVersionGetter* self = new (ELeave) CProtocolVersionGetter(aManager, aTaskInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CProtocolVersionGetter::CProtocolVersionGetter(MTaskHandlerManager* aManager,
                                               const TTaskArrivedEventData& aTaskInfo)
    : CTaskHandler(aManager, aTaskInfo, EGetProtocolVersionCancelEvent, &iEventSpecDes)
    {
    }

void CProtocolVersionGetter::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iEventMediator.Connect());
    User::LeaveIfError(iVpnServ.Connect());
    }
    
CProtocolVersionGetter::~CProtocolVersionGetter()
    {
    Cancel();
    iEventMediator.Close();
    iVpnServ.Close();
    }

void CProtocolVersionGetter::StartTaskHandling()
    {
    GotoState(KStateGetProtocolVersion);
    }

void CProtocolVersionGetter::ChangeStateL()
    {
    switch (NextState())
        {
        case KStateGetProtocolVersion:
            StateGetProtocolVersionL();
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }

void CProtocolVersionGetter::CancelOngoingOperation()
    {
    switch (CurrState())
        {
        case KStateGetProtocolVersion:
            // There's no ongoing external request to cancel
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }
    
void CProtocolVersionGetter::StateGetProtocolVersionL()
    {
    SetCurrState(KStateGetProtocolVersion);
    
    // The protocol version getting process
    // consists of one step only
    GetProtocolVersionL();

    TaskComplete(KErrNone);
    }

void CProtocolVersionGetter::ReportResult(TInt aStatus)
    {
    // Report the process status and protocol version
    // information to the Event Mediator

    iEventData.iTaskStatus = aStatus;
    // Protocol version is already in iEventData
    
    TPckg<TGetProtocolVersionEventData> eventDataDes(iEventData);
    ReportEvent(EGetProtocolVersionEvent, iEventSpecDes, eventDataDes);
    }

void CProtocolVersionGetter::GetProtocolVersionL()
    {
    HBufC8* policyData = NULL;
    
    // Load policy data via VPN Manager
    User::LeaveIfError(iVpnServ.GetPolicyData(iEventSpecDes().iPolicyId, policyData));
    CleanupStack::PushL(policyData);
    
    ParseProtocolVersionL(*policyData);

    CleanupStack::PopAndDestroy(); // policyData
    }
    
void CProtocolVersionGetter::ParseProtocolVersionL(const TDesC8& aPolicyData)
    {
    // Parse the data
    CIpSecurityPiece* pieceData  = new (ELeave) CIpSecurityPiece;
    CleanupStack::PushL(pieceData);
    pieceData->ConstructL();

   // Copy policy to 16bit buffer
    TInt length = aPolicyData.Length();
    HBufC *policyDataHBufC16 = HBufC::NewL(length);
    CleanupStack::PushL(policyDataHBufC16);
    TPtr ptr(policyDataHBufC16->Des());
    ptr.Copy(aPolicyData);

    TIpSecParser parser(ptr); // Store policy to secparser
    TInt err = parser.ParseAndIgnoreIKEL(pieceData);
    if (err != KErrNone)
        {
        HBufC* errorInfo = HBufC16::NewL(200);
        errorInfo->Des().Copy(pieceData->iErrorInfo);
        LOG(TPtr ptr = errorInfo->Des(); Log::Printf(_L("Parsing error info: %S\n"), &ptr));
        delete errorInfo;
        CleanupStack::PopAndDestroy(2); // pieceData, policyDataHBufC16
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(1); // policyDataHBufC16

    CSecurityPolicy *securityPolicy = pieceData->Policies();
    CSelectorList* selectorList = securityPolicy->SelectorList();
    TInt selectorCount = selectorList->Count();
    for (TInt i = 0; i < selectorCount; i++)
        {
        CPolicySelector *policySelector = selectorList->At(i);
        if (policySelector->iRemote.Family() == KAfInet6)
            {
            iEventData.iProtocolVersion = EVersionIp6;
            CleanupStack::PopAndDestroy(1); // pieceData
            return;
            }
        }
    if (selectorCount > 0)
        {
        iEventData.iProtocolVersion = EVersionIp;
        }

    CleanupStack::PopAndDestroy(1); // pieceData
    }
