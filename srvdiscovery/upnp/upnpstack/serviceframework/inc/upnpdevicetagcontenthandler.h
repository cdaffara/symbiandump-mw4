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
* Description:  Declares the CUpnpDeviceTagContentHandler class
*
*/



#ifndef __UPNPDEVICETAGCONTENTHANDLER_H__
#define __UPNPDEVICETAGCONTENTHANDLER_H__

#include <babitflags.h>

#include "upnpcontenthandler.h"

class CUpnpDevice;

using namespace Xml;

/**
*  This class handles device subtree of xml device descriptions
*  It extends CUpnpContentHandler class, thus it is subsequent implementation of MContentHandler interface
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser   
*
*  @since Series60 2.6
*/
NONSHARABLE_CLASS( CUpnpDeviceTagContentHandler ) : public CUpnpContentHandler
{
protected: 
    enum TState
        { 
        EInitial,
        EDeviceType, 
        EDeviceProperty,
        EUDN
        };      
        
public:
    
    /**
    * Two-phased constructor
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceTagContentHandler class
    */	
    static CUpnpDeviceTagContentHandler* NewL(
            CUpnpContentHandlersController& aController,
            CUpnpDevice& aResultDevice, TParseType aParseType );
    
    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceTagContentHandler class
    */	
    static CUpnpDeviceTagContentHandler* NewLC( 
            CUpnpContentHandlersController& aController,
            CUpnpDevice& aResultDevice, TParseType aParseType );
    
    /**
    * Destructor of CUpnpDeviceTagContentHandler class
    * @since Series60 3.2     
    */	
    virtual ~CUpnpDeviceTagContentHandler();
    
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
     
     virtual TBool InterestedInAllNamespaces();
     
protected:

    /**
    * Default C++ constructor 
    * @since Series60 3.2     
    */	
	CUpnpDeviceTagContentHandler( CUpnpContentHandlersController& aController, 
	        CUpnpDevice& aResultStateVariable, TParseType aParseType );
	
	/**
    * Helper function that do necessary operation when device property is found
    *
    * @param aFlagPosition  position of tag in iFoungTags
    * @param.aPropertyName  name of property to process
    **/
    void ProcessDevicePropertyL( TInt aFlagPosition, const TDesC8& aPropertyName );
    
private:
    CUpnpDevice& iResultDevice;
    TState iCurrentState;
    TBitFlags16 iFoundTags;
    TPtrC8 iCurrentDevicePropertyName;
    TParseType iParseType;
    HBufC8* iNameWithPrefix;
    
};

#endif //__UPNPDEVICETAGCONTENTHANDLER_H__