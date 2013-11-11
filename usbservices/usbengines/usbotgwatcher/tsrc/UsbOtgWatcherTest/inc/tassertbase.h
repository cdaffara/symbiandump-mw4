/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef TASSERTBASE_H
#define TASSERTBASE_H

#include <StifTestModule.h>
#include <StifLogger.h>

class TAssertBase
    {
public:
    // Pointer to logger
    // Not own
    CStifLogger * iLog;

    // Test result
    TTestResult iResult;
    
    void SetStifLogger(CStifLogger* aLog)
        {
        iLog = aLog;
        }
    
    TBool IsTestResultNoneError()
        {
        return iResult.iResult == KErrNone;
        }
    };

#endif //ASSERTBASE_H
