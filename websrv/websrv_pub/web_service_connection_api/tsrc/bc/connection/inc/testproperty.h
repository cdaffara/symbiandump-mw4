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











#ifndef TestProperty_H
#define TestProperty_H

// INCLUDES
#include <e32base.h>
#include <MSenProperty.h>


// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class CTestProperty: public MSenProperty, public CBase
    {
 	public:
	 	static CTestProperty* NewL();
	 	static CTestProperty* NewLC();
		TPtrC8 Name() ;

		TPtrC8 Type() ;

		TPtrC8 Value();

		TInt IntValue(TInt& aValue) ;

		TInt BoolValue(TBool& aValue) ;

		TInt ValueTokensL(const TDesC8& aDelimiter,
		              RPointerArray<TPtrC8>& aTokens) ;
		              
	private:
		CTestProperty();
    };

#endif