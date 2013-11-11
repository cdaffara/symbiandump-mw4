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
* Description:  test functions for alftexturemanager.h
*
*/



// [INCLUDE FILES]
#include <alf/alftexturemanager.h>
#include <alf/alfbitmapprovider.h>

#include "testplatalfvisual.h"


// CONSTANTS
_LIT( KEmptyImagePath, "" );
_LIT( KEmptyImageFileName, "" );
_LIT( KTestImageFileName, "testfile" );

const TInt KExistingTextureId = 1000;

const TInt KProvidedBitmapWidth = 64;
const TInt KProvidedBitmapHeight = 64;
const TInt KTextureMaxWidth = 64;
const TInt KTextureMaxHeight = 64;


// FORWARD DECLARATION
// class CTestBmpProvider for testing CAlfTextureManager
class CTestBmpProvider : public CBase, public MAlfBitmapProvider
    {
public:
    /**
     * Constructor of CTestBmpProvider.
     * @param aWidth    The width of provided bitmap.
     * @param aHeight   The height of provided bitmap.
     */
    CTestBmpProvider( TInt aWidth, TInt aHeight )
        {
        iProvidedBmpSize = TSize( aWidth, aHeight );
        }
    
    /**
     * From CAlfBitmapProvider
     */
    void ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap)
        {
        aBitmap = new ( ELeave ) CFbsBitmap;
        aMaskBitmap = new ( ELeave ) CFbsBitmap;
        aBitmap->Create( iProvidedBmpSize, EColor64K );
        aMaskBitmap->Create( iProvidedBmpSize, EGray256 );
        }
    
