/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class definition of TDmAdCertUtil.
*
*/



#ifndef __DMADCERTUTIL_H__
#define __DMADCERTUTIL_H__

class TDmAdCertUtil
    {
public:
    static void ParseAvaL(const CX520AttributeTypeAndValue& aAva, TDes8& aOut);
    static void CertDnL(const CX500DistinguishedName& aName, TDes8& aOut);
    static HBufC8* Sha1DigestL(const TDesC8& aData);
    static HBufC8* RSAKeyIdentifierLC(const TDesC8& aKeyData);
    };

#endif
