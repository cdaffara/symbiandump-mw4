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
* Description:   ECom interface definition for HSS management services.
*
*/



#ifndef HSSMGMTINTERFACE_H
#define HSSMGMTINTERFACE_H

// INCLUDES
#include "hssmgmtcommon.h"
#include "hssmgmtscaninfo.h"
#include "hssiapsettings.h"


// CLASS DECLARATION
/**
* @brief ECom interface class for HSS management services.
*
* This class contains the methods for managing HSS connections
* and querying the statuses of various connection variables.
*/
class MHssMgmtInterface
    {
    public:

        /**
        * Activate the notification service.
        * 
        * After the client has enabled the notification service, it can
        * receive asynchronous notifications from the server.
        * @param aCallback The class that implements the callback interface.
        */
        virtual void ActivateNotificationsL( MHssMgmtNotifications& aCallback ) = 0;
        
        /**
        * Cancel the notification service.
        */
        virtual void CancelNotifications() = 0;

        /**
        * Perform a scan and return the detected HSS networks.
        * @param aResults Results of the scan.
        * @return KErrNone if successful, otherwise one of the system-wide
        *         error codes.
        */
        virtual TInt GetScanResults( CHssMgmtScanInfo& aResults ) = 0;


        /**
        * Create and Protect IAP, register Client.
        *
        * @param aUid UID of the client.
        * @param aIapName Name of the IAP to be registered.
        * @param aWaitNote Indication of UI authentication, if in use TRUE, else FALSE. 
        * @return IAP ID if successful, otherwise 0 is returned (0 can't be IAP id).
        */
        virtual TUint Register( const TUid aUid,
                                const TDesC& aIapName, 
                                const TUint aWaitNote ) = 0;
        
        
        /**
        * Delete IAP and unregister Client.
        * 
        * @param aUid, UID of the client.
        * @param aIapId, ID of the IAP.
        * @param aNetworkId, ID of Network which IAP belongs to. 
        * @return KErrNone, if IAP removal successful; 
        *         otherwise one of the other system-wide error codes.
        */
        virtual TInt UnRegister( const TUid aUid,
                                 const TUint aIapId, 
                                 const TUint aNetworkId ) = 0;
        
        /**
        * Change IAP settings.
        * 
        * @param aIapId ID of the IAP.
        * @param aSettings Settings of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt ChangeSettings( const TUint aIapId, 
                                     const THssIapSettings &aSettings ) = 0;
        
        /**
        * Associate IAP with network.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt Join( const TUint aIapId ) = 0;
        
        /**
        * Cancel IAP association.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */        
        virtual TInt CancelStart( const TUint aIapId ) = 0;
        
        /**
        * Stop associating IAP.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */                
        virtual TInt Stop( const TUint aIapId ) = 0;
        
        /**
        * Inform login completion.
        * 
        * @param aIapId ID of the IAP.
        * @param aStatus, Result of login procedure.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */                        
        virtual TInt LoginComplete( const TUint aIapId, const TInt aStatus ) = 0;
        
        /**
        * Inform logout completion.
        * 
        * @param aIapId ID of the IAP.
        * @return KErrNone if HotSpot FW was able to receive message;
        * otherwise one of the other system-wide error codes.
        */                        
        virtual TInt LogoutComplete( const TUint aIapId ) = 0;
        
        /**
        * Check status of BackgroundScan.
        * @return 0 if off, otherwise number as scan interval in seconds.
        */
		    virtual TUint32 CheckBackgroundScan() = 0;
		
		    /**
        * Get the BSSID of the currently connected AP.
        * @param aBssId, MAC address of a connection
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
		    virtual TInt GetConnectionBssid( THssBssid& aBssId ) = 0;

		    /**
        * Add an SSID list to the given IAP.
        * @param aIapId, IAP ID given.
        * @param aSsidList, SSID list to add to the IAP.
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
	 	    virtual TInt AddIapSsidList(
                TUint aIapId,
                const CArrayFixFlat<THssSsid>& aSsidList ) = 0;

        /**
        * Gets all Iaps of client
        * @param aUId, UID of client
        * @param aIapIdArray, Array for client's Iaps
        * @return None
        */
        virtual TInt GetClientIaps( const TUid aUid, RArray<TUint>& aIapIdArray ) = 0;

        /**
        * Set login and logout timer values of this client.
        * If given values aren't suitable, the default values are set.
        * @param aClientUid, Client's UID: clients may have own timer values,
        * @param aLoginTimerValue, login timer value in seconds,
        * @param aLogoutTimerValue, logout timer value in seconds.
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
        virtual TInt SetTimerValues (
                TUid aClientUid,
                TUint aLoginTimerValue,
                TUint aLogoutTimerValue ) = 0;
        
    };

#endif // HSSMGMTINTERFACE_H
            
// End of File
