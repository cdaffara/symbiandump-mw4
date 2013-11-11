/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains common data structures used by Internet Connectivity 
*                Test Service.
*
*/


#ifndef ICTSCOMMON_H
#define ICTSCOMMON_H

// INCLUDES
#include <e32std.h>

// CONSTANTS

// Length of HTTP FW response
const TInt KHttpResponseLength = 1024;

// HTTP::HEAD max response time for timer. 5 seconds.
const TInt KHttpResponseTime = 5000000; 

// DATA TYPES
    
// Values of testing result
enum TIctsTestResult
    {
    // Test successful
    EConnectionOk,
    // Browser (or other) authentication was needed.
    EHttpAuthenticationNeeded,
    // Test was not successful
    EConnectionNotOk,
    // Timeout 
    ETimeout
    };

// CLASS DECLARATION
/** 
* Callback interface for Internet Connectivity Test Service notifications.
*
* This virtual methods should be inherited and implemented by the
* client.
*
* The client enables observer calls when creating an instance of Icts.
* @see CIctsClientInterface::NewL.
* @lib ictsclientinterface.dll
* @since S60 5.0
*/
class MIctsObserver
    {
    public:

        /**
        * Connectivity test result will become here
        * @param aResult Result of connectivity test
        * @param aString String containing location information when redirect
        */
        virtual void ConnectivityObserver( TIctsTestResult aResult, 
                                           const TDesC& aString ) = 0;
    
    };
    
#endif // ICTSCOMMON_H
            
// End of File
