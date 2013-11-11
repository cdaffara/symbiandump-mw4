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
// @internalComponent
// 
//

#include <e32base.h>
#include "rmemcell.h"
#include "rmemchunk.h"

EXPORT_C TInt RMemChunk::Alloc ( TInt aSize, RMemoryAllocator& aAllocator )
	{
	if ( aSize == 0 )
		{
		return KErrNone;
		}
		
	__ASSERT_ALWAYS ( iNextCell == NULL, User::Invariant () ); //TODO
	iNextCell = aAllocator.ChunkManager ()->Alloc ( aSize );
	return iNextCell == NULL ? KErrNoMemory : KErrNone;
	}
	
EXPORT_C void RMemChunk::CreateL ( const TDesC8& aDes, RMemoryAllocator& aAllocator )
	{
	User::LeaveIfError ( Create ( aDes, aAllocator ) );
	}
	
EXPORT_C TInt RMemChunk::Create ( const TDesC8& aDes, RMemoryAllocator& aAllocator )
	{
	TInt err = Alloc ( aDes.Length (), aAllocator );
	if ( err == KErrNone )
		{
		// copy descriptor to the memchunk allocated.
		CopyIn(aDes);
		}
	return  err;
	}
	
EXPORT_C void RMemChunk::Free ()
	{
	if ( iNextCell )
		iNextCell->Free ();
	iNextCell = NULL;
	}

EXPORT_C void RMemChunk::CopyIn(const TDesC8& aDes, TInt aOffset)
	{
	__ASSERT_ALWAYS(iNextCell!=NULL, User::Invariant ());
	__ASSERT_ALWAYS(aOffset>=0, User::Invariant ());

	TUint8* ptr = (TUint8*)aDes.Ptr();
	TInt len = aDes.Length();

	TInt n;
	RMemCell* m=iNextCell;

	while (len>0 && m!=NULL)
		{
		n = len > m->Length() ? m->Length() : len;
		Mem::Copy(m->Ptr(), ptr, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	}

EXPORT_C void RMemChunk::CopyOut(TDes8& aDes, TInt aOffset) const
	{
	__ASSERT_ALWAYS(aOffset>=0, User::Invariant ());

	TUint8* ptr = (TUint8*)aDes.Ptr();
	TInt len = aDes.Length();

	TInt n;
	RMemCell* m = iNextCell;
	
	while (len>0 && m!=NULL)
		{
		n = len > m->Length() ? m->Length() : len;
		Mem::Copy(ptr, m->Ptr(), n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	aDes.SetLength(ptr-aDes.Ptr());
	}



EXPORT_C TBool RMemChunk::Goto(TInt anOffset, RMemCell* &resBuf, TInt& resOffset, TInt& resLength, RMemCell* &resPrevBuf) const
	{
	__ASSERT_ALWAYS(iNextCell!=NULL, User::Invariant ());
	__ASSERT_ALWAYS(anOffset>=0, User::Invariant ());

	TInt o = 0;
	RMemCell* p = NULL, *m = iNextCell;

	while (m!=NULL)
		{
		if (o + m->Length() > anOffset)
			{
			resBuf = m;
			resOffset = (anOffset - o) + m->Offset();
			resLength = m->Length() - (anOffset - o);
			resPrevBuf = p;
			return ETrue;
			}
		o += m->Length();
		p = m;
		m = m->Next();
		}

	__ASSERT_ALWAYS(o==anOffset, User::Invariant ());
	return EFalse;
	}

EXPORT_C TInt RMemChunk::Length() const
	{
	TInt len = 0;
	RMemCell* m;
	for (m = iNextCell; m!=NULL; m = m->Next())
		len += m->Length();

	return len;
	}

EXPORT_C void RMemChunk::TrimStart(TInt anOffset)
	{
	__ASSERT_ALWAYS(iNextCell!=NULL, User::Invariant ());
	__ASSERT_ALWAYS(anOffset>=0, User::Invariant ());

	if (anOffset==0)
		return;

	RMemCell* m, *p;
	TInt o, n;
	if (!Goto(anOffset, m, o, n, p))
		{
		Free();
		return;
		}

	if (p!=NULL) 
		{
		p->Unlink();
		iNextCell->Free();

		iNextCell = m;
		}

	if (o!=m->Offset()) 
		m->SetData(o, n);
	}

EXPORT_C void RMemChunk::Assign(RMemChunk& aChain)
	{
	__ASSERT_ALWAYS(iNextCell==NULL, User::Invariant ());
	iNextCell = aChain.iNextCell;
	aChain.Init();
	}

EXPORT_C RMemCell* TMemCellIterator::operator ++(TInt)
	{ 
	RMemCell* m=iMemCell; 
	if ( m!=NULL) 
		iMemCell=iMemCell->Next(); 
	return m; 
	}

EXPORT_C RMemCell* RMemChunk::Last() const
	{
	__ASSERT_ALWAYS(iNextCell!=NULL, User::Invariant ());

	return iNextCell->Last();
	}


EXPORT_C void RMemChunk::Append(RMemChunk& aChain)
	{
	__ASSERT_ALWAYS(iNextCell!=aChain.iNextCell, User::Invariant ());

	if (iNextCell!=NULL)
		{
		RMemCell* last	= Last();
		last->Link(aChain.iNextCell);
		}
	else
		iNextCell = aChain.iNextCell;

	aChain.Init();
	}

EXPORT_C RMemCell* RMemChunk::Remove()
	{
	if (IsEmpty())
		return NULL;
	else
		{
		RMemCell* m = iNextCell;
		iNextCell = iNextCell->Next();
		m->Unlink();
		return m;
		}
	}

EXPORT_C TInt RMemChunk::NumBufs() const
	{
	__ASSERT_ALWAYS(iNextCell!=NULL, User::Invariant ());

	TInt len = 0;
	RMemCell* m;
	for (m = iNextCell; m!=NULL; m = m->Next())
		len++;

	return len;
	}




