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
* Description:  Implementation of the Classes CUpnpSubscriberLibrary
*                          MUpnpSubscriberLibraryObserver
*
*/


#ifndef C_CUPNPSUBSCRIBERLIBRARY_H
#define C_CUPNPSUBSCRIBERLIBRARY_H

// INCLUDES

//#include "upnpsubscriberlibraryelement.h"

#include "upnptimeoutelement.h"
#include "upnperrors.h"


// FORWARD DECLARATIONS

class CUpnpGenaMessage;
class CUpnpStateVariable;
class MUpnpSubscriberLibraryObserver;
class CUpnpSubscriberLibraryElement;
// CLASS DECLARATION

/**
*  The Subscriber Library main class. 
*  This class contains an array of subscriber library elements 
*  (of type CUpnpSubscriberLibraryElement). This list is maintained up-to-date 
*  by received subsription GENA messages. Every element in the library has 
*  a timeout. If subscription isn't renewed, it is removed from the library 
*  after subscription timeout is expired. A subsriber is also removed, 
*  if it send a unsubsription message.
*
*  @since Series60 2.6
*/
class CUpnpSubscriberLibrary : public CBase, public MUpnpTimeoutElementParent
{
public: // Constructors and destructor

    /** 
    * Two-phased constructor.
    * @param aObserver
    * @return The new instance.
    */
    static CUpnpSubscriberLibrary* NewL( MUpnpSubscriberLibraryObserver& aObserver );
    
    /** 
    * Destructor.
    */
    virtual ~CUpnpSubscriberLibrary();
    
public: // New functions

    /**
    * Adds a new subscriber or renews an existing.
    * @since Series60 2.6
    * @param aMessage GENA-message.
    * @param aElement CUpnpSubscriberLibraryElement.
    * @return TUpnpErrorCode.
    */
    TUpnpErrorCode AddInfoL( CUpnpGenaMessage* aMessage, 
                      CUpnpSubscriberLibraryElement** aElement );
    
    /**
    * Find SID
    * @since Series60 2.6
    * @param aSid SID of an subscriber
    * @return Index number. If SID isn't found KErrNotFound is returned.
    */
    TInt Find( const TDesC8& aSid );
    
    /**
    * Returns subscriber library.
    * @since Series60 2.6
    * @return Array of subscriber library elements.
    */
    RPointerArray<CUpnpSubscriberLibraryElement> SubscriberLibrary() const;
    
    /**
    * Remove subscriber
    * @since Series60 2.6
    * @param aSid SID of an subscriber to be removed.
    * @return EHttpOk if succeed, EPreconditionFailed if not.
    */
    TUpnpErrorCode Remove( const TDesC8& aSid );
    
public: // From MUpnpTimeoutElementParent
    
    /**
    * Timeout callback function
    * @since Series60 2.6
    * @param aElement The element whose timeout has ran out
    */
    void TimeoutExpiredL( CUpnpTimeoutElement* aElement );
    
private: // Constructors

    /**
    * C++ default constructor.
    * @param aObserver Observer.
    */
    CUpnpSubscriberLibrary( MUpnpSubscriberLibraryObserver& aObserver );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
private: 

    // Subscribe library observer, not owned
    MUpnpSubscriberLibraryObserver&         iObserver;

    // List of subscriber library elements, owned
    RPointerArray<CUpnpSubscriberLibraryElement>    iElementArray;
};

#endif  // C_CUPNPSUBSCRIBERLIBRARY_H

//End of File