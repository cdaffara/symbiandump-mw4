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

#ifndef __TUPNPMESSAGEPANIC_H_
#define __TUPNPMESSAGEPANIC_H_

#include <e32std.h>

class TUPnPMessagePanic
	{
public:
	
	enum TUPnPMessagePanicCode
		{
		EHeaderInvalidType			= 0,
		EHeaderFieldMissing			= 1,
		EMissingRequest				= 2,
		EMissingResponse			= 3,
		ENoBodyPartInDataSupplier	= 4,
		ENoMessage					= 5
		};
	
	static void Panic (TUPnPMessagePanicCode aPanic);	
	};

#endif /*TUPNPMESSAGEPANIC_H_*/
