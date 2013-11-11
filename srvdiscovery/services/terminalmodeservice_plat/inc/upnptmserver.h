/**
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
* Description: CUpnpTmServer class declaration
*
*/

#ifndef __UPNPTMSERVER_H__
#define __UPNPTMSERVER_H__

//  Include Files
#include <e32base.h>    // CBase 
#include <upnptmserverdeviceinfo.h>
#include <upnptmserverobserver.h>
#include <upnpremotableapp.h>

//Forward Declaration
class CUpnpTmServerImpl;


/**
 * The Main interface for the Terminal Mode Service. 
 * The client of this class (Automotive Server or equivalent) is responsible
 * for registering/unregistering the remotable apps and updating the status 
 * of the running apps. 
 * The client should also "start" and "stop" the service through the API
 * provided in this class
 */
class CUpnpTmServer: public CBase
    {
public:
    /**
    * Standard Symbian Two-phased constructor. 
    * @param aDeviceInfo The IAP and device Settings that the 
    * 					 Terminal Mode Service needs to operate. 
    * 					 A valid IAP ID is mandatory. 
    * @param aUpnpTmServerObserver	The callback interface that the client
    * 											application must implement.
    */  
    IMPORT_C static CUpnpTmServer* NewL( CUpnpTmServerDeviceInfo& aDeviceInfo,
                                   MUpnpTmServerObserver& aUpnpTmServerObserver );
    ~CUpnpTmServer();

public:
    /**
    * Method to register a single App (aka Remotable App ). 
    * The caller must create a CUpnpRemotableApp object and 
    * populate it before calling this method. 
    * @param aRemotableApp	A pointer to a Remotable App object.
	*						"Ownership" of the object is passed.
	*						Must not be NULL.
    */    
    IMPORT_C void RegisterAppL( CUpnpRemotableApp* aRemotableApp );
    /**
    * Method to register a list of Apps (aka Remotable Apps). 
    * The caller must create all the CUpnpRemotableApp objects 
    * and populate them before calling this method. 
    * This list of apps will be appended to existing list of
    * registered apps. 
    * @param aRemotableAppList	A RPointerArray of Remotable App objects.
	*							"Ownership" of all the objects is passed.
	*		         			None of the objects must be NULL.
    */    
    IMPORT_C void RegisterAppsL( const RPointerArray<CUpnpRemotableApp>& aRemotableAppList );
    /**
    * Method to unregister a single App from the service. 
    * The specified App must be registered with the 
    * Terminal Mode Service. 
    * @param aAppId	The ID of the app to be unregistered
    */    
    IMPORT_C TInt UnRegisterApp( TUint aAppId );
    /**
    * Method to unregister multiple Apps from the service. 
    * The specified Apps must be registered with the 
    * Terminal Mode Service. 
    * In case of duplicate appIDs ,TM service will sort out the duplicate ones
    * and will unregister only once for a particular appID.
    * Even if a single app from the requested array is unable to get 
    * un-registered the method should return KErrNotFound.
    * @param aAppIdArray The list of IDs of the apps to be unregistered
    */    
    IMPORT_C TInt UnRegisterApps( const RArray<TUint>& aAppIdArray );  
    /**
     * Method to set the XML signature of the registered apps
     * This is done as specified in XML Signature Syntax.
     * The signature value is opaque to the TM Service.
     * @param aSignature XML formatted buffer
     */
    IMPORT_C void SetXmlSignatureL( const TDesC8& aSignature );    
    /**
    * Method to Start the Terminal Moder Server Device and its services.
    * This must be called to initiate all the UPnP related
    * activities. 
    */    
    IMPORT_C void StartL();
    /**
    * Method to Stop the Terminal Moder Server Device and its services.
    * No further UPnP related activities will occur after this. 
    */    
    IMPORT_C void StopL();
    /**
    * Method to fetch the Remotable App object by passing the 
    * App ID of the same. Method is invoked by the Automotive Server
    * when it wishes to modify any of the existing remotable app.
    * @param aAppId App ID of the requested App
    * @param aErr[out] Error code
    * @return Returns reference to CUpnpRemotableApp object 
    */ 
    IMPORT_C CUpnpRemotableApp& GetRemotableApp( TUint aAppId, TInt& aErr );
    /**
    * Method through which the notification of applications whose status 
    * has changed is sent across to the Car Kit through the Service .
    * TM Service ignores the duplicate appIDs and sends only the list of
    * unique appIDs.
    * @param aUpdatedAppIdList List of appIDs whose status has changed
    */    
    IMPORT_C void UpdateAppStatusL( const RArray<TUint>& aUpdatedAppIdList );
    /**
    * Method through which the notification of applications whose entries
    * in the application list have changed is sent across to the Car Kit 
    * through the Service.
    * TM Service ignores the duplicate appIDs and sends only the list of
    * unique appIDs.
    * @param aUpdatedAppIdList List of appIDs whose entries haave changed
    */  
    IMPORT_C void UpdateAppListL( const RArray<TUint>& aUpdatedAppIdList );
    /**
    * Method through which the notification of profileIDs which are not used
    * used by any Terminal Mode service hosted on the Terminal Mode device
    * is sent across to the Car Kit through the Service.
    * TM Service ignores the duplicate profileIDs and sends only the list of
    * unique profileIDs.
    * @param aUnusedProfileIdList List of profile IDs for profiles which are 
    *                  currently not being used by any Terminal Mode service 
    */  
    IMPORT_C void UpdateUnusedProfileIdsL( const RArray<TUint>& aUnusedProfileIdList );
    
private:
    CUpnpTmServer();
    void ConstructL( CUpnpTmServerDeviceInfo& aDeviceInfo, 
                             MUpnpTmServerObserver& aUpnpRemoteServer );

private:
    // The "body"/implementation   
    CUpnpTmServerImpl*   iTmServerImpl;   
    };

#endif  // __UPNPTMSERVER_H__

