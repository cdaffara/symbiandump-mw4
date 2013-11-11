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
*                server drawer to keep track on when a transition should be
*                always allowed.
*
*/


#include "uidexception.h"


CFullUidException::CFullUidException(const TUid& aUid) : 
	iUid(aUid)
	{
	}
	
TBool CFullUidException::IsAllowed(const TUid& aFromUid, const TUid& aToUid)
	{
	if(aFromUid == iUid)
		return ETrue;
	if(aToUid == iUid)
		return ETrue;
	return EFalse;
	}

CPairedUidException::CPairedUidException(const TUid& aFromUid, const TUid& aToUid) : 
	iFromUid(aFromUid),
	iToUid(aToUid)
	{
	}
	
TBool CPairedUidException::IsAllowed(const TUid& aFromUid, const TUid& aToUid)
	{
	if((aFromUid == iFromUid) && (aToUid == iToUid))
		return ETrue;
	return EFalse;
	}
