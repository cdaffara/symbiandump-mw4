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











#ifndef TestMSenMessage_H
#define TestMSenMessage_H

// INCLUDES
#include <e32base.h>
#include <MSenMessage.h>




// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class CTestMSenMessage: public MSenMessage, public CBase
    {
 	public:
	 	static CTestMSenMessage* NewL();
	 	static CTestMSenMessage* NewLC();
	  	virtual MSenMessage::TClass Type() ;
	  	virtual MSenMessage::TDirection Direction() ;
		virtual TInt SetContext(MSenMessageContext* apNotOwnedContext) ;
	    virtual MSenMessageContext* Context() ;
	    virtual TInt SetProperties(MSenProperties* apOwnedProperties) ;
	    virtual MSenProperties* Properties() ;
	    virtual TBool IsSafeToCast(TClass aType) ;
	    virtual TInt TxnId() ; 
	    virtual MSenMessage* CloneL() ;
	private:
		CTestMSenMessage();
    };

#endif