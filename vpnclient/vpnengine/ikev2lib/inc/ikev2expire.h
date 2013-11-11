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
* CExpire. This class is used to handle PFKEY Expire primitives received 
* from IPSec plug-in. 
*
*/
#ifndef __IKEV2EXPIRE_H__
#define __IKEV2EXPIRE_H__

#include <e32base.h>

class TPfkeyMessage;
//
//
//  CIkev2Expire
//  This class is used to handle PFKEY Expire primitives received from
//  Ipsec plug-in. 
//
class CIkev2Expire : public CBase
{
    public:
        CIkev2Expire() {}        
        ~CIkev2Expire() {}
        static CIkev2Expire* NewL(const TPfkeyMessage& aPfkeyMessage);
        static CIkev2Expire* GetNext(CIkev2Expire** aAnchor);
        static void Link(CIkev2Expire* aExpire, CIkev2Expire** aAnchor);
        static void PurgeQue(CIkev2Expire** aAnchor);                
        TPtrC8 SPI();
        void SetSPI(const TDesC8& aSPI);
        TUint8 Protocol();
        void SetProtocol(TUint8 aProt);

    private:        
        TBuf8<4>  iSPI;           // Ipsec SPI value in (local SPI)
        TUint8    iProtocol;      // Ipsec protocol code
        TUint8    iReserved[3];   // For alignment 
        CIkev2Expire*  iNext;          // Next pending acquire
        
};

#endif /* __IKEV2EXPIRE_H__ */
