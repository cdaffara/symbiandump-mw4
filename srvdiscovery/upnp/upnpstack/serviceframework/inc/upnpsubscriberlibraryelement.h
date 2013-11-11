/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the CUpnpSubscriberLibraryElement class
*
*/


#ifndef C_CUPNPSUBSCRIBERLIBRARYELEMENT_H
#define C_CUPNPSUBSCRIBERLIBRARYELEMENT_H

// INCLUDES

//#include <e32std.h>
#include <in_sock.h>
#include "upnptimeoutelement.h"
#include "upnperrors.h"

// FORWARD DECLARATIONS

class CUpnpGenaMessage;

// CONSTANTS

_LIT8( KDefaultTimeout, "300" );
const TUint KMaxRenewTimeout = 2100;
const TUint KDefaultTimeoutValue = 300;
// 2^31-1 // to avoid warning!
const TUint32 KMaxSeq = 2146583647; 
const TUint32 KMinSeq = 1;

// CLASS DECLARATION

/**
*  A class which is used to hold subscriber information.
*  This class is used by CUpnpSubscriberLibrary to keep a list 
*  of service subscribers.
*
*  @since Series60 2.6
*/
class CUpnpSubscriberLibraryElement : public CUpnpTimeoutElement
{
public: // Constructors and destructor
    
    /** 
    * Two-phased constructor.
    * @param aParent
    * @return The new instance.
    */
    static CUpnpSubscriberLibraryElement* NewL( MUpnpTimeoutElementParent& aParent );
    
    /** 
    * Destructor.
    */
    virtual ~CUpnpSubscriberLibraryElement();
        
public: // New functions

    /**
    * Adds a new subscriber. For a new subscriber, SEQ defaults to zero.
    * @since Series60 2.6
    * @param aMessage A valid subscribe message.
    * @return EHttpOk or error.
    */
    TUpnpErrorCode AddSubscriberL(CUpnpGenaMessage* aMessage);

    /**
    * Renews subscription.
    * @since Series60 2.6
    * @param aMessage A valid subscription renewal message.
    * @return EHttpOk or error.
    */
    TUpnpErrorCode RenewSubscriberL(CUpnpGenaMessage* aMessage);
        
public: // New functions

    /**
    * Get SID header
    * @since Series60 2.6
    * @return Subscriber ID
    */
    TDesC8& Sid();

    /**
    * Get callback address
    * @since Series60 2.6
    * @return Callback Address
    */
    TInetAddr& CallbackAddress();
        
    /**
    * Get callback path
    * @since Series60 2.6
    * @return Callback path
    */
    TDesC8& CallbackPath();

    /**
    * Get SEQ header
    * @since Series60 2.6
    * @return Sequence key
    */
    TUint32 Seq() const;

    /**
    * Get timeout
    * @since Series60 2.6
    * @return Subscription expiring timeout
    */
    TDesC8& Timeout();
    
    /**
    * Icreases SEQ by 1. If maximum is reached, resets to 1.
    * @since Series60 2.6
    */
    void IncreaseSeq();
     
    /**
    * Set Subscriber ID.
    * @since Series60 2.6
    * @param aSid UUID.
    */
    void SetSidL( const TDesC8& aSid );

    /**
    * Setter for Callback Address.
    * @since Series60 2.6
    * @param aIp IPv4
    * @param aPort Port
    */
    void SetCallbackAddress( const TDesC8& aIp, 
                                      const TDesC8& aPort );

    /**
    * Set Callback path.
    * @since Series60 2.6
    * @param aCallbackPath Callback path
    */
    void SetCallbackPathL( const TDesC8& aCallbackPath );

    /**
    * Set Sequence key.
    * @since Series60 2.6
    * @param aSeq
    */
    void SetSeq( TUint32 aSeq );

    /**
    * Set Subscription expiring timeout.
    * @since Series60 2.6
    * @param aTimeout
    */
    void SetTimeoutL( const TDesC8& aTimeout );
    
    void SetSessId(TInt aSessId);
    
    TInt SessId();
        
private: // Constructors

    /**
    * C++ default constructor.
    */
    CUpnpSubscriberLibraryElement( MUpnpTimeoutElementParent& aParent );
       
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
        
private: // New functions

    /**
    * Sets subscription info.
    * @since Series60 2.6
    * @param aSid
    * @param aCallbackIp
    * @param aCallbackPort
    * @param aCallbackPath
    * @param aTimeout
    */
    void AddInfoL( const TDesC8& aSid, 
                            const TDesC8& aCallbackIp, 
                            const TDesC8& aCallbackPort, 
                            const TDesC8& aCallbackPath, 
                            const TDesC8& aTimeout );
        
private: 

    // SID buffer, owned
    HBufC8* iSid;

    // Callback address, owned
    TInetAddr iCallbackAddress;

    // Callbck path, owned
    HBufC8* iCallbackPath;

    // SEQ buffer, owned
    TUint32 iSeq;

    // Timeout buffer, owned
    HBufC8* iTimeout;
    
    //SessionId
    TInt iSessId;
};

#endif //   C_CUPNPSUBSCRIBERLIBRARYELEMENT_H

//  End of File