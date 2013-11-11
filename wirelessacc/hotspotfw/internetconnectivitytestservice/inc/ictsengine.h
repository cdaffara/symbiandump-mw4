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
* Description:   Engine for plugins. Contains logic how to use them.
*
*/





#ifndef CICTSEngine_H
#define CICTSEngine_H

#include <e32base.h>
#include <centralrepository.h>

#include "ictscommon.h"
#include "ictshttphandler.h"
#include "ictspollingintervaltimer.h"
#include "ictspollingtimetimer.h"
#include "ictspollingintervalobserver.h"
#include "ictspollingtimeobserver.h"
#include "hssinterface.h"

// CONSTANTS
_LIT8( KHttp, "http" ); 
const TInt KMaxIpLength = 100;
const TInt KStringLength = 1000;
const TInt KMaxProtocolNameLength = 56;
const TUint KStartPollingWait = 2500000; // 2.5s

// FORWARD DECLARATIONS
class RHssInterface;

/**
 *  Engine
 *
 *  @lib ictsclientinterface.lib
 */
class CIctsEngine : public CBase, 
                    public MIctsPollingIntervalObserver,
                    public MIctsPollingTimeObserver
    {

    public:

        /**
        * Two-phased constructor.
        *
        * @param aIapId IAP id
        * @param aNetworkId Network id
        * @param aClient Client for callback calls
        */
        static CIctsEngine* NewL( TUint32 aIapId, 
                                  TUint32 aNetworkId,
                                  MIctsObserver& aClient );
   
        /**
        * Destructor.
        */
        virtual ~CIctsEngine();
    
        /**
        * Starts basic internet connectivity test.
        *
        * @return None
        */
        void StartL();
        
        /**
        * Cancels connectivity test.
        *
        * @return None
        */
        void CancelStartL();
        
        /**
        * Starts polling.
        *
        * @param aTime Polling time used
        * @param aInterval Time interval of execution
        * @return None
        */
        void StartPolling( TInt aTime, TInt aInterval );

        /**
        * Stops polling
        *
        * @return None
        */
        void StopPolling();
    
    
        /**
        * Callback from CHttpHandler 
        *
        * @param aResult Result of HTTP::GET
        * @param aString HTTP Response body (if exists)
        * @return None
        */
        void HttpEventL( TIctsTestResult aResult , TDesC& aString );
    

        // from MPollingIntervaObserver

        /**
        * Callback from CIctsPollingIntervalTimer
        *
        * @return None
        */
        virtual void PollingIntervalTimeout();
    
        // from MPollingTimeObserver

        /**
        * Callback from CIctsPollingTimeTimer
        *
        * @return None
        */
        virtual void PollingTimeTimeout();

    private:

        /**
        * C++ default constructor.
        *
        * @param aIapId IAP id
        * @param aNetworkId Network id
        * @param aClient Client for callback calls
        */
        CIctsEngine( TUint32 aIapId, 
                             TUint32 aNetworkId, 
                             MIctsObserver& aClient );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
   
        /**
        * Checks user defined ICT settings stored in CenRep
        *
        * @return TTestConnQueryResults if successful, otherwise one of the system-wide
        *         error codes.
        */
        TInt CheckSettingL();
    
    private: // data

        /**
        * IAP identifier
        */
        TUint32 iIapId;
    
        /**
        * Network identifier
        */
        TUint32 iNetworkId;

        /**
        * Return string to client. Can contain
        * redirect URL or error information.
        */
        TBuf<KStringLength> iString;
    
        /**
        * Status if we are polling or not
        */
        TBool iPollingStatus;
    
        /**
        * Interval of polling
        */
        TInt iPollingInterval;
    
        /**
        * Intance of CenRep
        * Own.  
        */
        CRepository* iRepository;
    
        /**
        * Contains IP address to be used  
        */
        TBuf8<KMaxIpLength> iIPAddress;
    
        /**
        * Timer for polling interval
        * Own.  
        */
        CIctsPollingIntervalTimer* iPollingIntervalTimer;
  
  
        /**
        * Timer for total polling time
        * Own.  
        */
        CIctsPollingTimeTimer* iPollingTimeTimer;  
    
        /**
        * Instance of HTTP handling class
        * Own.  
        */
        CIctsHttpHandler* iHttpHandler;
    
        /**
        * Reference of client using test service 
        * Not own.  
        */
        MIctsObserver& iClient;
        
        /**
        * Handle to a session with a hotspot server
        */
        RHssInterface iHsServer;

    };


#endif // CICTSEngine_H
