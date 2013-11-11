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
* Description:  Domain API 
*
*/




#ifndef ICTSCLIENTINTERFACE_H
#define ICTSCLIENTINTERFACE_H

#include <e32base.h>
#include <badesca.h>
#include <ictscommon.h>

class CIctsEngine;

/**
 *  Interface class for clients to use
 *
 *  @lib ictsclientinterface.lib
 */
class CIctsClientInterface : public CBase
    {

    public:

        /**
        * Static constructor.
        */    
        IMPORT_C static CIctsClientInterface* NewL( TUint32 aIapId, 
                                                    TUint32 aNetworkId,
                                                    MIctsObserver& aClient );

        /**
        * Destructor.
        */
        virtual ~CIctsClientInterface();
    
        /**
        * Starts internet connectivity test.
        *
        * @return None
        */
        IMPORT_C void StartL();

        /**
        * Cancels the internet connectivity test.
        *
        * @return None
        */
        IMPORT_C void CancelStartL();

        /**
        * Starts polling.
        *
        * @param aTime Polling time used
        * @param aInterval Time interval of execution
        * @return None
        */
        IMPORT_C void StartPolling( TInt aTime,
                                    TInt aInterval );

        /**
        * Stops polling
        *
        * @return None
        */
        IMPORT_C void StopPolling();


    private:

        /**
        * C++ default constructor.
        */
        CIctsClientInterface();
    
        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aIapId IAP id 
        * @param aNetworkId Network id
        * @param aClient Client using test service
        */
        void ConstructL( TUint32 aIapId, 
                         TUint32 aNetworkId, 
                         MIctsObserver& aClient );


    private: // data

        /**
        * Intance of engine that handles ECom and protocols -> all tests.
        * Own.  
        */
        CIctsEngine* iEngine;
    
    };


#endif // ICTSCLIENTINTERFACE_H
