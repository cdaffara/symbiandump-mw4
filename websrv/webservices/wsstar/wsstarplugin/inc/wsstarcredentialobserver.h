/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/
#ifndef WSSTARCREDENTIALOBSERVER_H
#define WSSTARCREDENTIALOBSERVER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include "wsstarservicesession.h"

class CWSStarCredentialObserver : public CTimer
{
public:
	// Cancel and destroy
	~CWSStarCredentialObserver();

	// Two-phased constructor.
	static CWSStarCredentialObserver* NewL(CWSStarServiceSession* aSession, RFileLogger* aLogger);

	// Two-phased constructor.
	static CWSStarCredentialObserver* NewLC(CWSStarServiceSession* aSession, RFileLogger* aLogger);
	

	void RunL();
public: // New functions
	// Function for making the initial request
	void Start( const TTimeIntervalMicroSeconds& aDiff);//const TTime &aTime);
	RFileLogger* Log() const;
private:
	// C++ constructor
	CWSStarCredentialObserver(CWSStarServiceSession* aSession, RFileLogger* aLogger);
	void StartCount();

	// Second-phase constructor
	void ConstructL();


private:
	TBool	iRunned;		// State of the active object
         CWSStarServiceSession* iSession;//not owned
         RFileLogger* iLogger; //not owned
         TInt64 iRestTime;
};

#endif // WSSTARCREDENTIALOBSERVER_H
