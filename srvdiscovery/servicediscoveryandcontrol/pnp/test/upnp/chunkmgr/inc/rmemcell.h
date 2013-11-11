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

#ifndef __RMEMCELL_H_
#define __RMEMCELL_H_

#include <e32base.h>

class RChunkPool;

class RMemCell
	{
public:
	RMemCell ( RChunkPool* aChunkPool, TUint8* aBuffer, TInt aBlockSize );	
	
	
	inline TUint8* Ptr ();
	
	void Free ();
	
	TAny* operator new ( TUint aSize, RMemCell* aPtr );
	void operator delete (  TAny* aPtr, RMemCell* aMemCell );
	
	inline TUint Size ();
	inline TUint8* Buffer();
	inline TInt Length() const;
	inline void SetOffset(TInt aOffset);
	inline TInt Offset() const;
	inline void SetData(TInt aOffset, TInt aLength);
	inline void Unlink();
	inline void Link(RMemCell* aBuf);
	inline RMemCell* Next ();
	RChunkPool* Pool ();	
	IMPORT_C RMemCell* Last();
	
	inline void SetLength ( TInt aLength );
	
private:
	TUint8* const 	iBuffer;
	RChunkPool* 	iChunkPool;
	const TInt 		iMaxSize;
	TInt 			iOffset;
	TInt			iLength;	
	RMemCell*		iNextCell;
	};

TUint RMemCell::Size ()	
	{
	return iMaxSize;
	}

inline TUint8* RMemCell::Buffer()
	{ 
	return iBuffer;
	}

inline TInt RMemCell::Length() const
	{ 
	return iLength; 
	}

inline void RMemCell::SetOffset(TInt aOffset)
	{ 
	iOffset = aOffset; 
	}

inline TInt RMemCell::Offset() const
	{ 
	return iOffset; 
	}

inline void RMemCell::Unlink()
	{ 
	iNextCell = NULL; 
	}

inline void RMemCell::SetData(TInt aOffset, TInt aLength)
	{ 
	SetOffset(aOffset);  
	SetLength(aLength); 
	}

inline void RMemCell::SetLength ( TInt aLength )
	{
	iLength = aLength;
	}

inline TUint8* RMemCell::Ptr ()
	{
	return iBuffer+iOffset;
	}

inline void RMemCell::Link(RMemCell* aBuf)
	{ 
	iNextCell = aBuf; 
	}
	
inline RMemCell* RMemCell::Next ()
	{
	return iNextCell;
	}


#endif /*__RMEMCELL_H_*/
