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
* Description:   General utility methods for certificate handling
*
*/




#ifndef IKECERT_H
#define IKECERT_H


#include <e32base.h>

class CX500DistinguishedName;
class CX509Certificate;

/**
 *  General utility methods for certificate handling
 *
 *  Contains static methods, which can be user to check thinks from 
 *  certificates.
 *
 *  @lib IkeCert.lib
 */
class IkeCert
    {
public:	
    IMPORT_C static HBufC8* GetCertificateFieldDERL(HBufC8* aCertBfr, TInt aField);
    IMPORT_C static HBufC8* GetCertificateFieldDERL(const CX509Certificate *aCert, TInt aField);
	static TBool AltNameExistsL(const CX509Certificate *aX509Cert, const TDesC8 &aId);
	IMPORT_C static HBufC8* GetSubjectAltNameDataL(const CX509Certificate *aX509Cert, TUint8 aIkeIdType);		
	static HBufC8* BuildPkcs1v15HashL(const TDesC8 &aHashIn);
	static void CleanupSequence(TAny* aArray);
	static TInt CheckValidityPeriod(const CX509Certificate& aCert, TInt aWarningMargin, TInt aErrorMargin );		
	static TInt VerifyCertExtensionsL(const CX509Certificate& aX509Cert);			

private:		
	static TUint8* BERGetLengthL(TUint8* aP, TInt &aLen);
    }; 


#endif // IKECERT_H
