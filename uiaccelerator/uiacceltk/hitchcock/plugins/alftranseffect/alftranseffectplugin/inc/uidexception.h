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


#ifndef _UIDEXCEPTION_H_
#define _UIDEXCEPTION_H_

#include <e32base.h>

class CUidException : public CBase
	{
public:
	virtual TBool IsAllowed(const TUid& aFromUid, const TUid& aToUid) = 0;
	};

class CFullUidException : public CUidException
	{
public:
	CFullUidException(const TUid& aUid);
	TBool IsAllowed(const TUid& aFromUid, const TUid& aToUid);
private:
	const TUid iUid;
	};

class CPairedUidException : public CUidException
	{
public:
	CPairedUidException(const TUid& aUidFrom, const TUid& aToUid);
	TBool IsAllowed(const TUid& aFromUid, const TUid& aToUid);
private:
	const TUid iFromUid;
	const TUid iToUid;
	};

#endif // _UIDEXCEPTION_H_
