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
* Description:  test functions for alfsymbiansbdrawer.h
*
*/



// [INCLUDE FILES]
#include <alf/alfsymbiansbdrawer.h>
#include <alf/alfscreenbuffer.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KTestDisplayPosX = 20;
const TInt KTestDisplayPosY = 10;
const TInt KTestDisplayWidth = 100;
const TInt KTestDisplayHeight = 200;

const TUid KTestScreenBufferUid = { 0x28032002 };


// FORWARD DECLARATION
//class CTestScreenBufferObserver for testing CAlfScreenBuffer
class CTestScreenBufferObserver : public CBase, public MAlfScreenBufferObserver
    {
public:
    /**
     * From MAlfScreenBufferObserver
     */
    TBool BufferComplete( TUid /*aBufferUid*/, TRect& /*aDisplayRect*/, TRect& /*aDirtyRect*/ )
        {
        return ETrue;
        }
    
    /**
     * From MAlfScreenBufferObserver
     */
    void HandleScreenBufferEvent( TUid /*aBufferUid*/, TInt /*aEvent*/ ) {}
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBufDrawerDrawBufferL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestBufDrawerDrawBufferL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBufDrawerDrawBufferL, "TestBufDrawerDrawBufferL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBufDrawerDrawBufferL );
    // Print to log file
    iLog->Log( KTestBufDrawerDrawBufferL );
    
    TRect dispRect = TRect(
                        TPoint( KTestDisplayPosX, KTestDisplayPosY ),
                        TSize( KTestDisplayWidth, KTestDisplayHeight )
                        );
    TRAPD( trapError,
           iAlfEnv->NewDisplayL(
                        dispRect, 0, NULL,
                        CAlfDisplay::EDisplayOffScreenBuffer,
                        KTestScreenBufferUid ) );
    // CAlfDisplay::EDisplayOffScreenBuffer is not supported for new display.
    STIF_ASSERT_EQUALS( KErrNotSupported, trapError );
/** Deprecated test
    CAlfDisplay* alfDisp = &( iAlfEnv->NewDisplayL(
                            dispRect, 0, NULL,
                            CAlfDisplay::EDisplayOffScreenBuffer,
                            KTestScreenBufferUid ) );
    STIF_ASSERT_NOT_NULL( alfDisp );
    STIF_ASSERT_EQUALS( EAlfRendererBitgdi, iAlfEnv->Renderer() );
    CTestScreenBufferObserver* observer = new ( ELeave ) CTestScreenBufferObserver;
    CleanupStack::PushL( observer );
    CAlfScreenBuffer* scrBuffer = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuffer );
    scrBuffer->AddObserverL( KTestScreenBufferUid, observer );
    CAlfSymbianBufferDrawer* drawer =
        ( CAlfSymbianBufferDrawer* ) scrBuffer->GetDrawingInterface(
                KAlfSymbianBufferDrawerUid, KTestScreenBufferUid );
    STIF_ASSERT_NOT_NULL( drawer );
    
    // create a bitmap, device and context to be used off-screen
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    TRect visibleRect = alfDisp->VisibleArea();
    User::LeaveIfError( bitmap->Create( visibleRect.Size(), EColor64K ) );
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bitmap );
    CleanupStack::PushL( device );
    CFbsBitGc* context = NULL;
    User::LeaveIfError( device->CreateContext( context ) ); 
    CleanupStack::PushL( context );

    // Draw
    drawer->DrawBuffer( *context, TPoint(), visibleRect );
    
    // Draw masked
    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( mask );
    User::LeaveIfError( mask->Create( visibleRect.Size(), EColor256 ) );
    drawer->DrawBufferMasked( *context, mask, TPoint(), visibleRect );
    
    CleanupStack::PopAndDestroy( mask );
    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( device );
    CleanupStack::PopAndDestroy( bitmap );
    CleanupStack::PopAndDestroy( scrBuffer );
    CleanupStack::PopAndDestroy( observer );
*/    
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBufDrawerGetBufferBitmapsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestBufDrawerGetBufferBitmapsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBufDrawerGetBufferBitmapsL, "TestBufDrawerGetBufferBitmapsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBufDrawerGetBufferBitmapsL );
    // Print to log file
    iLog->Log( KTestBufDrawerGetBufferBitmapsL );
    
    TRect dispRect = TRect(
                        TPoint( KTestDisplayPosX, KTestDisplayPosY ),
                        TSize( KTestDisplayWidth, KTestDisplayHeight )
                        );
    TRAPD( trapError,
           iAlfEnv->NewDisplayL(
                        dispRect, 0, NULL,
                        CAlfDisplay::EDisplayOffScreenBuffer,
                        KTestScreenBufferUid ) );
    // CAlfDisplay::EDisplayOffScreenBuffer is not supported for new display.
    STIF_ASSERT_EQUALS( KErrNotSupported, trapError );
/** Deprecated test    
    CAlfDisplay* alfDisp = &( iAlfEnv->NewDisplayL(
                            dispRect, 0, NULL,
                            CAlfDisplay::EDisplayOffScreenBuffer,
                            KTestScreenBufferUid ) );
    STIF_ASSERT_NOT_NULL( alfDisp );
    STIF_ASSERT_EQUALS( EAlfRendererBitgdi, iAlfEnv->Renderer() );
    CTestScreenBufferObserver* observer = new ( ELeave ) CTestScreenBufferObserver;
    CleanupStack::PushL( observer );
    CAlfScreenBuffer* scrBuffer = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuffer );
    scrBuffer->AddObserverL( KTestScreenBufferUid, observer );
    CAlfSymbianBufferDrawer* drawer =
        ( CAlfSymbianBufferDrawer* ) scrBuffer->GetDrawingInterface(
                KAlfSymbianBufferDrawerUid, KTestScreenBufferUid );
    STIF_ASSERT_NOT_NULL( drawer );
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    drawer->GetBufferBitmaps( bitmap, mask );
    CleanupStack::PopAndDestroy( scrBuffer );
    CleanupStack::PopAndDestroy( observer );
*/
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBufDrawerReleaseDrawerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestBufDrawerReleaseDrawerL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBufDrawerReleaseDrawerL, "TestBufDrawerReleaseDrawerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBufDrawerReleaseDrawerL );
    // Print to log file
    iLog->Log( KTestBufDrawerReleaseDrawerL );
    
    TRect dispRect = TRect(
                        TPoint( KTestDisplayPosX, KTestDisplayPosY ),
                        TSize( KTestDisplayWidth, KTestDisplayHeight )
                        );
    TRAPD( trapError,
           iAlfEnv->NewDisplayL(
                        dispRect, 0, NULL,
                        CAlfDisplay::EDisplayOffScreenBuffer,
                        KTestScreenBufferUid ) );
    // CAlfDisplay::EDisplayOffScreenBuffer is not supported for new display.
    STIF_ASSERT_EQUALS( KErrNotSupported, trapError );
/** Deprecated test    
    CAlfDisplay* alfDisp = &( iAlfEnv->NewDisplayL(
                            dispRect, 0, NULL,
                            CAlfDisplay::EDisplayOffScreenBuffer,
                            KTestScreenBufferUid ) );
    STIF_ASSERT_NOT_NULL( alfDisp );
    STIF_ASSERT_EQUALS( EAlfRendererBitgdi, iAlfEnv->Renderer() );
    CTestScreenBufferObserver* observer = new ( ELeave ) CTestScreenBufferObserver;
    CleanupStack::PushL( observer );
    CAlfScreenBuffer* scrBuffer = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuffer );
    scrBuffer->AddObserverL( KTestScreenBufferUid, observer );
    CAlfSymbianBufferDrawer* drawer =
        ( CAlfSymbianBufferDrawer* ) scrBuffer->GetDrawingInterface(
                KAlfSymbianBufferDrawerUid, KTestScreenBufferUid );
    STIF_ASSERT_NOT_NULL( drawer );
    drawer->ReleaseDrawer();
    drawer = NULL;
    CleanupStack::PopAndDestroy( scrBuffer );
    CleanupStack::PopAndDestroy( observer );
*/
    return KErrNone;
    }


//  [End of File]
