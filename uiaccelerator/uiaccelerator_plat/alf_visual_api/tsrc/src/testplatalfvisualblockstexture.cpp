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
* Description:  test functions for alftexture.h
*
*/



// [INCLUDE FILES]
#include <alf/alftexture.h>
#include <alf/alfbitmapprovider.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KExistingTextureId = 1000;

const TInt KProvidedBitmapWidth = 64;
const TInt KProvidedBitmapHeight = 64;

_LIT( KImageName, "OneExistingTexture" );


// FORWARD DECLARATION
class CTestBmpProvider : public CBase, public MAlfBitmapProvider
    {
public:
    /**
     * Provide a bitmap and mask from the given UID.
     * This method should create a bitmap and its mask, usually by loading it from disk.
     */
    void ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask)
        {
        aBitmap = new ( ELeave ) CFbsBitmap;
        aBitmapMask = new ( ELeave ) CFbsBitmap;
        TSize bmpSize( KProvidedBitmapWidth, KProvidedBitmapHeight );
        aBitmap->Create( bmpSize, EColor64K );
        aBitmapMask->Create( bmpSize, EGray256 );
        }
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTextureGetMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTextureGetMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTextureGetMethods, "TestTextureGetMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTextureGetMethods );
    // Print to log file
    iLog->Log( KTestTextureGetMethods );
    
    CAlfTexture* texture = &( iAlfEnv->TextureManager().BlankTexture() );
    texture->Id();
    texture->ServerHandle();
    texture->FileName();
    texture->HasContent();
    texture->MaxTextureSize();
    texture->Size();
    texture->IsAnimated();
    texture->Priority();
    texture->AutoSizeParams();
    texture->RefCountingEnabled();
    texture->RefCountingAction();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTextureAnimation
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTextureAnimation( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTextureAnimation, "TestTextureAnimation" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTextureAnimation );
    // Print to log file
    iLog->Log( KTestTextureAnimation );
    
    CAlfTexture* texture = &( iAlfEnv->TextureManager().BlankTexture() );
    texture->StartAnimation();
    texture->StopAnimation();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTextureSetMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTextureSetMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTextureSetMethodsL, "TestTextureSetMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTextureSetMethodsL );
    // Print to log file
    iLog->Log( KTestTextureSetMethodsL );
    
    CTestBmpProvider* bmpProvider = new ( ELeave ) CTestBmpProvider;
    CleanupStack::PushL( bmpProvider );
    iAlfEnv->TextureManager().DefineFileNameL(
                        KExistingTextureId, KImageName );
    CAlfTexture* texture = &( iAlfEnv->TextureManager().CreateTextureL(
                    KExistingTextureId, bmpProvider, EAlfTextureFlagAutoSize ) );
    texture->SetPriority( texture->Priority() );
    texture->SetAutoSizeParams( texture->AutoSizeParams() );
    texture->EnableRefCounting( texture->RefCountingEnabled() );
    texture->SetRefCountingAction( texture->RefCountingAction() );
    
    TAlfTextureAutoSizeParams newparams = texture->AutoSizeParams();
    newparams.SetSizeLowerThreshold( TAlfTextureAutoSizeParams::EHigh );
    texture->SetAutoSizeParams(newparams);
    TAlfTextureAutoSizeParams resultparams = texture->AutoSizeParams();
    TInt target = TAlfTextureAutoSizeParams::EHigh;
    STIF_ASSERT_EQUALS( target, resultparams.SizeLowerThreshold() );

    CleanupStack::PopAndDestroy( bmpProvider );

    
    return KErrNone;
    }


//  [End of File]
