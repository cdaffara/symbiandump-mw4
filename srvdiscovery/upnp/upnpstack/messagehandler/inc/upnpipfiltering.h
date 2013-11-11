/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


#ifndef C_CUPNPIPFILTERING_H
#define C_CUPNPIPFILTERING_H

#include <e32cmn.h>
#include <in_sock.h>
#include <ES_SOCK.H>
#include <CommDbConnPref.h>

#include "upnpmessagehandlerengine.h"
#include "upnpmessagehandlerengineobserver.h"
#include "upnpconnectionmanagerproxy.h"

class CUpnpIPFilteringDNSQuery;

class CUpnpIPFiltering : public CBase
    {

public:
   /**
    * Two-phased constructor.
    * @param aSocketServer 
    * @param aIap an interface numbe which going to be use for connection
    * @param aObserver
    */
    static CUpnpIPFiltering* NewL( RSocketServ& aSocketServer, TInt aIap,
                                   MMessageHandlerEngineObserver& aObserver );
    /**
    * Two-phased constructor.
    * @param aSocketServer 
    * @param aIap an interface numbe which going to be use for connection
    * @param aObserver
    */
    static CUpnpIPFiltering* NewLC( RSocketServ& aSocketServer, TInt aIap,
                                    MMessageHandlerEngineObserver& aObserver );

    /**
    * Destructor.
    */
    virtual ~CUpnpIPFiltering();

    /**
    * Adds address to the whitelist
    * @since S60 v3.2
    * @param aAddress anaddress which going to be addres to the list
    * @return KErrNone if succesfully added or index number if already exist on list
    */
    TInt AddAddressL( const TInetAddr& aAddress );

    /**
    * Adds address to the whitelist
    * @since S60 v3.2
    * @param aQuery query to DNS server
    */
    void AddAddressL( CUpnpIPFilteringDNSQuery* aQuery  );
    
    
    /**
    * Removes address from the whitelist
    * @since S60 v3.2
    * @param aAddress anaddress which going to be removed
    * @return KErrNone if succesfully removed
    */
    TInt RemoveAddressL( const TInetAddr& aAddress );

    /**
    * Removes address from the whitelist
    * @since S60 v3.2
    * @param aQuery query to DNS server
    * @return KErrNone if succesfully removed
    */
    void RemoveAddressL( CUpnpIPFilteringDNSQuery* aQuery  );
    
    /**
    * Removes all addressses from the list
    *
    */
    TInt RemoveAll( );

    /**
    * Checks if address is on the whitelist
    * @since S60 v3.2
    * @param aAddress an address to be checked
    * @return True if address is on whitelist
    */
    TBool  IsAllowed( const TInetAddr& aAddress ) const;
   
    /**
    * Counts object on the whitelist
    * @since S60 v3.2
    * @return number of addresses on the list
    */
    TInt Count() const;
     
    /**
    * 
    * @since S60 v3.2
    * @return list of addresses
    */
    const TDesC8& GetIPFilterList( );
     
    /**
    * Completes asynchronous DNS request, and removes it from the procesed list
    */
    void RequestCompleted( CUpnpIPFilteringDNSQuery* iQuery );
    
    
    /**
    * Callback if there are changes on the list
    */ 
    void IPListChange();
     
private:


    CUpnpIPFiltering( RSocketServ& aSocketServer, TInt aIap,
                      MMessageHandlerEngineObserver& aObserver );

    void ConstructL( RSocketServ& aSocketServer );

    
    TInt FindInList( const TInetAddr& aAddress ) const;
    
    void IPList2Des8L();
    
private: 


    RArray<TInetAddr> iAllowedIPsList;  //owned
    
    RSocketServ& iSockServ;             //not owned
    
    CUpnpConnectionManagerProxy* iConnectionManagerProxy; //owned
    
    TInt iActiveIap;    
    
    RBuf8 iListBuf;                     //owned
    
    RPointerArray<CUpnpIPFilteringDNSQuery> iDNSQueriesList; //owned
    
    MMessageHandlerEngineObserver& iObserver;//not owned - changes on the list
    };

#endif // C_CUPNPIPFILTERING_H
