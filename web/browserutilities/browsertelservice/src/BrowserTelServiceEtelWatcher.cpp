/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*	  Definition of class CBrowserTelServiceEtelWatcher.
*
*
*/

 
//INCLUDE FILES
// Const file name for make call
#include <mmtsy_names.h>

#include "BrowserTelServiceEtelWatcher.h"



// ================= MEMBER FUNCTIONS =======================

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::NewLC()
//-----------------------------------------------------------------------------
//
CBrowserTelServiceEtelWatcher* CBrowserTelServiceEtelWatcher::NewLC(
				MBrowserTelServiceEtelWatcherObserver* aObserver )
	{
	CBrowserTelServiceEtelWatcher* self = 
		new( ELeave ) CBrowserTelServiceEtelWatcher;
	CleanupStack::PushL( self );
	self->ConstructL( aObserver );
	return self;
	}

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::NewL()
//-----------------------------------------------------------------------------
//
CBrowserTelServiceEtelWatcher* CBrowserTelServiceEtelWatcher::NewL(
				MBrowserTelServiceEtelWatcherObserver* aObserver )
	{
	CBrowserTelServiceEtelWatcher* self = 
		CBrowserTelServiceEtelWatcher::NewLC( aObserver );
	CleanupStack::Pop();
	return self;
	}

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::ConstructL()
//-----------------------------------------------------------------------------
//
void CBrowserTelServiceEtelWatcher::ConstructL(
	MBrowserTelServiceEtelWatcherObserver* aObserver )
	{
	iObserver = aObserver;

#ifndef __WINS__
	RTelServer::TPhoneInfo info;
	RPhone::TLineInfo lineInfo;

	// Connect to the telephony server and load the TSY.
	User::LeaveIfError( iRTelServer.Connect() );
	User::LeaveIfError( iRTelServer.LoadPhoneModule( KMmTsyModuleName ) );
	// Get the details for the first (and only) phone.
	User::LeaveIfError( iRTelServer.GetPhoneInfo( 0, info ) );
	// Open the phone.
	User::LeaveIfError( iRPhone.Open( iRTelServer, info.iName ) );
	// Get the information for the voice line, line 0.
	User::LeaveIfError( iRPhone.GetLineInfo( 0, lineInfo ) );
	// Open the line. iName will now be "VoiceLine1".
	User::LeaveIfError( iRLine.Open( iRPhone, lineInfo.iName ) );
	
	CActiveScheduler::Add( this );
#endif  // __WINS__
	}

//-----------------------------------------------------------------------------
// Default C++ constructor
//-----------------------------------------------------------------------------
//
CBrowserTelServiceEtelWatcher::CBrowserTelServiceEtelWatcher() : 
	CActive( EPriorityNormal )
	{
	}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
CBrowserTelServiceEtelWatcher::~CBrowserTelServiceEtelWatcher()
	{
#ifndef __WINS__
	// Cancle current activity
	Cancel();

	// Close all open sessions
	iRLine.Close();
	iRPhone.Close();
	iRTelServer.Close();
#endif  // __WINS__
	}

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::NotifyOnEtelStatusChangeL()
//-----------------------------------------------------------------------------
//
void CBrowserTelServiceEtelWatcher::NotifyOnEtelStatusChangeL()
	{
#ifndef __WINS__
	if( !IsActive() )
		{
		// Request notification about a change in line status
		iRLine.NotifyStatusChange( iStatus,iCallStatus );	
		SetActive();
		}
#endif
	}

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::NotifyOnEtelStatusChangeCancel()
//-----------------------------------------------------------------------------
//
void CBrowserTelServiceEtelWatcher::NotifyOnEtelStatusChangeCancel()
	{
#ifndef __WINS__
    Cancel();
#endif
	}

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::RunL()
//-----------------------------------------------------------------------------
//
void CBrowserTelServiceEtelWatcher::RunL()
	{
	if( iStatus.Int() == KErrNone )
		{
		if( iCallStatus == RCall::EStatusIdle )
			{	// We are only interested in idle state, which occures 
				// when the call has been hung up
			iObserver->BrowserTelServiceEtelWatcherEvent( iCallStatus );
			}
	
		// Renew the notification request,
		iRLine.NotifyStatusChange( iStatus,iCallStatus );	
		SetActive();
		}
	}

//-----------------------------------------------------------------------------
// CBrowserTelServiceEtelWatcher::DoCancel()
//-----------------------------------------------------------------------------
//
void CBrowserTelServiceEtelWatcher::DoCancel()
	{
	iRLine.NotifyStatusChangeCancel();
	}
