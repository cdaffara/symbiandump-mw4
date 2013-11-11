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
* Description:  test functions for alfviewportlayout.h
*
*/



// [INCLUDE FILES]
#include <alf/alfviewportlayout.h>
#include <alf/alfpropertyowner.h>

#include "testplatalfvisual.h"


// Forward Declaration
// class CTestAlfViewportLayout for testing CAlfViewportLayout
class CTestAlfViewportLayout : public CAlfViewportLayout
    {
public:
    // constructor
    CTestAlfViewportLayout() {}
    
    // ConstructL
    void ConstructL( CAlfControl& aOwner )
        {
        CAlfViewportLayout::ConstructL( aOwner );
        }
    
    // From CAlfViewportLayout
    void DoRemoveAndDestroyAllD()
        {
        CAlfViewportLayout::DoRemoveAndDestroyAllD();
        }
    
    // From CAlfViewportLayout
    void PropertyOwnerExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        CAlfViewportLayout::PropertyOwnerExtension( aExtensionUid, aExtensionParams );
        }
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutAddNewL, "TestVpLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestVpLayoutAddNewL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutConstructorL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutConstructorL, "TestVpLayoutConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutConstructorL );
    // Print to log file
    iLog->Log( KTestVpLayoutConstructorL );
    
    CAlfViewportLayout* layout = new ( ELeave ) CAlfViewportLayout();
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    CleanupStack::PopAndDestroy( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutConstructL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutConstructL, "TestVpLayoutConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutConstructL );
    // Print to log file
    iLog->Log( KTestVpLayoutConstructL );
    
    CAlfViewportLayout* layout = new ( ELeave ) CAlfViewportLayout();
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    layout->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutSetVirtualSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutSetVirtualSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutSetVirtualSizeL, "TestVpLayoutSetVirtualSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutSetVirtualSizeL );
    // Print to log file
    iLog->Log( KTestVpLayoutSetVirtualSizeL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    TAlfRealSize size;
    layout->SetVirtualSize( size, 1 );
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutSetViewportSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutSetViewportSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutSetViewportSizeL, "TestVpLayoutSetViewportSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutSetViewportSizeL );
    // Print to log file
    iLog->Log( KTestVpLayoutSetViewportSizeL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    TAlfRealSize size;
    layout->SetViewportSize( size, 1 );
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutSetViewportPosL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutSetViewportPosL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutSetViewportPosL, "TestVpLayoutSetViewportPosL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutSetViewportPosL );
    // Print to log file
    iLog->Log( KTestVpLayoutSetViewportPosL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    TAlfRealPoint pos;
    layout->SetViewportPos( pos, 1 );
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutRemoveAndDestroyAllDL, "TestVpLayoutRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestVpLayoutRemoveAndDestroyAllDL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( layout );
    layout->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutUpdateChildrenLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutUpdateChildrenLayoutL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutUpdateChildrenLayoutL, "TestVpLayoutUpdateChildrenLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutUpdateChildrenLayoutL );
    // Print to log file
    iLog->Log( KTestVpLayoutUpdateChildrenLayoutL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    layout->UpdateChildrenLayout();
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutFindTagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutFindTagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutFindTagL, "TestVpLayoutFindTagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutFindTagL );
    // Print to log file
    iLog->Log( KTestVpLayoutFindTagL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    _LIT8( KTagText, "Caption" );
    layout->SetTagL( KTagText );
    STIF_ASSERT_NOT_NULL( layout->FindTag( KTagText ) );
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutBaseUnitL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutBaseUnitL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutBaseUnitL, "TestVpLayoutBaseUnitL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutBaseUnitL );
    // Print to log file
    iLog->Log( KTestVpLayoutBaseUnitL );
    
    CAlfViewportLayout* layout = CAlfViewportLayout::AddNewL( *iAlfCtl );
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    layout->BaseUnit();
    CleanupStack::Pop( layout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutDoRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutDoRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutDoRemoveAndDestroyAllDL, "TestVpLayoutDoRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutDoRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestVpLayoutDoRemoveAndDestroyAllDL );
    
    CTestAlfViewportLayout* testLayout = new ( ELeave ) CTestAlfViewportLayout;
    CleanupStack::PushL( testLayout );
    STIF_ASSERT_NOT_NULL( testLayout );
    testLayout->ConstructL( *iAlfCtl );
    testLayout->DoRemoveAndDestroyAllD();
    CleanupStack::Pop( testLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestVpLayoutPropertyOwnerExtensionL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestVpLayoutPropertyOwnerExtensionL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestVpLayoutPropertyOwnerExtensionL, "TestVpLayoutPropertyOwnerExtensionL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestVpLayoutPropertyOwnerExtensionL );
    // Print to log file
    iLog->Log( KTestVpLayoutPropertyOwnerExtensionL );
    
    CTestAlfViewportLayout* testLayout = new ( ELeave ) CTestAlfViewportLayout;
    CleanupStack::PushL( testLayout );
    STIF_ASSERT_NOT_NULL( testLayout );
    testLayout->ConstructL( *iAlfCtl );
    TBool inFocusChain( EFalse );
    TAny* pInFocusChain = &inFocusChain;
    testLayout->PropertyOwnerExtension( KUidAlfPropOwnerExtControlFocusChainChanged, &pInFocusChain );
    CleanupStack::PopAndDestroy( testLayout );
    
    return KErrNone;
    }


//  [End of File]
