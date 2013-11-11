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

#include "chunkmgrteststepbase.h"

// create an chunk manager instance with default block size alloc & info
static const TInt K_BlockSize = 128;
static const TInt K_MinGrowth = 64;
static const TInt K_ThresholdGrowth = 40;
static const TInt K_InitialAllocation = 128;

CChunkMgrTestStepBase::CChunkMgrTestStepBase ()
	{
	}

CChunkMgrTestStepBase::~CChunkMgrTestStepBase ()
	{
	}

CChunkManager * CChunkMgrTestStepBase::CreateChunkMgrL ( TInt aMaxHeapSize )
	{	
	CChunkManager* mgr = CChunkManager::NewL ( aMaxHeapSize );
	if ( mgr )
		{
		mgr->AddPoolL ( K_BlockSize, K_InitialAllocation, K_MinGrowth, K_ThresholdGrowth );
		}
	
	iChkMgr = mgr;
			
	return mgr;	
	}
