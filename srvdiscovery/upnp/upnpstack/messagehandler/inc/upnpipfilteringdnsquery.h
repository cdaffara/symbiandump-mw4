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


#ifndef C_CUPNPIPFILTERINGDNSQUERY_H
#define C_CUPNPIPFILTERINGDNSQUERY_H

#include <e32cmn.h>
#include <in_sock.h>
#include <ES_SOCK.H>

#include "upnpipfiltering.h"

 /**
 *  Encapsulates DNS query
 *
 *  @since S60 v3.2
 */
class CUpnpIPFilteringDNSQuery : public CActive
    {

private:

    enum TActionType
        {
        EAddAddress,
        ERemoveAddress,
        };

public:
    /**
    * Two-phased constructor.
    * @param aMessage a messagy which will ber cmpleted when queris resoves name
    * @param aRemoteName a name to be resolved by DNS server
    */
    static CUpnpIPFilteringDNSQuery* NewL( const RMessage2& aMessage, 
                                           const TDesC8& aRemoteName );
     
    /**
    * Two-phased constructor.
    * @param aMessage a messagy which will ber cmpleted when queris resoves name
    * @param aRemoteName a name to be resolved by DNS server
    */
    static CUpnpIPFilteringDNSQuery* NewLC( const RMessage2& aMessage, 
                                            const TDesC8& aRemoteName );

    /**
    * Destructor.
    */
    virtual ~CUpnpIPFilteringDNSQuery();
    
    /**
    * Marks current query as addAddress
    */    
    void AddAddress( );
    
    /**
    * Marks current query as removeAddress
    */    
    void RemoveAddress( );
    
    /**
    * Starts to process current query
    * @param 
    * @param aConnection an network interface which will be used during queries
    * @param aListOwner here we store results of queries
    */    
    void StartDNSQuery( RSocketServ& aSockServ, RConnection& aConnection , 
                        CUpnpIPFiltering* aListOwner ); 
    
public://from CActive
     
    /**
    * Completes with query result
    */
    void RunL();    
    
    /**
    * Handels leaves from RunL method
    */
    TInt RunError( TInt aError );
   
    
private:
    /**
    * Constructor
    */
    CUpnpIPFilteringDNSQuery( const RMessage2& aMessage );

    /**
    * Two-phased constructor.
    */
    void ConstructL( const TDesC8& aRemoteName );
    
    void RequestCompleted( TInt aStatus );
    
    /**
    * Evaluates query result
    */
    TInt Status( TInt aStatus );
    
private://from CActive
    
    void DoCancel();

private: 

    RHostResolver iHostResolver;        //owned
    TActionType iAction;
    RBuf iRemoteName;                   //owned
    TNameEntry iAddrDetails;
    
    TInt iDNSStatus;
    const RMessage2 iMessage;           //not owned
    
    CUpnpIPFiltering* iIPListOwner;     //not owned
    };

#endif // C_CUPNPIPFILTERINGDNSQUERY_H
