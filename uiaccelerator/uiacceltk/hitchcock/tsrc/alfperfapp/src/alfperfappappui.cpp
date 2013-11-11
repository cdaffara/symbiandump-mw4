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
* Description:  CAlfPerfAppAppUi implementation
*
*/


#include <avkon.hrh>
#include <alfperfapp.rsg>
#include "alfperfappappui.h"
#include "alfperfappmodel.h"

#include "alfperfapptestcaseselectionview.h"
#include "alfperfapptestcaseexecutionview.h"

// ============================ MEMBER FUNCTIONS ===============================

CAlfPerfAppAppUi::CAlfPerfAppAppUi( CAlfPerfAppModel& aModel )
    : iModel( aModel )
    {
    }

CAlfPerfAppAppUi::~CAlfPerfAppAppUi()
    {
    }

// -----------------------------------------------------------------------------
// Constructs the application.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppAppUi::ConstructL()
    {
    CAknViewAppUi::BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    iModel.LoadTestSetL( R_ALFPERFAPP_TESTSET );

    CAlfPerfAppTestCaseSelectionView* selectionView = 
        CAlfPerfAppTestCaseSelectionView::NewLC( iModel );
    AddViewL( selectionView );
    CleanupStack::Pop( selectionView );
    SetDefaultViewL( *selectionView );
    
    CAlfPerfAppTestCaseExecutionView* executionView =
        CAlfPerfAppTestCaseExecutionView::NewLC( iModel );
    AddViewL( executionView );
    CleanupStack::Pop( executionView );
    }

// -----------------------------------------------------------------------------
// Common commands are handled here.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EAknCmdExit:
        case EEikCmdExit:
            Exit();
            break;

        default:
            break;
        }
    }

void CAlfPerfAppAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknViewAppUi::HandleResourceChangeL( aType );
    }
