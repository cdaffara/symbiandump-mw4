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

#include "test01createdeletechunkMgr.h"


TVerdict CTest01CreateDeleteChunkMgr::doTestStepL ( void )
	{
	__UHEAP_MARK;	
	// -------------- step 1 --------------------
	CleanupStack::PushL ( CreateChunkMgrL ( KMinHeapSize ) );

	//-------------- step 2 --------------------
	CleanupStack::PopAndDestroy ( iChkMgr );
	
	__UHEAP_MARKEND;
	return EPass;
	}
