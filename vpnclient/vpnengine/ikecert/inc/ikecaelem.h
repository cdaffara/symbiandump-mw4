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
* Description:   Class containing information about one CA cert
*
*/



#ifndef C_IKECAELEM_H
#define C_IKECAELEM_H

#include <e32base.h>

class CX509Certificate;

/**
 *  Class containing the certificate and it's key info.
 * 
 *  @lib IkeCert.lib
 *  @since S60 v3.0
 */
class CIkeCaElem : public CBase
    {
public:
	IMPORT_C static CIkeCaElem* NewL(HBufC8* aCert);
	IMPORT_C ~CIkeCaElem();
	
	IMPORT_C CX509Certificate*  Certificate() const;
	IMPORT_C const TDesC8& KeyHash() const;
			
private:
	CX509Certificate*  iCaCert;
	HBufC8*     iKeyInfo;
    };  

#endif // C_IKECAELEM_H
