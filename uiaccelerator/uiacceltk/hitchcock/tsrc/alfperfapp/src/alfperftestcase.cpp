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
* Description:  alfperfapp model class function implementation.
*
*/

#include <eikenv.h>
#include "alfperfapptestcase.h"

TTestCaseSpecificResultText CAlfPerfAppTestCase::getCaseSpecificResultL()
    {
        TTestCaseSpecificResultText emptyResult = 0;
        return emptyResult;
    }

CEikonEnv* CAlfPerfAppTestCase::EikonEnv()
    {
    return iEnv;
    }

CAlfPerfAppTestCase::CAlfPerfAppTestCase(TInt aSequenceIndex)
    {
    iEnv = CEikonEnv::Static();
    iSequenceIndex = aSequenceIndex;
    }

TInt CAlfPerfAppTestCase::SequenceIndex()
    {
    return iSequenceIndex;
    }
