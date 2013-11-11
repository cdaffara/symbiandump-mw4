/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*/
 

#include "wrtharvesterpsnotifier.h"
#include "wrtharvester.h"
#include <UikonInternalPSKeys.h>//For MMC Observing
#include <usbmsshared.h>//For USB monitor

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Creates an instance of CCHPluginNotifier implementation
// ---------------------------------------------------------------------------
//
CWrtHarvesterPSNotifier* CWrtHarvesterPSNotifier::NewL( 
					CWrtHarvester* aHarvester, TPropertyKeys aKey )
    {
    CWrtHarvesterPSNotifier* self = new (ELeave) CWrtHarvesterPSNotifier( 
            aHarvester, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWrtHarvesterPSNotifier::~CWrtHarvesterPSNotifier()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::CCHPluginNotifier
// C++ default constructor
// -----------------------------------------------------------------------------
//
CWrtHarvesterPSNotifier::CWrtHarvesterPSNotifier(
		CWrtHarvester* aHarvester, TPropertyKeys aKey ) : CActive( EPriorityNormal )
    {    
    iHarvester = aHarvester;
    iKey = aKey;
    }

// -----------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CWrtHarvesterPSNotifier::ConstructL()
    {
    CActiveScheduler::Add( this );
    TInt r(KErrNone);
    if( iKey != EWidgetMMCAltered  )
    	{	
    	// define property to be integer type
    	r = RProperty::Define( KPropertyCat, iKey, RProperty::EInt );
    	}

    if ( r != KErrAlreadyExists || r != KErrNone )
        {
        User::LeaveIfError( r );
        }
    // Attach the key
    if( iKey == EWidgetMMCAltered )
		{
		User::LeaveIfError( iProperty.Attach( KPSUidUikon,KUikMMCInserted ));
		}
    else if( iKey == EWidgetMassStorageMode )
        {
        User::LeaveIfError( iProperty.Attach( KUsbMsDriveState_Category,EUsbMsDriveState_DriveStatus ));
        }    	
    else
    	{
    	User::LeaveIfError( iProperty.Attach( KPropertyCat, iKey));
   	 	}
    
    
    // Set the initial value to indicate that first widget can be launched
    if( iKey == EWidgetUIState )
        {
        User::LeaveIfError( iProperty.Set( 1 ));
        }

    // initial subscription
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPSNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::RunL
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPSNotifier::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt value( 0 );
    TInt r (KErrNone);
    TUsbMsDrivesStatus allDrivesStatus;
    if( iKey != EWidgetMMCAltered && iKey != EWidgetMassStorageMode )
    	{
    	iProperty.Get( KPropertyCat, iKey, value );	
    	}    
    else
        {        
        r = iProperty.Get( allDrivesStatus );
        }
    if( r == KErrNone )
        {
        if( iKey == EWidgetUIState && value == 1 )
            {
            iHarvester->TryLaunchNextOperationL();
            }
        else if(iKey == EWidgetUIState && value == 2)
            {
            iHarvester->ClearAllOperations();
            SetValue(1);
            }
        else if( iKey == EWidgetRegAltered && value == 1 )
            {
             iHarvester->UpdateL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCHPluginNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPSNotifier::RunError(TInt /*aError*/)
    {
    if ( !IsActive())
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCHPluginNotifier::Value
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPSNotifier::GetValue( TInt& aValue)
    {
    return iProperty.Get( aValue );
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::Value
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPSNotifier::SetValue( TInt aValue )
    {
    return iProperty.Set( aValue );
    }

// End of File
