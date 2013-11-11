/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the CUpnpServiceTagContentHandler class
*
*/



#ifndef __UPNPSERVICETAGCONTENTHANDLER_H__
#define __UPNPSERVICETAGCONTENTHANDLER_H__

#include <babitflags.h>

#include "upnpcontenthandler.h"

class CUpnpService;

using namespace Xml;

/**
*  This class handles icon subtree of service descriptions
*  It extends CUpnpContentHandler class, thus it is subsequent implementation of MContentHandler interface
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser   
*
*/
NONSHARABLE_CLASS( CUpnpServiceTagContentHandler ) : public CUpnpContentHandler
{
protected: 
    enum TState
        { 
        EInitial,
        EServiceType,
        EServiceId, 
        ESCPDURL,
        EControlURL,
        EEventSubURL
        };
        
public:
    
    /**
    * Two-phased constructor
    * @return instance of CUpnpServiceTagContentHandler class
    */	
    static CUpnpServiceTagContentHandler* NewL(
            CUpnpContentHandlersController& aController, CUpnpService& aResultService );
    
    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @return instance of CUpnpServiceTagContentHandler class
    */	
    static CUpnpServiceTagContentHandler* NewLC( 
            CUpnpContentHandlersController& aController, CUpnpService& aResultService );
    
    /**
    * Destructor of CUpnpServiceTagContentHandler class
    */	
    virtual ~CUpnpServiceTagContentHandler();
    
public:	// from CUpnpContentHandler
    
    /**
    * This method is a callback to indicate an element has been parsed.
    * @param				aElement is a handle to the element's details.
    * @param				aAttributes contains the attributes for the element.
    */
    virtual void OnStartElementL( const RTagInfo& aElement, 
                                  const RAttributeArray& aAttributes );

    /**
    * This method is a callback to indicate the end of the element has been reached.
    * @param              aElement is a handle to the element's details.
    */
    virtual void OnEndElementL( const RTagInfo& aElement );

    /**
    * This method is a callback that sends the content of the element.
    * @param              aBytes is the raw content data for the element in one chunk
    */
     virtual void OnContentL( const TDesC8& aBytes );    
    
protected:
    
    /**
    * Default C++ constructor 
    */	
	CUpnpServiceTagContentHandler( CUpnpContentHandlersController& aController, 
	        CUpnpService& aResultService );

    /**
    * Process content of serviceType tag.
    * @param aBytes  text content of tag 
    **/
	void ProcessServiceTypeContentL( const TDesC8& aBytes );

    /**
    * Do necessary operation when specyfic tag inside service tag have been found
    * @param aFlagPosition  position of tag in iFoungTags
    * @param aStateToSet    state indicate that specific tag have been found
    **/
    void ProcessInternalTagL( TInt aFlagPosition, TState aStateToSet );
	
private:
    CUpnpService& iResultService;
    TState iCurrentState;
    TBitFlags8 iFoundTags;
    
};

#endif //__UPNPSERVICETAGCONTENTHANDLER_H__