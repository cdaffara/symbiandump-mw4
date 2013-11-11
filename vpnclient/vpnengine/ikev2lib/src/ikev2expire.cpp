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
* Description:    
* CIkev2Expire. This class is used to handle PFKEY Expire primitives received 
* from IPSec plug-in.
*
*/
#include "ikev2expire.h"
#include "pfkeymsg.h"

//
//  CIkev2Expire
//
//  This class is used to handle PFKEY Expire primitives received from
//  Ipsec plug-in. 
//

CIkev2Expire* CIkev2Expire::NewL(const TPfkeyMessage& aPfkeyMessage)
{
    CIkev2Expire* Expire = new (ELeave)CIkev2Expire();
    TPtrC8 spiPtr(reinterpret_cast<const TUint8*>(&aPfkeyMessage.iSa.iExt->sadb_sa_spi), 
                  sizeof(aPfkeyMessage.iSa.iExt->sadb_sa_spi));
    Expire->SetSPI(spiPtr);
    Expire->SetProtocol(aPfkeyMessage.iBase.iMsg->sadb_msg_satype);
    return Expire;
}

void CIkev2Expire::Link(CIkev2Expire* aExpire, CIkev2Expire** aAnchor)
{
    ASSERT(aExpire && aAnchor);
    aExpire->iNext = NULL;
    CIkev2Expire* Last  = *aAnchor;
    if ( Last )
    {   
        while ( Last->iNext )
        {
            Last = Last->iNext;
        }
        Last->iNext = aExpire;
    }
    else *aAnchor = aExpire;    
}

CIkev2Expire* CIkev2Expire::GetNext(CIkev2Expire** aAnchor)
{
    ASSERT(aAnchor);
    CIkev2Expire* Elem = *aAnchor;
    if ( Elem )
        *aAnchor = Elem->iNext;
    return Elem;
}  

void CIkev2Expire::PurgeQue(CIkev2Expire** aAnchor)
{
    ASSERT(aAnchor);
    CIkev2Expire* Elem = *aAnchor;
    while ( Elem )
    {
        *aAnchor = Elem->iNext;
        delete Elem;
        Elem = *aAnchor;
    }   
}

TPtrC8 CIkev2Expire::SPI() 
    { 
    return iSPI;
    }

void CIkev2Expire::SetSPI(const TDesC8& aSPI) 
    { 
    iSPI = aSPI;
    }

TUint8 CIkev2Expire::Protocol() 
    { 
    return iProtocol;
    }

void CIkev2Expire::SetProtocol(TUint8 aProt) 
    { 
    iProtocol = aProt;
    }

