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
#include <e32base.h>
#include "testfileobserver.h"

 	CTestFileObserver* CTestFileObserver::NewL()
 	{
 		CTestFileObserver* pTest = new (ELeave) CTestFileObserver();
 		return pTest;
 	}
 	CTestFileObserver* CTestFileObserver::NewLC()
 	{
 		CTestFileObserver* pTest = new (ELeave) CTestFileObserver();
 		CleanupStack::PushL(pTest);
 		return pTest;
 	}
 	CTestFileObserver::CTestFileObserver()
 	{
 		
 	}
 	void CTestFileObserver::TransferProgress(TInt aTxnId , TBool aIncoming , const TDesC8& aMessage,
            const TDesC8& aCid , TInt aProgress )
  	{
	}


