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


#ifndef _UIDBLOCK_H_
#define _UIDBLOCK_H_

#include <e32base.h>

class CUidBlock : public CBase
	{
public:
	virtual TBool IsBlocked(const TUid& aFromUid, const TUid& aToUid) = 0;
	};


class CFullUidBlock : public CUidBlock
	{
public:
	CFullUidBlock(const TUid& aUid);
	TBool IsBlocked(const TUid& aFromUid, const TUid& aToUid);
private:
	const TUid iUid;
	};


class CPairedUidBlock : public CUidBlock
	{
public:
	CPairedUidBlock(const TUid& aUidFrom, const TUid& aToUid);
	TBool IsBlocked(const TUid& aFromUid, const TUid& aToUid);
private:
	const TUid iFromUid;
	const TUid iToUid;
	};

#endif
