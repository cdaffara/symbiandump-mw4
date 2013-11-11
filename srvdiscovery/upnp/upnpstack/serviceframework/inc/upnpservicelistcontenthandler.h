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
* Description:  Declares the CUpnpServiceListContentHandler class
*
*/



#ifndef __UPNPSERVICELISTCONTENTHANDLER_H__
#define __UPNPSERVICELISTCONTENTHANDLER_H__

#include "upnpcontenthandler.h"

class CUpnpDevice;

using namespace Xml;

/**
*  This class handles service list subtree of device descriptions
*  It extends CUpnpContentHandler class, thus it is subsequent implementation of MContentHandler interface
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser   
*
*/
NONSHARABLE_CLASS( CUpnpServiceListContentHandler ) : public CUpnpContentHandler
{               
public:
    
    /**
    * Two-phased constructor
    * @return instance of CUpnpServiceListContentHandler class
    */	
    static CUpnpServiceListContentHandler* NewL(
            CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice );
    
    /**
    * Two-phased constructor. Leaves the object on CleanupStack 
    * @return instance of CUpnpServiceListContentHandler class
    */	
    static CUpnpServiceListContentHandler* NewLC( 
            CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice );
    
    /**
    * Destructor of CUpnpServiceListContentHandler class
    *   
    */	
    virtual ~CUpnpServiceListContentHandler();
    
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
	CUpnpServiceListContentHandler( CUpnpContentHandlersController& aController, 
	                                CUpnpDevice& aResultDevice );
	
private:
    CUpnpDevice& iResultDevice;
    
};

#endif //__UPNPSERVICELISTCONTENTHANDLER_H__