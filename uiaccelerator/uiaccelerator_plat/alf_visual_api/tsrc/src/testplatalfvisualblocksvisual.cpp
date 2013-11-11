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
#include <alf/alfvisual.h>
#include <alf/alfvisualfactory.h>
#include <touchlogicalfeedback.h>
#include <alf/alfpropertyowner.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TUint KRefFlag = 0;

const TInt KNum10 = 10;
const TInt KNum20 = 20;
const TInt KNum30 = 30;
const TInt KNum40 = 40;
const TInt KNum50 = 50;
const TInt KNum65 = 65;
const TInt KNum88 = 88;
const TInt KNum100 = 100;
const TInt KNum120 = 120;
const TInt KNum200 = 200;
const TInt KNum300 = 300;

const TReal32 KReal1 = 20.4;
const TReal32 KReal2 = 30.3;
const TReal32 KReal3 = 40.3;
const TReal32 KReal4 = 50.4;
const TReal32 KReal5 = 0.6;
const TReal32 KReal6 = 0.3;
const TReal32 KReal7 = 12.3;


// FORWARD DECLARATION
// class CTestAlfVisual for testing CAlfVisual
class CTestAlfVisual : public CAlfVisual
    {
public:
    // From CAlfVisual
    void PropertyOwnerExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        CAlfVisual::PropertyOwnerExtension( aExtensionUid, aExtensionParams );
        }
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVCAlfVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVCAlfVisualL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVCAlfVisualL, "TestCAlfVCAlfVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVCAlfVisualL );
    // Print to log file
    iLog->Log( KTestCAlfVCAlfVisualL );
    
    CAlfVisual* alfVisual = new ( ELeave ) CAlfVisual();
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVConstructL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVConstructL, "TestCAlfVConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVConstructL );
    // Print to log file
    iLog->Log( KTestCAlfVConstructL );
    
    CAlfVisual* alfVisual = new ( ELeave ) CAlfVisual();
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVRemoveAndDestroyAllDL, "TestCAlfVRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestCAlfVRemoveAndDestroyAllDL );
    
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
            EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVIdentifierL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVIdentifierL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVIdentifierL, "TestCAlfVIdentifierL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVIdentifierL );
    // Print to log file
    iLog->Log( KTestCAlfVIdentifierL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    STIF_ASSERT_NOT_EQUALS( 0, alfVisual->Identifier() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetFlagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetFlagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetFlagL, "TestCAlfVSetFlagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetFlagL );
    // Print to log file
    iLog->Log( KTestCAlfVSetFlagL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    // Create layout parent for testing purposes
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) ); 
    parentLayout->SetFlag( EAlfVisualFlagManualLayout );
    parentLayout->SetPos( TAlfRealPoint( KNum40, KNum50 ) );
    parentLayout->SetSize( TAlfRealSize( KNum100, KNum120 ) );
    parentLayout->UpdateChildrenLayout();
    TAlfTimedPoint size = alfVisual->Size(); 
    STIF_ASSERT_EQUALS( KNum100+0.f, size.iX.Target() );
    STIF_ASSERT_EQUALS( KNum120+0.f, size.iY.Target() );
    TAlfTimedPoint pos = alfVisual->Pos(); 
    STIF_ASSERT_EQUALS( 0.f, pos.iX.Target() );
    STIF_ASSERT_EQUALS( 0.f, pos.iY.Target() );
    
    // 1# EAlfVisualFlagManualSize
    alfVisual->SetSize( TAlfRealSize( KNum10, KNum20 ) );   // 1. resize without the flag
    parentLayout->UpdateChildrenLayout();           // 2. relayout siblings
    size = alfVisual->Size();                       // 3. check that the visual still has the original size
    STIF_ASSERT_EQUALS( KNum100+0.f, size.iX.Target() );
    STIF_ASSERT_EQUALS( KNum120+0.f, size.iY.Target() );
    alfVisual->SetFlag( EAlfVisualFlagManualSize ); // 4. set the manual size flag
    alfVisual->SetSize( TAlfRealSize( KNum10, KNum20 ) );   // 5. resize again
    parentLayout->UpdateChildrenLayout();           // 6. relayout siblings
    size = alfVisual->Size();                       // 7. check the the new size is still valid
    STIF_ASSERT_EQUALS( KNum10+0.f, size.iX.Target() );
    STIF_ASSERT_EQUALS( KNum20+0.f, size.iY.Target() );
    
    // 2# EAlfVisualFlagManualPosition
    alfVisual->SetPos( TAlfRealPoint( KNum10, KNum20 ) );   // 1. replace without the flag
    parentLayout->UpdateChildrenLayout();           // 2. relayout siblings
    pos = alfVisual->Pos();                         // 3. check that the visual still has the original position
    STIF_ASSERT_EQUALS( 0.f, pos.iX.Target() );
    STIF_ASSERT_EQUALS( 0.f, pos.iY.Target() );
    alfVisual->SetFlag( EAlfVisualFlagManualPosition );// 4. set the manual position flag
    alfVisual->SetPos( TAlfRealPoint( KNum10, KNum20 ) );      // 5. replace again
    parentLayout->UpdateChildrenLayout();              // 6. relayout siblings
    pos = alfVisual->Pos();                            // 7. check the the new posotion is still valid
    STIF_ASSERT_EQUALS( KNum10+0.f, pos.iX.Target() );
    STIF_ASSERT_EQUALS( KNum20+0.f, pos.iY.Target() );
    
    // 3# AlfVisualFlagClipping
    alfVisual->SetFlag( AlfVisualFlagClipping );
    
    // 4# EAlfVisualFlagLayoutUpdateNotification
    parentLayout->UpdateChildrenLayout();
    
    // 5# EAlfVisualFlag3DProjection
    alfVisual->SetFlag( EAlfVisualFlag3DProjection );
    
    // 6# EAlfVisualFlagNoDepthOcclusion
    alfVisual->SetFlag( EAlfVisualFlagNoDepthOcclusion );
    
    // 7# EAlfVisualFlagLocalHorizon
    alfVisual->SetFlag( EAlfVisualFlagLocalHorizon );
    
    // 8# EAlfVisualFlagClipping
    alfVisual->SetFlag( EAlfVisualFlagClipping );
    
    // 8# EAlfVisualFlagHOriginCenter
    alfVisual->SetFlag( EAlfVisualFlagHOriginCenter );
    
    // 8# EAlfVisualFlagHOriginRight
    alfVisual->SetFlag( EAlfVisualFlagHOriginRight );
    
    // 8# EAlfVisualFlagVOriginCenter
    alfVisual->SetFlag( EAlfVisualFlagVOriginCenter );
    
    // 8# EAlfVisualFlagVOriginBottom
    alfVisual->SetFlag( EAlfVisualFlagVOriginBottom );
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetFlagsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetFlagsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetFlagsL, "TestCAlfVSetFlagsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetFlagsL );
    // Print to log file
    iLog->Log( KTestCAlfVSetFlagsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TUint flags = EAlfVisualFlagVOriginCenter +
                  EAlfVisualFlagNoDepthOcclusion +
                  EAlfVisualFlagClipping;
    alfVisual->SetFlags( flags );
    TUint retFlags = alfVisual->Flags();
    STIF_ASSERT_NOT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagVOriginCenter ) );
    STIF_ASSERT_NOT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagNoDepthOcclusion ) );
    STIF_ASSERT_NOT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagClipping ) );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVClearFlagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVClearFlagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVClearFlagL, "TestCAlfVClearFlagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVClearFlagL );
    // Print to log file
    iLog->Log( KTestCAlfVClearFlagL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TUint flags = EAlfVisualFlagVOriginCenter +
                  EAlfVisualFlagNoDepthOcclusion +
                  EAlfVisualFlagClipping;
    alfVisual->SetFlags( flags );
    alfVisual->ClearFlag( EAlfVisualFlagNoDepthOcclusion );
    alfVisual->ClearFlag( EAlfVisualFlagClipping );
    TUint retFlags = alfVisual->Flags();
    STIF_ASSERT_NOT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagVOriginCenter ) );
    STIF_ASSERT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagNoDepthOcclusion ) );
    STIF_ASSERT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagClipping ) );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVClearFlagsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVClearFlagsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVClearFlagsL, "TestCAlfVClearFlagsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVClearFlagsL );
    // Print to log file
    iLog->Log( KTestCAlfVClearFlagsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TUint flags = EAlfVisualFlagVOriginCenter +
                  EAlfVisualFlagNoDepthOcclusion +
                  EAlfVisualFlagClipping;
    alfVisual->SetFlags( flags );
    alfVisual->ClearFlags( EAlfVisualFlagNoDepthOcclusion + EAlfVisualFlagClipping );
    TUint retFlags = alfVisual->Flags();
    STIF_ASSERT_NOT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagVOriginCenter ) );
    STIF_ASSERT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagNoDepthOcclusion ) );
    STIF_ASSERT_EQUALS( KRefFlag, ( retFlags & EAlfVisualFlagClipping ) );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVFlagsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVFlagsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVFlagsL, "TestCAlfVFlagsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVFlagsL );
    // Print to log file
    iLog->Log( KTestCAlfVFlagsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    alfVisual->SetFlag( EAlfVisualFlagVOriginCenter );
    TUint retFlags = alfVisual->Flags();
    STIF_ASSERT_NOT_EQUALS( KRefFlag , ( retFlags & EAlfVisualFlagVOriginCenter ) );
    
    alfVisual->SetFlag( EAlfVisualFlagNoDepthOcclusion );
    retFlags = alfVisual->Flags();
    STIF_ASSERT_NOT_EQUALS( KRefFlag , ( retFlags & EAlfVisualFlagNoDepthOcclusion ) );
    
    alfVisual->SetFlag( EAlfVisualFlagClipping );
    retFlags = alfVisual->Flags();
    STIF_ASSERT_NOT_EQUALS( KRefFlag , ( retFlags & EAlfVisualFlagClipping ) );
    
    alfVisual->ClearFlag( EAlfVisualFlagNoDepthOcclusion );
    retFlags = alfVisual->Flags();
    STIF_ASSERT_EQUALS( KRefFlag , ( retFlags & EAlfVisualFlagNoDepthOcclusion ) );
    
    alfVisual->ClearFlag( EAlfVisualFlagVOriginCenter );
    retFlags = alfVisual->Flags();
    STIF_ASSERT_EQUALS( KRefFlag , ( retFlags & EAlfVisualFlagVOriginCenter ) );
    
    alfVisual->ClearFlag( EAlfVisualFlagClipping );
    retFlags = alfVisual->Flags();
    STIF_ASSERT_EQUALS( KRefFlag , ( retFlags & EAlfVisualFlagClipping ) );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVLayoutL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVLayoutL, "TestCAlfVLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVLayoutL );
    // Print to log file
    iLog->Log( KTestCAlfVLayoutL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    STIF_ASSERT_NULL( alfVisual->Layout() );
    
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    STIF_ASSERT_NOT_NULL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) ); // calls SetLayout()
    
    STIF_ASSERT_EQUALS( parentLayout, alfVisual->Layout() );
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVPosL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVPosL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVPosL, "TestCAlfVPosL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVPosL );
    // Print to log file
    iLog->Log( KTestCAlfVPosL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    TAlfRealPoint pos( KNum20, KNum30 );
    alfVisual->SetPos( pos );
    
    TAlfTimedPoint result = alfVisual->Pos();
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.ValueNow() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.Target() );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetPosL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetPosL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetPosL, "TestCAlfVSetPosL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetPosL );
    // Print to log file
    iLog->Log( KTestCAlfVSetPosL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    TAlfRealPoint pos( KNum20, KNum30 );
    alfVisual->SetPos( pos );
    
    TAlfTimedPoint result = alfVisual->Pos();
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.ValueNow() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.Target() );
    
    pos = TAlfRealPoint( KNum40, KNum50 );
    alfVisual->SetPos( pos, KNum10);
    
    result = alfVisual->Pos();
    STIF_ASSERT_NOT_EQUALS( KNum40+0.f, result.iX.ValueNow() );
    STIF_ASSERT_NOT_EQUALS( KNum50+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum40+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum50+0.f, result.iY.Target() );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSizeL, "TestCAlfVSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    TAlfRealSize size( KNum20, KNum30 );
    alfVisual->SetSize( size );
    
    TAlfTimedPoint result = alfVisual->Size();
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.ValueNow() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.Target() );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetSizeL, "TestCAlfVSetSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSetSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    TAlfRealSize size( KNum20, KNum30 );
    alfVisual->SetSize( size );
    
    TAlfTimedPoint result = alfVisual->Size();
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.ValueNow() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum20+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum30+0.f, result.iY.Target() );
    
    size = TAlfRealSize( KNum40, KNum50 );
    alfVisual->SetSize( size, KNum10);
    
    result = alfVisual->Size();
    STIF_ASSERT_NOT_EQUALS( KNum40+0.f, result.iX.ValueNow() );
    STIF_ASSERT_NOT_EQUALS( KNum50+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum40+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum50+0.f, result.iY.Target() );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVOpacityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVOpacityL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVOpacityL, "TestCAlfVOpacityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVOpacityL );
    // Print to log file
    iLog->Log( KTestCAlfVOpacityL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfTimedValue opacity = alfVisual->Opacity();
    STIF_ASSERT_EQUALS( 1.f, opacity.ValueNow() );
    STIF_ASSERT_EQUALS( 1.f, opacity.Target() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetOpacityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetOpacityL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetOpacityL, "TestCAlfVSetOpacityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetOpacityL );
    // Print to log file
    iLog->Log( KTestCAlfVSetOpacityL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    TAlfTimedValue opacity;
    opacity.SetValueNow( 0.2 );
    alfVisual->SetOpacity( opacity );
    
    TAlfTimedValue result = alfVisual->Opacity();
    STIF_ASSERT_EQUALS( 0.2f, result.ValueNow() );
    STIF_ASSERT_EQUALS( 0.2f, result.Target() );
    
    opacity.SetTarget( KReal5, KNum10 );
    alfVisual->SetOpacity( opacity );
    
    result = alfVisual->Opacity();
    STIF_ASSERT_NOT_EQUALS( KReal5, result.ValueNow() );
    STIF_ASSERT_EQUALS( KReal5, result.Target() );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVEnvL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVEnvL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVEnvL, "TestCAlfVEnvL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVEnvL );
    // Print to log file
    iLog->Log( KTestCAlfVEnvL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    STIF_ASSERT_EQUALS( iAlfEnv, &alfVisual->Env() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDisplayL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDisplayL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDisplayL, "TestCAlfVDisplayL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDisplayL );
    // Print to log file
    iLog->Log( KTestCAlfVDisplayL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->Display();
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVTagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVTagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVTagL, "TestCAlfVTagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVTagL );
    // Print to log file
    iLog->Log( KTestCAlfVTagL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    const TDesC8& tag = alfVisual->Tag();
    STIF_ASSERT_EQUALS( 0, tag.Length() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVUpdateChildrenLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVUpdateChildrenLayoutL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVUpdateChildrenLayoutL, "TestCAlfVUpdateChildrenLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVUpdateChildrenLayoutL );
    // Print to log file
    iLog->Log( KTestCAlfVUpdateChildrenLayoutL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->UpdateChildrenLayout();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDisplayRectTargetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDisplayRectTargetL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDisplayRectTargetL, "TestCAlfVDisplayRectTargetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDisplayRectTargetL );
    // Print to log file
    iLog->Log( KTestCAlfVDisplayRectTargetL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    // Create layout parent for testing purposes
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    STIF_ASSERT_NOT_NULL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) );
    parentLayout->SetFlag( EAlfVisualFlagManualLayout );
    parentLayout->SetPos( TAlfRealPoint( KNum40, KNum50 ) );
    parentLayout->SetSize( TAlfRealSize( KNum100, KNum120 ) );
    parentLayout->UpdateChildrenLayout();
    
    alfVisual->DisplayRectTarget();
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVLocalToDisplayL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVLocalToDisplayL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVLocalToDisplayL, "TestCAlfVLocalToDisplayL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVLocalToDisplayL );
    // Print to log file
    iLog->Log( KTestCAlfVLocalToDisplayL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    // Create layout parent for testing purposes
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    STIF_ASSERT_NOT_NULL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) );
    parentLayout->SetFlag( EAlfVisualFlagManualLayout );
    parentLayout->SetPos( TAlfRealPoint( KNum40, KNum50 ) );
    parentLayout->SetSize( TAlfRealSize( KNum100, KNum120 ) );
    parentLayout->UpdateChildrenLayout();
    
    iAlfDisplay->Roster().ShowL( *iAlfCtrlGroup );
    
    TAlfRealPoint result = alfVisual->LocalToDisplay( TPoint( KNum65, KNum88 ) );
    
    STIF_ASSERT_EQUALS( result.iX, KNum65+KNum40+0.f );
    STIF_ASSERT_EQUALS( result.iY, KNum88+KNum50+0.f );

    iAlfDisplay->Roster().Hide( *iAlfCtrlGroup );
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVLocalToDisplayWithRealPointL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVLocalToDisplayWithRealPointL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVLocalToDisplayWithRealPointL, "TestCAlfVLocalToDisplayWithRealPointL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVLocalToDisplayWithRealPointL );
    // Print to log file
    iLog->Log( KTestCAlfVLocalToDisplayWithRealPointL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    // Create layout parent for testing purposes
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    STIF_ASSERT_NOT_NULL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) );
    parentLayout->SetFlag( EAlfVisualFlagManualLayout );
    parentLayout->SetPos( TAlfRealPoint( KNum40, KNum50 ) );
    parentLayout->SetSize( TAlfRealSize( KNum100, KNum120 ) );
    parentLayout->UpdateChildrenLayout();
    
    TAlfRealPoint result = alfVisual->LocalToDisplay( TAlfRealPoint( KNum65, KNum88 ) );
    
    STIF_ASSERT_EQUALS( result.iX, KNum65+KNum40+0.f );
    STIF_ASSERT_EQUALS( result.iY, KNum88+KNum50+0.f );
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDisplayToLocalL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDisplayToLocalL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDisplayToLocalL, "TestCAlfVDisplayToLocalL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDisplayToLocalL );
    // Print to log file
    iLog->Log( KTestCAlfVDisplayToLocalL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    // Create layout parent for testing purposes
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    STIF_ASSERT_NOT_NULL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) );
    parentLayout->SetFlag( EAlfVisualFlagManualLayout );
    parentLayout->SetPos( TAlfRealPoint( KNum40, KNum50 ) );
    parentLayout->SetSize( TAlfRealSize( KNum100, KNum120 ) );
    parentLayout->UpdateChildrenLayout();
    
    iAlfDisplay->Roster().ShowL( *iAlfCtrlGroup );
    
    TPoint result = alfVisual->DisplayToLocal( TPoint( KNum10, KNum20 ) );
    
    STIF_ASSERT_EQUALS( result.iX, KNum10-KNum40 );
    STIF_ASSERT_EQUALS( result.iY, KNum20-KNum50 );
    
    iAlfDisplay->Roster().Hide( *iAlfCtrlGroup );
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDisplayToLocalWithRealPointL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDisplayToLocalWithRealPointL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDisplayToLocalWithRealPointL, "TestCAlfVDisplayToLocalWithRealPointL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDisplayToLocalWithRealPointL );
    // Print to log file
    iLog->Log( KTestCAlfVDisplayToLocalWithRealPointL );
    
    CAlfVisual* alfVisual = iAlfCtl->AppendVisualL( EAlfVisualTypeVisual );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    // Create layout parent for testing purposes
    CAlfLayout* parentLayout = iAlfCtl->AppendLayoutL( EAlfLayoutTypeDeck );
    CleanupStack::PushL( parentLayout );
    STIF_ASSERT_NOT_NULL( parentLayout );
    User::LeaveIfError( parentLayout->Append( alfVisual ) );
    parentLayout->SetFlag( EAlfVisualFlagManualLayout );
    parentLayout->SetPos( TAlfRealPoint( KNum40, KNum50 ) );
    parentLayout->SetSize( TAlfRealSize( KNum100, KNum120 ) );
    parentLayout->UpdateChildrenLayout();
    
    TAlfRealPoint result = alfVisual->DisplayToLocal( TAlfRealPoint( KNum10, KNum20 ) );
    
    STIF_ASSERT_EQUALS( result.iX, KNum10-KNum40+0.f );
    STIF_ASSERT_EQUALS( result.iY, KNum20-KNum50+0.f );
    
    CleanupStack::Pop( parentLayout );
    CleanupStack::Pop( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVEnableBrushesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVEnableBrushesL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVEnableBrushesL, "TestCAlfVEnableBrushesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVEnableBrushesL );
    // Print to log file
    iLog->Log( KTestCAlfVEnableBrushesL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EnableBrushesL();
    alfVisual->EnableBrushesL( EFalse );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVBrushesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVBrushesL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVBrushesL, "TestCAlfVBrushesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVBrushesL );
    // Print to log file
    iLog->Log( KTestCAlfVBrushesL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EnableBrushesL( EFalse );
    STIF_ASSERT_NULL( alfVisual->Brushes() );
    alfVisual->EnableBrushesL( ETrue );
    STIF_ASSERT_NOT_NULL( alfVisual->Brushes() );
    alfVisual->EnableBrushesL( EFalse );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMoveToFrontL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMoveToFrontL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMoveToFrontL, "TestCAlfVMoveToFrontL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMoveToFrontL );
    // Print to log file
    iLog->Log( KTestCAlfVMoveToFrontL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->MoveToFront();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVEnableTransformationL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVEnableTransformationL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVEnableTransformationL, "TestCAlfVEnableTransformationL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVEnableTransformationL );
    // Print to log file
    iLog->Log( KTestCAlfVEnableTransformationL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EnableTransformationL();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVTransformationL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVTransformationL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVTransformationL, "TestCAlfVTransformationL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVTransformationL );
    // Print to log file
    iLog->Log( KTestCAlfVTransformationL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EnableTransformationL();
    alfVisual->Transformation();
    alfVisual->EnableTransformationL( EFalse );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetPaddingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetPaddingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetPaddingL, "TestCAlfVSetPaddingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetPaddingL );
    // Print to log file
    iLog->Log( KTestCAlfVSetPaddingL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetPadding( KNum100 );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetPaddingWithPointL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetPaddingWithPointL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetPaddingWithPointL, "TestCAlfVSetPaddingWithPointL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetPaddingWithPointL );
    // Print to log file
    iLog->Log( KTestCAlfVSetPaddingWithPointL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TPoint point( KNum100, KNum50 );
    alfVisual->SetPadding( point );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetPaddingWithMetricL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetPaddingWithMetricL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetPaddingWithMetricL, "TestCAlfVSetPaddingWithMetricL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetPaddingWithMetricL );
    // Print to log file
    iLog->Log( KTestCAlfVSetPaddingWithMetricL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfBoxMetric boxMetric;
    alfVisual->SetPadding( boxMetric );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVPaddingInPixelsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVPaddingInPixelsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVPaddingInPixelsL, "TestCAlfVPaddingInPixelsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVPaddingInPixelsL );
    // Print to log file
    iLog->Log( KTestCAlfVPaddingInPixelsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfBoxMetric boxMetric;
    alfVisual->PaddingInPixels();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetTagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetTagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetTagL, "TestCAlfVSetTagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetTagL );
    // Print to log file
    iLog->Log( KTestCAlfVSetTagL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    _LIT8( KTagText, "Caption" );
    TBufC8<KNum20> text( KTagText );
    alfVisual->SetTagL( text );
    const TDesC8& tag = alfVisual->Tag();
    STIF_ASSERT_EQUALS( text.Length(), tag.Length() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVIsTaggedL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVIsTaggedL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVIsTaggedL, "TestCAlfVIsTaggedL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVIsTaggedL );
    // Print to log file
    iLog->Log( KTestCAlfVIsTaggedL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->IsTagged();
    STIF_ASSERT_FALSE( alfVisual->IsTagged() );
    _LIT8( KTagText, "Caption" );
    TBufC8<KNum20> text( KTagText );
    alfVisual->SetTagL( text );
    STIF_ASSERT_TRUE( alfVisual->IsTagged() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVFindTagL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVFindTagL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVFindTagL, "TestCAlfVFindTagL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVFindTagL );
    // Print to log file
    iLog->Log( KTestCAlfVFindTagL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    _LIT8( KTagText, "Caption" );
    TBufC8<KNum20> text( KTagText );
    alfVisual->SetTagL( text );
    _LIT8( KWhat, "What..." );
    STIF_ASSERT_NULL( alfVisual->FindTag( KWhat ) );
    STIF_ASSERT_EQUALS( alfVisual, alfVisual->FindTag( text ) );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVConstructWithMoreParamsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVConstructWithMoreParamsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVConstructWithMoreParamsL, "TestCAlfVConstructWithMoreParamsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVConstructWithMoreParamsL );
    // Print to log file
    iLog->Log( KTestCAlfVConstructWithMoreParamsL );
    
    // AlfVisualFactory::NewVisualL calls CAlfVisual::ConstructL with more params
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVCommsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVCommsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVCommsL, "TestCAlfVCommsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVCommsL );
    // Print to log file
    iLog->Log( KTestCAlfVCommsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->Comms();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDepthOffsetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDepthOffsetL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDepthOffsetL, "TestCAlfVDepthOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDepthOffsetL );
    // Print to log file
    iLog->Log( KTestCAlfVDepthOffsetL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfTimedValue timedValue;
    alfVisual->SetDepthOffset( timedValue );
    TAlfTimedValue result = alfVisual->DepthOffset();
    STIF_ASSERT_EQUALS( 0.f, result.ValueNow() );
    STIF_ASSERT_EQUALS( 0.f, result.Target() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetDepthOffsetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetDepthOffsetL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetDepthOffsetL, "TestCAlfVSetDepthOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetDepthOffsetL );
    // Print to log file
    iLog->Log( KTestCAlfVSetDepthOffsetL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    
    TAlfTimedValue offset;
    offset.SetValueNow( KReal6 );
    alfVisual->SetDepthOffset( offset );
    
    TAlfTimedValue result = alfVisual->DepthOffset();
    STIF_ASSERT_EQUALS( KReal6, result.ValueNow() );
    STIF_ASSERT_EQUALS( KReal6, result.Target() );
    
    offset.SetTarget( KReal5, KNum10 );
    alfVisual->SetDepthOffset( offset );
    
    result = alfVisual->DepthOffset();
    STIF_ASSERT_NOT_EQUALS( KReal5, result.ValueNow() );
    STIF_ASSERT_EQUALS( KReal5, result.Target() );
    
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVOwnerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVOwnerL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVOwnerL, "TestCAlfVOwnerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVOwnerL );
    // Print to log file
    iLog->Log( KTestCAlfVOwnerL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    STIF_ASSERT_EQUALS( iAlfCtl, &alfVisual->Owner() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetOwnerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetOwnerL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetOwnerL, "TestCAlfVSetOwnerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetOwnerL );
    // Print to log file
    iLog->Log( KTestCAlfVSetOwnerL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetOwner( *iAlfCtl );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetOriginL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetOriginL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetOriginL, "TestCAlfVSetOriginL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetOriginL );
    // Print to log file
    iLog->Log( KTestCAlfVSetOriginL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetOrigin( EAlfVisualHOriginLeft, EAlfVisualVOriginTop );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVHorizontalOriginL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVHorizontalOriginL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVHorizontalOriginL, "TestCAlfVHorizontalOriginL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVHorizontalOriginL );
    // Print to log file
    iLog->Log( KTestCAlfVHorizontalOriginL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetOrigin( EAlfVisualHOriginLeft, EAlfVisualVOriginTop );
    STIF_ASSERT_EQUALS( EAlfVisualHOriginLeft, alfVisual->HorizontalOrigin() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVVerticalOriginL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVVerticalOriginL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVVerticalOriginL, "TestCAlfVVerticalOriginL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVVerticalOriginL );
    // Print to log file
    iLog->Log( KTestCAlfVVerticalOriginL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetOrigin( EAlfVisualHOriginLeft, EAlfVisualVOriginTop );
    STIF_ASSERT_EQUALS( EAlfVisualVOriginTop, alfVisual->VerticalOrigin() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVAlignByOriginL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVAlignByOriginL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVAlignByOriginL, "TestCAlfVAlignByOriginL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVAlignByOriginL );
    // Print to log file
    iLog->Log( KTestCAlfVAlignByOriginL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealRect realRect( TRect( 0, 0, 2, 2 ) );
    alfVisual->AlignByOrigin( realRect );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetMinSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetMinSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetMinSizeL, "TestCAlfVSetMinSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetMinSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSetMinSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TSize size( KNum20, KNum30 );
    alfVisual->SetMinSize( size );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetMinSizeWithRealSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetMinSizeWithRealSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetMinSizeWithRealSizeL, "TestCAlfVSetMinSizeWithRealSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetMinSizeWithRealSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSetMinSizeWithRealSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealSize size( KReal1, KReal2 );
    alfVisual->SetMinSize( size );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMinSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMinSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMinSizeL, "TestCAlfVMinSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMinSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVMinSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TSize minSize( KNum20, KNum30 );
    alfVisual->SetMinSize( minSize );
    TSize result = alfVisual->MinSize();
    STIF_ASSERT_EQUALS( minSize.iHeight , result.iHeight );
    STIF_ASSERT_EQUALS( minSize.iWidth , result.iWidth );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMinSizeInBaseUnitsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMinSizeInBaseUnitsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMinSizeInBaseUnitsL, "TestCAlfVMinSizeInBaseUnitsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMinSizeInBaseUnitsL );
    // Print to log file
    iLog->Log( KTestCAlfVMinSizeInBaseUnitsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealSize minSize( KReal1, KReal2 );
    alfVisual->SetMinSize( minSize );
    TAlfRealSize result = alfVisual->MinSizeInBaseUnits();
    STIF_ASSERT_EQUALS( minSize.iWidth, result.iWidth );
    STIF_ASSERT_EQUALS( minSize.iHeight, result.iHeight );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetMaxSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetMaxSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetMaxSizeL, "TestCAlfVSetMaxSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetMaxSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSetMaxSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TSize maxSize( KNum20, KNum30 );
    alfVisual->SetMaxSize( maxSize );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetMaxSizeWithRealSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetMaxSizeWithRealSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetMaxSizeWithRealSizeL, "TestCAlfVSetMaxSizeWithRealSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetMaxSizeWithRealSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSetMaxSizeWithRealSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealSize maxSize( KReal1, KReal2 );
    alfVisual->SetMaxSize( maxSize );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMaxSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMaxSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMaxSizeL, "TestCAlfVMaxSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMaxSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVMaxSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TSize maxSize( KNum20, KNum30 );
    alfVisual->SetMaxSize( maxSize );
    TSize result = alfVisual->MaxSize();
    STIF_ASSERT_EQUALS( maxSize.iHeight , result.iHeight );
    STIF_ASSERT_EQUALS( maxSize.iWidth , result.iWidth );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMaxSizeInBaseUnitsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMaxSizeInBaseUnitsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMaxSizeInBaseUnitsL, "TestCAlfVMaxSizeInBaseUnitsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMaxSizeInBaseUnitsL );
    // Print to log file
    iLog->Log( KTestCAlfVMaxSizeInBaseUnitsL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealSize maxSize( KReal1, KReal2 );
    alfVisual->SetMaxSize( maxSize );
    TAlfRealSize result = alfVisual->MaxSizeInBaseUnits();
    STIF_ASSERT_EQUALS( maxSize.iWidth, result.iWidth );
    STIF_ASSERT_EQUALS( maxSize.iHeight, result.iHeight );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetRectL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetRectL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetRectL, "TestCAlfVSetRectL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetRectL );
    // Print to log file
    iLog->Log( KTestCAlfVSetRectL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealRect rect ( TRect( KNum20, KNum30, KNum40, KNum50 ) );
    alfVisual->SetFlag( EAlfVisualFlagManualLayout );
    alfVisual->SetRect( rect );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMoveL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMoveL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMoveL, "TestCAlfVMoveL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMoveL );
    // Print to log file
    iLog->Log( KTestCAlfVMoveL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TPoint offset( KNum20, KNum20 );
    alfVisual->SetFlag( EAlfVisualFlagManualLayout );
    alfVisual->Move( offset );
    TAlfTimedPoint visualPos = alfVisual->Pos();
    STIF_ASSERT_EQUALS( visualPos.iX.Target(), offset.iX+0.f );
    STIF_ASSERT_EQUALS( visualPos.iY.Target(), offset.iY+0.f );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMoveWithRealPointL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMoveWithRealPointL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMoveWithRealPointL, "TestCAlfVMoveWithRealPointL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMoveWithRealPointL );
    // Print to log file
    iLog->Log( KTestCAlfVMoveWithRealPointL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealPoint offset( KReal1, KReal7 );
    alfVisual->SetFlag( EAlfVisualFlagManualLayout );
    alfVisual->Move( offset );
    TAlfTimedPoint visualPos = alfVisual->Pos();
    STIF_ASSERT_EQUALS( visualPos.iX.Target(), offset.iX );
    STIF_ASSERT_EQUALS( visualPos.iY.Target(), offset.iY );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVMimicL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVMimicL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVMimicL, "TestCAlfVMimicL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVMimicL );
    // Print to log file
    iLog->Log( KTestCAlfVMimicL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetFlag( EAlfVisualFlagManualLayout );
    alfVisual->Mimic( *alfVisual );
    CleanupStack::PopAndDestroy( alfVisual );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetCenteredPosAndSizeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetCenteredPosAndSizeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetCenteredPosAndSizeL, "TestCAlfVSetCenteredPosAndSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetCenteredPosAndSizeL );
    // Print to log file
    iLog->Log( KTestCAlfVSetCenteredPosAndSizeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TPoint centerPoint( KNum20, KNum30 );
    TSize size( KNum40, KNum50 );
    alfVisual->SetFlag( EAlfVisualFlagManualLayout );
    alfVisual->SetCenteredPosAndSize( centerPoint, size );
    TAlfTimedPoint visualPos = alfVisual->Pos();
    TAlfTimedPoint visualSize = alfVisual->Size();
    STIF_ASSERT_EQUALS( visualPos.iX.Target() , centerPoint.iX-(size.iWidth/2.f) );
    STIF_ASSERT_EQUALS( visualPos.iY.Target() , centerPoint.iY-(size.iHeight/2.f) );
    STIF_ASSERT_EQUALS( visualSize.iX.Target(), size.iWidth+0.f );
    STIF_ASSERT_EQUALS( visualSize.iY.Target(), size.iHeight+0.f );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetCenteredPosAndSizeWithRealL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetCenteredPosAndSizeWithRealL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetCenteredPosAndSizeWithRealL, "TestCAlfVSetCenteredPosAndSizeWithRealL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetCenteredPosAndSizeWithRealL );
    // Print to log file
    iLog->Log( KTestCAlfVSetCenteredPosAndSizeWithRealL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfRealPoint centerPoint( KReal1, KReal2 );
    TAlfRealSize size( KReal3, KReal4 );
    alfVisual->SetFlag( EAlfVisualFlagManualLayout );
    alfVisual->SetCenteredPosAndSize( centerPoint, size );
    TAlfTimedPoint visualPos = alfVisual->Pos();
    TAlfTimedPoint visualSize = alfVisual->Size();
    STIF_ASSERT_EQUALS( visualPos.iX.Target(), centerPoint.iX-(size.iWidth/2.f) );
    STIF_ASSERT_EQUALS( visualPos.iY.Target(), centerPoint.iY-(size.iHeight/2.f) );
    STIF_ASSERT_EQUALS( visualSize.iX.Target(), size.iWidth );
    STIF_ASSERT_EQUALS( visualSize.iY.Target(), size.iHeight );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDisplayRectL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDisplayRectL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDisplayRectL, "TestCAlfVDisplayRectL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDisplayRectL );
    // Print to log file
    iLog->Log( KTestCAlfVDisplayRectL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->DisplayRect();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVEffectiveOpacityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVEffectiveOpacityL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVEffectiveOpacityL, "TestCAlfVEffectiveOpacityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVEffectiveOpacityL );
    // Print to log file
    iLog->Log( KTestCAlfVEffectiveOpacityL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EffectiveOpacity();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetClippingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetClippingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetClippingL, "TestCAlfVSetClippingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetClippingL );
    // Print to log file
    iLog->Log( KTestCAlfVSetClippingL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetClipping();
    alfVisual->SetClipping( EFalse );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVClippingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVClippingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVClippingL, "TestCAlfVClippingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVClippingL );
    // Print to log file
    iLog->Log( KTestCAlfVClippingL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetClipping();
    STIF_ASSERT_TRUE( alfVisual->Clipping() );
    alfVisual->SetClipping( EFalse );
    STIF_ASSERT_FALSE( alfVisual->Clipping() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVPaddingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVPaddingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVPaddingL, "TestCAlfVPaddingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVPaddingL );
    // Print to log file
    iLog->Log( KTestCAlfVPaddingL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TPoint result = alfVisual->Padding();
    STIF_ASSERT_EQUALS( 0, result.iX );
    STIF_ASSERT_EQUALS( 0, result.iY );
    alfVisual->SetPadding( KNum10 );
    result = alfVisual->Padding();
    STIF_ASSERT_EQUALS( KNum10, result.iX );
    STIF_ASSERT_EQUALS( KNum10, result.iY );
    alfVisual->SetPadding( TPoint( KNum10, KNum20 ) );
    result = alfVisual->Padding();
    STIF_ASSERT_EQUALS( KNum10, result.iX );
    STIF_ASSERT_EQUALS( KNum20, result.iY );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVPaddingAsMetricL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVPaddingAsMetricL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVPaddingAsMetricL, "TestCAlfVPaddingAsMetricL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVPaddingAsMetricL );
    // Print to log file
    iLog->Log( KTestCAlfVPaddingAsMetricL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->PaddingAsMetric();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVHorizontalPaddingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVHorizontalPaddingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVHorizontalPaddingL, "TestCAlfVHorizontalPaddingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVHorizontalPaddingL );
    // Print to log file
    iLog->Log( KTestCAlfVHorizontalPaddingL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TInt result = alfVisual->HorizontalPadding();
    STIF_ASSERT_EQUALS( 0, result );
    alfVisual->SetPadding( KNum10 );
    result = alfVisual->HorizontalPadding();
    STIF_ASSERT_EQUALS( KNum10, result );
    alfVisual->SetPadding( TPoint( KNum10, KNum20 ) );
    result = alfVisual->HorizontalPadding();
    STIF_ASSERT_EQUALS( KNum10, result );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVVerticalPaddingL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVVerticalPaddingL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVVerticalPaddingL, "TestCAlfVVerticalPaddingL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVVerticalPaddingL );
    // Print to log file
    iLog->Log( KTestCAlfVVerticalPaddingL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TInt result = alfVisual->VerticalPadding();
    STIF_ASSERT_EQUALS( 0, result );
    alfVisual->SetPadding( KNum10 );
    result = alfVisual->VerticalPadding();
    STIF_ASSERT_EQUALS( KNum10, result );
    alfVisual->SetPadding( TPoint( KNum10, KNum20 ) );
    result = alfVisual->VerticalPadding();
    STIF_ASSERT_EQUALS( KNum20, result );
    alfVisual->VerticalPadding();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetPosWithTimedPointL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetPosWithTimedPointL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetPosWithTimedPointL, "TestCAlfVSetPosWithTimedPointL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetPosWithTimedPointL );
    // Print to log file
    iLog->Log( KTestCAlfVSetPosWithTimedPointL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfTimedPoint point;
    point.iX.SetValueNow( KNum20 );
    point.iY.SetValueNow( KNum30 );
    point.iX.SetTarget( KNum200, KNum10 );
    point.iY.SetTarget( KNum300, KNum10 );
    alfVisual->SetPos( point );
    TAlfTimedPoint result = alfVisual->Pos();
    STIF_ASSERT_NOT_EQUALS( KNum200+0.f, result.iX.ValueNow() );
    STIF_ASSERT_NOT_EQUALS( KNum300+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum200+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum300+0.f, result.iY.Target() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetSizeWithTimedPointL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetSizeWithTimedPointL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetSizeWithTimedPointL, "TestCAlfVSetSizeWithTimedPointL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetSizeWithTimedPointL );
    // Print to log file
    iLog->Log( KTestCAlfVSetSizeWithTimedPointL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TAlfTimedPoint size;
    size.iX.SetValueNow( KNum20 );
    size.iY.SetValueNow( KNum30 );
    size.iX.SetTarget( KNum200, KNum10 );
    size.iY.SetTarget( KNum300, KNum10 );
    alfVisual->SetSize( size );
    TAlfTimedPoint result = alfVisual->Size();
    STIF_ASSERT_NOT_EQUALS( KNum200+0.f, result.iX.ValueNow() );
    STIF_ASSERT_NOT_EQUALS( KNum300+0.f, result.iY.ValueNow() );
    STIF_ASSERT_EQUALS( KNum200+0.f, result.iX.Target() );
    STIF_ASSERT_EQUALS( KNum300+0.f, result.iY.Target() );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVCopyValuesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVCopyValuesL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVCopyValuesL, "TestCAlfVCopyValuesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVCopyValuesL );
    // Print to log file
    iLog->Log( KTestCAlfVCopyValuesL );
    
    CAlfVisual* visual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( visual );
    STIF_ASSERT_NOT_NULL( visual );
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->CopyValues( *visual );
    CleanupStack::PopAndDestroy( alfVisual );
    CleanupStack::PopAndDestroy( visual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVPauseL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVPauseL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVPauseL, "TestCAlfVPauseL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVPauseL );
    // Print to log file
    iLog->Log( KTestCAlfVPauseL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->Pause();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVResumeL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVResumeL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVResumeL, "TestCAlfVResumeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVResumeL );
    // Print to log file
    iLog->Log( KTestCAlfVResumeL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->Resume();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDoRemoveAndDestroyAllDL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDoRemoveAndDestroyAllDL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDoRemoveAndDestroyAllDL, "TestCAlfVDoRemoveAndDestroyAllDL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDoRemoveAndDestroyAllDL );
    // Print to log file
    iLog->Log( KTestCAlfVDoRemoveAndDestroyAllDL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->DoRemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVSetTactileFeedbackL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVSetTactileFeedbackL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVSetTactileFeedbackL, "TestCAlfVSetTactileFeedbackL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVSetTactileFeedbackL );
    // Print to log file
    iLog->Log( KTestCAlfVSetTactileFeedbackL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->SetTactileFeedbackL( 0 , 0 );
    alfVisual->SetTactileFeedbackL( 1 , 1 );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVHasTactileFeedbackL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVHasTactileFeedbackL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVHasTactileFeedbackL, "TestCAlfVHasTactileFeedbackL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVHasTactileFeedbackL );
    // Print to log file
    iLog->Log( KTestCAlfVHasTactileFeedbackL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback() );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 0 ) );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 1 ) );
    alfVisual->SetTactileFeedbackL( 0 , 0 );
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback() );
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback( 0 ) );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 1 ) );
    alfVisual->RemoveTactileFeedback();
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback() );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 0 ) );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 1 ) );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVRemoveTactileFeedbackL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVRemoveTactileFeedbackL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVRemoveTactileFeedbackL, "TestCAlfVRemoveTactileFeedbackL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVRemoveTactileFeedbackL );
    // Print to log file
    iLog->Log( KTestCAlfVRemoveTactileFeedbackL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback() );
    alfVisual->RemoveTactileFeedback(); // do nothing
    alfVisual->RemoveTactileFeedback( 0 ); // do nothing
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback() );
    alfVisual->SetTactileFeedbackL( 0 , 0 ); // add 0
    alfVisual->SetTactileFeedbackL( 1 , 1 ); // add 1
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback( 0 ) );
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback( 1 ) );
    alfVisual->RemoveTactileFeedback( 0 ); // remove 0
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 0 ) );
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback( 1 ) );
    alfVisual->SetTactileFeedbackL( 0 , 2 ); // add 0
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback( 1 ) );
    STIF_ASSERT_TRUE( alfVisual->HasTactileFeedback( 0 ) );
    alfVisual->RemoveTactileFeedback(); // remove all
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 0 ) );
    STIF_ASSERT_TRUE( !alfVisual->HasTactileFeedback( 1 ) );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVEnableDropShadowL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVEnableDropShadowL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVEnableDropShadowL, "TestCAlfVEnableDropShadowL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVEnableDropShadowL );
    // Print to log file
    iLog->Log( KTestCAlfVEnableDropShadowL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EnableDropShadowL();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVDropShadowHandlerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVDropShadowHandlerL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVDropShadowHandlerL, "TestCAlfVDropShadowHandlerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVDropShadowHandlerL );
    // Print to log file
    iLog->Log( KTestCAlfVDropShadowHandlerL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
                    EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    alfVisual->EnableDropShadowL();
    alfVisual->DropShadowHandler();
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestCAlfVPropertyOwnerExtensionL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestCAlfVPropertyOwnerExtensionL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestCAlfVPropertyOwnerExtensionL, "TestCAlfVPropertyOwnerExtensionL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestCAlfVPropertyOwnerExtensionL );
    // Print to log file
    iLog->Log( KTestCAlfVPropertyOwnerExtensionL );
    
    CTestAlfVisual* alfVisual = new ( ELeave ) CTestAlfVisual;
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    TBool inFocusChain( EFalse );
    TAny* pInFocusChain = &inFocusChain;
    alfVisual->PropertyOwnerExtension( KUidAlfPropOwnerExtControlFocusChainChanged, &pInFocusChain );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }


//  [End of File]
