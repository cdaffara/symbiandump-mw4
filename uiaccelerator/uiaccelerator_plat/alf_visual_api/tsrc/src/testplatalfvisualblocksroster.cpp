/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test functions for alfroster.h
*
*/



// [INCLUDE FILES]
#include <alf/alfroster.h>
#include <alf/alfmetric.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KNum3 = 3;
const TInt KNum4 = 4;
const TInt KNum5 = 5;
const TReal32 KMagnitude = 5;
const TInt KControlId = 222;


// FORWARD DECLARATION
//class CTestControlGroupOrderChangedObserver for testing CAlfRoster
class CTestControlGroupOrderChangedObserver
: public CBase, public MAlfControlGroupOrderChangedObserver
    {
public:
    /**
     * Default constructor
     */
    CTestControlGroupOrderChangedObserver( CAlfRoster& aRoster )
    : iRoster( aRoster ), iNoOfCalls( 0 )
        {}
    
    /**
     * Default destructor
     */
    ~CTestControlGroupOrderChangedObserver()
        {
        iRoster.RemoveControlGroupOrderChangedObserver( *this );
        }
    
    /**
     * ConstructL
     */
    void ConstructL()
        {
        iRoster.AddControlGroupOrderChangedObserverL( *this );
        }
    
    /**
     * From MAlfControlGroupOrderChangedObserver
     */
    void NotifyControlGroupOrderChanged()
        {
        iNoOfCalls++;
        }
    
    /**
     * Accessor to the number of calls to the callback method
     */
    TInt NumberOfCalls() const
        {
        return iNoOfCalls;
        }
    
private:
    /** Roster to which this callback object is added to. */
    CAlfRoster& iRoster;
    
    /**
     * Number of calls to the callback method
     */
    TInt iNoOfCalls;
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterCtrlGroupMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterCtrlGroupMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterCtrlGroupMethodsL, "TestRosterCtrlGroupMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterCtrlGroupMethodsL );
    // Print to log file
    iLog->Log( KTestRosterCtrlGroupMethodsL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    
    CAlfControlGroup& group1 = iAlfEnv->NewControlGroupL( 2 );
    CAlfControlGroup& group2 = iAlfEnv->NewControlGroupL( KNum3 );
    CAlfControlGroup& group3 = iAlfEnv->NewControlGroupL( KNum4 );
    CAlfControlGroup& group4 = iAlfEnv->NewControlGroupL( KNum5 );
    
    STIF_ASSERT_EQUALS( KErrNotFound, roster->Find( group1 ) );
    STIF_ASSERT_EQUALS( 0, roster->Count() );
    
    // Create controls and append them to control group 1
    CAlfControl* control = new (ELeave) CAlfControl;
    CleanupStack::PushL( control );
    control->ConstructL( *iAlfEnv );
    group1.AppendL( control );
    CleanupStack::Pop( control );
    CAlfControl* control1 = new (ELeave) CAlfControl;
    CleanupStack::PushL( control1 );
    control1->ConstructL( *iAlfEnv );
    group1.AppendL( control1 );
    CleanupStack::Pop( control1 );
    
    roster->ShowL( group1 );
    STIF_ASSERT_EQUALS( 1, roster->Count() );
    roster->ShowL( group2 );
    STIF_ASSERT_EQUALS( 2, roster->Count() );
    roster->ShowL( group3 );
    STIF_ASSERT_EQUALS( KNum3, roster->Count() );
    roster->ShowL( group4 );
    STIF_ASSERT_EQUALS( KNum4, roster->Count() );
    
    STIF_ASSERT_EQUALS( 0, roster->Find( group1 ) );
    STIF_ASSERT_EQUALS( 1, roster->Find( group2 ) );
    STIF_ASSERT_EQUALS( 2, roster->Find( group3 ) );
    STIF_ASSERT_EQUALS( KNum3, roster->Find( group4 ) );
    
    if ( roster->Count() > 0 )
        {
        STIF_ASSERT_EQUALS( &group1, &roster->ControlGroup( 0 ) );
        }
    
    // Verify that the controls are shown
    STIF_ASSERT_EQUALS( iAlfDisplay, control->Display() );
    STIF_ASSERT_EQUALS( iAlfDisplay, control1->Display() );
    
    roster->ShowL( group3, KAlfRosterShowAtBottom );
    
    STIF_ASSERT_EQUALS( 0, roster->Find( group3 ) );
    STIF_ASSERT_EQUALS( 1, roster->Find( group1 ) );
    STIF_ASSERT_EQUALS( 2, roster->Find( group2 ) ); 
    STIF_ASSERT_EQUALS( KNum3, roster->Find( group4 ) );
    
    if ( roster->Count() > 0 )
        {
        STIF_ASSERT_EQUALS( &group3, &roster->ControlGroup( 0 ) );
        }
    
    roster->ShowL( group3, KNum3 );
     
    STIF_ASSERT_EQUALS( 0, roster->Find( group1 ) );
    STIF_ASSERT_EQUALS( 1, roster->Find( group2 ) ); 
    STIF_ASSERT_EQUALS( 2, roster->Find( group4 ) );
    STIF_ASSERT_EQUALS( KNum3, roster->Find( group3 ) );
    
    if ( roster->Count() > 0 )
        {
        STIF_ASSERT_EQUALS( &group1, &roster->ControlGroup( 0 ) );
        }
    
    // Focus the appended control
    roster->SetFocus( *control );
    STIF_ASSERT_TRUE( control->Focus() );
    
    roster->Hide( group1 );
    STIF_ASSERT_EQUALS( KErrNotFound, roster->Find( group1 ) );
    STIF_ASSERT_EQUALS( KNum3, roster->Count() );
    roster->Hide( group2 );
    STIF_ASSERT_EQUALS( 2, roster->Count() );
    roster->Hide( group3 );
    STIF_ASSERT_EQUALS( 1, roster->Count() );
    roster->Hide( group4 );
    STIF_ASSERT_EQUALS( 0, roster->Count() );
    
    // Verify that focus was removed from control and 
    // that the control was hidden
    STIF_ASSERT_FALSE( control->Focus() );
    STIF_ASSERT_NULL( control->Display() );
    
    // Add a control to a hidden control group
    // Create controls and append them to iAlfCtrlGroup
    CAlfControl* control2 = new (ELeave) CAlfControl;
    CleanupStack::PushL( control2 );
    control2->ConstructL( *iAlfEnv );
    group1.AppendL( control2 );
    CleanupStack::Pop( control2 );
    
    // Verify that the control2 is not shown
    STIF_ASSERT_NULL( control2->Display() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterCtrlMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterCtrlMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterCtrlMethodsL, "TestRosterCtrlMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterCtrlMethodsL );
    // Print to log file
    iLog->Log( KTestRosterCtrlMethodsL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    STIF_ASSERT_NULL( roster->FindControl( KControlId ) );
    iAlfCtl->SetId( KControlId );
    roster->ShowL( *iAlfCtrlGroup );
    roster->SetFocus( *iAlfCtl );
    STIF_ASSERT_TRUE( iAlfCtl->Focus() );
    STIF_ASSERT_EQUALS( iAlfCtl, roster->FindControl( KControlId ) );
    roster->ClearFocus();
    roster->Hide( *iAlfCtrlGroup );
    STIF_ASSERT_FALSE( iAlfCtl->Focus() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterVisualMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterVisualMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterVisualMethodsL, "TestRosterVisualMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterVisualMethodsL );
    // Print to log file
    iLog->Log( KTestRosterVisualMethodsL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    
    CAlfLayout* layout = CAlfLayout::AddNewL( *iAlfCtl );
    
    roster->ShowL( *iAlfCtrlGroup );
    
    TInt err = KErrNone;
//    TRAP( err, roster->ShowVisualL( *layout ) );
//    roster->MoveVisualToFront( *layout );
    roster->HideVisual( *layout );
    if ( err != KErrNone )
        {
        return KErrNone;
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterPointerEventObserverMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterPointerEventObserverMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterPointerEventObserverMethodsL, "TestRosterPointerEventObserverMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterPointerEventObserverMethodsL );
    // Print to log file
    iLog->Log( KTestRosterPointerEventObserverMethodsL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    
    STIF_ASSERT_EQUALS( KErrNone,
            roster->AddPointerEventObserver( EAlfPointerEventReportDrag, *iAlfCtl ) );
    STIF_ASSERT_EQUALS( KErrNone,
            roster->AddPointerEventObserver( EAlfPointerEventReportLongTap, *iAlfCtl ) );
    STIF_ASSERT_EQUALS( KErrNone,
            roster->AddPointerEventObserver( EAlfPointerEventReportUnhandled, *iAlfCtl ) );
    
    STIF_ASSERT_EQUALS( KErrNone,
            roster->RemovePointerEventObserver( EAlfPointerEventReportDrag, *iAlfCtl ) );
    STIF_ASSERT_EQUALS( KErrNone,
            roster->RemovePointerEventObserver( EAlfPointerEventReportLongTap, *iAlfCtl ) );
    STIF_ASSERT_EQUALS( KErrNone,
            roster->RemovePointerEventObserver( EAlfPointerEventReportUnhandled, *iAlfCtl ) );
    
    STIF_ASSERT_EQUALS( KErrNone,
            roster->SetPointerEventObservers( EAlfPointerEventReportDrag, *iAlfCtl ) );
    STIF_ASSERT_EQUALS(
            KErrNone,
            roster->SetPointerEventObservers(
                    EAlfPointerEventReportDrag | EAlfPointerEventReportUnhandled,
                    *iAlfCtl
                    )
            );
    STIF_ASSERT_EQUALS( KErrNone,
            roster->SetPointerEventObservers( EAlfPointerEventReportLongTap, *iAlfCtl ) );
    STIF_ASSERT_EQUALS( KErrNone
            , roster->SetPointerEventObservers( 0, *iAlfCtl ) );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterSetPointerDragThresholdL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterSetPointerDragThresholdL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterSetPointerDragThresholdL, "TestRosterSetPointerDragThresholdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterSetPointerDragThresholdL );
    // Print to log file
    iLog->Log( KTestRosterSetPointerDragThresholdL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    
    STIF_ASSERT_EQUALS( KErrNone,
            roster->SetPointerEventObservers( EAlfPointerEventReportDrag, *iAlfCtl ) );
    STIF_ASSERT_EQUALS(
            KErrNone,
            roster->SetPointerDragThreshold(
                    *iAlfCtl,
                    TAlfXYMetric( KMagnitude, KMagnitude )
                    )
            );
    STIF_ASSERT_EQUALS(
            KErrNone,
            roster->SetPointerDragThreshold(
                    *iAlfCtl,
                    TAlfXYMetric(
                            TAlfMetric( KMagnitude, EAlfUnitS60 ),
                            TAlfMetric( 0.2, EAlfUnitRelativeToDisplay )
                            )
                    )
            );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterDisableLongTapEventsWhenDraggingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterDisableLongTapEventsWhenDraggingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterDisableLongTapEventsWhenDraggingL, "TestRosterDisableLongTapEventsWhenDraggingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterDisableLongTapEventsWhenDraggingL );
    // Print to log file
    iLog->Log( KTestRosterDisableLongTapEventsWhenDraggingL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    
    STIF_ASSERT_EQUALS( KErrNone,
            roster->SetPointerEventObservers( EAlfPointerEventReportDrag, *iAlfCtl ) );
    STIF_ASSERT_EQUALS( KErrNone,
            roster->DisableLongTapEventsWhenDragging( *iAlfCtl ));
    STIF_ASSERT_EQUALS( KErrNone,
            roster->DisableLongTapEventsWhenDragging( *iAlfCtl, EFalse ) );
    
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRosterCtrlGroupOrderChangedObserverL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRosterCtrlGroupOrderChangedObserverL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRosterCtrlGroupOrderChangedObserverL, "TestRosterCtrlGroupOrderChangedObserverL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRosterCtrlGroupOrderChangedObserverL );
    // Print to log file
    iLog->Log( KTestRosterCtrlGroupOrderChangedObserverL );
    
    CAlfRoster* roster = &( iAlfDisplay->Roster() );
    STIF_ASSERT_NOT_NULL( roster );
    // Create test control groups
    CAlfControlGroup& group2 = iAlfEnv->NewControlGroupL( 2 );
    CAlfControlGroup& group3 = iAlfEnv->NewControlGroupL( KNum3 );
    
    // Show first control group [Roster: cg1]
    roster->ShowL( *iAlfCtrlGroup );
    
    // Create an observer
    CTestControlGroupOrderChangedObserver* observer =
                new ( ELeave ) CTestControlGroupOrderChangedObserver( *roster );
    CleanupStack::PushL( observer );
    observer->ConstructL();
    
    // Show second control group [Roster: cg2, cg1]
    roster->ShowL( group2 );
              
    // Remove observer
    roster->RemoveControlGroupOrderChangedObserver( *observer );
    
    // Swap order of control group 1 and 2 [Roster: cg1, cg2]
    roster->ShowL( *iAlfCtrlGroup );
    
    // Re-add observer
    roster->AddControlGroupOrderChangedObserverL( *observer );
    
    // Reswap order of control group 1 and 2 [Roster: cg2, cg1]
    roster->ShowL( group2 );

    // Don't modify the order of control groups [Roster: cg2, cg1]
    roster->ShowL( group2 );

    // Delete control group 2 [Roster: cg1]
    iAlfEnv->DeleteControlGroup( 2 );
    
    // Show new control group [Roster: cg3, cg1]
    roster->ShowL( group3 );
    
    // Hide old control group [Roster: cg3]
    roster->Hide( *iAlfCtrlGroup );
    
    // Hide hidden control group [Roster: cg3]
    roster->Hide( *iAlfCtrlGroup );

    // Hide last control group [Roster: ]
    roster->Hide( group3 );
    
    CleanupStack::PopAndDestroy( observer );
    
    // Verify that control group order change does not panic
    roster->ShowL( *iAlfCtrlGroup );
    
    return KErrNone;
    }


//  [End of File]
