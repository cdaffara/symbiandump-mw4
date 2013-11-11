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
* Description:  Internal address structure
*
*/


// CLASS HEADER
#include "internaladdress.h"

EXPORT_C CInternalAddress::CInternalAddress( TInt aGranularity )
 : CArrayPtrFlat<TInetAddr>( aGranularity )
    {    
    }

CInternalAddress::~CInternalAddress()
    {
    ResetAndDestroy();
    }

EXPORT_C CInternalAddress* CInternalAddress::NewL( const CInternalAddress& aData )
    {
    CInternalAddress* internalAddr = new (ELeave) CInternalAddress(1);
    internalAddr->CopyL(aData);
    return internalAddr;
    }   

EXPORT_C void CInternalAddress::CopyL( const CInternalAddress& aData )
    {
    for ( TInt i=0; i<aData.Count(); i++ )
        {
        TInetAddr* dns_addr = new (ELeave) TInetAddr(*(aData.At(i)));
        CleanupStack::PushL(dns_addr);
        AppendL(dns_addr);
        CleanupStack::Pop();
        }
    iClientIntAddr = aData.iClientIntAddr;
    }   
