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
* Description:  test functions for alftextstyle.h
*
*/



// [INCLUDE FILES]
#include <alf/alftextstyle.h>

#include "testplatalfvisual.h"

// CONSTANTS
const TInt KTextPaneHeight = 20;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleParentIdL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleParentIdL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleParentIdL, "TestStyleParentIdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleParentIdL );
    // Print to log file
    iLog->Log( KTestStyleParentIdL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    STIF_ASSERT_NOT_NULL( styleMgr );
    
    // create a new TextStyle A
    TInt styleAId = styleMgr->CreatePlatformTextStyleL(
                        EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    
    // create a new style B having A as parent
    TInt styleBId = styleMgr->CreatePlatformTextStyleL(
                        EAknLogicalFontSecondaryFont, styleAId );
    
    // clone TextStyle C from B
    TInt styleCId = styleMgr->CopyTextStyleL( styleBId );
       
    CAlfTextStyle* styleA = styleMgr->TextStyle( styleAId );
    CAlfTextStyle* styleB = styleMgr->TextStyle( styleBId );
    CAlfTextStyle* styleC = styleMgr->TextStyle( styleCId );

    TInt resultAId = styleA->Id();
    TInt resultBParentId = styleB->ParentId();
    TInt resultCParentId = styleC->ParentId();

    STIF_ASSERT_EQUALS( resultAId, resultBParentId );
    STIF_ASSERT_EQUALS( resultAId, resultCParentId );
    STIF_ASSERT_EQUALS( resultBParentId, resultCParentId );
    
    styleB->SetParentId( resultBParentId );
    styleC->SetParentId( resultCParentId );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleIdL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleIdL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleIdL, "TestStyleIdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleIdL );
    // Print to log file
    iLog->Log( KTestStyleIdL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    STIF_ASSERT_NOT_NULL( styleMgr );
    
    TInt styleAId = styleMgr->CreatePlatformTextStyleL(
                            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    TInt styleBId = styleMgr->CreatePlatformTextStyleL(
                            EAknLogicalFontSecondaryFont, styleAId );
    TInt styleCId = styleMgr->CopyTextStyleL( styleBId );
       
    CAlfTextStyle* styleA = styleMgr->TextStyle( styleAId );
    CAlfTextStyle* styleB = styleMgr->TextStyle( styleBId );
    CAlfTextStyle* styleC = styleMgr->TextStyle( styleCId );

    STIF_ASSERT_NOT_EQUALS( styleA->Id(), styleB->Id() );
    STIF_ASSERT_NOT_EQUALS( styleB->Id(), styleC->Id() );
    STIF_ASSERT_NOT_EQUALS( styleA->Id(), styleC->Id() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleTextColorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleTextColorL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleTextColorL, "TestStyleTextColorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleTextColorL );
    // Print to log file
    iLog->Log( KTestStyleTextColorL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->SetTextColor( KRgbRed );
    TRgb txtColor = style->TextColor();
    STIF_ASSERT_EQUALS( KRgbRed, txtColor );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleTextSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleTextSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleTextSizeL, "TestStyleTextSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleTextSizeL );
    // Print to log file
    iLog->Log( KTestStyleTextSizeL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->SetTextSizeInTwips( 1 );
    TInt txtSizeInTwips = style->TextSizeInTwips();
    STIF_ASSERT_EQUALS( 1, txtSizeInTwips );
    style->SetTextSizeInPixels( 1 );
    TInt txtSizeInPixels = style->TextSizeInPixels();
    STIF_ASSERT_EQUALS( 1, txtSizeInPixels );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleBoldL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleBoldL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleBoldL, "TestStyleBoldL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleBoldL );
    // Print to log file
    iLog->Log( KTestStyleBoldL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->SetBold( ETrue );
    TBool isBold = style->IsBold();
    STIF_ASSERT_TRUE( isBold );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleItalicL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleItalicL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleItalicL, "TestStyleItalicL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleItalicL );
    // Print to log file
    iLog->Log( KTestStyleItalicL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->SetItalic( ETrue );
    TBool isItalic = style->IsItalic();
    STIF_ASSERT_TRUE( isItalic );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleUnderlineL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleUnderlineL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleUnderlineL, "TestStyleUnderlineL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleUnderlineL );
    // Print to log file
    iLog->Log( KTestStyleUnderlineL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->SetUnderline( ETrue );
    TBool isUnderline = style->IsUnderline();
    STIF_ASSERT_TRUE( isUnderline );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleStrikeThroughL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleStrikeThroughL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleStrikeThroughL, "TestStyleStrikeThroughL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleStrikeThroughL );
    // Print to log file
    iLog->Log( KTestStyleStrikeThroughL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->SetStrikeThrough( ETrue );
    TBool isStrikeThrough = style->IsStrikeThrough();
    STIF_ASSERT_TRUE( isStrikeThrough );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleTypefaceNameL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleTypefaceNameL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleTypefaceNameL, "TestStyleTypefaceNameL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleTypefaceNameL );
    // Print to log file
    iLog->Log( KTestStyleTypefaceNameL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    style->TypefaceNameL();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleSetTextPaneHeightInPixelsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleSetTextPaneHeightInPixelsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleSetTextPaneHeightInPixelsL, "TestStyleSetTextPaneHeightInPixelsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleSetTextPaneHeightInPixelsL );
    // Print to log file
    iLog->Log( KTestStyleSetTextPaneHeightInPixelsL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    TInt styleId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    CAlfTextStyle* style = styleMgr->TextStyle( styleId );
    
    style->SetTextPaneHeightInPixels( KTextPaneHeight );
    TInt size = style->TextSizeInPixels();
    TBool within = ( size <= KTextPaneHeight );
    STIF_ASSERT_TRUE( within );
    
    return KErrNone;
    }


//  [End of File]
