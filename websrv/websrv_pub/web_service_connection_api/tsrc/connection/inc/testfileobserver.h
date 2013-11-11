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










#ifndef TestFileObserver_H
#define TestFileObserver_H

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
class CTestFileObserver: public MSenFilesObserver, public CBase
{
 	public:
	 	static CTestFileObserver* NewL();
	 	static CTestFileObserver* NewLC();
		void TransferProgress(TInt aTxnId = 0, TBool aIncoming = 0, const TDesC8& aMessage= KNullDesC8,
            const TDesC8& aCid = KNullDesC8, TInt aProgress = 0)    ;         
		
//	private:
		CTestFileObserver();
    };

#endif