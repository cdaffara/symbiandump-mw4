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

#ifndef __RRequest_H__
#define __RRequest_H__ 

// System includes
#include <uri8.h>
#include <stringpool.h>

// Local includes
#include "rupnpmessage.h"

class RRequest : public RUPnPMessage
/** 
An HTTP Request.  This class extends the abstract HTTP Message to add an HTTP
method to be invoked on the resource at the remote HTTP server, and the URL that
identifies that resource.
@publishedAll
@released
*/
	{
 public:
	/** Get Methods
		Methods for reading information from the response.
	*/
	//@{
	// Gets the method name
	// @return The method to be used in the HTTP request.
	IMPORT_C RStringF Method() const;
	// Gets the URI
	// @return The URI to be used in the HTTP request.
	IMPORT_C const TUriC8& URI() const;
	/**
		Returns ETrue version of HTTP used by the server is 1.0
	*/
	IMPORT_C TBool IsHTTP10() const;
	//@}
	/**Set Methods
		
		Methods for setting information. These methods will not be of
		use to the client; they are intended for the use of advanced
		filters
	*/
	//@{
	// Sets the method name
	// @param aMethod The method name to be used in the HTTP request.
	IMPORT_C void  SetMethod(RStringF aMethod);
	// Sets the URI
	// @param aURI The URI to be used in the HTTP request.
	IMPORT_C void SetURIL(const TUriC8& aURI);
	// Sets the HTTP version
	IMPORT_C void SetVersion(TVersion aVersion);
	//@}
	friend class CRequest;
	};

#endif //	__RRequest_H__  
