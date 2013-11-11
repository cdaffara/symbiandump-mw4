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
#include "testproperty.h"
#include <SenIdentityProvider.h>
#include <e32base.h>

 	CTestProperty* CTestProperty::NewL()
 	{
 		CTestProperty* pTest = new (ELeave) CTestProperty();
 		return pTest;
 	}
 	CTestProperty* CTestProperty::NewLC()
 	{
 		CTestProperty* pTest = new (ELeave) CTestProperty();
 		CleanupStack::PushL(pTest);
 		return pTest;
 	}
 	CTestProperty::CTestProperty()
 	{
 		
 	}
 	
 	TPtrC8 CTestProperty::Name() 
	{
		return _L8("");
	}
	TPtrC8 CTestProperty::Type() 
	{
	return _L8("");
	}
	TPtrC8 CTestProperty::Value()
	{
	return _L8("");
	}
	TInt CTestProperty::IntValue(TInt& aValue) 
	{
	return KErrNone;
	}
	TInt CTestProperty::BoolValue(TBool& aValue) 
	{
	return KErrNone;	
	}


	TInt CTestProperty::ValueTokensL(const TDesC8& aDelimiter,
	                              RPointerArray<TPtrC8>& aTokens) 
	{
	return KErrNone;
	}

