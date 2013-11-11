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
* Description:  Utility for building internal address info.
*
*/


#include "ikedebug.h"
#include "internaladdress.h"
#include "vpnmandefs.h"

// CLASS HEADER
#include "vpnaddrinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Build internal address info object from internal address information. 
// ---------------------------------------------------------------------------
//
EXPORT_C void VPNAddrInfo::BuildVPNAddrInfo( const CInternalAddress* aInternalAddr,
                                    const TInetAddr& aDnsServerAddr,
                                    TVPNAddress& aVPNAddress,
                                    MIkeDebug& aDebug )
    {
    __ASSERT_DEBUG( aInternalAddr != NULL,
                    User::Invariant() );
    
    aVPNAddress.iVPNIfAddr = aInternalAddr->iClientIntAddr;

    //
    // Add DNS address(es) to the virtual TVPNAddress object
    // 
    TInt dnsCount = aInternalAddr->Count();
    if ( dnsCount )
        {
        aVPNAddress.iVPNIfDNS1 = *(aInternalAddr->At(0));
        if ( dnsCount > 1 )
            {
            aVPNAddress.iVPNIfDNS2 = *(aInternalAddr->At(1));          
            }
        }
    else
        {
        if ( aDnsServerAddr.Address() != KAFUnspec )
            {
#ifdef _DEBUG                   
            TBuf<39> addrBuf;
            aDnsServerAddr.OutputWithScope( addrBuf );
            aDebug.LogWriteF(_L("DNS Server Address in IKE data %S"), &addrBuf);
#endif //_DEBUG                         
            aVPNAddress.iVPNIfDNS1 = aDnsServerAddr;
            }
        else
            {
#ifdef _DEBUG            
            aDebug.LogWrite(_L("DNS server not defined in policy"));            
#endif // _DEBUG            
            }
        }   
    }   


