/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     
*     
*
*/



// INCLUDE FILES
#include "PhotoOperationWait.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhotoOperationWait::NewL
// -----------------------------------------------------------------------------
//
CPhotoOperationWait* CPhotoOperationWait::NewL( TInt aPriority )
	{
	CPhotoOperationWait* self = new( ELeave )CPhotoOperationWait( aPriority );	
	return self;
	}

	
// -----------------------------------------------------------------------------
// CPhotoOperationWait::~COperationWait
// -----------------------------------------------------------------------------
//
CPhotoOperationWait::~CPhotoOperationWait()
	{
    Cancel();
	}
	
// -----------------------------------------------------------------------------
// CPhotoOperationWait::Wait
// -----------------------------------------------------------------------------
//
void CPhotoOperationWait::Wait()
	{
	iStatus = KRequestPending;
	SetActive();
	iSafeWait.Start();
	}

// -----------------------------------------------------------------------------
// CPhotoOperationWait::RunL
// -----------------------------------------------------------------------------
//
void CPhotoOperationWait::RunL()
	{
	if ( iSafeWait.IsStarted() )
       {
       iSafeWait.AsyncStop();
       }
	}

// -----------------------------------------------------------------------------
// CPhotoOperationWait::SetViewActivate
// -----------------------------------------------------------------------------
//
void CPhotoOperationWait::SetViewActivate( TBool aViewActivate )
	{
	iViewActivate = aViewActivate;
	}

	
// -----------------------------------------------------------------------------
// CPhotoOperationWait::IsStarted
// -----------------------------------------------------------------------------
//
TBool CPhotoOperationWait::IsStarted()
	{
	return iViewActivate;
	}


// -----------------------------------------------------------------------------
// CPhotoOperationWait::COperationWait
// -----------------------------------------------------------------------------
//
CPhotoOperationWait::CPhotoOperationWait( TInt aPriority ): CActive( aPriority )
	{
	iViewActivate = EFalse;
	CActiveScheduler::Add( this );
	}
	

// ---------------------------------------------------------
// CPhotoOperationWait::HandleServerAppExit()
// ---------------------------------------------------------
//
void CPhotoOperationWait::HandleServerAppExit( TInt /*aReason*/ )
	{
	TRequestStatus* s = &iStatus;
	User::RequestComplete( s, KErrNone );	
	}
	

// -----------------------------------------------------------------------------
// CPhotoOperationWait::DoCancel
// -----------------------------------------------------------------------------
//
void CPhotoOperationWait::DoCancel()
	{
	if ( iSafeWait.IsStarted()  )
       {
       iSafeWait.AsyncStop();
       iStatus = KErrNone;
       }

	//For fast swap window closing, we don't receive HandleServerAppExit call
	//so we need to call it. Otherwise, the thread for this active object is not
	//released
	TRequestStatus* s = &iStatus;
	User::RequestComplete( s, KErrNone );	
	}

