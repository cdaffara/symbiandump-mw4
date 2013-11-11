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
* Description:  test functions for alfvisual.h
*
*/



// [INCLUDE FILES]
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>

#include "testplatalfvisual.h"

// CONSTANTS
_LIT8( KVisualTag, "visual" );
const TInt KBufSize = 64;

// FORWARD DECLARATION
class CTestCAlfControl : public CAlfControl
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfControl();
    
    /**
    * Destructor.
    */
    ~CTestCAlfControl();
    
    /**
     * It test protected function NotifyControlVisibility of CAlfControl
     * @param aIsVisible  ETrue, if the control is now visible on the display.
     *                    EFalse, if the control is about to the hidden on the display.
     * @param aDisplay    The display on which the control's visibility is changing.
     * return symbian error code
     */
    TInt TestNotifyControlVisibility( TBool aIsVisible, CAlfDisplay& aDisplay );
    
    /**
     * It test protected function VisualAddedL of CAlfControl
     * @param aVisual  Visual that was added.
     * return symbian error code
     */
    TInt TestVisualAddedL( CAlfVisual* aVisual );
    
    /**
     * It test protected function VisualRemoved of CAlfControl
     * @param aVisual  Visual that was removed.
     * return symbian error code
     */
    TInt TestVisualRemoved( CAlfVisual* aVisual );
    
    /**
     * It test protected function ConnectionAddedL of CAlfControl
     * @param aConnectedControl Connected child control
     * @parar aRole Role of the child control.
     * return symbian error code
     */
    TInt TestConnectionAddedL( CAlfControl* aConnectedControl, 
                                            TInt aRole );
    
    /**
     * It test protected function ConnectionRemoved of CAlfControl
     * @param aConnectedControl Removed child control
     * @parar aRole Role of the child control.
     * return symbian error code
     */
    TInt TestConnectionRemoved( CAlfControl* aConnectedControl, 
                                             TInt aRole );
    
    /**
     * It test protected function HostChangingL of CAlfControl
     * @param aNewHost New host control. If NULL, the host is removed.
     * return symbian error code
     */
    TInt TestHostChangingL( CAlfControl* aNewHost );
    
    /*
     * It test protected function VisualAddedL of CAlfControl
     * @param aExtensionUid extension uid
     * @parar aExtensionParams extension parameters
     * return symbian error code
     * */
    TInt TestPropertyOwnerExtension(const TUid& aExtensionUid, 
            TAny** aExtensionParams);
    
    };

