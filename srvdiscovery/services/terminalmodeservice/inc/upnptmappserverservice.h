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
* Description: CUpnpTmAppServerService class declaration
*
*/

#ifndef __UPNPTMAPPSERVERSERVICE_H__
#define __UPNPTMAPPSERVERSERVICE_H__


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
* TerminalMode Application Server Service class. It encapsulates all the major actions 
* needed for accessing and controlling the remotable apps from the Control Point.
*
*/

class CUpnpTmAppServerService: public CUpnpServiceImplementation, 
                               public MUpnpHttpServerTransactionCreator,
                               public MUpnpHttpServerObserver
    {               
public: // Constructors and destructors
    
    /**
    * Two-phased constructor.
    * @param aService Parent service 
    */
    static CUpnpTmAppServerService* NewL( CUpnpDevice& aUpnpDevice, const TDesC& aDescriptionPath , 
                                                                CUpnpTmServerImpl& aTmServerImpl );
                                      
    /**
    * Destructor function called automatically from the destructor of a 
    * derived class
    **/
    ~CUpnpTmAppServerService();
               
public: //from CUpnpServiceImplementation
    /**
    * This functions is a entry point for actions. See coments in base class
    */
    void ActionReceivedLD( CUpnpAction* aAction );
    void AppStatusUpdateEventL( const TDesC8& aStatusUpdateBuffer );    
    void AppListUpdateEventL( const TDesC8& aListUpdateBuffer );  

public: //from MUpnpHttpServerTransactionCreator
    void NewTransactionL( const TDesC8& aMethod, const TDesC8& aUri,    
                       const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans );
        
private: //From MUpnpHttpServerObserver
    void HttpEventLD( CUpnpHttpMessage* aMessage );
   
private:      
    TUpnpErrorCode GetAppListActionL( CUpnpAction* aAction );
    TUpnpErrorCode LaunchAppActionL( CUpnpAction* aAction );
    TUpnpErrorCode TerminateAppActionL( CUpnpAction* aAction );
    TUpnpErrorCode GetAppStatusActionL( CUpnpAction* aAction );

private: // Constructors
    
    /**
    * C++ default constructor.
    */
    CUpnpTmAppServerService( CUpnpDevice& aUpnpDevice, CUpnpTmServerImpl& aTmServerImpl );

    /**
    * Second phase of the constructor.
    * Initialises a DOM tree.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL(const TDesC& aDescriptionPath );
                    
    void ConstructHttpL( );
    TInt ConvertDescriptorToInt( const TDesC8& aDes, TUint& aErr );
    
private:
    CUpnpHttpServerSession*     iHttpServerSession;
    CUpnpDevice&                iUpnpDevice;
    CUpnpTmServerImpl&          iTmServerImpl;
    };

#endif  // __UPNPTMAPPSERVERSERVICE_H__

// End Of File
