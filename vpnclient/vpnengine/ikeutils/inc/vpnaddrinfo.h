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


#ifndef VPNADDRINFO_H
#define VPNADDRINFO_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CInternalAddress;
class MIkeDebug;
class TInetAddr;
class TVPNAddress;

/**
 *  VPN address info utility.
 *
 *  Utility class for building internal address info.
 *
 *  @lib internal (kmdserver.exe)
 */
class VPNAddrInfo
    {
public:    

    /**
     * Builds internal address info object.
     * @param aInternalAddr Internal address
     * @param aDnsServerAddr DNS server address
     * @param aVPNAddress Internal address info (returned)
     * @param aDebug Debug trace interface
     */
    IMPORT_C static void BuildVPNAddrInfo( const CInternalAddress* aInternalAddr,
                                           const TInetAddr& aDnsServerAddr,
                                           TVPNAddress& aVPNAddress,
                                           MIkeDebug& aDebug );        
    
    };


#endif // VPNADDRINFO_H
