/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Security policy module payload structures
*
*/


#ifndef T_SECPOLPAYLOAD_H
#define T_SECPOLPAYLOAD_H

#include <ip4_hdr.h>
#include <ip6_hdr.h>

// FORWARD DECLARATIONS
class TInet6Options;
class TInet6HeaderFragment;
class TInet6HeaderRouting;
class TInet6HeaderAH;
class TInet6HeaderESP;
class TInet6HeaderICMP;
class TInet6HeaderTCP;
class TInet6HeaderUDP;

/**
 *  THdrIP4 class.
 *  Class for dumping IPv4 IP header.
 *
 *  @lib internal (kmdserver.exe)
 */
class THdrIP4 : public TInet6HeaderIP4
    {
public:
    void Dump( TDes& aStr, TInt aLength );
    TBool IsUnicast();  
    };

/**
 *  THdrIP6 class.
 *  Class for dumping IPv6 IP header.
 *
 *  @lib internal (kmdserver.exe)
 */
class THdrIP6 : public TInet6HeaderIP
    {
public:
    void Dump( TDes &aStr, TInt aLength );
    TBool IsUnicast();      
    };

/**
 *  Secpol payload.
 *  Payload structure for reading message from Secpol reader.
 *
 *  @lib internal (kmdserver.exe)
 */
class TSecpolPayload
    {
public:
    TSecpolPayload( const TUint8* aPtr );
    
    void Dump( TDes &aStr, TInt aLength, TInt aProtocol );
    
public: // data    
    union
        {
        const TUint8*               iRaw;
        const TInet6HeaderESP*      iESP;
        const TInet6HeaderAH*       iAH;
        const TInet6HeaderIP*       iIP6;
        const TInet6HeaderIP4*      iIP4;
        const TInet6HeaderTCP*      iTCP;
        const TInet6HeaderUDP*      iUDP;
        const TInet6HeaderICMP*     iICMP;
        const TInet6Options*        iOPT;
        const TInet6HeaderRouting*  iRTH;
        const TInet6HeaderFragment* iFRH;
        };
    };

#endif // T_SECPOLPAYLOAD_H
