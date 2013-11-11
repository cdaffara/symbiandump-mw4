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
* Description:  test functions for alftextureprocessor.h
*
*/



// [INCLUDE FILES]
#include <alf/alftextureprocessor.h>
#include <avkon.mbg>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtProcessorBlurL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtProcessorBlurL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtProcessorBlurL, "TestTxtProcessorBlurL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtProcessorBlurL );
    // Print to log file
    iLog->Log( KTestTxtProcessorBlurL );
    
    CAlfTextureManager* txtMgr = &( iAlfEnv->TextureManager() );
    
    CAlfImageVisual* imgVisual = CAlfImageVisual::AddNewL( *iAlfCtl );
    imgVisual->SetScaleMode(CAlfImageVisual::EScaleFitInside);
    TAlfImage magnifyingGlassImage(
            KAknsIIDQgnIndiFindGlass,
            iAlfDisplay->VisibleArea().Size(),
            EAspectRatioPreserved,
            &AknIconUtils::AvkonIconFileName(), 
            EMbmAvkonQgn_indi_find_glass,
            EMbmAvkonQgn_indi_find_glass_mask,
            0.f,0.f,1.f,1.f,
            TAlfTextureFlags( EAlfTextureFlagGenerateShadow | EAlfTextureFlagSkinContent ),
            2,
            txtMgr );
    imgVisual->SetImage( magnifyingGlassImage );
    
    CAlfTexture* tex = ( CAlfTexture* ) &( imgVisual->Image().Texture() );

    txtMgr->Processor().BlurL( *tex, *tex );
    txtMgr->Processor().BlurL( *tex, *tex, iAlfDisplay->VisibleArea().Size() );

    return KErrNone;
    }


//  [End of File]
