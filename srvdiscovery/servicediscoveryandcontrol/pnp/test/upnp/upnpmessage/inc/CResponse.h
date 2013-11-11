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

#ifndef __CRESPONSE_H__
#define __CRESPONSE_H__

#include <stringpool.h>

#include "rresponse.h"
#include "CMessage.h"

/** An HTTP Response.  Provides the real implementation of a response,
	as proxied in the public API by RResponse.
	@see RResponse
*/
class CResponse : public CMessage
	{
 public:
	/** Constructs a response
	*/
	IMPORT_C static CResponse* NewL(CHeaderCodec& aCodec, RStringPool& aStringPool);

	/** Destructor
	*/
	IMPORT_C ~CResponse();

	/** Returns the status (returned as a reference; use this call to set it)
	*/
	inline TInt& Status();

	/** Returns the status string (returned as a reference, which you can set)
	*/
	inline RStringF& StatusString();

	/** Returns ETrue if the HTTP protocol version is 1.0
	 */
	inline TBool IsHTTP10() const;
	
	/** Obtain a handle for this response that can be used by the client.
	*/
	inline RResponse Handle();

	/** Returns the string pool.
	*/
	RStringPool& StringPool();
	
	/** Returns the string pool.
	*/
	const RStringPool& StringPool() const;
	
 	/** Returns the HTTP version (returned as a reference, which you can set)
   	*/
   	inline TVersion& Version();

   	/** Resets the request object.
  	 */
  	inline void Reset();

 private:
	/** Constructor
	*/
	CResponse(RStringPool& aStringPool);
	
 private:

	/** The HTTP status code obtained from the HTTP server's response
	*/
	TInt iStatus;

	/** The HTTP status description string obtained from the HTTP server's response
	*/
	RStringF iStatusString;

	/** The HTTP version number (major/minor) obtained from the HTTP server's response
	*/
	TVersion iVersion;
	
	/** The string pool
	*/
	RStringPool& iStringPool;

	};

#include "CResponse.inl"


#endif // __CRESPONSE_H__
