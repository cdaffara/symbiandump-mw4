/**
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
* Description: CTestTimer class declaration
*
*/

#ifndef TESTTIMER_H_
#define TESTTIMER_H_

#include <e32base.h>
#include <e32std.h>

class CTmServerTest;

class CTestTimer:public CActive
    {
public:
    static CTestTimer* NewL(CTmServerTest& aTmServerTest);
    ~CTestTimer();
    void After(const TInt aDuration);
    
private:
    CTestTimer(CTmServerTest& aTmServerTest);
    
private: // From CActive
    void RunL();
    void DoCancel();

private:
    RTimer iTimer;
    CTmServerTest& iTmServerTest;
    };

#endif //TESTTIMER_H_
