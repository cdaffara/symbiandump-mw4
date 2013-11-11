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
* Description:  connection header declaration      
*
*/










#ifndef TestProvider_H
#define TestProvider_H

// INCLUDES

#include <e32base.h>
#include <MSenAuthenticationProvider.h>



// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class CTestProvider: public MSenAuthenticationProvider, public CBase
{
 	public:
	 	static CTestProvider* NewL();
	 	static CTestProvider* NewLC();
		              
		const CSenIdentityProvider* IdentityProviderL() ;
		const TPtrC8 UsernameL() ;
		const TPtrC8 PasswordL() ;
		TAny* ExtendedInterface(const TInt32 /* aUid */) ;     
	private:
		CTestProvider();
    };

#endif