private:
    /**
     * OWN : The size of provided bitmap.
     */
    TSize iProvidedBmpSize;
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrEnv
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrEnv( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrEnv, "TestTxtMgrEnv" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrEnv );
    // Print to log file
    iLog->Log( KTestTxtMgrEnv );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    STIF_ASSERT_EQUALS( iAlfEnv, &txtMgr->Env() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrTextureL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrTextureL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrTextureL, "TestTxtMgrTextureL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrTextureL );
    // Print to log file
    iLog->Log( KTestTxtMgrTextureL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    CAlfTexture* texture = &txtMgr->CreateTextureL(
            KExistingTextureId, bmpProvider, EAlfTextureFlagDefault );
    CleanupStack::PushL( texture );
    STIF_ASSERT_EQUALS( KExistingTextureId, texture->Id() );
    
    const CAlfTexture* texture2 = txtMgr->Texture( KExistingTextureId );
    STIF_ASSERT_NOT_NULL( texture2 );
    STIF_ASSERT_EQUALS( KExistingTextureId, texture2->Id() );
    
    CAlfTexture* texture3 = txtMgr->TextureL( KExistingTextureId );
    STIF_ASSERT_NOT_NULL( texture3 );
    STIF_ASSERT_EQUALS( KExistingTextureId, texture3->Id() );
    
    CleanupStack::Pop( texture );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrImagePathL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrImagePathL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrImagePathL, "TestTxtMgrImagePathL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrImagePathL );
    // Print to log file
    iLog->Log( KTestTxtMgrImagePathL );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->SetImagePathL( KEmptyImagePath );
    const TDesC& path = txtMgr->ImagePath();
    STIF_ASSERT_TRUE( path == KEmptyImagePath );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrBlankTexture
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrBlankTexture( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrBlankTexture, "TestTxtMgrBlankTexture" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrBlankTexture );
    // Print to log file
    iLog->Log( KTestTxtMgrBlankTexture );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    const CAlfTexture& texture = ( ( const CAlfTextureManager* ) txtMgr )->BlankTexture();
    STIF_ASSERT_EQUALS( 0, texture.Id() );
    
    CAlfTexture& texture2 = txtMgr->BlankTexture();
    STIF_ASSERT_EQUALS( 0, texture2.Id() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrLoadTextureL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrLoadTextureL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrLoadTextureL, "TestTxtMgrLoadTextureL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrLoadTextureL );
    // Print to log file
    iLog->Log( KTestTxtMgrLoadTextureL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->CreateTextureL( KExistingTextureId, bmpProvider, EAlfTextureFlagDefault );
    txtMgr->LoadTextureL( KEmptyImageFileName, EAlfTextureFlagDefault, KExistingTextureId );
    TSize txtMaxSize( KTextureMaxWidth, KTextureMaxHeight );
    txtMgr->LoadTextureL( KExistingTextureId, txtMaxSize, EAlfTextureFlagDefault );
    txtMgr->LoadTextureL( KEmptyImageFileName, txtMaxSize, EAlfTextureFlagDefault, KExistingTextureId );
    txtMgr->LoadAnimatedTextureL(KEmptyImageFileName, txtMaxSize, EAlfTextureFlagDefault, KExistingTextureId );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrCreateTextureL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrCreateTextureL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrCreateTextureL, "TestTxtMgrCreateTextureL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrCreateTextureL );
    // Print to log file
    iLog->Log( KTestTxtMgrCreateTextureL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    CAlfTexture& texture = txtMgr->CreateTextureL( KExistingTextureId,
                                        bmpProvider, EAlfTextureFlagDefault );
    STIF_ASSERT_EQUALS( KExistingTextureId, texture.Id() );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrUnloadTextureL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrUnloadTextureL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrUnloadTextureL, "TestTxtMgrUnloadTextureL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrUnloadTextureL );
    // Print to log file
    iLog->Log( KTestTxtMgrUnloadTextureL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->DefineFileNameL( KExistingTextureId, KTestImageFileName );
    txtMgr->CreateTextureL( KExistingTextureId, bmpProvider, EAlfTextureFlagDefault );
    txtMgr->UnloadTexture( KTestImageFileName );
    STIF_ASSERT_FALSE( txtMgr->Texture( KExistingTextureId )->HasContent() );
    txtMgr->DefineFileNameL( KExistingTextureId, KTestImageFileName );
    txtMgr->CreateTextureL( KExistingTextureId, bmpProvider, EAlfTextureFlagDefault );
    txtMgr->UnloadTexture( KExistingTextureId );
    STIF_ASSERT_FALSE( txtMgr->Texture( KExistingTextureId )->HasContent() );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrUpdateTextureL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrUpdateTextureL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrUpdateTextureL, "TestTxtMgrUpdateTextureL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrUpdateTextureL );
    // Print to log file
    iLog->Log( KTestTxtMgrUpdateTextureL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->DefineFileNameL( KExistingTextureId, KTestImageFileName );
    txtMgr->UpdateTextureFromFileL( KExistingTextureId, &KEmptyImageFileName );
    txtMgr->UpdateTextureFromBitmapL( KExistingTextureId, bmpProvider );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrDefineFileNameL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrDefineFileNameL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrDefineFileNameL, "TestTxtMgrDefineFileNameL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrDefineFileNameL );
    // Print to log file
    iLog->Log( KTestTxtMgrDefineFileNameL );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->DefineFileNameL( KExistingTextureId, KEmptyImageFileName );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrPrependImagePath
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrPrependImagePath( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrPrependImagePath, "TestTxtMgrPrependImagePath" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrPrependImagePath );
    // Print to log file
    iLog->Log( KTestTxtMgrPrependImagePath );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    TFileName fileName( KEmptyImageFileName );
    txtMgr->PrependImagePath( fileName );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrObserverMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrObserverMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrObserverMethodsL, "TestTxtMgrObserverMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrObserverMethodsL );
    // Print to log file
    iLog->Log( KTestTxtMgrObserverMethodsL );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->AddLoadObserverL( NULL );
    txtMgr->AddStateObserverL( NULL );
    txtMgr->AddAutoSizeObserverL( NULL );
    txtMgr->RemoveLoadObserver( NULL );
    txtMgr->RemoveStateObserver( NULL );
    txtMgr->RemoveAutoSizeObserver( NULL );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrProcessor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrProcessor( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrProcessor, "TestTxtMgrProcessor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrProcessor );
    // Print to log file
    iLog->Log( KTestTxtMgrProcessor );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->Processor();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrSetAutomaticTextureIdRange
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrSetAutomaticTextureIdRange( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrSetAutomaticTextureIdRange, "TestTxtMgrSetAutomaticTextureIdRange" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrSetAutomaticTextureIdRange );
    // Print to log file
    iLog->Log( KTestTxtMgrSetAutomaticTextureIdRange );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->SetAutomaticTextureIdRange( 1, 2 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrIsLoadedL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrIsLoadedL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrIsLoadedL, "TestTxtMgrIsLoadedL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrIsLoadedL );
    // Print to log file
    iLog->Log( KTestTxtMgrIsLoadedL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->CreateTextureL( KExistingTextureId, bmpProvider, EAlfTextureFlagDefault );
    txtMgr->IsLoaded( KEmptyImageFileName );
    txtMgr->IsLoaded( KExistingTextureId );
    txtMgr->IsLoaded( txtMgr->Texture( KExistingTextureId ) );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtMgrTextureIdL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtMgrTextureIdL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtMgrTextureIdL, "TestTxtMgrTextureIdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtMgrTextureIdL );
    // Print to log file
    iLog->Log( KTestTxtMgrTextureIdL );
    
    CTestBmpProvider* bmpProvider =
        new ( ELeave ) CTestBmpProvider( KProvidedBitmapWidth, KProvidedBitmapHeight );
    CleanupStack::PushL( bmpProvider );
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    txtMgr->CreateTextureL( KExistingTextureId, bmpProvider, EAlfTextureFlagDefault );
    txtMgr->TextureId( KEmptyImageFileName );
    CleanupStack::PopAndDestroy( bmpProvider );
    
    return KErrNone;
    }


//  [End of File]
