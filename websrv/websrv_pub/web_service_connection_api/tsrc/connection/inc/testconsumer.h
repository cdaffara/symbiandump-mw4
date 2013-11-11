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










#ifndef TestConsumer_H
#define TestConsumer_H

// INCLUDES


#include <MSenServiceConsumer.h>



// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class CTestConsumer: public MSenServiceConsumer, public CBase
{
 	public:
	 	static CTestConsumer* NewL();
	 	static CTestConsumer* NewLC();
		void HandleMessageL(const TDesC8& aMessage = KNullDesC8) ;
		void HandleErrorL(const TInt aErrorCode = 0, 
                                  const TDesC8& aErrorMessage= KNullDesC8);
 		void SetStatus(const TInt aStatus = 0);
		
//	private:
		CTestConsumer();
    };

#endif