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
* Description:   Implementation of HotSpot Server API
*
*/


#ifndef HSSINTERFACE_H
#define HSSINTERFACE_H

#include <e32base.h>
#include "hssmgmtcommon.h"
#include "hssiapsettings.h"
#include "hssiaphandler.h"
#include "hssscanhandler.h"
#include "hssscanlist.h"
#include "hsssrvnotifications.h"
	
class MHssSrvNotifications;
class CHssSrvNotifications;

NONSHARABLE_CLASS( RHssInterface ) : public RSessionBase 
{
    public:

 		/**
        * Client makes the connection to the server.        
        * @return General Symbian error code.
        */
        IMPORT_C TInt Connect();

        /**
        * Version information.
        * @return Version information.
        */
        TVersion Version() const;
        
        /**
        * Close the connection; release the handle to the server.
        */
        IMPORT_C void Close();
        
        /** 
        * Set a message to pend notification.
        * @param aReturnValue Status of the calling active object.
        * @param aReturnData  (OUT) Data returned by the notification.
        */
        void WaitForNotification( TRequestStatus& aReturnValue, 
                                  TDes8& aReturnData );   
        
       /** 
        * Cancel pending notification message from server.
        * CHssSrvNotifications class uses this method to cancel pending message at server 
        * side.
        */
        void CancelWaitForNotification();

       /**
        * Activate the notification service.
        * 
        * After the client has enabled the notification service, it can
        * receive asynchronous notifications from the server.
        * @param aCallback The class that implements the callback interface.
        */
        IMPORT_C void ActivateNotificationsL( MHssSrvNotifications& aCallback );
        
        /**
        * Cancel the notification service.
        */
        IMPORT_C void CancelNotifications();

        /**
        * Perform a scan and return the detected WLAN networks.
        * @param aResults Results of the scan.
        * @return KErrNone if successful, otherwise one of the system-wide
        *         error codes.
        */
        IMPORT_C TInt GetScanResults( HssScanList& aResults );

        /**
        * Create and Protect IAP, register HotSpot Client.
        * @param aUid, UID of the client.
        * @param aIapName, Name of the iap to be created.
        * @param aWaitnote, TRUE or FALSE indication of external prowserplugin need.
        * @return On successful completion returns IAP ID, otherwise return value is 0.
        */
		IMPORT_C TUint Register( const TUid aUid, 
		                         const TDesC& aIapName, 
		                         const TUint aWaitnote );

        /**
        * Delete IAP and unregister HotSpot Client.
        * @param IapID, ID of the IAP to be unregistered.
        * @param Uid, UID of the client.
        * @return KErrNone if successful
        */
		IMPORT_C TInt UnRegister( const TUint aIapId, 
		                          const TUint aNetworkId,
		                          const TUid aUid );

        /**
        * Change IAP settings.
        * @param aIapId, ID of the IAP.
        * @param aSettings, Complete IAP settings struct
        * @return KErrNone if successful
        */
		IMPORT_C TInt ChangeSettings( const TUint aIapId, 
		                              const THssIapSettings& aSettings);

        /**
        * Associate IAP with network.
        * @param aIapId, ID of the IAP.
        * @return KErrNone if successful
        */
		IMPORT_C TInt Join( const TUint aIapId );

        /**
        * Cancel start call.
        * @param aIapId, ID of the IAP.
        * @return KErrNone if successful
        */
        IMPORT_C TInt CancelStart( const TUint aIapId );

        /**
        * Stop associating IAP.
        * @param aIapId, ID of the IAP.
        * @return KErrNone if successful
        */
		IMPORT_C TInt Stop( const TUint aIapId );
                
        /**
        * End connection.
        * @param aIapId, ID of the IAP.
        * @param aStatus, Status of the calling active object.
        * @return KErrNone, if successful; otherwise one of the other system-wide error codes.
        */
		IMPORT_C TInt LoginComplete( const TUint aIapId, TInt aStatus );
		
		/**
        * End connection.
        * @param aIapID, ID of the IAP.
        * @return KErrNone if successful
        */
		IMPORT_C TInt LogoutComplete( const TUint aIapId );
		
		/**
        * StartLogin for NIFMAN Configuration Daemon.
        * @param aIapId, ID of the IAP.
        * @param aNetworkId, ID of the network.
        * @param aStatus, Status of the calling active object.
        */
		IMPORT_C void StartLogin ( const TUint aIapId, 
		                           const TUint aNetworkId, 
		                           TRequestStatus& aStatus );
		
	    /**
        * CancelLogin for DHCP Configuration Daemon.
        * @return KErrNone if successful
        */
	    IMPORT_C TInt CancelLogin ();
	    
	    /**
        * Start for WLAN agent.
        * @param aIapId, ID of the IAP.
        * @param aStatus, Status of the calling active object.
        */
	    IMPORT_C void Start( const TUint aIapId, TRequestStatus& aStatus );
	    
	    /**
        * Start again for WLAN agent.
        * @param aIapID, ID of the IAP.
        * @param aStatus, Status of the calling active object.
        */
	    IMPORT_C void StartAgain( const TUint aIapId, TRequestStatus& aStatus );
	    
	    /**
        * Close connection for WLAN agent.
        * @param aIapID, ID of the IAP.
        * @param aStatus, Status of the calling active object.
        */
	    IMPORT_C void CloseConnection( const TUint aIapId, TRequestStatus& aStatus );	    
		
	    /**
        * Check status of BackgroundScan.
        * @return 0 if off, otherwice number as scan interval.
        */
		IMPORT_C TUint32 CheckBackgroundScanL();
		
		/**
        * Cancel for WLAN agent.
        * @param aIapId, ID of the IAP.
        */
	    IMPORT_C void Cancel( const TUint aIapId );
	
	    /**
        * Shutdown server when uninstalling .sis -file.
        */
	    IMPORT_C TInt ShutdownServerL();
	    
        /**
        * Set UI state of HotSpot browser logon application.
		* @param aIapId, ID of the IAP.
		* @param aState, value of THsBrowserUiStates.
        */
        IMPORT_C void SetUiState( const TUint aIapId, const TUint aState );

        /**
        * Get ID of the IAP .
        * @return IAP ID.
        */
        inline TUint IapId();
		IMPORT_C TUint32 GetIap(TIpcArgs aArgs);
	
		/**
        * Starts HotSpot browser for browser based authentication.
        * @param aStatus, Status of the calling active object.
        */
		IMPORT_C void StartBrowser( const TDesC& aString,
								    const TUint aIapId,
									const TUint aNetId,
									TRequestStatus& aStatus );

		/**
        * Get the BSSID of the currently connected AP.
        * @param aBssId, MAC address of a connection
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
		IMPORT_C TInt GetConnectionBssid( THssBssid& aBssId );
		
        /**
        * Add an SSID list to the given IAP.
        * @param aIapId, IAP ID given.
        * @param aSsidList, SSID list to add to the IAP.
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
        IMPORT_C TInt AddIapSsidList(
                TUint aIapId,
                const CArrayFixFlat<THssSsid>& aSsidList );
                        
        /**
        * Set login and logout timer values of this client.
        * If given values aren't suitable, the default values are set.
        * @param aClientUid, Client's UID: clients may have own timer values,
        * @param aLoginTimerValue, login timer value,
        * @param aLogoutTimerValue, logout timer value.
        * @return KErrNone if successful, 
        * otherwise one of the other system-wide error codes.
        */
        IMPORT_C TInt SetTimerValues (
                TUid aClientUid,
                TUint aLoginTimerValue,
                TUint aLogoutTimerValue );
        
        /**
        * Gets all Iaps of client
        * @param aUId, UID of client
        * @param aIapIdArray, Array for client's Iaps
        * @return None
        */
        IMPORT_C void GetClientIapsL( const TUid aUid, RArray<TUint>& aIapIdArray );
        
	protected: // Methods

        /**
        * Start the server during Connect() if the server is not loaded.
        * @return KErrNone if successful otherwise another of the system wide error codes.
        */
        TInt StartServer();
               
    private: // Methods
    
        /**
        * Stops active connection
        * @param aIapId, ID of the IAP.
        * @param aNetworkId, ID of the network.
        * @return KErrNone if successful otherwise another of the system wide error codes.
        */
        TInt StopConnectionL( const TUint aIapId, const TUint aNetworkId );
     
    private: // Data
    
        /**
        * Callback interface to HSS management.
        */
        CHssSrvNotifications* iNotify;
    	 
    	/**
    	* ID of the IAP.
    	*/
        TUint iIapId;				       
        
        /**
    	* Handler for changing IAP settings
    	*/
        CHssIapHandler* iIapHandler;
        
        /**
    	* Handler for changing IAP settings
    	*/
        CHssScanHandler* iScanHandler;			       
        
        /**
     	* Parameter array type
     	*/
        TIpcArgs iArgs;
 };

#endif // HSSINTERFACE_H
            
// End of File
