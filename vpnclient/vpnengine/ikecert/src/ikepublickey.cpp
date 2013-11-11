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
* Description: Class containing information about one public key.
*
*/




#include <x509cert.h>
#include "ikepublickey.h"

//
// CPublickey Class
//
CIkePublicKey* CIkePublicKey::NewL(const CX509Certificate& aCert)
    {
	CIkePublicKey* PubKey = new (ELeave) CIkePublicKey();
	CleanupStack::PushL(PubKey);			
	
	switch ( aCert.PublicKey().AlgorithmId() )
	    {
		case ERSA:
			PubKey->iAlgorithm = EPKIRSA;
			PubKey->iKeyData = HBufC8::NewL(aCert.PublicKey().KeyData().Length());
			PubKey->iKeyData->Des().Copy(aCert.PublicKey().KeyData());
            CleanupStack::Pop(PubKey);  //PubKey only removed from cleanup stack
			break;
			
		case EDSA:
			PubKey->iAlgorithm = EPKIDSA;
			PubKey->iKeyData = HBufC8::NewL(aCert.PublicKey().KeyData().Length());
			PubKey->iKeyData->Des().Copy(aCert.PublicKey().KeyData());
			PubKey->iKeyParams = HBufC8::NewL(aCert.PublicKey().EncodedParams().Length());
			PubKey->iKeyParams->Des().Copy(aCert.PublicKey().EncodedParams());	
            CleanupStack::Pop(PubKey);  //PubKey only removed from cleanup stack
			break;
			
		default:
		    CleanupStack::PopAndDestroy(PubKey);
			PubKey = NULL;
			break;
	    }
	
	return PubKey; 	
    }	


CIkePublicKey::CIkePublicKey() 
:iAlgorithm(EPKIInvalidAlgorithm)
    {
    }				
    
    
CIkePublicKey::~CIkePublicKey() 
    {
    delete iKeyParams; 
    delete iKeyData;
    }    


TPKIKeyAlgorithm CIkePublicKey::Algorithm() const
    {
    return iAlgorithm;
    }
    
    
const TDesC8& CIkePublicKey::KeyData() const
    {
    return *iKeyData;
    }
    
    
const TDesC8& CIkePublicKey::KeyParams() const
    {
    return *iKeyParams;
    }
