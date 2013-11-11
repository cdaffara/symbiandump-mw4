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
// This file contains Rose Model ID comments - please do not delete
// 
//

#ifndef __RResponse_H__
#define __RResponse_H__

// System includes
#include <e32std.h>
#include <stringpool.h>

// Local includes
#include "rupnpmessage.h"


class RResponse : public RUPnPMessage
/** 
An HTTP Response.  This class extends the abstract HTTP Message to add an HTTP
version, a status code and status text.
@publishedAll
@released
*/
	{
public:
	/**Get Methods
		Methods for reading information from the response.
	*/
	//@{
	/** 
		Returns the status code
	*/
	IMPORT_C TInt StatusCode() const;
	/** Returns the status text, that is the text after the number on
        the first line of the response. */
	IMPORT_C RStringF StatusText() const;
	/**
		Returns ETrue version of HTTP used by the server is 1.0
	*/
	IMPORT_C TBool IsHTTP10() const;
	//@}
	/** Set Methods 

		Methods for setting information. These methods will not be of
		use to the client; they are intended for the use of advanced
		filters
	*/
	//@{
	/** Sets the status code.
	 */
	IMPORT_C void SetStatusCode(TInt aStatus);
	// Sets the status string
	IMPORT_C void SetStatusText(RStringF aStatusString);
	// Sets the HTTP version
	IMPORT_C void SetVersion(TVersion aVersion);
	//@}

	friend class CResponse;
	};



#endif // __RResponse_H__
