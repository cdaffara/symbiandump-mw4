/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   State machine interface. Used for the statemachine in the 
*                server drawer to keep track on when a transition should be made
*                and when not.
*
*/


#include "uidblock.h"

CFullUidBlock::CFullUidBlock(const TUid& aUid) : 
	iUid(aUid)
	{
	}
	
TBool CFullUidBlock::IsBlocked(const TUid& aFromUid, const TUid& aToUid)
	{
	if(aFromUid == iUid)
	    {
		return ETrue;
	    }
	if(aToUid == iUid)
	    {
		return ETrue;
	    }
	return EFalse;
	}

CPairedUidBlock::CPairedUidBlock(const TUid& aFromUid, const TUid& aToUid) : 
	iFromUid(aFromUid),
	iToUid(aToUid)
	{
	}
	
TBool CPairedUidBlock::IsBlocked(const TUid& aFromUid, const TUid& aToUid)
	{
	if((aFromUid == iFromUid) && (aToUid == iToUid))
		return ETrue;
	return EFalse;
	}
