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
// Insert comment here
// @file
// @internalAll
// 
//

#ifndef __CSOCKETOPENER_H__
#define __CSOCKETOPENER_H__

// System includes
#include <e32base.h>

// Forward class references.
class CSocketHandler;
class MSocketHandlerObserver;

class CSocketOpener : public CBase
	{
	public:
		IMPORT_C static CSocketOpener* NewL ( MSocketHandlerObserver& aObserver );
//		IMPORT_C void MakeSocket ( const TDesC& aName );
		IMPORT_C void MakeSocket ( TUint aAddrFamily,  TUint aSockType, TUint aProtocol );       
		IMPORT_C ~CSocketOpener ();

	private:
		CSocketOpener ();
		void ConstructL ( MSocketHandlerObserver& aObserver );

	private:
		CSocketHandler* iHandler;
	};


#endif // __SOCKETOPENER_H__
