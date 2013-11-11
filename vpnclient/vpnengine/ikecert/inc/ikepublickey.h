/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class containing information about one public key
*
*/



#ifndef C_IKEPUBLICKEY_H
#define C_IKEPUBLICKEY_H

#include <e32base.h>
#include "pkidefs.h"

class CX509Certificate;

/**
 *  Class containing information about one public key.
 *  Contains: key parameters, key data and key type info.
 *
 *  @lib IkeCert.lib
 *  @since S60 v3.0
 */
class CIkePublicKey : public CBase
    {
public:
    static CIkePublicKey* NewL(const CX509Certificate& aCert);		
    ~CIkePublicKey();
    
    TPKIKeyAlgorithm Algorithm() const;
    const TDesC8& KeyData() const;
    const TDesC8& KeyParams() const;
    
private:    
	CIkePublicKey();
	
	HBufC8*  iKeyParams;
	HBufC8*  iKeyData;
	TPKIKeyAlgorithm iAlgorithm;
    };

#endif // ? C_PUBLICKEY_H
