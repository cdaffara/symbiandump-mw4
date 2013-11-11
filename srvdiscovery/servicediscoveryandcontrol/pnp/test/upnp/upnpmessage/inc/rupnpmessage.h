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
// RHTTPMessage.h
// 
//

#ifndef __RHTTPMESSAGE_H__
#define __RHTTPMESSAGE_H__

// Forward declarations
class RHTTPHeaders;
class MHTTPDataSupplier;
class CMessage;


class RUPnPMessage 
/** 
An abstract HTTP message. In RFC2616, an HTTP message is defined as having a
header and an optional body.  This class is specialised for HTTP requests and
responses.
@publishedAll
@released
*/
	{
 public:
	// Default (uninitialised) constructor.
	inline RUPnPMessage();

	/** Returns the header collection for the message */
	IMPORT_C RHTTPHeaders GetHeaderCollection();

	/** Sets the messages body, replacing any existing body. The body
        is supplied as a MHTTPDataSupplier, which will then return the
        data in 1 or more chunks on request.  
		@param aBody The data supplier that will supply the body.  */
	IMPORT_C void SetBody(MHTTPDataSupplier& aBody);

	/** Removes the body */
	IMPORT_C void RemoveBody();

	/** Determine whether this message has any associated body data.
		@return ETrue if there is some body data, EFalse if not.
	*/
	IMPORT_C TBool HasBody() const;

	/** Gets the body. The body is supplied as a MHTTPDataSupplier,
        which can be used to return the current data chunk, and to
        acknowledge when that chunk has been consumed by the client.

		Note that the client can assume the body object will remain
		unchanged from when it receives the first data until the end
		of the transaction.

		@return A data supplier that provides access to a body
		data chunk. NULL if the body has not been set, or has been removed */
	IMPORT_C MHTTPDataSupplier* Body() const;

 protected:
	CMessage* iImplementation;
};


inline RUPnPMessage::RUPnPMessage()
		: iImplementation(NULL)
	{
	};


#endif // __RUPnPMessage_H__ 
