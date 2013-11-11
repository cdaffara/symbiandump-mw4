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
* Description:  test function implement of CTestPlatAlfVisual
*
*/


// [INCLUDE FILES]
#include <alf/alfanchorlayout.h>
#include <alf/alfcontrol.h>
#include <alf/alfenv.h>
#include <alf/alftextvisual.h>
#include <alf/alfmetric.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TReal32 KRealX = 10.0;
const TReal32 KRealY = 10.0;
const TReal32 KRealWidth = 100.0;
const TReal32 KRealHeight = 100.0;

const TReal32 KValueNow = 0.3;

//for test alfanchorlayout.h protected
class CTestAlfAnchorLayout : public CAlfAnchorLayout
    {
public:
    /**
     * From CAlfAnchorLayout
     */
    void DoRemoveAndDestroyAllD()
        {
        CAlfAnchorLayout::DoRemoveAndDestroyAllD();
        }
    
    /**
     * From CAlfAnchorLayout
     */
    void PropertyOwnerExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        CAlfAnchorLayout::PropertyOwnerExtension( aExtensionUid, aExtensionParams );
        }
    
    };



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutAddNewL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutAddNewL, "TestAnchorLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutAddNewL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );

    STIF_ASSERT_NOT_NULL( layout );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutConstructorL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutConstructorL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutConstructorL, "TestAnchorLayoutConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutConstructorL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutConstructorL );

    CAlfAnchorLayout* layout = new ( ELeave ) CAlfAnchorLayout();
    CleanupStack::PushL( layout );
    STIF_ASSERT_NOT_NULL( layout );
    CleanupStack::PopAndDestroy( layout );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutConstructL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutConstructL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutConstructL, "TestAnchorLayoutConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutConstructL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutConstructL );

    CAlfAnchorLayout* layout = new ( ELeave ) CAlfAnchorLayout();
    CleanupStack::PushL( layout );
    layout->ConstructL( *iAlfCtl );

    CleanupStack::PopAndDestroy( layout );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutResetL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutResetL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutResetL, "TestAnchorLayoutResetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutResetL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutResetL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );

    layout->Reset();

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutSetAnchorL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutSetAnchorL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutSetAnchorL, "TestAnchorLayoutSetAnchorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutSetAnchorL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutSetAnchorL );
    
    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    
    TAlfTimedPoint point;
    point.iX.SetValueNow( 0.1 );
    point.iY.SetValueNow( 0.2 );
    layout->SetAnchor( EAlfAnchorTopLeft, 0, EAlfAnchorOriginLeft,
                       EAlfAnchorOriginTop, EAlfAnchorMetricRelativeToSize,
                       EAlfAnchorMetricRelativeToSize, point );

    point.iX.SetValueNow( KValueNow );
    point.iY.SetValueNow( KValueNow );
    layout->SetAnchor( EAlfAnchorBottomRight, 0,
                       EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                       EAlfAnchorMetricRelativeToSize,
                       EAlfAnchorMetricRelativeToSize, point );

    layout->UpdateChildrenLayout();

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutSetRelativeAnchorRectL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutSetRelativeAnchorRectL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutSetRelativeAnchorRectL, "TestAnchorLayoutSetRelativeAnchorRectL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutSetRelativeAnchorRectL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutSetRelativeAnchorRectL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    
    TAlfRealPoint topLeftOffset;
    TAlfRealPoint bottomRightOffset;
         
    layout->SetRelativeAnchorRect( 0, EAlfAnchorOriginLeft,
                                   EAlfAnchorOriginBottom,
                                   topLeftOffset,
                                   EAlfAnchorOriginLeft,
                                   EAlfAnchorOriginBottom,
                                   bottomRightOffset );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutRemoveAnchorL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutRemoveAnchorL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutRemoveAnchorL, "TestAnchorLayoutRemoveAnchorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutRemoveAnchorL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutRemoveAnchorL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );

    layout->RemoveAnchor( EAlfAnchorTopLeft, 0 );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutAttachEdgeL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutAttachEdgeL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutAttachEdgeL, "TestAnchorLayoutAttachEdgeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutAttachEdgeL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutAttachEdgeL );
    
    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    
    TAlfMetric offset( 0.1, EAlfUnitMySize );
         
    TInt attached0 = layout->Attach( 0, EAlfAnchorTypeLeft, offset,
                                     EAlfAnchorAttachmentOriginLeft,
                                     EAlfAnchorAttachToParent );
    TInt attached1 = layout->Attach( 0, EAlfAnchorTypeBottom,
                                     offset, EAlfAnchorAttachmentOriginTop,
                                     EAlfAnchorAttachToParent );
    STIF_ASSERT_EQUALS( attached0, KErrNone );
    STIF_ASSERT_EQUALS( attached1, KErrNone );

    // try to create a cycle
    TInt attached2 = layout->Attach( 1, EAlfAnchorTypeRight, offset,
                                     EAlfAnchorAttachmentOriginLeft, 0 );
    TInt attached3 = layout->Attach( 2, EAlfAnchorTypeLeft, offset,
                                     EAlfAnchorAttachmentOriginRight, 1 );
    TInt attached4 = layout->Attach( 0, EAlfAnchorTypeLeft, offset,
                                     EAlfAnchorAttachmentOriginLeft, 2 );
    STIF_ASSERT_EQUALS( attached2, KErrNone );
    STIF_ASSERT_EQUALS( attached3, KErrNone );
    STIF_ASSERT_EQUALS( attached4, KErrArgument ); // this one should fail as it completes the cycle
    
    // and along the vertical axis, other way around this time
    TInt attached5 = layout->Attach( 2, EAlfAnchorTypeTop, offset, 
                                     EAlfAnchorAttachmentOriginBottom, 0 );
    TInt attached6 = layout->Attach( 1, EAlfAnchorTypeBottom,
                                     offset, EAlfAnchorAttachmentOriginTop, 2 );
    TInt attached7 = layout->Attach( 0, EAlfAnchorTypeBottom, offset,
                                     EAlfAnchorAttachmentOriginBottom, 1 );
    STIF_ASSERT_EQUALS( attached5, KErrNone );
    STIF_ASSERT_EQUALS( attached6, KErrNone );
    STIF_ASSERT_EQUALS( attached7, KErrArgument ); // this one should fail as it completes the cycle
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutAttachCornerL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutAttachCornerL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutAttachCornerL, "TestAnchorLayoutAttachCornerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutAttachCornerL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutAttachCornerL );
    
    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    
    TAlfTimedPoint point;
    point.iX.SetValueNow( 0.1 );
    point.iY.SetValueNow( 0.2 );
    layout->SetAnchor( EAlfAnchorTopLeft, 0, EAlfAnchorOriginLeft,
                       EAlfAnchorOriginTop, EAlfAnchorMetricRelativeToSize,
                       EAlfAnchorMetricRelativeToSize, point );

    point.iX.SetValueNow( KValueNow );
    point.iY.SetValueNow( KValueNow );
    layout->SetAnchor( EAlfAnchorBottomRight, 0,
                       EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                       EAlfAnchorMetricRelativeToSize,
                       EAlfAnchorMetricRelativeToSize, point );

    layout->UpdateChildrenLayout();
    
    TInt ordinal(0);
    TAlfAnchorType type( EAlfAnchorTypeLeft );
    TAlfXYMetric offset(
        TAlfMetric( 0.1, EAlfUnitMySize ),
        TAlfMetric( 0.1, EAlfUnitMySize ) );
    TAlfAnchorAttachmentOrigin attachmentOrigin( EAlfAnchorAttachmentOriginLeft );
    TInt attachmentOrdinal( EAlfAnchorAttachToParent );
          
    layout->Attach( ordinal, type, offset, attachmentOrigin, attachmentOrdinal );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutAttachBoxL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutAttachBoxL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutAttachBoxL, "TestAnchorLayoutAttachBoxL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutAttachBoxL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutAttachBoxL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    
    TInt ordinal(0);
    TAlfBoxMetric offset(
        TAlfMetric( 0.1, EAlfUnitMySize ),
        TAlfMetric( -0.1, EAlfUnitMySize ),
        TAlfMetric( 0.1, EAlfUnitMySize ),
        TAlfMetric( -0.1, EAlfUnitMySize ) );
    TAlfAnchorAttachmentOrigin attachmentOrigin( EAlfAnchorAttachmentOriginNone );
    TInt attachmentOrdinal( EAlfAnchorAttachToParent );
          
    layout->Attach( ordinal, offset, attachmentOrigin, attachmentOrdinal );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutDetachL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutDetachL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutDetachL, "TestAnchorLayoutDetachL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutDetachL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutDetachL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );

    layout->Detach( 0 );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutDetachTypeL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutDetachTypeL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutDetachTypeL, "TestAnchorLayoutDetachTypeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutDetachTypeL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutDetachTypeL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );
    layout->SetSize( TAlfRealSize( KRealWidth, KRealHeight ) );
    layout->SetPos( TAlfRealPoint( KRealX, KRealY ) );
    
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );
    CAlfTextVisual::AddNewL( *iAlfCtl, layout );

    layout->Detach( 0, EAlfAnchorTypeLeft );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutRemoveAndDestroyAllDL, "TestAnchorLayoutRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutRemoveAndDestroyAllDL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );

    layout->RemoveAndDestroyAllD( );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutUpdateChildrenLayoutL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutUpdateChildrenLayoutL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutUpdateChildrenLayoutL, "TestAnchorLayoutUpdateChildrenLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutUpdateChildrenLayoutL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutUpdateChildrenLayoutL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );

    layout->UpdateChildrenLayout( );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutFindTagL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutFindTagL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutFindTagL, "TestAnchorLayoutFindTagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutFindTagL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutFindTagL );
    _LIT8( KTest, "Test" );
    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );

    layout->FindTag( KTest );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutBaseUnitL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutBaseUnitL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutBaseUnitL, "TestAnchorLayoutBaseUnitL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutBaseUnitL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutBaseUnitL );

    CAlfAnchorLayout* layout = CAlfAnchorLayout::AddNewL( *iAlfCtl );

    layout->BaseUnit();

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutDoRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutDoRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutDoRemoveAndDestroyAllDL, "TestAnchorLayoutDoRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutDoRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutDoRemoveAndDestroyAllDL );

    CTestAlfAnchorLayout* layout = new ( ELeave ) CTestAlfAnchorLayout();
    CleanupStack::PushL( layout );
    layout->ConstructL( *iAlfCtl );
    layout->DoRemoveAndDestroyAllD();
    CleanupStack::Pop( layout );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAnchorLayoutPropertyOwnerExtensionL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestAnchorLayoutPropertyOwnerExtensionL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAnchorLayoutPropertyOwnerExtensionL, "TestAnchorLayoutPropertyOwnerExtensionL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAnchorLayoutPropertyOwnerExtensionL );
    // Print to log file
    iLog->Log( KTestAnchorLayoutPropertyOwnerExtensionL );

    CTestAlfAnchorLayout* layout = new ( ELeave ) CTestAlfAnchorLayout();
    CleanupStack::PushL( layout );
    layout->ConstructL( *iAlfCtl );
    TUid extensionUid = { 0x00000001 };
    layout->PropertyOwnerExtension( extensionUid, 0 );

    CleanupStack::PopAndDestroy( layout );
    return KErrNone;
    }


//  [End of File]

