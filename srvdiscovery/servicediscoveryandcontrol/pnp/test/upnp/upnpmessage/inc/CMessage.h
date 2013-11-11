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

#ifndef __CMESSAGE_H__
#define __CMESSAGE_H__

// System includes
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>

// Local includes
#include "CHeaders.h"
#include "rhttpheaders.h"

/** Abstract base class for implementation of HTTP requests and responses
*/
class CMessage : public CBase
	{
public:
	/** Destructor.
		(NB Although this method has an implementation - this class must be abstract)
	*/
	~CMessage() = 0;
	
	/** Specify the body data supplier for this HTTP message.
		@see RHTTPMessage::AddBody
	*/
	inline void AddBody(MHTTPDataSupplier* aBody);

	/** Determine whether this HTTP message has a body or not.
		@see RHTTPMessage::HasBody
	*/
	inline TBool HasBody() const;

	/** Obtain this HTTP message's body data supplier.
		@see RHTTPMessage::GetBody
	*/
	inline MHTTPDataSupplier* Body() const;
	
protected:

	/** Does base class 2 phase construction
	*/
	inline void ConstructL(CHeaderCodec& aCodec);

private:

	CHeaders* 				iHeaders;
	MHTTPDataSupplier* 		iDataSupplier;
	
	friend class RUPnPMessage;
	friend class RRequest;
	friend class RResponse;
	};

#include "CMessage.inl"


#endif // __CMESSAGE_H__
