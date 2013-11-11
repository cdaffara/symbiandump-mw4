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


#include <rmemcell.h>
#include <rmemchunk.h>
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>

#include "upnpmemoryutils.h"


TInt TUPnPMemoryUtils::CreateMemChunk(RMemChunk& aMemChunk, RMBufChain& aData, RMemoryAllocator& aAllocator)
	{
	TInt n, n1, n2;
	TUint8* p1, *p2;
	RMBuf* m1;
	RMemCell* m2;

	TInt len = aData.Length();
	
	if(!len)
		{
		aMemChunk.Init();
		return KErrNone;
		}
		
	TInt err = aMemChunk.Alloc ( len, aAllocator );
	
	if(err != KErrNone)
		{
		return err;
		}
	m1 = aData.First();
	p1 = m1->Ptr();
	n1 = m1->Length();
	
	m2 = aMemChunk.First();
	p2 = m2->Ptr();
	n2 = m2->Length();

	while (len>0)
		{
		__ASSERT_DEBUG(n1>0 && n2>0, User::Invariant ());

		n = n1 < n2 ? n1 : n2;

		Mem::Copy(p2, p1, n);

		if (n1 -= n, n1 == 0)
			{
			if (m1 = m1->Next(), m1==NULL)
				break;
			p1 = m1->Ptr();
			n1 = m1->Length();
			}
		else
			p1 += n;

		if (n2 -= n, n2 == 0)
			{
			if (m2 = m2->Next(), m2==NULL)
				break;
			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			p2 += n;

		len -= n;
		}
	
	return KErrNone;
	}

TInt TUPnPMemoryUtils::CreateMBuf(RMBufChain& aBufChain, RMemChunk& aData)
	{
	TInt n, n1, n2;
	TUint8* p1, *p2;
	RMemCell* m1;
	RMBuf* m2;

	TInt len = aData.Length();
	
	if(!len)
		{
		aBufChain.Init();
		return KErrNone;
		}
		
	TInt err = aBufChain.Alloc ( len );
	
	if(err != KErrNone)
		{
		return err;
		}
	m1 = aData.First();
	p1 = m1->Ptr();
	n1 = m1->Length();
	
	m2 = aBufChain.First();
	p2 = m2->Ptr();
	n2 = m2->Length();

	while (len>0)
		{
		__ASSERT_DEBUG(n1>0 && n2>0, User::Invariant ());

		n = n1 < n2 ? n1 : n2;

		Mem::Copy(p2, p1, n);

		if (n1 -= n, n1 == 0)
			{
			if (m1 = m1->Next(), m1==NULL)
				break;
			p1 = m1->Ptr();
			n1 = m1->Length();
			}
		else
			p1 += n;

		if (n2 -= n, n2 == 0)
			{
			if (m2 = m2->Next(), m2==NULL)
				break;
			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			p2 += n;

		len -= n;
		}

	return KErrNone;
	}



