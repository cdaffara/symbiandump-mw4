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
* Description:        
*
*/











// INCLUDE FILES
#include "testprovider.h"
#include <e32base.h>

 	CTestProvider* CTestProvider::NewL()
 	{
 		CTestProvider* pTest = new (ELeave) CTestProvider();
 		return pTest;
 	}
 	CTestProvider* CTestProvider::NewLC()
 	{
 		CTestProvider* pTest = new (ELeave) CTestProvider();
 		CleanupStack::PushL(pTest);
 		return pTest;
 	}
 	CTestProvider::CTestProvider()
 	{
 		
 	}

	const CSenIdentityProvider* CTestProvider::IdentityProviderL() 
	{
		return NULL;
	}
	const TPtrC8 CTestProvider::UsernameL() 
	{
		return _L8("");
	}
	const TPtrC8 CTestProvider::PasswordL() 
	{
		return _L8("");
	}
	TAny* CTestProvider::ExtendedInterface(const TInt32 /* aUid */) 
	{
		 return NULL;
	}

