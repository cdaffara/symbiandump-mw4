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
#include "CResponse.h"


EXPORT_C CResponse* CResponse::NewL(CHeaderCodec& aCodec, RStringPool& aStringPool)
	{
	CResponse* that = new (ELeave) CResponse(aStringPool);
	CleanupStack::PushL(that);
	that->ConstructL(aCodec);
	CleanupStack::Pop(that);
	return that;
	}

EXPORT_C CResponse::~CResponse()
	{
	iStatusString.Close();
	}

