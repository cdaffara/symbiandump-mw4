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
* Description:  test functions for alfpropertyowner.h
*
*/



// [INCLUDE FILES]
#include <alf/alfpropertyowner.h>
#include <alf/alfvisual.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KFooIntValue = 23;
const TInt KFooIntValue2 = 19;

_LIT8( KFooName, "FooProperty" );
_LIT8( KFooName2, "Bar" );
_LIT8( KFooName3, "listbox.opacity" );
_LIT( KFooValue, "Hello world!" );
_LIT( KFooValue2, "My test string... This will be stored as a property value." );


// FORWARD DECLARATION
// class CTestPropertyOwner for testing CAlfPropertyOwner
class CTestPropertyOwner : public CAlfPropertyOwner
    {
public:
    void TestPropertyOwnerExtension()
        {
        TBool inFocusChain( EFalse );
        TAny* pInFocusChain = &inFocusChain;
        CAlfPropertyOwner::PropertyOwnerExtension( KUidAlfPropOwnerExtControlFocusChainChanged, &pInFocusChain );
        }
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerConstructorL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerConstructorL, "TestPropOwnerConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerConstructorL );
    // Print to log file
    iLog->Log( KTestPropOwnerConstructorL );
    
    CAlfPropertyOwner* owner = new ( ELeave ) CAlfPropertyOwner();
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    CleanupStack::PopAndDestroy( owner );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerPropertyRemoveL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerPropertyRemoveL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerPropertyRemoveL, "TestPropOwnerPropertyRemoveL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerPropertyRemoveL );
    // Print to log file
    iLog->Log( KTestPropOwnerPropertyRemoveL );
    
    CAlfPropertyOwner* owner = new ( ELeave ) CAlfVisual;
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    owner->PropertySetIntegerL( KFooName, 0 );
    owner->PropertySetIntegerL( KFooName2, 0 );
    owner->PropertySetIntegerL( KFooName3, 0 );
    TBool success = owner->PropertyFindInteger( KFooName );
    STIF_ASSERT_TRUE( success );
    success = owner->PropertyRemove( KFooName );
    STIF_ASSERT_TRUE( success );
    CleanupStack::PopAndDestroy( owner );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerPropertyClearL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerPropertyClearL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerPropertyClearL, "TestPropOwnerPropertyClearL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerPropertyClearL );
    // Print to log file
    iLog->Log( KTestPropOwnerPropertyClearL );
    
    CAlfPropertyOwner* owner = new ( ELeave ) CAlfVisual;
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    owner->PropertySetIntegerL( KFooName, 0 );
    owner->PropertySetIntegerL( KFooName2, 0 );
    owner->PropertySetIntegerL( KFooName3, 0 );
    
    owner->PropertyClear();
       
    STIF_ASSERT_TRUE( !owner->PropertyFindInteger( KFooName ) ); 
    STIF_ASSERT_TRUE( !owner->PropertyFindInteger( KFooName2 ) ); 
    STIF_ASSERT_TRUE( !owner->PropertyFindInteger( KFooName3 ) );

    CleanupStack::PopAndDestroy( owner );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerPropertyIntegerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerPropertyIntegerL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerPropertyIntegerL, "TestPropOwnerPropertyIntegerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerPropertyIntegerL );
    // Print to log file
    iLog->Log( KTestPropOwnerPropertyIntegerL );
    
    CAlfPropertyOwner* owner = new ( ELeave ) CAlfVisual;
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    
    TInt ret = 0;
    
    // Set initial value.
    owner->PropertySetIntegerL( KFooName, KFooIntValue );
    TBool success = owner->PropertyFindInteger(KFooName, &ret);
    STIF_ASSERT_TRUE( success );
    STIF_ASSERT_EQUALS( KFooIntValue, ret );
    
    // Set a new value.
    owner->PropertySetIntegerL( KFooName, KFooIntValue2 );
    success = owner->PropertyFindInteger(KFooName, &ret);
    STIF_ASSERT_TRUE( success );
    STIF_ASSERT_EQUALS( KFooIntValue2, ret );
    
    STIF_ASSERT_TRUE( owner->PropertyRemove( KFooName ) );
    
    STIF_ASSERT_TRUE( !owner->PropertyFindInteger( KFooName ) );
    
    CleanupStack::PopAndDestroy( owner );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerPropertyStringL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerPropertyStringL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerPropertyStringL, "TestPropOwnerPropertyStringL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerPropertyStringL );
    // Print to log file
    iLog->Log( KTestPropOwnerPropertyStringL );
    
    CAlfPropertyOwner* owner = new ( ELeave ) CAlfVisual;
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    
    TDesC* ret = NULL;
    
    owner->PropertySetStringL( KFooName, KFooValue2 );
    owner->PropertySetStringL( KFooName2, KFooValue );
    owner->PropertySetStringL( KFooName3, KFooValue );
    
    TBool success = owner->PropertyFindString( KFooName, &ret );
    
    STIF_ASSERT_TRUE( success );
    
    success = *ret == KFooValue2;
    STIF_ASSERT_TRUE( success );
    
    CleanupStack::PopAndDestroy( owner );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerCopyPropertyL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerCopyPropertyL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerCopyPropertyL, "TestPropOwnerCopyPropertyL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerCopyPropertyL );
    // Print to log file
    iLog->Log( KTestPropOwnerCopyPropertyL );
    
    CAlfPropertyOwner* originalOwner = new ( ELeave ) CAlfPropertyOwner();
    CleanupStack::PushL( originalOwner );
    STIF_ASSERT_NOT_NULL( originalOwner );
    originalOwner->PropertySetStringL( KFooName, KFooValue );
    CAlfPropertyOwner* owner = new ( ELeave ) CAlfPropertyOwner();
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    owner->CopyProperty( *originalOwner, KFooName );
    CleanupStack::PopAndDestroy( owner );
    CleanupStack::PopAndDestroy( originalOwner );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestPropOwnerPropertyOwnerExtensionL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestPropOwnerPropertyOwnerExtensionL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestPropOwnerPropertyOwnerExtensionL, "TestPropOwnerPropertyOwnerExtensionL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestPropOwnerPropertyOwnerExtensionL );
    // Print to log file
    iLog->Log( KTestPropOwnerPropertyOwnerExtensionL );
    
    CTestPropertyOwner* owner = new ( ELeave ) CTestPropertyOwner;
    CleanupStack::PushL( owner );
    STIF_ASSERT_NOT_NULL( owner );
    owner->TestPropertyOwnerExtension();
    CleanupStack::PopAndDestroy( owner );
    
    return KErrNone;
    }


//  [End of File]
