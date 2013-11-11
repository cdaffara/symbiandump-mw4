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
* Description: CUpnpRemotableApp class declaration
*
*/

#ifndef __UPNPREMOTABLEAPP_H__
#define __UPNPREMOTABLEAPP_H__

#include <e32base.h>    
#include <upnpterminalmodeicon.h>
#include <upnptminfoelement.h>

/**
* Class represents a Remotable App or just "app" in the 
* context of Terminal Mode Service. 
* The Automotive Server (or equivalent) is responsible for creating an object 
* of this class and registering it with the Terminal Mode Service. 
* The Terminal Mode Service will maintain a list of these registered apps throughout 
* it's lifetime. The Automotive Server can remove apps from this list by 
* unregistering them. 
*/
class CUpnpRemotableApp : public CBase
    {
public: 
    /**
    * Standard Symbian Two-Phase constructor
    * @param	aAppId			The locally unique ID of the App being registered.
	*							The Terminal Mode Service will check for the uniqueness
	*							of this ID when this Remotable app object is registered.
	* @param	aAppName		The Name of the App being registered. The Terminal Mode
	*							Service will *not* check for uniqueness. However, it's 
	*							recommended that this is unique as well.
	* @see	CUpnpTmServer::RegisterAppL, CUpnpTmServer::RegisterAppsL
    */    
    IMPORT_C static CUpnpRemotableApp* NewL( TUint aAppId ,const TDesC8& aAppName );
	
	~CUpnpRemotableApp();	
    /**
     * Method that sets the brief description about the application( remotable app )
     * An optional element.
     * @param aDescription Description of the Application
     */
    IMPORT_C void SetAppDescriptionL( const TDesC8& aDescription );
    /**
     * Method to set the list of allowed profile IDs associated with the remotable app.
     * An optional element.
     * @aProfileIdList  Reference to an array of profile IDs.
     *                  A copy of array is maintained.
     */
    IMPORT_C void SetAllowedProfileIdListL( const RArray<TUint>& aProfileIdList );
    /**
    * Method that adds a new icon to this Remotable App.
    * An optional element.
    * @param	aIcon	[in] The Icon object. Must be instantiated by the caller prior to
	*					this call. Should be non-NULL.
	*					The ownership is transferred to the CUpnpRemotableApp object.
	* @see	CUpnpTerminalModeIcon
    */    
	IMPORT_C void AddIconL( CUpnpTerminalModeIcon* aIcon );
    /**
    * Method that creates a new terminal mode info object and adds it to the list of
	* terminal mode info objects supported by this app. The terminal mode info object thus 
	* created will be internally maintained by Remotable App object.A reference of the same
	* will be returned to the caller. 
	* Only protocolID of the remotingInfo element is the required element as per the schema,
	* otherwise all other info types and their elements are optional.
    * @param aTerminalModeInfoType	Terminal Mode Info Type as an Enum value. 
	* @see	CUpnpTerminalModeInfoElement
    */    
	IMPORT_C CUpnpTmInfoElement& CreateTmInfoElementL( CUpnpTmInfoElement
	                                ::TTerminalModeInfoType aTerminalModeInfoType );
    /**
    * Method is used to define the current status of the resource.
    * An optional element.
    * @param aResourceStatus  Status of the resource. Can take one of these values;
    *        Free, busy or NA
    */    
	IMPORT_C void SetResourceStatusL( const TDesC8& aResourceStatus );
    /**
    * "Getter" method for retrieving the list of icons associated with this
	* Remotable app object.An optional element.
    * @return Returns RPointerArray of icons.
	* @see CUpnpTerminalModeIcon
    */    
	inline const RPointerArray<CUpnpTerminalModeIcon>& IconList()const;
    /**
    * "Getter" Method for retrieving the list of terminal mode info details associated with
    * this Remotable app object.
    * @return Returns RPointerArray of terminal mode info objects. 
    * @see  CUpnpTerminalModeInfo
    */    
    inline const RPointerArray<CUpnpTmInfoElement>& TmInfoElementList()const;

	// inlined "Getter" methods 
	inline TUint AppId()const;
	inline const TDesC8& AppName()const;
	inline const TDesC8& AppDescription()const;
	inline const TDesC8& ResourceStatus()const;
	inline const RArray<TUint>& AllowedProfileIdList()const;
	
protected:
	CUpnpRemotableApp( TUint aAppId );
    void ConstructL( const TDesC8& aAppName );

private:
    TUint                                   iAppId;
    RBuf8                                   iAppName;
    RBuf8                                   iAppDescription;
    RBuf8                                   iResourceStatus;
	RPointerArray<CUpnpTerminalModeIcon>    iIconList;
	RPointerArray<CUpnpTmInfoElement>       iTmInfoElementList;
	RArray<TUint>                           iProfileIdList;
    };

#include <upnpremotableapp.inl>

#endif //__UPNPREMOTABLEAPP_H__

