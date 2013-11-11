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
* Description:  IPSec SA data structure for PFKEY Update and Add primitives
*
*/


// CLASS HEADER
#include "ipsecsadata.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TIpsecSAData::TIpsecSAData()
 : iSAType(0),
   iSeq(0),
   iSrc(),
   iDst(),
   iProtocol(0),
   iSrcIdent(0,0),
   iSrcIdType(0),
   iDstIdent(0,0),
   iDstIdType(0),
   iPid(0),
   iSPI(0),
   iAuthAlg(0),
   iEncrAlg(0),
   iAuthKey(0,0),
   iEncrKey(0,0),
   iFlags(0),
   iReplayWindowLength(0),
   iInternalAddress(),
   iGenericExtension(0,0),
   iHard(0),
   iSoft(0)
    {    
    };
   
