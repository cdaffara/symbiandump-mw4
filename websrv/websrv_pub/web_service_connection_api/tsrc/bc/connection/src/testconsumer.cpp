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
#include "testconsumer.h"

 	CTestConsumer* CTestConsumer::NewL()
 	{
 		CTestConsumer* pTest = new (ELeave) CTestConsumer();
 		return pTest;
 	}
 	CTestConsumer* CTestConsumer::NewLC()
 	{
 		CTestConsumer* pTest = new (ELeave) CTestConsumer();
 		CleanupStack::PushL(pTest);
 		return pTest;
 	}
 	CTestConsumer::CTestConsumer()
 	{
 		
 	}
 	void CTestConsumer::HandleMessageL(const TDesC8& aMessage) 
 	{
 	}
	void CTestConsumer::HandleErrorL(const TInt aErrorCode, 
                                  const TDesC8& aErrorMessage)
	{
	}
 	void CTestConsumer::SetStatus(const TInt aStatus)
 	{
	}


