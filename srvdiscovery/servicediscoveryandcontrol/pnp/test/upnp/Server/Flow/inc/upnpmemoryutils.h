/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef UPNPMEMORYUTILS_H_
#define UPNPMEMORYUTILS_H_

#include <e32base.h>
#include <rmemchunk.h>
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>

class TUPnPMemoryUtils
	{
	public:
		static TInt TUPnPMemoryUtils::CreateMemChunk(RMemChunk& aMemChunk, RMBufChain& aData, RMemoryAllocator& aAllocator);
		static TInt TUPnPMemoryUtils::CreateMBuf(RMBufChain& aBufChain, RMemChunk& aData);
	};


#endif /*UPNPMEMORYUTILS_H_*/
