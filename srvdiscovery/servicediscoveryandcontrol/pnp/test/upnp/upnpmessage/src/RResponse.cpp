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
#include "rresponse.h"


EXPORT_C void RResponse::SetStatusCode(TInt aStatus)
	{
	static_cast<CResponse*>(iImplementation)->Status() = aStatus;
	}
EXPORT_C TInt RResponse::StatusCode() const
	{
	return static_cast<CResponse*>(iImplementation)->Status();
	}
EXPORT_C void RResponse::SetStatusText(RStringF aStatusString)
	{
	RStringF& statusString = 
		static_cast<CResponse*>(iImplementation)->StatusString();
	statusString.Close();
	statusString = aStatusString.Copy();
	}
EXPORT_C RStringF RResponse::StatusText() const
	{
	return static_cast<CResponse*>(iImplementation)->StringPool().
		StringF(static_cast<CResponse*>(iImplementation)->StatusString());
	}
EXPORT_C void RResponse::SetVersion(TVersion aVersion)
	{
	static_cast<CResponse*>(iImplementation)->Version() = aVersion;
	}
EXPORT_C TBool RResponse::IsHTTP10() const
	{
	return static_cast<CResponse*>(iImplementation)->IsHTTP10();
	}
