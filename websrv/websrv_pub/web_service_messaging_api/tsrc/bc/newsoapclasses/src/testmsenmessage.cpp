/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:          test class implementation for MSenMessage.h.
*
*/












// INCLUDE FILES
#include "testmsenmessage.h"
#include <e32base.h>

 	CTestMSenMessage* CTestMSenMessage::NewL()
 	{
 		CTestMSenMessage* pTestMSenMessage = new (ELeave) CTestMSenMessage();
 		return pTestMSenMessage;
 	}
 	CTestMSenMessage* CTestMSenMessage::NewLC()
 	{
 		CTestMSenMessage* pTestMSenMessage = new (ELeave) CTestMSenMessage();
 		CleanupStack::PushL(pTestMSenMessage);
 		return pTestMSenMessage;
 	}
 	CTestMSenMessage::CTestMSenMessage()
 	{
 		
 	}
  	MSenMessage::TClass CTestMSenMessage::Type() 
  	{
  		return EMessageBase;
  	}
  	MSenMessage::TDirection CTestMSenMessage::Direction() 
  	{
  		return EOutbound;
  	}
	TInt CTestMSenMessage::SetContext(MSenMessageContext* apNotOwnedContext) 
	{
		return KErrNone;
	}
    MSenMessageContext* CTestMSenMessage::Context() 
    {
    	return NULL;
    }
    TInt CTestMSenMessage::SetProperties(MSenProperties* apOwnedProperties)
    {
    	return KErrNone;
    }
    MSenProperties* CTestMSenMessage::Properties() 
    {
    	return NULL;
    }
    TBool CTestMSenMessage::IsSafeToCast(TClass aType)
    {
    	return ETrue;
    }
    TInt CTestMSenMessage::TxnId() 
    {
    	return KErrNone;
    } 
    MSenMessage* CTestMSenMessage::CloneL() 
    {
    	return NULL;
    }

