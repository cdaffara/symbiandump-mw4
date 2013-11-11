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


#if ( !defined __CHUNKMGRTESTSTEPBASE_H__ )
#define __CHUNKMGRTESTSTEPBASE_H__

#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <cchunkmanager.h>

// forward declarations

class CChunkMgrTestSuite;

class CChunkMgrTestStepBase: public CTestStep
	{			
public:
	static const TInt KMinHeapSize	= 256 * 1024;
	
	CChunkMgrTestStepBase ();
	~CChunkMgrTestStepBase ();		
	
protected:
	CChunkManager* CreateChunkMgrL ( TInt aMaxHeapSize );
	CChunkManager *iChkMgr;
	};

#endif /* __CHUNKMGRTESTSTEPBASE_H__ */
