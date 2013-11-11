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
* Description:  test functions for alfscreenbuffer.h
*
*/



// [INCLUDE FILES]
#include <alf/alfscreenbuffer.h>
#include <alf/alfsymbiansbdrawer.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KTestDisplayPosX = 20;
const TInt KTestDisplayPosY = 10;
const TInt KTestDisplayWidth = 100;
const TInt KTestDisplayHeight = 200;

const TUid KTestScreenBufferUid = { 0x28032002 };
const TUid KDummyUid = { 0x12345678 };


// FORWARD DECLARATION
// class CTestScreenBufferObserver for testing CAlfScreenBuffer
 class CTestScreenBufferObserver : public CBase, public MAlfScreenBufferObserver
     {
 public:
     /**
      * Alf calls this when all the content have been draw to the back buffer.
      */
     TBool BufferComplete( TUid /*aBufferUid*/, TRect& /*aDisplayRect*/, TRect& /*aDirtyRect*/ )
         {
         return ETrue;
         }
     
     /**
      * Alf calls this when e.g. the back buffer is created or deleted.
      */
     void HandleScreenBufferEvent( TUid /*aBufferUid*/, TInt /*aEvent*/ ) {}
     };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestScrBufNewLMethodsL
// -----------------------------------------------------------------------------

TInt CTestPlatAlfVisual::TestScrBufNewLMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestScrBufNewLMethodsL, "TestScrBufNewLMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestScrBufNewLMethodsL );
    // Print to log file
    iLog->Log( KTestScrBufNewLMethodsL );
    
    CAlfScreenBuffer* scrBuf = NULL;
    scrBuf = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuf );
    STIF_ASSERT_NOT_NULL( scrBuf );
    CleanupStack::PopAndDestroy( scrBuf );
    
    scrBuf = CAlfScreenBuffer::NewL();
    CleanupStack::PushL( scrBuf );
    STIF_ASSERT_NOT_NULL( scrBuf );
    CleanupStack::PopAndDestroy( scrBuf );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestScrBufGetMethodsL
// -----------------------------------------------------------------------------

TInt CTestPlatAlfVisual::TestScrBufGetMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestScrBufGetMethodsL, "TestScrBufGetMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestScrBufGetMethodsL );
    // Print to log file
    iLog->Log( KTestScrBufGetMethodsL );
    
    TRect dispRect( TPoint( KTestDisplayPosX, KTestDisplayPosY ),
                    TSize( KTestDisplayWidth, KTestDisplayHeight ) );
    CAlfDisplay* display = NULL;
    TRAPD( err,
           display = &iAlfEnv->NewDisplayL(
                                   dispRect, 0, NULL,
                                   CAlfDisplay::EDisplayOffScreenBuffer,
                                   KTestScreenBufferUid
                                   )
          );
    if ( ( err == KErrNotSupported ) &&
         ( iAlfEnv->Renderer() != EAlfRendererBitgdi ) )
        {
        // Ok, as off-screen buffer should work only in BitGdi mode
            err = KErrNone;
        }
    else
        {
        User::LeaveIfError( err );
        display->SetVisibleArea( dispRect );
        }   
    
    CAlfScreenBuffer* scrBuf = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuf );
    STIF_ASSERT_NOT_NULL( scrBuf );
    
    MAlfBufferDrawer* result = scrBuf->GetDrawingInterface( KDummyUid, KDummyUid );
    STIF_ASSERT_EQUALS( result, ( MAlfBufferDrawer* )NULL );
    result = scrBuf->GetDrawingInterface( KAlfSymbianBufferDrawerUid, KDummyUid );
    STIF_ASSERT_EQUALS( result, ( MAlfBufferDrawer* )NULL );
    result = scrBuf->GetDrawingInterface( KDummyUid, KTestScreenBufferUid );
    STIF_ASSERT_EQUALS( result, ( MAlfBufferDrawer* )NULL );
    
    // *** Test with observer
    CTestScreenBufferObserver* observer = new ( ELeave ) CTestScreenBufferObserver;
    CleanupStack::PushL( observer );
    STIF_ASSERT_NOT_NULL( observer );
    
    scrBuf->AddObserverL( KTestScreenBufferUid, observer );
    result = scrBuf->GetDrawingInterface( KDummyUid, KDummyUid );
    STIF_ASSERT_EQUALS( result, ( MAlfBufferDrawer* )NULL );
    result = scrBuf->GetDrawingInterface( KAlfSymbianBufferDrawerUid, KDummyUid );
    STIF_ASSERT_EQUALS( result, ( MAlfBufferDrawer* )NULL );
    result = scrBuf->GetDrawingInterface( KDummyUid, KTestScreenBufferUid );
    STIF_ASSERT_EQUALS( result, ( MAlfBufferDrawer* )NULL );
    MAlfBufferDrawer* drawer = scrBuf->GetDrawingInterface(
                            KAlfSymbianBufferDrawerUid, KTestScreenBufferUid );
    if ( iAlfEnv->Renderer() == EAlfRendererBitgdi )
        {
        STIF_ASSERT_NOT_EQUALS( drawer, ( MAlfBufferDrawer* )NULL );
        }
    else
        {
        STIF_ASSERT_EQUALS( drawer, ( MAlfBufferDrawer* )NULL );
        }

    // Test ReleaseDrawer
    if ( drawer )
        {
        drawer->ReleaseDrawer();
        }
    drawer = NULL;
    
    // *** Test with removed observer
    scrBuf->RemoveObserver( KTestScreenBufferUid );
    drawer = scrBuf->GetDrawingInterface( KAlfSymbianBufferDrawerUid, KTestScreenBufferUid );
    STIF_ASSERT_EQUALS( drawer, ( MAlfBufferDrawer* )NULL );
    
    MAlfBufferGc* resultGc = scrBuf->GetGraphicsContext( KDummyUid, KDummyUid );
    STIF_ASSERT_EQUALS( resultGc, ( MAlfBufferGc* )NULL );
    
    CleanupStack::PopAndDestroy( observer );
    CleanupStack::PopAndDestroy( scrBuf );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestScrBufRequestMethodsL
