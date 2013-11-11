/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "ikev2ipsecsarekeydata.h"

_LIT8(KZeroDescriptor, "");

CIpsecSARekeyData* CIpsecSARekeyData::NewL(const TInt aReplayWindow, 
                                           const TIpsecSALifetime* aHard, 
                                           const TIpsecSALifetime* aSoft,
                                           const CArrayFix<TIkeV2TrafficSelector>& aTS_i, 
                                           const CArrayFix<TIkeV2TrafficSelector>& aTS_r,  
                                           const TDesC8& aLocalId, const TDesC8& aRemoteId)
    {
    CIpsecSARekeyData* self = 
        new (ELeave)CIpsecSARekeyData(aReplayWindow, aHard, aSoft);
    CleanupStack::PushL(self);
    self->ConstructL(aTS_i, aTS_r, aLocalId, aRemoteId);
    CleanupStack::Pop();        
    return self;
    }


CIpsecSARekeyData::CIpsecSARekeyData(const TInt aReplayWindow, 
                                     const TIpsecSALifetime* aHard, 
                                     const TIpsecSALifetime* aSoft)
:iReplayWindow(aReplayWindow), iHard(*aHard), iSoft(*aSoft),
 iTS_i(0), iTS_r(0), iLocalId(0), iRemoteId(0)
    {
    }


void CIpsecSARekeyData::ConstructL(const CArrayFix<TIkeV2TrafficSelector>& aTS_i, 
                                   const CArrayFix<TIkeV2TrafficSelector>& aTS_r, 
                                   const TDesC8& aLocalId, const TDesC8& aRemoteId)
    {

    iTS_i = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(aTS_i.Count());
    for (TInt i = 0; i < aTS_i.Count(); i++)
        {
        iTS_i->AppendL(aTS_i[i]);
        }        

    iTS_r = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(aTS_r.Count());
    for (TInt i = 0; i < aTS_r.Count(); i++)
        {
        iTS_r->AppendL(aTS_r[i]);
        }

    iLocalId = aLocalId.AllocL();
    iRemoteId = aRemoteId.AllocL();
    }


CIpsecSARekeyData::~CIpsecSARekeyData()
    {    
    delete iTS_i;
    delete iTS_r;
    delete iLocalId;
    delete iRemoteId;
    }


TInt CIpsecSARekeyData::ReplayWindow() const
    {
    return iReplayWindow;
    }


TIpsecSALifetime CIpsecSARekeyData::HardLifetime() const
    {
    return iHard;
    }


TIpsecSALifetime CIpsecSARekeyData::SoftLifetime() const
    {
    return iSoft;
    }            

const TPtrC8 CIpsecSARekeyData::LocalId() const
    {
    if (iLocalId == NULL)
        {
        return KZeroDescriptor();
        }
    else
        {
        return TPtrC8(*iLocalId);
        }
    }


const TPtrC8 CIpsecSARekeyData::RemoteId() const
    {
    if (iRemoteId == NULL)
        {
        return KZeroDescriptor();
        }
    else
        {
        return TPtrC8(*iRemoteId);
        }
    }


CArrayFix<TIkeV2TrafficSelector>* CIpsecSARekeyData::TsIL() const
    {
    __ASSERT_DEBUG(iTS_i != NULL, User::Invariant());
    return CopyTsL(*iTS_i);
    }


CArrayFix<TIkeV2TrafficSelector>* CIpsecSARekeyData::TsRL() const
    {
    __ASSERT_DEBUG(iTS_r != NULL, User::Invariant());
    return CopyTsL(*iTS_r);    
    }


CArrayFix<TIkeV2TrafficSelector>* CIpsecSARekeyData::CopyTsL(const CArrayFix<TIkeV2TrafficSelector>& aTS) const
    {
    CArrayFix<TIkeV2TrafficSelector>* selectorList = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(2);
    CleanupStack::PushL(selectorList);
    
    for (TInt i = 0; i < aTS.Count(); ++i)
        {
        TIkeV2TrafficSelector selectorCopy(aTS[i]);
        selectorList->AppendL(selectorCopy);
        }    
    CleanupStack::Pop(selectorList);    
    return selectorList;
    }

