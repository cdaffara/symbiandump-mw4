/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  implementation of Control Point
*
*/


#ifndef C_CUPNPSERVICEINFO_H
#define C_CUPNPSERVICEINFO_H

// INCLUDES

#include <e32base.h>
#include <badesca.h>
#include "upnpdispatchercustomer.h"
#include "upnpdispatcher.h"
#include "upnpnotifytimer.h"

// FORWARD DECLARATIONS
class CUpnpControlPoint;

// CLASS DECLARATION
/**
*  @brief Implementation of the CUpnpServiceInfo
*
*  @since Series60 2.0
*/
NONSHARABLE_CLASS( CUpnpServiceInfo ) : public CBase, public MUpnpNotifyTimerObserver
    {
    public: // Constructors

        /**
        * Factory method
        * @since Series60 3.1
        * @param aControlPoint pointer to Control Point.
        * @param aService Upnp Service
        */
        static CUpnpServiceInfo* CUpnpServiceInfo::NewLC(
                            CUpnpControlPoint* aControlPoint,
                            CUpnpService* aService );

        /**
        * Destructor
        */
        virtual ~CUpnpServiceInfo();

        /**
        * Starts timer
        * @param aTimeout subscribtion timeout
        */
        void StartTimerL( const TDesC8& aTimeout );

        /**
        * Getter.
        */
        TInt SessionId();

        /**
        * Getter.
        */
        CUpnpService* Service();

        /**
        * Getter.
        */
        const TDesC8& Sid() const;

        /**
        * Setter.
        */
        void SetSidL( const TDesC8& aSid );

        /**
        * Get SEQ of the current service. SEQ is the sequence id of last
        * eventing message from this service. SEQ can be inspected
        * to observe eventing operations.
        * @since Series60 2.0
        * @return SEQ of last received event.
        */
        TUint32 Seq() const;

        /**
        * Set SEQ
        * @since Series60 2.0
        * @param aSeq SEQ of a event.
        */
        void SetSeq( TUint32 aSeq );

    public: // From MNotifyTimerObserve

       /**
        * Callback function used to inform about the timer events
        * @since Series60 2.0
        */
        void TimerEventL( CUpnpNotifyTimer* /*aTimer*/ );
        /*
        * ConstructSubscribtionPathLC
        */
        HBufC8* ConstructSubscribtionPathLC( const TDesC8& aControlPointPath,
                                               TInetAddr& aAddr );
    
        /**
        * These functions create Gena subscription message,
        * which will be used for data retrieval.
        */
        CUpnpGenaMessage* CreateSubscriptionMessageL(
                                          const TDesC8& aPath,
                                          THTTPMsgType aType );

        /*
        * CreateSubscriptionMessageL
        */
        CUpnpGenaMessage* CreateSubscriptionMessageL(
                                              THTTPMsgType aType);

    private:

        /**
        * Constructor
        */
        CUpnpServiceInfo( CUpnpControlPoint * aControlPoint,
                          CUpnpService* aService );
        /**
        * ConstructL
        */
        void ConstructL();

        /*
        * ResubscribeL
        */
        void ResubscribeL();

      

    private:

        // Sid buffer, owned
        HBufC8* iSid;

        // For Control Point, owned
        TUint32 iSeq;

        // Session ID, owned
        TInt iSessionId;

        // Service, not owned
        CUpnpService* iService;

        // Resubscribe timer, owned
        CUpnpNotifyTimer* iResubscribeTimer;

        // Parent control point, not owned
        CUpnpControlPoint* iControlPoint;

        // Timeout, owned
        TInt iTimeOut;
    };

#endif // C_CUPNPSERVICEINFO_H

// End Of File