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


#ifndef __CCHUNKALLOCATOR_H_
#define __CCHUNKALLOCATOR_H_

class CChunkAllocator : public CBase
	{
public:
	static CChunkAllocator* NewL ( TInt aHeapSize );
	~CChunkAllocator ();
	
	TAny* Alloc ( TInt aSize );
	void Free ( TAny* aPtr );
	
private:
	CChunkAllocator ();
	void ConstructL ( TInt aHeapSize );

private:
	RHeap* iHeap;
	};


#endif /*__CCHUNKALLOCATOR_H_*/