// -----------------------------------------------------------------------------
// CTestCAlfControl::CTestCAlfControl
// -----------------------------------------------------------------------------
//
CTestCAlfControl::CTestCAlfControl(){}
// -----------------------------------------------------------------------------
// CTestCAlfControl::~CTestCAlfControl
// -----------------------------------------------------------------------------
//
CTestCAlfControl::~CTestCAlfControl(){}
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestNotifyControlVisibility
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestNotifyControlVisibility( TBool aIsVisible, 
        CAlfDisplay& aDisplay )
    {
    NotifyControlVisibility( aIsVisible, aDisplay );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestVisualAddedL
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestVisualAddedL( CAlfVisual* aVisual )
    {
    VisualAddedL( aVisual );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestVisualRemoved
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestVisualRemoved( CAlfVisual* aVisual )
    {
    VisualRemoved( aVisual );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestConnectionAddedL
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestConnectionAddedL( CAlfControl* aConnectedControl, 
                                            TInt aRole )
    {
    ConnectionAddedL( aConnectedControl, aRole );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestConnectionRemoved
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestConnectionRemoved( CAlfControl* aConnectedControl, 
                                             TInt aRole )
    {
    ConnectionRemoved( aConnectedControl, aRole );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestHostChangingL
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestHostChangingL( CAlfControl* aNewHost )
    {
    HostChangingL( aNewHost );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestCAlfControl::TestPropertyOwnerExtension
// -----------------------------------------------------------------------------
//
TInt CTestCAlfControl::TestPropertyOwnerExtension(const TUid& aExtensionUid, 
            TAny** aExtensionParams)
    {
    PropertyOwnerExtension( aExtensionUid, aExtensionParams );
    return KErrNone;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlConstructorL, "TestAlfCtrlConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlConstructorL );
    // Print to log file
    iLog->Log( KTestAlfCtrlConstructorL );
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    STIF_ASSERT_NOT_NULL( ctrl );
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlConstructL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlConstructL, "TestAlfCtrlConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlConstructL );
    // Print to log file
    iLog->Log( KTestAlfCtrlConstructL );
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlIdL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlIdL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlIdL, "TestAlfCtrlIdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlIdL );
    // Print to log file
    iLog->Log( KTestAlfCtrlIdL );
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    ctrl->SetId( 1 );
    TInt vId = ctrl->Id();
    STIF_ASSERT_EQUALS( 1, vId );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlGetCtrlMember
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlGetCtrlMember( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlGetCtrlMember, "TestAlfCtrlGetCtrlMember" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlGetCtrlMember );
    // Print to log file
    iLog->Log( KTestAlfCtrlGetCtrlMember );
    CAlfEnv* env = &( iAlfCtl->Env() );
    STIF_ASSERT_SAME( iAlfEnv, env );
    
    CAlfControlGroup* group = iAlfCtl->ControlGroup();
    STIF_ASSERT_SAME( iAlfCtrlGroup, group );
    
    CAlfDisplay* display = iAlfCtl->Display();
    STIF_ASSERT_SAME( iAlfDisplay, display );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlProcessVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlProcessVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlProcessVisualL, "TestAlfCtrlProcessVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlProcessVisualL );
    // Print to log file
    iLog->Log( KTestAlfCtrlProcessVisualL );
    CAlfVisual* vVisual = new ( ELeave ) CAlfVisual();
    CleanupStack::PushL( vVisual );
    vVisual->ConstructL( *iAlfCtl );
    TInt err = iAlfCtl->Append( vVisual );
    if ( err != KErrNone )
        {
        CleanupStack::PopAndDestroy( vVisual );
        return err;
        }
    CleanupStack::Pop( vVisual );
    TBuf8<KBufSize> vBuf(KVisualTag);
    vVisual->SetTagL( vBuf );
    STIF_ASSERT_EQUALS( 1, iAlfCtl->VisualCount() );
    STIF_ASSERT_NOT_NULL( iAlfCtl->FindTag( vBuf ) );
    STIF_ASSERT_SAME( vVisual, &( iAlfCtl->Visual( 0 ) ) );
    
    iAlfCtl->VisualLayoutUpdated( *vVisual );
    iAlfCtl->VisualPrepareDrawFailed( *vVisual, -1 );
    iAlfCtl->VisualDestroyed( *vVisual );
    
    iAlfCtl->Remove( vVisual );
    STIF_ASSERT_EQUALS( 0, iAlfCtl->VisualCount() );
    iAlfCtl->AppendVisualL( EAlfVisualTypeText );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlProcessConnectL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlProcessConnectL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlProcessConnectL, "TestAlfCtrlProcessConnectL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlProcessConnectL );
    // Print to log file
    iLog->Log( KTestAlfCtrlProcessConnectL );
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    STIF_ASSERT_EQUALS( 0, (TInt)ctrl->Host() );
    const TInt KAlfRoleSlave = 234;
    iAlfCtl->AddConnectionL( ctrl, KAlfRoleSlave );
    STIF_ASSERT_EQUALS( (TInt)iAlfCtl, (TInt)ctrl->Host() );
    STIF_ASSERT_EQUALS( 0, iAlfCtl->FindConnection( ctrl ) );
    STIF_ASSERT_EQUALS( 1, iAlfCtl->ConnectionCount() );
    STIF_ASSERT_EQUALS( (TInt)ctrl, (TInt)&iAlfCtl->Connection( 0 ) );
    iAlfCtl->ConnectionByOrdinal( 0 );
    STIF_ASSERT_EQUALS( KAlfRoleSlave, iAlfCtl->ConnectionRole( 0 ) );
    iAlfCtl->ConnectionOrdinal( 0 );
    
    iAlfCtl->RemoveConnection( ctrl );
    STIF_ASSERT_EQUALS( 0, iAlfCtl->ConnectionCount() );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlProcessRole
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlProcessRole( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlProcessRoleL, "TestAlfCtrlProcessRoleL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlProcessRoleL );
    // Print to log file
    iLog->Log( KTestAlfCtrlProcessRoleL );
    iAlfCtl->SetRole( 1 );
    STIF_ASSERT_EQUALS( 1, iAlfCtl->Role() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlProcessHostL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlProcessHostL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlProcessHostL, "TestAlfCtrlProcessHostL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlProcessHostL );
    // Print to log file
    iLog->Log( KTestAlfCtrlProcessHostL );
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    const TInt id = 123;
    ctrl->SetHostId( id );
    STIF_ASSERT_EQUALS( id, ctrl->HostId() );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlHostDisplayL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlHostDisplayL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlHostDisplayL, "TestAlfCtrlHostDisplayL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlHostDisplayL );
    // Print to log file
    iLog->Log( KTestAlfCtrlHostDisplayL );
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    
    const TPoint source( 23 , 34 );
    const TPoint converted = ctrl->HostToDisplay( source );
    STIF_ASSERT_EQUALS( source.iX, converted.iX );
    STIF_ASSERT_EQUALS( source.iY, converted.iY );\
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlDisplayPos
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlDisplayPos( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlDisplayPos, "TestAlfCtrlDisplayPos" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlDisplayPos );
    // Print to log file
    iLog->Log( KTestAlfCtrlDisplayPos );
    iAlfCtl->DisplayArea();
    iAlfCtl->DisplayCenter();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlEventL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlEventL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlEventL, "TestAlfCtrlEventL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlEventL );
    // Print to log file
    iLog->Log( KTestAlfCtrlEventL );
    TAlfEvent vEvent( 0 );
    iAlfCtl->AcceptInput();
    iAlfCtl->OfferEventL( vEvent );
    const TInt num = 23;
    TAlfCustomEventCommand event( num, iAlfCtl );
    iAlfEnv->Send( event,num );
    iAlfCtl->CancelAllCommands();
    const TPoint point( num, num );
    TBool result = iAlfCtl->HitTest( point );
    STIF_ASSERT_FALSE( result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlProcessFocus
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlProcessFocus( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlProcessFocus, "TestAlfCtrlProcessFocus" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlProcessFocus );
    // Print to log file
    iLog->Log( KTestAlfCtrlProcessFocus );
    iAlfCtl->AcquireFocus();
    iAlfCtl->RelinquishFocus();
    TBool vBool = iAlfCtl->Focus();
    STIF_ASSERT_FALSE( vBool );
    iAlfCtl->FocusedConnection();
    vBool = iAlfCtl->IsFocusing();
    STIF_ASSERT_FALSE( vBool );
    iAlfCtl->FocusChanged( *iAlfDisplay, EFalse );
    STIF_ASSERT_FALSE( iAlfCtl->IsFocusing() );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlLayoutL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlLayoutL, "TestAlfCtrlLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlLayoutL );
    // Print to log file
    iLog->Log( KTestAlfCtrlLayoutL );
    
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    iAlfCtl->AddConnectionL( ctrl, 0 );
    iAlfCtl->AppendLayoutL( EAlfLayoutTypeFlow );
    iAlfCtl->ContainerLayout( ctrl );
    STIF_ASSERT_NULL( iAlfCtl->ContainerLayout( NULL ) );
    iAlfCtl->RemoveConnection( ctrl );
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlIdentifier
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlIdentifier( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlIdentifier, "TestAlfCtrlIdentifier" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlIdentifier );
    // Print to log file
    iLog->Log( KTestAlfCtrlIdentifier );
    
    const TInt vId = iAlfCtl->Identifier();
    STIF_ASSERT_NOT_EQUALS( 0, vId );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlBounds
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlBounds( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlBounds, "TestAlfCtrlBounds" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlBounds );
    // Print to log file
    iLog->Log( KTestAlfCtrlBounds );
    
    const TRect vRect = iAlfCtl->Bounds();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlShowL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlShowL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlShowL, "TestAlfCtrlShowL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlShowL );
    // Print to log file
    iLog->Log( KTestAlfCtrlShowL );
    
    iAlfCtl->ShowL( *iAlfDisplay );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlAddAndRemoveVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlAddAndRemoveVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlAddAndRemoveVisualL, "TestAlfCtrlAddAndRemoveVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlAddAndRemoveVisualL );
    // Print to log file
    iLog->Log( KTestAlfCtrlAddAndRemoveVisualL );
    CTestCAlfControl* ctrl = new ( ELeave ) CTestCAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    CAlfVisual* vVisual = new ( ELeave ) CAlfVisual();
    CleanupStack::PushL( vVisual );
    vVisual->ConstructL( *ctrl );
    CleanupStack::Pop( vVisual );
    ctrl->TestVisualAddedL( vVisual );
    ctrl->TestVisualRemoved( vVisual );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlNotifyControlVisibilityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlNotifyControlVisibilityL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlNotifyControlVisibilityL, "TestAlfCtrlNotifyControlVisibilityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlNotifyControlVisibilityL );
    // Print to log file
    iLog->Log( KTestAlfCtrlNotifyControlVisibilityL );
    CTestCAlfControl* ctrl = new ( ELeave ) CTestCAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    ctrl->TestNotifyControlVisibility( EFalse, *iAlfDisplay );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlAddAndRemoveConnectionL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlAddAndRemoveConnectionL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlAddAndRemoveConnectionL, "TestAlfCtrlAddAndRemoveConnectionL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlAddAndRemoveConnectionL );
    // Print to log file
    iLog->Log( KTestAlfCtrlAddAndRemoveConnectionL );
    CTestCAlfControl* ctrl = new ( ELeave ) CTestCAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    ctrl->TestConnectionAddedL( iAlfCtl, 1 );
    ctrl->TestConnectionRemoved( iAlfCtl, 1 );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlHostChangingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlHostChangingL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlHostChangingL, "TestAlfCtrlHostChangingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlHostChangingL );
    // Print to log file
    iLog->Log( KTestAlfCtrlHostChangingL );
    CTestCAlfControl* ctrl = new ( ELeave ) CTestCAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    ctrl->TestHostChangingL( NULL );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlPropertyOwnerExtensionL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlPropertyOwnerExtensionL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlPropertyOwnerExtensionL, "TestAlfCtrlPropertyOwnerExtensionL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlPropertyOwnerExtensionL );
    // Print to log file
    iLog->Log( KTestAlfCtrlPropertyOwnerExtensionL );
    CTestCAlfControl* ctrl = new ( ELeave ) CTestCAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    
    TUid vUid = { 0x00000000 };
    ctrl->TestPropertyOwnerExtension( vUid, NULL );
    
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// End of file

