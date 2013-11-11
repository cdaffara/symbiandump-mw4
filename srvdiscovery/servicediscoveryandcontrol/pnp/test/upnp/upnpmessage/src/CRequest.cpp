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

// System includes
#include <e32std.h>

// Local includes
#include "CRequest.h"


EXPORT_C CRequest* CRequest::NewL(CHeaderCodec& aCodec, RStringPool& aStringPool)
	{
	CRequest* req = new (ELeave) CRequest(aStringPool);
	CleanupStack::PushL(req);
	req->ConstructL(aCodec);
	CleanupStack::Pop(req);
	return req;
	}

EXPORT_C CRequest::~CRequest()
	{
	delete iURI;
	iMethod.Close();
	}


