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
* Description:  Identity data structure for IKEv2
*
*/

#ifndef IKEV2IDENTITY_H_
#define IKEV2IDENTITY_H_

#include <e32base.h>

NONSHARABLE_CLASS(CIkeV2Identity) : public CBase
    {
public:
    static CIkeV2Identity* NewL(TUint8 aIdType, const TDesC8& aIdentity);
    ~CIkeV2Identity();
    
    TUint8 IdType() const;
    TPtrC8 Identity() const;
    
    /**
     * Returns id payload data, excluding the 
     * IKE payload fixed header:
     * 
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * !   ID Type     !                 RESERVED                      |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * !                                                               !
     * ~                   Identification Data                         ~
     * !                                                               !
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * 
     */
    TPtrC8 PayloadData() const;
    
private:    
    void ConstructL(TUint8 aIdType, const TDesC8& aIdentity);
        
    HBufC8* iIdPayloadData;
    };

#endif /* IKEV2IDENTITY_H_ */
