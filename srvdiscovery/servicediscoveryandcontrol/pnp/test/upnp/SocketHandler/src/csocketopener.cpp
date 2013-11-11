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
// @file
// @internalAll
// 
//

#include "csocketopener.h"
#include "csockethandler.h"
#include "sockhandlerdefs.h"

using namespace SockHandler;

EXPORT_C CSocketOpener* CSocketOpener::NewL ( MSocketHandlerObserver& aObserver )
	{
	CSocketOpener* socketOpener = new (ELeave) CSocketOpener ();
	CleanupStack::PushL ( socketOpener );
	socketOpener->ConstructL ( aObserver );
	CleanupStack::Pop ();
	return socketOpener;
	}

EXPORT_C CSocketOpener::~CSocketOpener ()
	{
	delete iHandler;
	}

CSocketOpener::CSocketOpener ()
	{
	}

void CSocketOpener::ConstructL ( MSocketHandlerObserver& aObserver )
	{
	iHandler = new (ELeave) CSocketHandler ( aObserver );
	}


/*EXPORT_C void CSocketOpener::MakeSocket ( const TDesC& aName )
	{
	TSocketHandlerParams params ( EOpenByName, &aName );
	iHandler->Activate ( params );
	}
*/
EXPORT_C void CSocketOpener::MakeSocket ( TUint aAddrFamily,  TUint aSockType, TUint aProtocol )
	{
	TSocketHandlerParams params ( EOpenByProtocol, aAddrFamily, aSockType, aProtocol );
	iHandler->Activate ( params );
	}




