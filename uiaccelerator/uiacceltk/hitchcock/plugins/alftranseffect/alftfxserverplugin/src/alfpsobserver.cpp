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
* Description: 
*
*/

#include <e32property.h>
#include <alflogger.h>
#include "alfpsobserver.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
CAlfPsObserver* CAlfPsObserver::NewL( MPsObserver* aClient, const TUid aCategory, 
    const TUint aKey )
    {
    __ALFFXLOGSTRING("CAlfPsObserver::NewL >>");
    CAlfPsObserver* self = new ( ELeave ) CAlfPsObserver( aClient, aCategory, aKey);
    CleanupStack::PushL( self );
    self->Construct( aCategory, aKey );
    CleanupStack::Pop();
    __ALFFXLOGSTRING("CAlfPsObserver::NewL <<");
    return self;
    }

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
CAlfPsObserver* CAlfPsObserver::New( MPsObserver* aClient, const TUid aCategory, 
    const TUint aKey )
    {
    __ALFFXLOGSTRING("CAlfPsObserver::New >>");
    CAlfPsObserver* self = new CAlfPsObserver( aClient, aCategory, aKey);
    if(self)
    	{
	    self->Construct( aCategory, aKey );
    	}
    __ALFFXLOGSTRING("CAlfPsObserver::New <<");
    return self;
    }
    
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
CAlfPsObserver::~CAlfPsObserver()
    {
    __ALFFXLOGSTRING("CAlfPsObserver::~CAlfPsObserver >>");
    Cancel();
    iProperty.Close();
    __ALFFXLOGSTRING("CAlfPsObserver::~CAlfPsObserver <<");
    }
    
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
void CAlfPsObserver::DoCancel()
    {
    __ALFFXLOGSTRING("CAlfPsObserver::DoCancel >>");
    iProperty.Cancel();
    __ALFFXLOGSTRING("CAlfPsObserver::DoCancel <<");
    }
    
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
void CAlfPsObserver::RunL()
    {
    __ALFFXLOGSTRING1("CAlfPsObserver::RunL - iStatus: %d ", iStatus.Int());
    if ( iStatus.Int() == KErrNone)
        {
        TInt val;
        iProperty.Subscribe( iStatus );
        iProperty.Get( val );
        iClient->PsValueUpdated( iCategory, iKey, val );
        SetActive();
        }
    }


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
void CAlfPsObserver::Construct( const TUid aCategory, const TUint aKey )
    {
    __ALFFXLOGSTRING("CAlfPsObserver::Construct >>");
    CActiveScheduler::Add( this );
    iProperty.Attach( aCategory, aKey );
    iProperty.Subscribe( iStatus );
    SetActive();
    __ALFFXLOGSTRING("CAlfPsObserver::Construct <<");
    }

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
CAlfPsObserver::CAlfPsObserver( MPsObserver* aClient, const TUid aCategory,
                          const TUint aKey )
    : CActive( EPriorityStandard ), iClient( aClient ),
      iCategory( aCategory ), iKey( aKey )
    {
    }
