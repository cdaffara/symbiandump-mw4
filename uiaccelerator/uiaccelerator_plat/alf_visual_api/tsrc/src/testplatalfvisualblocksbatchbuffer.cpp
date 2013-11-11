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
#include <alf/alfbatchbuffer.h>
#include <alf/alfborderbrush.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KBufferSize = 2222;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBBSetAutoFlushMode
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBBSetAutoFlushMode( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBBSetAutoFlushModeL, "TestBBSetAutoFlushModeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBBSetAutoFlushModeL );
    // Print to log file
    iLog->Log( KTestBBSetAutoFlushModeL );
    
    CAlfBatchBuffer* buffer = &( iAlfEnv->BatchBufferHandler() );
    buffer->SetAutoFlushMode( EAlfAutoFlushInstant );
    STIF_ASSERT_EQUALS( EAlfAutoFlushInstant, buffer->AutoFlushMode() );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBBAutoFlushMode
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBBAutoFlushMode( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBBAutoFlushModeL, "TestBBAutoFlushModeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBBAutoFlushModeL );
    // Print to log file
    iLog->Log( KTestBBAutoFlushModeL );

    CAlfBatchBuffer* buffer = &( iAlfEnv->BatchBufferHandler() );
    TAlfAutoFlushMode defaultMode = buffer->AutoFlushMode();
    STIF_ASSERT_EQUALS( defaultMode, EAlfAutoFlushDeferred );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBBFlushBatchBufferL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBBFlushBatchBufferL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBBFlushBatchBufferL, "TestBBFlushBatchBufferL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBBFlushBatchBufferL );
    // Print to log file
    iLog->Log( KTestBBFlushBatchBufferL );

    CAlfBatchBuffer* buffer = &( iAlfEnv->BatchBufferHandler() );

    buffer->FlushBatchBuffer();
    STIF_ASSERT_EQUALS( 0, buffer->GetBatchBufferInfo( EAlfBatchBufferInfoCommandsAllocate ) );
     
    // create a command on the buffer
    CAlfBorderBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 2,2,2,2 );
    brush->SetColor( KRgbRed ); // this should leave the command on the buffer
    
    STIF_ASSERT_NOT_EQUALS( 0, buffer->GetBatchBufferInfo( EAlfBatchBufferInfoCommandsAllocate ) );
     
    buffer->FlushBatchBuffer();
    STIF_ASSERT_EQUALS( 0, buffer->GetBatchBufferInfo( EAlfBatchBufferInfoCommandsAllocate ) );
     
    // clean up
    CleanupStack::PopAndDestroy( brush );
    
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBBSetMaxBatchBufferSize
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBBSetMaxBatchBufferSize( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBBSetMaxBatchBufferSizeL, "TestBBSetMaxBatchBufferSizeL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBBSetMaxBatchBufferSizeL );
    // Print to log file
    iLog->Log( KTestBBSetMaxBatchBufferSizeL );

    CAlfBatchBuffer* buffer = &( iAlfEnv->BatchBufferHandler() );
    buffer->SetMaxBatchBufferSize( KBufferSize );
    STIF_ASSERT_EQUALS( KBufferSize, buffer->GetBatchBufferInfo( EAlfBatchBufferInfoMaxSize ) );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBBGetBatchBufferInfoL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBBGetBatchBufferInfoL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBBGetBatchBufferInfoL, "TestBBGetBatchBufferInfoL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBBGetBatchBufferInfoL );
    // Print to log file
    iLog->Log( KTestBBGetBatchBufferInfoL );

    CAlfBatchBuffer* buffer = &( iAlfEnv->BatchBufferHandler() );
    // Change some of the values
    buffer->SetMaxBatchBufferSize( KBufferSize );
    
    // create a command on the buffer
    CAlfBorderBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 2,2,2,2 );
    brush->SetColor( KRgbRed ); // this should leave the command on the buffer
    
    // Get the data
    TInt gotDefaultSize = buffer->GetBatchBufferInfo( EAlfBatchBufferInfoDefaultBufferSize );
    TInt gotMaxSize = buffer->GetBatchBufferInfo( EAlfBatchBufferInfoMaxSize );
    TInt gotCurrentlyAllocatedSize = buffer->GetBatchBufferInfo( EAlfBatchBufferInfoCurrentlyAllocated );
    TInt gotCommandsAllocateSize = buffer->GetBatchBufferInfo( EAlfBatchBufferInfoCommandsAllocate );
    
    // Check;
    ASSERT( gotDefaultSize > 0 ); // this is really the only check we can do
    STIF_ASSERT_EQUALS( gotMaxSize, KBufferSize );
    STIF_ASSERT_EQUALS( gotCurrentlyAllocatedSize ,  gotDefaultSize ); // one command should fit in the default buffer
    ASSERT( gotCommandsAllocateSize > 0 ); 
     
    // clean up
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }


//  [End of File]
