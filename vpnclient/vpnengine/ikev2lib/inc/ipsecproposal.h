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
* Description: IPSec Proposal handling.
*
*/

#ifndef __IPSECPROPOSAL_H_
#define __IPSECPROPOSAL_H_
#include <e32base.h>

class TPfkeyMessage;
class TPayloadIkev2;
class CIpsecSaSpecList;

class IpsecProposal
{
public:
	static HBufC8* BuildIpsecSaRequestL(const TPfkeyMessage& aPfkeyMessage, TUint16 aDHGroup);
	static HBufC8* BuildIpsecSaRequestL(const TUint8 aSaType, 
                                        const TUint8 aEncryptAlg, const TUint16 aEncryptMaxbits, 
                                        const TUint8 aAuthAlg, const TUint16 aFlags, TUint16 aDHGroup);
	static HBufC8* BuildIpsecSaFromPolicyL(const CIpsecSaSpecList& aSaList, TUint16 aDhGroup);	
};
	
#endif