// -----------------------------------------------------------------------------

TInt CTestPlatAlfVisual::TestScrBufRequestMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestScrBufRequestMethodsL, "TestScrBufRequestMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestScrBufRequestMethodsL );
    // Print to log file
    iLog->Log( KTestScrBufRequestMethodsL );
    
    TRect dispRect( TPoint( KTestDisplayPosX, KTestDisplayPosY ),
                    TSize( KTestDisplayWidth, KTestDisplayHeight ) );
    CAlfDisplay* display = NULL;
    TRAPD( err,
           display = &iAlfEnv->NewDisplayL(
                                   dispRect, 0, NULL,
                                   CAlfDisplay::EDisplayOffScreenBuffer,
                                   KTestScreenBufferUid
                                   )
          );
    if ( ( err == KErrNotSupported ) &&
         ( iAlfEnv->Renderer() != EAlfRendererBitgdi ) )
        {
        // Ok, as off-screen buffer should work only in BitGdi mode
            err = KErrNone;
        }
    else
        {
        User::LeaveIfError( err );
        display->SetVisibleArea( dispRect );
        }   
    
    CAlfScreenBuffer* scrBuf = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuf );
    STIF_ASSERT_NOT_NULL( scrBuf );
    
    CTestScreenBufferObserver* observer = new ( ELeave ) CTestScreenBufferObserver;
    CleanupStack::PushL( observer );
    STIF_ASSERT_NOT_NULL( observer );
    
    scrBuf->RequestNextBuffer( KTestScreenBufferUid ); 
    scrBuf->RequestNextBuffer( KDummyUid );
    
    scrBuf->AddObserverL( KTestScreenBufferUid, observer );

    // ***
    scrBuf->RequestNextBuffer( KTestScreenBufferUid );
    scrBuf->RequestNextBuffer( KDummyUid );
    scrBuf->RequestNextBuffer( KTestScreenBufferUid );

    scrBuf->AddObserverL( KDummyUid, observer );

    scrBuf->RequestNextBuffer( KTestScreenBufferUid );
    scrBuf->RequestNextBuffer( KDummyUid );
    scrBuf->RequestNextBuffer( KTestScreenBufferUid );

    scrBuf->RemoveObserver( KDummyUid );
    scrBuf->RemoveObserver( KTestScreenBufferUid );
    
    CleanupStack::PopAndDestroy( observer );
    CleanupStack::PopAndDestroy( scrBuf );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestScrBufObserverMethodsL
// -----------------------------------------------------------------------------

TInt CTestPlatAlfVisual::TestScrBufObserverMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestScrBufObserverMethodsL, "TestScrBufObserverMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestScrBufObserverMethodsL );
    // Print to log file
    iLog->Log( KTestScrBufObserverMethodsL );
    
    TRect dispRect( TPoint( KTestDisplayPosX, KTestDisplayPosY ),
                    TSize( KTestDisplayWidth, KTestDisplayHeight ) );
    CAlfDisplay* display = NULL;
    TRAPD( err,
           display = &iAlfEnv->NewDisplayL(
                                   dispRect, 0, NULL,
                                   CAlfDisplay::EDisplayOffScreenBuffer,
                                   KTestScreenBufferUid
                                   )
          );
    if ( ( err == KErrNotSupported ) &&
         ( iAlfEnv->Renderer() != EAlfRendererBitgdi ) )
        {
        // Ok, as off-screen buffer should work only in BitGdi mode
            err = KErrNone;
        }
    else
        {
        User::LeaveIfError( err );
        display->SetVisibleArea( dispRect );
        }   
    
    CAlfScreenBuffer* scrBuf = CAlfScreenBuffer::NewL( *iAlfEnv );
    CleanupStack::PushL( scrBuf );
    STIF_ASSERT_NOT_NULL( scrBuf );
    
    CTestScreenBufferObserver* observer = new ( ELeave ) CTestScreenBufferObserver;
    CleanupStack::PushL( observer );
    STIF_ASSERT_NOT_NULL( observer );
    
    scrBuf->AddObserverL( KTestScreenBufferUid, observer );
    scrBuf->RemoveObserver( KTestScreenBufferUid );
    
    scrBuf->AddObserverL( KTestScreenBufferUid, observer, 1 ); // Just test non null priority even if it do not has an effect
    scrBuf->RemoveObserver( KTestScreenBufferUid );
    
    scrBuf->AddObserverL( KTestScreenBufferUid, observer );
    TRAP( err, scrBuf->AddObserverL( KTestScreenBufferUid, observer ) );
    STIF_ASSERT_EQUALS( err, KErrAlreadyExists );
    
    scrBuf->RemoveObserver( KTestScreenBufferUid );
    
    scrBuf->AddObserverL( KTestScreenBufferUid, 0, observer );
    scrBuf->RemoveObserver( KTestScreenBufferUid );
    
    CleanupStack::PopAndDestroy( observer );
    CleanupStack::PopAndDestroy( scrBuf );
    
    return KErrNone;
    }


//  [End of File]
