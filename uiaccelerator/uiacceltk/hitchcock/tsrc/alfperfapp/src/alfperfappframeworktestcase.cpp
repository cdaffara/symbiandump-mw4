/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CAlfPerfAppFrameworkTestCase implementation.
*
*/


#include "alfperfappframeworktestcase.h"
#include "alfperfapp.hrh"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppFrameworkTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId > EAlfPerfAppFrameworkMin && 
           aCaseId < EAlfPerfAppFrameworkMax;
    }

CAlfPerfAppFrameworkTestCase* CAlfPerfAppFrameworkTestCase::NewL( TInt aCaseId, TInt aSequenceIndex)
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppFrameworkTestCase* self = 
        new (ELeave) CAlfPerfAppFrameworkTestCase( aCaseId, aSequenceIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppFrameworkTestCase::~CAlfPerfAppFrameworkTestCase()
    {
    }

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppFrameworkTestCase::SetupL( 
        CAlfEnv& aEnv, const TRect& /*aVisibleArea*/, TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iEnv, User::Invariant() );
    iEnv = &aEnv;
    
    TInt errorCode = KErrNone;
    switch ( iCaseId )
        {   
        case EAlfPerfAppFrameworkSetupFailSynch:
            User::Leave( KErrGeneral );
            break;
            
        case EAlfPerfAppFrameworkSetupFailAsynch:
            errorCode = KErrGeneral;
            break;
        
        case EAlfPerfAppFrameworkEmpty:
        case EAlfPerfAppFrameworkExecuteFailSynch:    
        case EAlfPerfAppFrameworkExecuteFailAsynch:
            break;

        case EAlfPerfAppFrameworkCreateFail:
        default:
            User::Invariant();
            break;
        }
        
    CAlfPerfAppTestCase::CompleteNow( aStatus, errorCode );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppFrameworkTestCase::ExecuteL( TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( iEnv, User::Invariant() );
    
    TInt errorCode = KErrNone;
    switch ( iCaseId )
        {
        case EAlfPerfAppFrameworkEmpty:
            break;
            
        case EAlfPerfAppFrameworkExecuteFailSynch:
            User::Leave( KErrGeneral );
            break;
            
        case EAlfPerfAppFrameworkExecuteFailAsynch:
            errorCode = KErrGeneral;
            break;
        
        case EAlfPerfAppFrameworkCreateFail:
        case EAlfPerfAppFrameworkSetupFailSynch:
        case EAlfPerfAppFrameworkSetupFailAsynch:
            // If setup fails, then ExecuteL must not be called.
        default:
            User::Invariant();
            break;            
        }
        
    CAlfPerfAppTestCase::CompleteNow( aStatus, errorCode );
    }

// -----------------------------------------------------------------------------
// Tears down.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppFrameworkTestCase::TearDown()
    {
    iEnv = NULL;
    }

void CAlfPerfAppFrameworkTestCase::HandleVisibleAreaChange( 
        const TRect& /*aRect*/ )
    {
    }

TInt CAlfPerfAppFrameworkTestCase::CaseID()
    {
    return iCaseId;
    }

CAlfPerfAppFrameworkTestCase::CAlfPerfAppFrameworkTestCase( TInt aCaseId, TInt aSequenceIndex )
    : CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
    {
    }

void CAlfPerfAppFrameworkTestCase::ConstructL()
    {
    switch ( iCaseId )
        {
        case EAlfPerfAppFrameworkCreateFail:
            User::Leave( KErrGeneral );
            break;
            
        default:
            // Simply continue with other cases.
            break;
        }
    }
