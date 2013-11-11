/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of HotSpot Server
*
*/


#ifndef HSSNOTIF_H
#define HSSNOTIF_H

//  INCLUDES
#include "hssinterface.h"
#include <wlanmgmtcommon.h>
#include <wlanmgmtinterface.h>
#include "hotspotclientserver.h"

// FORWARD DECLARATIONS
class CHotSpotSession;
class CSessionNotification;

// CLASS DECLARATION
/**
* HssNotifications, callback implementation of MWlanMgmtNotifications.
*/
class HssNotifications : public MWlanMgmtNotifications
	{
   public:

    	/**
    	* Constructor.
    	*/
		HssNotifications(CHotSpotSession& aSession): iSession(aSession){}
        /**
        * Connection status has changed.
        */
        virtual void ConnectionStateChanged( TWlanConnectionMode /* aNewState */ ) {};
        
        /**
        * BSSID has changed (i.e. AP handover).
        */
        virtual void BssidChanged( TWlanBssid& /* aNewBSSID */ ) {};

        /**
        * Connection has been lost.
        */
        virtual void BssLost() {};

        /**
        * Connection has been regained.
        */
        virtual void BssRegained() {};

        /**
        * New networks have been detected during scan.
        */
        virtual void NewNetworksDetected();

        /**
        * One or more networks have been lost since the last scan.
        */
        virtual void OldNetworksLost();

        /**
        * The used transmit power has been changed.
        * @param aPower The transmit power in mW.
        */
        virtual void TransmitPowerChanged( TUint /* aPower */ ) {};
        
        /**
        * Received signal strength level has been changed.
        * @param aRssClass specifies the current class of the received signal
        * @param aRss RSS level in absolute dBm values.
        */
        virtual void RssChanged(TWlanRssClass /* aRssClass */, TUint /* aRss */ ) {};

   		private:
   		
        /**
        * Callback interface to Session.
        */
   		CHotSpotSession& iSession;

	};
// CLASS DECLARATION
/**
* Base class for all notification types.
*
* Basically this only determines the interface how Server is forwarding 
* notifications to Sessions or WAL.
*/
class CNotificationBase : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CNotificationBase() {};

    public: // New functions

        /**
         * AddNotification
         * @param aNotification identifier
         * @param aData content of the notification
         */
        virtual void AddNotification(
            TUint aNotification,
            TDes8& aData ) = 0;            
    };

// CLASS DECLARATION
/**
* Forwards notifications to a session.
*/
NONSHARABLE_CLASS( CSessionNotification ) : public CNotificationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSessionNotification* NewL( CHotSpotSession& aSession );

        /**
        * Destructor.
        */
        virtual ~CSessionNotification();

    public: // Functions from base classes

        /**
         * AddNotification
         * @param aNotification identifier
         * @param aData content of the notification
         */
        void AddNotification( TUint aNotification, TDes8& aData );

    private: // Methods

        /**
        * C++ default constructor.
        */
        CSessionNotification( CHotSpotSession& aSession );

    private:    // Data

        /** Interface to session to forward notifications. */
        CHotSpotSession& iSession;
    };
    

#endif // HSSNOTIF_H
