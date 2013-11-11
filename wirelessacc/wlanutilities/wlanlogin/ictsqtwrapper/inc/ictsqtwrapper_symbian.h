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
* Description: Private symbian implementation of ICTS client interface wrapper 
*
*/

#ifndef ICTSQTWRAPPERSYMBIAN_H
#define ICTSQTWRAPPERSYMBIAN_H

// System includes
#include <ictsclientinterface.h>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class IctsWrapperPrivate: public MIctsObserver
{

public:
    
    IctsWrapperPrivate(int aIapId, int aNetId, IctsWrapper *aWrapper);
    
    ~IctsWrapperPrivate();
    
    void ConnectivityObserver( TIctsTestResult aResult, const TDesC& aString );
    
    void startConnectivityTest();

    void startPolling(int pollingTime, int pollingInterval);
    
    void stopPolling();
    
private:
    
    IctsWrapper *q_ptr; //!< Pointer to public implementation
    
    CIctsClientInterface* iIct; //!< Pointer to ICTS client interface
};

#endif // ICTSQTWRAPPERSYMBIAN_H
