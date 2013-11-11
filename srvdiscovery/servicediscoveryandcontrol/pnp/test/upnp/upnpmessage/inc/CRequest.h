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

#ifndef __CREQUEST_H__
#define __CREQUEST_H__

#include <stringpool.h>

#include "rrequest.h"
#include "CMessage.h"

/** An HTTP Request.  Provides the real implementation of a request,
	as proxied in the public API by RRequest.
	@see RRequest
*/
class CRequest : public CMessage
	{
public:

	/** Constructs a request
	*/
	IMPORT_C static CRequest* NewL(CHeaderCodec& aCodec, RStringPool& aStringPool);
	/** Destructor
	*/
	IMPORT_C ~CRequest();

	/** Returns the method
	*/
	const RStringF& Method() const;

	/** Returns the method (as a reference so you can set it)
	*/
	RStringF& Method();

	/** Returns the URI (as a reference so you can set it)
	*/
	CUri8*& URI();

	/** Obtain a handle for this request that can be used by the client.
	*/
	inline RRequest Handle();

	/** Returns the string pool.
	*/
	RStringPool& StringPool();
	
	/** Returns the string pool
	*/
	const RStringPool& StringPool() const;
	
	/** Returns ETrue if the HTTP protocol version is 1.0
	 */
	inline TBool IsHTTP10() const;
	
  	/** Returns the HTTP version (returned as a reference, which you can set)
  	*/
  	inline TVersion& Version();
  	
  	/** Resets the request object.
  	 */
  	inline void Reset();

private:

	/** Constructor
	*/
	CRequest(RStringPool& aStringPool);

private:

	/** The URI
	*/
	CUri8* iURI;

	/** The method
	*/
	RStringF iMethod;
	
	/** The string pool
	*/
	RStringPool& iStringPool;
	
	/** The HTTP version number (major/minor) obtained from the HTTP server's response
	*/
	TVersion iVersion;
	
	};

#include "CRequest.inl"

#endif // __CREQUEST_H__
