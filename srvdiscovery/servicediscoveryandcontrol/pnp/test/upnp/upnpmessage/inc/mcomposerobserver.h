// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __MCOMPOSEROBSERVER_H_
#define __MCOMPOSEROBSERVER_H_

// System includes.
#include <e32std.h>

/**
The MComposerObserver class is the observer API for the upnp request and response composer.
It allows the composer to notify its observer when it has message data ready to 
send and when the message has been completed.
*/
class MComposerObserver
	{	
public:
/**
	Notifies the observer that there is a message data that is ready to be sent.
	The API will be invoked by the composer after it has composed the message start line and
	the message headers. It'll again be invoked once the message body data is ready.
*/	
	virtual void MessageDataReadyL(RBuf8& aData) = 0;

/**
	Signals the end of the composing activity.
*/	
	virtual void ComposingConcluded() = 0;
	
/**
	Error notifier. The composer has experienced an error.
	@param aError The error code. Possible error codes are:
	KErrNoMemory - When there is insufficient memory for allocations,
	KErrCorrupt - When the body data supplier indicated that the last data part even though it 
				  had not supplied all the data it specified. Or the body data supplier 
				  has supplied all the data it specified but has not indicated a 
                  last data part.
    KErrNotFound - When the Locate() and Find() APIs of TDesC8 fail.
*/	
	virtual void ComposerError(TInt aError) = 0;
	};

#endif /*MCOMPOSEROBSERVER_H_*/
