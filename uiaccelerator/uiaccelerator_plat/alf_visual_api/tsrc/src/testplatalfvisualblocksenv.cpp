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
#include <alf/alfbatchbuffer.h>
#include "testplatalfvisual.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvNewFuncsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvNewFuncsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvNewFuncsL, "TestAlfEnvNewFuncsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvNewFuncsL );
    // Print to log file
    iLog->Log( KTestAlfEnvNewFuncsL );
    
    STIF_ASSERT_NOT_NULL( iAlfEnv );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvClient
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvClient( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvClient, "TestAlfEnvClient" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvClient );
    // Print to log file
    iLog->Log( KTestAlfEnvClient );
    
    iAlfEnv->Client();
    iAlfEnv->TokenClient();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvNewDisplayL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvNewDisplayL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvNewDisplayL, "TestAlfEnvNewDisplayL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvNewDisplayL );
    // Print to log file
    iLog->Log( KTestAlfEnvNewDisplayL );
    
    TRect vRect( 0, 0, 1, 1 );
    TUid vUid = { 0x00000001 };
    STIF_ASSERT_EQUALS( 1, iAlfEnv->DisplayCount() );
    STIF_ASSERT_SAME( iAlfDisplay, &iAlfEnv->PrimaryDisplay() );
    TRAPD( err, iAlfEnv->NewDisplayL( vRect, CAlfEnv::ENewDisplayAsCoeControl, NULL,
            CAlfDisplay::EDisplayNormal, vUid ) );
    if ( err != KErrNone )
        {
        return KErrNone;
        }
    else
        {
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvCtrlGroupL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvCtrlGroupL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvCtrlGroupL, "TestAlfEnvCtrlGroupL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvCtrlGroupL );
    // Print to log file
    iLog->Log( KTestAlfEnvCtrlGroupL );
    const TInt groupName = 23;
    CAlfControlGroup& ctrlgroup = iAlfEnv->NewControlGroupL( groupName );
    iAlfEnv->ControlGroup( ctrlgroup.ResourceId() );
    CAlfControlGroup* group = iAlfEnv->FindControlGroup( groupName );
    STIF_ASSERT_NOT_EQUALS( 0, (TInt)group );
    iAlfEnv->DeleteControlGroup( groupName );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvProcessFresh
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvProcessFresh( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvProcessFresh, "TestAlfEnvProcessFresh" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvProcessFresh );
    // Print to log file
    iLog->Log( KTestAlfEnvProcessFresh );
    
    TAlfRefreshMode mode = EAlfRefreshModeManual;
    iAlfEnv->SetRefreshMode(mode);
    mode = EAlfRefreshModeAutomatic;
    iAlfEnv->SetRefreshMode(mode);
    mode = EAlfRefreshModeForced;
    iAlfEnv->SetRefreshMode(mode);
    
    iAlfEnv->RefreshMode();
    
    TReal32 framerate = 0.0;
    iAlfEnv->SetMaxFrameRate(framerate);
    framerate = 1000.0;
    iAlfEnv->SetMaxFrameRate(framerate);
    framerate = 0.0;
    iAlfEnv->SetMaxFrameRate(framerate);
    iAlfEnv->ContinueRefresh();
    iAlfEnv->PauseRefresh();
    CAlfEnv::RefreshCallBack( iAlfEnv );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvProcessTextureManagerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvProcessTextureManagerL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvProcessTextureManagerL, "TestAlfEnvProcessTextureManagerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvProcessTextureManagerL );
    // Print to log file
    iLog->Log( KTestAlfEnvProcessTextureManagerL );
    
    TUid vUid = { 0x00000002 };
    iAlfEnv->CreateSharedTextureManagerL( vUid );
    CAlfTextureManager* found1 = iAlfEnv->SharedTextureManager( vUid );
    STIF_ASSERT_NOT_EQUALS( 0 , (TInt)found1);
    iAlfEnv->TextureManager();
    iAlfEnv->TextStyleManager();
    iAlfEnv->DeleteSharedTextureManager( vUid );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvObserverL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvObserverL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvObserverL, "TestAlfEnvObserverL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvObserverL );
    // Print to log file
    iLog->Log( KTestAlfEnvObserverL );
    
    iAlfEnv->AddActionObserverL( NULL );
    iAlfEnv->RemoveActionObserver( NULL );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvProcessCmd
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvProcessCmd( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvProcessCmd, "TestAlfEnvProcessCmd" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvProcessCmd );
    // Print to log file
    iLog->Log( KTestAlfEnvProcessCmd );
    const TInt cmdnum = 23;
    const TInt num = 1000000;
    TAlfCustomEventCommand command( cmdnum );
    iAlfEnv->Send(command);
    iAlfEnv->Send( command, num );
    iAlfEnv->CancelCommands( &command );
    iAlfEnv->CancelCommands( NULL );
    iAlfEnv->Send( command, num );
    iAlfEnv->CancelCommands( &command, EAlfOpNone );
    iAlfEnv->Send( command, num );
    iAlfEnv->CancelCommands( &command, EAlfCommandTypeNone );
    iAlfEnv->Send( command, num );
    iAlfEnv->CancelCustomCommands( NULL );
    iAlfEnv->Send( command, num );
    iAlfEnv->CancelCustomCommands( NULL, cmdnum );
    iAlfEnv->Send( command, num );
    iAlfEnv->MilliSecondsUntilCommand( &command );
    iAlfEnv->MilliSecondsUntilCommand( &command, EAlfOpNone );
    iAlfEnv->MilliSecondsUntilCommand( &command, EAlfCommandTypeNone );
    iAlfEnv->MilliSecondsUntilCustomCommand( NULL );
    iAlfEnv->MilliSecondsUntilCustomCommand( NULL, 0 );
    
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvKeyAndPointerEventL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvKeyAndPointerEventL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvKeyAndPointerEventL, "TestAlfEnvKeyAndPointerEventL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvKeyAndPointerEventL );
    // Print to log file
    iLog->Log( KTestAlfEnvKeyAndPointerEventL );
    
    TKeyEvent vEvent;
    TPointerEvent vPointEvent;
    iAlfEnv->HandleKeyEventL( vEvent, EEventKeyDown );
    iAlfEnv->HandlePointerEventL( vPointEvent, *iAlfDisplay );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvNotifyL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvNotifyL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvNotifyL, "TestAlfEnvNotifyL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvNotifyL );
    // Print to log file
    iLog->Log( KTestAlfEnvNotifyL );
    
    iAlfEnv->NotifySkinChangedL();
    iAlfEnv->NotifyLayoutChangedL();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvProcessWidgetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvProcessWidgetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvProcessWidgetL, "TestAlfEnvProcessWidgetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvProcessWidgetL );
    // Print to log file
    iLog->Log( KTestAlfEnvProcessWidgetL );
    iAlfEnv->WidgetFactory();
    iAlfEnv->AppendWidgetL( NULL );
    char* p = "test";
    iAlfEnv->FindWidget( p );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvGetAttributes
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvGetAttributes( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvGetAttributes, "TestAlfEnvGetAttributes" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvGetAttributes );
    // Print to log file
    iLog->Log( KTestAlfEnvGetAttributes );

    iAlfEnv->LayoutMetricsUtility();
    iAlfEnv->Renderer();
    iAlfEnv->LastReceivedPtrEvent();
    CAlfBatchBuffer& batch = iAlfEnv->BatchBufferHandler();
    const TAlfAutoFlushMode defaultFlushMode = batch.AutoFlushMode();
    STIF_ASSERT_EQUALS( defaultFlushMode, EAlfAutoFlushDeferred );
    iAlfEnv->Version();
    iAlfEnv->Extension( 0 );
    iAlfEnv->Display( 0 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvFindControl
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvFindControl( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvFindControl, "TestAlfEnvFindControl" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvFindControl );
    // Print to log file
    iLog->Log( KTestAlfEnvFindControl );

    iAlfCtl->SetId( 1 );
    CAlfControl* vTemp = iAlfEnv->FindControl( 1 );
    STIF_ASSERT_SAME( iAlfCtl, vTemp );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvFindDisplayIndex
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvFindDisplayIndex( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvFindDisplayIndex, "TestAlfEnvFindDisplayIndex" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvFindDisplayIndex );
    // Print to log file
    iLog->Log( KTestAlfEnvFindDisplayIndex );

    TInt vIndex = iAlfEnv->FindDisplayIndex( *iAlfDisplay );
    STIF_ASSERT_EQUALS( 0, vIndex );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvAddExtension
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvAddExtension( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvAddExtension, "TestAlfEnvAddExtension" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvAddExtension );
    // Print to log file
    iLog->Log( KTestAlfEnvAddExtension );

    iAlfEnv->AddExtension( 0, NULL );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvSetAndReSetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvSetAndReSetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvSetAndReSetL, "TestAlfEnvSetAndReSetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvSetAndReSetL );
    // Print to log file
    iLog->Log( KTestAlfEnvSetAndReSetL );

    const TInt num = 1000000;
    iAlfEnv->SetIdleThreshold( num );
    iAlfEnv->Release();
    iAlfEnv->RestoreL();
    iAlfEnv->SetFullScreenDrawing( ETrue );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEnvStatic
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEnvStatic( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEnvStatic, "TestAlfEnvStatic" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEnvStatic );
    // Print to log file
    iLog->Log( KTestAlfEnvStatic );
    
    CAlfEnv* vTemp = CAlfEnv::Static();
    STIF_ASSERT_SAME( iAlfEnv, vTemp );
    vTemp = NULL;
    
    return KErrNone;
    }

// End of file

