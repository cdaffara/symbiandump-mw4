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
* Description:  IPSec SA data structure for PFKEY Update and Add primitives
*
*/


#ifndef T_IPSECSADATA_H
#define T_IPSECSADATA_H

#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class TIpsecSALifetime;

/**
 *  IPSec SA data structure.
 *
 *  @lib ikeutils.lib
 */
class TIpsecSAData
    {
public:
    IMPORT_C TIpsecSAData();
    
public: // data
    TUint8      iSAType;
    TUint32     iSeq;
    TInetAddr   iSrc;
    TInetAddr   iDst;
    TUint8      iProtocol;
    TPtr8       iSrcIdent;
    TUint16     iSrcIdType;
    TPtr8       iDstIdent;
    TUint16     iDstIdType;
    TUint32     iPid;
    TUint32     iSPI;
    TUint8      iAuthAlg;
    TUint8      iEncrAlg;
    TPtrC8      iAuthKey;
    TPtrC8      iEncrKey;
    TUint32     iFlags;
    TUint8      iReplayWindowLength;
//
//  Private Nokia VPN specific extensions
//
    TInetAddr   iInternalAddress;
//
//  Genereric private format PFKEY extension. In this phase extesion consists   
//  Information for ESP UDP encapsulation (NAT Traversal)
//
    TPtrC8      iGenericExtension;
    
    TIpsecSALifetime* iHard;
    TIpsecSALifetime* iSoft;
    };

#endif // T_IPSECSADATA_H
