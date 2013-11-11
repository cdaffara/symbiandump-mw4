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
#include "cchunkmanager.h"
#include "cmemchunk.h"
#include "rchunkpool.h"
#include "rmemcell.h"

RMemCell::RMemCell ( RChunkPool* aChunkPool, TUint8* aBuffer, TInt aBlockSize )
	: iBuffer ( aBuffer ),
	  iChunkPool ( aChunkPool ),
	  iMaxSize ( aBlockSize ),
	  iOffset ( 0 ),
	  iLength ( aBlockSize ),
	  iNextCell ( NULL )
	{
	}

TAny* RMemCell::operator new ( TUint , RMemCell* aPtr )
	{
	return aPtr;
	}
	
void RMemCell::operator delete ( TAny* , RMemCell* )
	{
	}

void RMemCell::Free ()
	{
	iChunkPool->ChunkManager().Free ( this );
	}
	
RChunkPool* RMemCell::Pool ()
	{
	return iChunkPool;
	}

EXPORT_C RMemCell* RMemCell::Last()
{
	RMemCell* c = iNextCell;
	RMemCell* p = this;

	while (c != NULL)
		{
		p = c;
		c = c->Next();
		}
	return p;
	}

