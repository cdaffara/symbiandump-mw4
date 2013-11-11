/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CAlfPerfAppTestCaseFactory implementation.
*
*/


#include "alfperfapptestcasefactory.h"

#include "alfperfappframeworktestcase.h"
#include "alfperfappimagetestcase.h"
#include "alfperfapplayouttestcase.h"
#include "alfperfapptexttestcase.h"
#include "alfperfappcovertestcase.h"
#include "alfperfappsuitetestcase.h"
#include "alfperfappgallerytestcase.h"
#include "alfperfappgallerytestcase.h"
#include "alfperfappavkontestcase.h"

// ============================ MEMBER FUNCTIONS ===============================

CAlfPerfAppTestCaseFactory* CAlfPerfAppTestCaseFactory::NewL()
    {
    CAlfPerfAppTestCaseFactory* self = new (ELeave) CAlfPerfAppTestCaseFactory;
    return self;
    }

CAlfPerfAppTestCaseFactory::~CAlfPerfAppTestCaseFactory()
    {
    }

// -----------------------------------------------------------------------------
// Creates test case corresponding to the aCaseId.
//  - If the case is a sequence case (marked by negative case id), pass the sequence
//    index to the case. Otherwise set it to 0 (to mark a non-sequence case)
//  - This is done because same cases can be called with or without sequence
//    functionality
// -----------------------------------------------------------------------------
//
CAlfPerfAppTestCase* CAlfPerfAppTestCaseFactory::CreateL( TInt aCaseId, TInt aSequenceIndex )
    {
    TInt index = 0;
    if(aCaseId < 0) // If this is a sequence case
        {
        aCaseId *= -1;
        index = aSequenceIndex;
        }
    
    if ( CAlfPerfAppFrameworkTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppFrameworkTestCase::NewL( aCaseId, index);
        }
        
    if ( CAlfPerfAppImageTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppImageTestCase::NewL( aCaseId, index );
        }

    if ( CAlfPerfAppLayoutTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppLayoutTestCase::NewL( aCaseId, index );
        }
        
    if ( CAlfPerfAppTextTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppTextTestCase::NewL( aCaseId, index );
        }   
    if ( CAlfPerfAppCoverTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppCoverTestCase::NewL( aCaseId, index );
        }

    if ( CAlfPerfAppSuiteTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppSuiteTestCase::NewL( aCaseId, index );
        }    

    if ( CAlfPerfAppGalleryTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppGalleryTestCase::NewL( aCaseId, index );
        }
    
    if ( CAlfPerfAppAvkonTestCase::IsSupported( aCaseId ) )
        {
        return CAlfPerfAppAvkonTestCase::NewL( aCaseId, index );
        }
    
    User::Leave( KErrNotSupported );
    return NULL;
    }

CAlfPerfAppTestCaseFactory::CAlfPerfAppTestCaseFactory()
    {
    }

