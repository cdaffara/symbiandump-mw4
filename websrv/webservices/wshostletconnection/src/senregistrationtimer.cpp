/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    not a public class       
*
*/




// INCLUDES
#include "senregistrationtimer.h"

// CLASS DECLARATION
CSenRegistrationTimer* CSenRegistrationTimer::NewL( MSenRegistrationActor& aActor, TInt aRegistrationTimeoutInSecs )
    {
    CSenRegistrationTimer* pNew = CSenRegistrationTimer::NewLC( aActor, aRegistrationTimeoutInSecs );
    CleanupStack::Pop( pNew );
    return pNew;
    }

CSenRegistrationTimer* CSenRegistrationTimer::NewLC( MSenRegistrationActor& aActor, TInt aRegistrationTimeoutInSecs )
    {
    CSenRegistrationTimer* pNew = new (ELeave)CSenRegistrationTimer( aActor, aRegistrationTimeoutInSecs );
    CleanupStack::PushL( pNew );
    pNew->ConstructL();
    return pNew;
    }
     
CSenRegistrationTimer::CSenRegistrationTimer( MSenRegistrationActor& aActor, TInt aRegistrationTimeoutInSecs )
//:CTimer( EPriorityNormal ), iActor(aActor), iRegistrationTimeoutInSecs( aRegistrationTimeoutInSecs ), iRegisterFilesObserver(EFalse)
:CActive( EPriorityNormal ), iActor(aActor), iRegistrationTimeoutInSecs( aRegistrationTimeoutInSecs ), iRegisterFilesObserver(EFalse)
    {
    }     

void CSenRegistrationTimer::ConstructL ()
	{
	User::LeaveIfError (iTimer.CreateLocal());	
	CActiveScheduler::Add(this);	
	}

          
CSenRegistrationTimer::~CSenRegistrationTimer()
    {
	CActive::Cancel();
	iTimer.Close();	
    }

void CSenRegistrationTimer::IssueRegistrations( TBool aRegisterFilesObserver )
    {
    iRegisterFilesObserver = aRegisterFilesObserver;
    // iRegisterMobilityObserver = aRegisterMobilityObserver;
    // iRegister<some>Observer = aRegister<some>Observer ;

    TInt interval( KErrNone );
	if( iRegistrationTimeoutInSecs > 0 )
		{
		interval = iRegistrationTimeoutInSecs * 1000 * 1000;
		}
	else // use the timeout value of 1 second (default)
		{
		interval = KSenDefaultRegistrationTimeout * 1000 * 1000;
		}
	if (!IsActive())
        {
        iTimer.After( iStatus, interval );
        if(!IsActive())
        	SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CSenRegistrationTimer::DoCancel
// -----------------------------------------------------------------------------
//	
void CSenRegistrationTimer::DoCancel ()
    {
    iRegisterFilesObserver = EFalse;
	iTimer.Cancel();
    }

void CSenRegistrationTimer::RunL()
    {
    if (iStatus.Int() == KErrNone)
    	{
    	if( iRegisterFilesObserver )
        	{
        	/* TInt err = */ 
        	TRAP_IGNORE( iActor.RegisterAndSubscribeFileProgressObserversL(); )
    		// err = 0; // not used in release builds
        	}
        // if ( iRegisterMobilityObserver  ) {} ...
        // if ( iRegister<some>Observer  ) {} ...
    	}
    }

//virtual TInt CSenRegistrationTimer::RunError(TInt aError) { ; }
//void CSenRegistrationTimer::DoCancel() { ; }    
// End of File
