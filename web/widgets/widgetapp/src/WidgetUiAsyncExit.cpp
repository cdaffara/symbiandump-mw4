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
*  CWidgetUiAsyncExit class can exit the Browser in async mode. It calls the CWidgetAppUi`s
*  Exit() method, when the object completes the request.
*
*
*/


#include "WidgetUiAsyncExit.h"
#include "ApiProvider.h"


// ---------------------------------------------------------------------------
// static CWidgetUiAsyncExit* CWidgetUiAsyncExit::NewL( )
// Create a CWidgetUiAsyncExit object. Leaves on failure.
// ---------------------------------------------------------------------------
//
CWidgetUiAsyncExit* CWidgetUiAsyncExit::NewL( MApiProvider& aApiProvider )
    {
    CWidgetUiAsyncExit* self = new( ELeave ) CWidgetUiAsyncExit( aApiProvider );
    return self;
    }

// ---------------------------------------------------------------------------
//	CWidgetUiAsyncExit::CWidgetUiAsyncExit( ):CActive(CActive::EPriorityStandard)
//  Constructs this object
// ---------------------------------------------------------------------------
//    
CWidgetUiAsyncExit::CWidgetUiAsyncExit( MApiProvider& aApiProvider ) :
    CActive(CActive::EPriorityStandard),
    iApiProvider( aApiProvider )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//	CWidgetUiAsyncExit::~CWidgetUiAsyncExit()
//  Destroy the object and release all memory objects
// ---------------------------------------------------------------------------
//    
CWidgetUiAsyncExit::~CWidgetUiAsyncExit()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
//	void CWidgetUiAsyncExit::Start()
//  Complete an asynchronous request.
// ---------------------------------------------------------------------------
//    
void CWidgetUiAsyncExit::Start()
    {
	if ( !IsActive() )
		{
		TRequestStatus* status = &iStatus;
		SetActive();    
		User::RequestComplete( status, KErrNone );
		}
    }

// ---------------------------------------------------------------------------
//	void CWidgetUiAsyncExit::DoCancel()
//  Cancel any outstanding requests.
// ---------------------------------------------------------------------------
//    
void CWidgetUiAsyncExit::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
//	void CWidgetUiAsyncExit::RunL()
//  Handles object`s request completion event.
// ---------------------------------------------------------------------------
//    
void CWidgetUiAsyncExit::RunL()
    {
    iApiProvider.ExitWidget( ); // Exit the widget.
    }

// End of file
