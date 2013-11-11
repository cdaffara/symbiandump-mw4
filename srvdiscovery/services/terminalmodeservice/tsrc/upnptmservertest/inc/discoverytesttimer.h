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
* Description: CDiscoveryTestTimer class declaration
*
*/


#ifndef DISCOVERYTESTTIMER_H_
#define DISCOVERYTESTTIMER_H_

#include <e32base.h>
#include <e32std.h>

class CTmServerTest; 

class CDiscoveryTestTimer:public CActive
    {
public:
    static CDiscoveryTestTimer* NewL(CTmServerTest& aTmServerTest);
    ~CDiscoveryTestTimer();
    void AfterDiscovery(const TInt aDuration);
    
private:
    CDiscoveryTestTimer(CTmServerTest& aTmServerTest);
    
private: // From CActive
    void RunL();
    void DoCancel();

private:
    RTimer iDiscoveryTimer;
    CTmServerTest& iTmServerTest;
    };

#endif  //DISCOVERYTESTTIMER_H_
