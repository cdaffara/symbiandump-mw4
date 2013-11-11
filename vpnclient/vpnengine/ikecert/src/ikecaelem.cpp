/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class containing information about one CA cert
*
*/




#include <x509cert.h>

#include "ikecaelem.h"
#include "ikecert.h"
#include "utlcrypto.h"
#include "ikev1pkiservice.h"
#include "ikecertconst.h"

EXPORT_C CIkeCaElem* CIkeCaElem::NewL(HBufC8* aCert)
    {
    ASSERT(aCert);
	CIkeCaElem* CaElem = new (ELeave) CIkeCaElem();
	CleanupStack::PushL(CaElem);		
	CaElem->iCaCert = CX509Certificate::NewL(*aCert);
	delete aCert;
	HBufC8* PublicKeyInfo = IkeCert::GetCertificateFieldDERL(CaElem->iCaCert, KPublicKeyInfo);
	CleanupStack::PushL(PublicKeyInfo);	
	
	//
	// Calculate "Key Id" value as a SHA1 hash Subject Public Key Info element
	// (specified so in IKEv2 draft) 
	//
	CaElem->iKeyInfo = HBufC8::NewL(IKEV2_CERT_KEYID_SIZE);
	CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
	TPtrC8 InData(PublicKeyInfo->Des());
	TPtrC8 KeyId = 	Digest->Final(InData);
	CaElem->iKeyInfo->Des().Copy(KeyId);
	delete Digest;
	
	CleanupStack::PopAndDestroy(PublicKeyInfo); 
	CleanupStack::Pop(CaElem);		
	return CaElem; 	
    }


EXPORT_C CIkeCaElem::~CIkeCaElem()
    {
    delete iCaCert;
    delete iKeyInfo;
    }
    

EXPORT_C CX509Certificate* CIkeCaElem::Certificate() const
    {
    return iCaCert;
    }
    
    
EXPORT_C const TDesC8& CIkeCaElem::KeyHash() const
    {
    return *iKeyInfo;
    }
    
