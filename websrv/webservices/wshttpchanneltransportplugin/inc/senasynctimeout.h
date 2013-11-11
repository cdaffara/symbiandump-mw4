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
* Description: Header declaration
*
*/









#ifndef ASYNCTIMEOUT_H
#define ASYNCTIMEOUT_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class CSenTxnState;

class CSenAsyncTimeOut : public CTimer
    {
public:
    // Cancel and destroy
    ~CSenAsyncTimeOut();

    // Two-phased constructor.
    static CSenAsyncTimeOut* NewL(CSenTxnState* aTxnState);

    // Two-phased constructor.
    static CSenAsyncTimeOut* NewLC(CSenTxnState* aTxnState);

public:
    // New functions
    // Function for making the initial request
    void StartL(TInt aTimeOutSec);

private:
    // C++ constructor
    CSenAsyncTimeOut(CSenTxnState* aTxnState);

    // Second-phase constructor
    void ConstructL();

private:
    // From CActive
    // Handle completion
    void RunL();


    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);

    CSenTxnState* iTxnState;//not owned
    };

#endif // ASYNCTIMEOUT_H
