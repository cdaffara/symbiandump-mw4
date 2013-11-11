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
* Description:  test functions for alftextvisual.h
*
*/



// [INCLUDE FILES]
#include <alf/alftextvisual.h>
#include <aknsconstants.h>
#include <alf/alfpropertyowner.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KGeneralNum = 10;

_LIT( KTestTxt, "Test" );
_LIT( KColorIndex, "0" );
_LIT( KTextColorTable, "KAknsIIDNone" );
_LIT8( KTagText, "Caption" );


// FORWARD DECLARATION
// CTestTextVisual for testing protected fuctions of CAlfTextVisual
class CTestTextVisual : public CAlfTextVisual
    {
public:
    /**
     * CTestTextVisual for testing protected fuctions of CAlfTextVisual
     */
    void TestProtectedFunctionsForTxtVisual()
        {
        TBool inFocusChain( EFalse );
        TAny* pInFocusChain = &inFocusChain;
        CAlfTextVisual::PropertyOwnerExtension( KUidAlfPropOwnerExtControlFocusChainChanged, &pInFocusChain );
        CAlfTextVisual::DoRemoveAndDestroyAllD();
        }
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualAddNewL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualAddNewL, "TestTxtVisualAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualAddNewL );
    // Print to log file
    iLog->Log( KTestTxtVisualAddNewL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualConstructorL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualConstructorL, "TestTxtVisualConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualConstructorL );
    // Print to log file
    iLog->Log( KTestTxtVisualConstructorL );
    
    CAlfTextVisual* txtVisual = new ( ELeave ) CAlfTextVisual;
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    CleanupStack::PopAndDestroy( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualConstructL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualConstructL, "TestTxtVisualConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualConstructL );
    // Print to log file
    iLog->Log( KTestTxtVisualConstructL );
    
    CAlfTextVisual* txtVisual = new ( ELeave ) CAlfTextVisual;
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualTxtMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualTxtMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualTxtMethodsL, "TestTxtVisualTxtMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualTxtMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualTxtMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetTextL( KTestTxt );
    txtVisual->Text();
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualStyleMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualStyleMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualStyleMethodsL, "TestTxtVisualStyleMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualStyleMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualStyleMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetStyle( EAlfTextStyleNormal );
    STIF_ASSERT_EQUALS( EAlfTextStyleNormal, txtVisual->Style() );
    txtVisual->SetTextStyle( 1 );
    STIF_ASSERT_EQUALS( 1, txtVisual->TextStyle() );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualSetAlignL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualSetAlignL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualSetAlignL, "TestTxtVisualSetAlignL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualSetAlignL );
    // Print to log file
    iLog->Log( KTestTxtVisualSetAlignL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetAlign( EAlfAlignHLeft, EAlfAlignVTop );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualSetLineSpacingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualSetLineSpacingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualSetLineSpacingL, "TestTxtVisualSetLineSpacingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualSetLineSpacingL );
    // Print to log file
    iLog->Log( KTestTxtVisualSetLineSpacingL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetLineSpacing( KGeneralNum );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualExtentsMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualExtentsMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualExtentsMethodsL, "TestTxtVisualExtentsMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualExtentsMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualExtentsMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->TextExtents();
    txtVisual->SubstringExtents( 1, 2 );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualMaxLineCountMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualMaxLineCountMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualMaxLineCountMethodsL, "TestTxtVisualMaxLineCountMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualMaxLineCountMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualMaxLineCountMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetMaxLineCount( KGeneralNum );
    STIF_ASSERT_EQUALS( KGeneralNum, txtVisual->MaxLineCount() );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualWrappingMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualWrappingMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualWrappingMethodsL, "TestTxtVisualWrappingMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualWrappingMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualWrappingMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetWrapping( CAlfTextVisual::ELineWrapManual );
    STIF_ASSERT_EQUALS( CAlfTextVisual::ELineWrapManual, txtVisual->Wrapping() );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualBackgroundTypeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualBackgroundTypeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualBackgroundTypeL, "TestTxtVisualBackgroundTypeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualBackgroundTypeL );
    // Print to log file
    iLog->Log( KTestTxtVisualBackgroundTypeL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->BackgroundType();
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualShadowOpacityMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualShadowOpacityMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualShadowOpacityMethodsL, "TestTxtVisualShadowOpacityMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualShadowOpacityMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualShadowOpacityMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    TAlfTimedValue opacity;
    txtVisual->SetShadowOpacity( opacity );
    txtVisual->ShadowOpacity();
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualColorMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualColorMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualColorMethodsL, "TestTxtVisualColorMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualColorMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualColorMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->SetColor( KRgbDarkGray );
    txtVisual->SetColor( KAknsIIDNone, 0 );
    txtVisual->SetColor( KTextColorTable, KColorIndex );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualEnableShadowL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualEnableShadowL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualEnableShadowL, "TestTxtVisualEnableShadowL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualEnableShadowL );
    // Print to log file
    iLog->Log( KTestTxtVisualEnableShadowL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->EnableShadow( ETrue );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualOffsetMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualOffsetMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualOffsetMethodsL, "TestTxtVisualOffsetMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualOffsetMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualOffsetMethodsL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    TAlfTimedPoint offset;
    txtVisual->SetOffset( offset );
    txtVisual->Offset();
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualRemoveAndDestroyAllDL, "TestTxtVisualRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestTxtVisualRemoveAndDestroyAllDL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualUpdateChildrenLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualUpdateChildrenLayoutL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualUpdateChildrenLayoutL, "TestTxtVisualUpdateChildrenLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualUpdateChildrenLayoutL );
    // Print to log file
    iLog->Log( KTestTxtVisualUpdateChildrenLayoutL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->UpdateChildrenLayout();
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualFindTagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualFindTagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualFindTagL, "TestTxtVisualFindTagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualFindTagL );
    // Print to log file
    iLog->Log( KTestTxtVisualFindTagL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    txtVisual->FindTag( KTagText );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualSetHighlightRangeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualSetHighlightRangeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualSetHighlightRangeL, "TestTxtVisualSetHighlightRangeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualSetHighlightRangeL );
    // Print to log file
    iLog->Log( KTestTxtVisualSetHighlightRangeL );
    
    CAlfTextVisual* txtVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    CleanupStack::PushL( txtVisual );
    STIF_ASSERT_NOT_NULL( txtVisual );
    TRgb yellow = KRgbYellow;
    TRgb red = KRgbDarkRed;
    txtVisual->SetTextL( KTestTxt );
    txtVisual->SetHighlightRange( 0, 1, yellow, red );
    CleanupStack::Pop( txtVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtVisualProtectedMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtVisualProtectedMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtVisualProtectedMethodsL, "TestTxtVisualProtectedMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtVisualProtectedMethodsL );
    // Print to log file
    iLog->Log( KTestTxtVisualProtectedMethodsL );
    
    CTestTextVisual *testVisual = new ( ELeave ) CTestTextVisual;
    CleanupStack::PushL( testVisual );
    STIF_ASSERT_NOT_NULL( testVisual );
    testVisual->ConstructL( *iAlfCtl );
    testVisual->TestProtectedFunctionsForTxtVisual();
    CleanupStack::Pop( testVisual );
    
    return KErrNone;
    }


//  [End of File]
