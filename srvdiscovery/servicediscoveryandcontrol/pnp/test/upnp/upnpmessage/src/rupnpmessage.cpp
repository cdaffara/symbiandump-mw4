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
// rhttpmessage.cpp
// 
//

// System includes
#include <e32std.h>

// Local includes
#include "CMessage.h"
#include "rupnpmessage.h"


EXPORT_C RHTTPHeaders RUPnPMessage::GetHeaderCollection()
	{
	return iImplementation->iHeaders->Handle();
	}

EXPORT_C void RUPnPMessage::SetBody(MHTTPDataSupplier& aBody)
	{
	iImplementation->AddBody(&aBody);
	}

EXPORT_C void RUPnPMessage::RemoveBody()
	{
	iImplementation->AddBody(NULL);
	}

EXPORT_C TBool RUPnPMessage::HasBody() const
	{
	return iImplementation->HasBody();
	}

EXPORT_C MHTTPDataSupplier* RUPnPMessage::Body() const
	{
	return iImplementation->Body();
	}

