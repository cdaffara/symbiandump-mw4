/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ECom interface implementation definition
*
*/



#ifndef HSSMGMTIMPL_H
#define HSSMGMTIMPL_H

#include "hssinterface.h"
#include "hssscanlist.h"
#include "hssmgmtclient.h"
#include "hssiapsettings.h"

/**
* Implementation for MHssMgmtInterface interface.
*
* This class uses services from RHssInterface class to implement
* most of the required functionality.
*/
class CHssMgmtImpl : public CHssMgmtClient, public MHssSrvNotifications
	{
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        */
        static CHssMgmtImpl* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CHssMgmtImpl();

       // Methods from base classes

        /**
        * (From MHssMgmtInterface) Activate the notification service.
        * 
        * After the client has enabled the notification service, it can
        * receive asynchronous notifications from the server.
        * @param aCallback The class that implements the callback interface.
        */
        virtual void ActivateNotificationsL( MHssMgmtNotifications& aCallback );
        
        /**
        * (From MHssMgmtInterface) Cancel the notification service.
        */
        virtual void CancelNotifications();

        /**
        * (From MHssMgmtInterface) Perform a scan and return the detected WLAN networks.
        * @param aResults Results of the scan.
        * @return KErrNone if successful, otherwise one of the system-wide
        *         error codes.
        */
        virtual TInt GetScanResults( CHssMgmtScanInfo& aResults );

        /**
        * (From MHssMgmtInterface) Create and Protect IAP, register Client.
        * @param aUid UID of the client.
        * @param aIapName Name of the IAP to be registered.
        * @param aWaitNote Indication of UI authentication, if in use TRUE, else FALSE. 
        * @return On successful completion returns IAP ID, otherwise return value is 0.
        */
        virtual TUint Register( const TUid aUid, 
                                const TDesC& aIapName, 
                                const TUint aWaitNote );
        
        /**
        * (From MHssMgmtInterface) Delete IAP and unregister Client.
        * 
        * @param aUid UID of the client.
        * @param aIapId ID of the IAP.
        * @return KErrNone, if IAP removal successful; 
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt UnRegister( const TUid aUid, 
                                 const TUint aIapId, 
                                 const TUint aNetworkId );
        
        /**
        * (From MHssMgmtInterface) Change IAP settings.
        * 
        * @param aIapId ID of the IAP.
        * @param aSettings Settings of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt ChangeSettings( const TUint aIapId, 
                                     const THssIapSettings &aSettings );
        
        /**
        * (From MHssMgmtInterface) Associate IAP with network.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt Join( const TUint aIapId );
        
        /**
        * (From MHssMgmtInterface) Cancel IAP association.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */        
        virtual TInt CancelStart( const TUint aIapId );
        
        /**
        * (From MHssMgmtInterface) Stop associating IAP.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */                
        virtual TInt Stop( const TUint aIapId );

        /**
        * (From MHssMgmtInterface) Inform login completion.
        * 
        * @param aIapId, ID of the IAP.
        * @param aStatus, Result of login procedure.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */                        
        virtual TInt LoginComplete( const TUint aIapId, const TInt aStatus );
        
        /**
        * (From MHssMgmtInterface) Inform logout completion.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */                        
        virtual TInt LogoutComplete( const TUint aIapId );
        
        /**
        * Check status of BackgroundScan.
        * @return 0 if off, otherwice number as scan interval.
        */
		virtual TUint32 CheckBackgroundScan();

		/**
        * Get the BSSID of the currently connected AP.
        * @param aBssId, MAC address of a connection
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt GetConnectionBssid( THssBssid& aBssId );
                 
        /**
        * Add an SSID list to the given IAP.
        * @param aIapId, IAP ID given.
        * @param aSsidList, SSID list to add to the IAP.
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt AddIapSsidList(
                TUint aIapId,
                const CArrayFixFlat<THssSsid>& aSsidList );
                        
        /**
        * Set login and logout timers for this client.
        * @param aClientUid, Client's UID: clients may have own timer values,
        * @param aLoginTimerValue, login timer value,
        * @param aLogoutTimerValue, logout timer value.
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt SetTimerValues(
                TUid aClientUid,
                TUint aLoginTimerValue,
                TUint aLogoutTimerValue );
                
        /**
        * Gets all Iaps of client
        * @param aUId, UID of client
        * @param aIapIdArray, Array for client's Iaps
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt GetClientIaps( const TUid aUid, RArray<TUint>& aIapIdArray );
        
	protected: // Methods
    
        /**
        * (From MHssNotify) New networks have been detected during scan.
        *
        * @param aIapId ID of the IAP.
        */
        virtual void NewNetworksDetected( TUint aIapId );

        /**
        * (From MHssNotify) One or more networks have been lost since the last scan.
        *
        * @param aIapId ID of the IAP.
        */
        virtual void OldNetworksLost( TUint aIapId );
    
    private: // Methods

        /**
        * C++ default constructor.
        */
        CHssMgmtImpl();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
    
    private:  // Data

        // Interface to RWLMServer
        RHssInterface iServer;

		// Client notification callback
		MHssMgmtNotifications* iClientNotification;
    };


#endif // HSSMGMTIMPL_H
