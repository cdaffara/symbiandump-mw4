/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  KMD server
*
*/


#include "ipsecsaspiretriever.h"
#include "pfkeysocketif.h"

EXPORT_C CIpsecSaSpiRetriever* CIpsecSaSpiRetriever::NewL(MIpsecSaSpiRetrieverCallback& aRetrieverCallback,
                                                          CPFKeySocketIf& aSocketIf)
    {
    CIpsecSaSpiRetriever* self = new (ELeave) CIpsecSaSpiRetriever(aRetrieverCallback, aSocketIf);
    return self;
    }

CIpsecSaSpiRetriever::CIpsecSaSpiRetriever(MIpsecSaSpiRetrieverCallback& aRetrieverCallback,
                                           CPFKeySocketIf& aSocketIf)
:CActive(EPriorityStandard), iRetrieverCallback(aRetrieverCallback), iSocketIf(aSocketIf)
    {
    CActiveScheduler::Add(this);
    }

CIpsecSaSpiRetriever::~CIpsecSaSpiRetriever()
    {
    Cancel();
    }

EXPORT_C void CIpsecSaSpiRetriever::GetIpsecSaSpi(const TUint32 aSpiRequestId,
                                                  const TUint8 aIpsecProtocol,
                                                  const TInetAddr& aSrc,
                                                  const TInetAddr& aDst)
    {
    iSpiRequestId = aSpiRequestId;
    iSocketIf.GetSpi(aIpsecProtocol, aSpiRequestId, aSrc, aDst, iSpi, iStatus);
    SetActive();
    }


void CIpsecSaSpiRetriever::RunL()
    {
    iRetrieverCallback.IpsecSaSpiRetrieved(iSpiRequestId, iStatus.Int(), iSpi);
    }

void CIpsecSaSpiRetriever::DoCancel()
    {
    iSocketIf.CancelGetSpi(iStatus);
    }

TInt CIpsecSaSpiRetriever::RunError(TInt /*aStatus*/)
    {    
    User::Invariant(); //RunL should never leave
    return KErrNone;
    }
    
