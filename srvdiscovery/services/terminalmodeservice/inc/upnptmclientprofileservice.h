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
* Description: CUpnpTmClientProfileService class declaration
*
*/

#ifndef __UPNPTMCLIENTPROFILESERVICE_H__
#define __UPNPTMCLIENTPROFILESERVICE_H__


// Include Files
#include <upnpserviceimplementation.h>
#include <upnphttpserverobserver.h>
#include <upnphttpservertransactioncreator.h>
#include <upnpcommonupnplits.h>
#include <upnpsettings.h>
#include <upnphttpserversession.h>
#include <upnphttpserverruntime.h>

// FORWARD DECLARATIONS
class CUPnPTmServerDevice;
class CUpnpTmServerImpl;

// CLASS DECLARATION

/**
 * TerminalMode Client Profile Service class. It encapsulates all the actions related to the client 
 * profile and their related state variables.
 */
class CUpnpTmClientProfileService: public CUpnpServiceImplementation
    {               
public: // Constructors and destructors
    /**
    * Two-phased constructor.
    * @param aService Parent service 
    */
    static CUpnpTmClientProfileService* NewL( CUpnpDevice& aUpnpDevice, const TDesC& aDescriptionPath , 
                                                                     CUpnpTmServerImpl& aTmServerImpl );
                                      
    /**
    * Destructor function called automatically from the destructor of a 
    * derived class
    **/
    ~CUpnpTmClientProfileService();
               
public: //from CUpnpServiceImplementation
    /**
    * This functions is a entry point for actions. See coments in base class
    */
    void ActionReceivedLD( CUpnpAction* aAction );
 
    void UnUsedProfileIdEventL(const TDesC8& aUnusedProfileIdBuffer);   
   
private:  
    // Actions supported by the client profile service
    TUpnpErrorCode GetMaxNumProfilesActionL( CUpnpAction* aAction );
    TUpnpErrorCode SetClientProfileActionL( CUpnpAction* aAction );
    TUpnpErrorCode GetClientProfileActionL( CUpnpAction* aAction );

private: // Constructors 
    /**
    * C++ default constructor.
    */
    CUpnpTmClientProfileService( CUpnpDevice& aUpnpDevice, 
                                    CUpnpTmServerImpl& aTmServerImp );
    /**
    * Second phase of the constructor.
    * Initialises a DOM tree.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL(const TDesC& aDescriptionPath );
    
private:
    CUpnpHttpServerSession*     iHttpServerSession;
    CUpnpDevice&                iUpnpDevice;
    CUpnpTmServerImpl&          iTmServerImpl;
    };

#endif  // __UPNPTMCLIENTPROFILESERVICE_H__

// End Of File
