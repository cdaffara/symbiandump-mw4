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



#ifndef WSOVICREDENTIALOBSERVER_H
#define WSOVICREDENTIALOBSERVER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include "wsoviservicesession.h"

class CWSOviCredentialObserver : public CTimer
{
public:
	// Cancel and destroy
	~CWSOviCredentialObserver();

	// Two-phased constructor.
	static CWSOviCredentialObserver* NewL(CWSOviServiceSession* aSession, RFileLogger* aLogger);

	// Two-phased constructor.
	static CWSOviCredentialObserver* NewLC(CWSOviServiceSession* aSession, RFileLogger* aLogger);
	

	void RunL();
public: // New functions
	// Function for making the initial request
	void Start( const TTimeIntervalSeconds& aDiff);//const TTime &aTime);
	RFileLogger* Log() const;
private:
	// C++ constructor
	CWSOviCredentialObserver(CWSOviServiceSession* aSession, RFileLogger* aLogger);
	void StartCount();

	// Second-phase constructor
	void ConstructL();


private:
	TBool	iRunned;		// State of the active object
         CWSOviServiceSession* iSession;//not owned
         RFileLogger* iLogger; //not owned
         TInt64 iRestTime;
};

#endif // WSOVICREDENTIALOBSERVER_H

