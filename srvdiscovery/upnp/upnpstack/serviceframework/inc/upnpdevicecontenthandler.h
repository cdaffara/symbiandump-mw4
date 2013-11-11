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
* Description:  Declares the CUpnpDeviceContentHandler class
*
*/



#ifndef __UPNPDEVICECONTENTHANDLER_H__
#define __UPNPDEVICECONTENTHANDLER_H__

#include "upnpcontenthandler.h"

class CUpnpDevice;
class CUpnpDeviceImplementation;
class CUpnpSilentDeviceImplementation;

using namespace Xml;

/**
*  This class handles xml device descriptions
*  It extends CUpnpContentHandler class, thus it is subsequent implementation of MContentHandler interface
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*  @since Series60 2.6
*/
NONSHARABLE_CLASS( CUpnpDeviceContentHandler ) : public CUpnpContentHandler
{               
public:

    /**
    * Two-phased constructor that will parse CUpnpDevice object. If aResultDevice 
    * is NULL the result object will be created during the parsing, and will be 
    * owned by this CUpnpDeviceContentHandler.
    * Otherwise result will be parsed to existing aResultDevice object
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceContentHandler class
    */  
    static CUpnpDeviceContentHandler* NewL( CUpnpContentHandlersController& aController, 
        CUpnpDevice* aResultDevice );
    
    /**
    * Two-phased constructor that will parse CUpnpDevice object. If aResultDevice 
    * is NULL the result object will be created during the parsing, and will be 
    * owned by this CUpnpDeviceContentHandler.
    * Otherwise result will be parsed to existing aResultDevice object.
    * Leaves the object on the CleanupStack
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceContentHandler class
    */  
    static CUpnpDeviceContentHandler* NewLC( CUpnpContentHandlersController& aController, 
        CUpnpDevice* aResultDevice );

    /**
    * Two-phased constructor that will parse CUpnpDeviceImplementation object. If 
    * aResultDeviceImpl is NULL the result object will be created during the parsing, 
    * and will be owned by this CUpnpDeviceContentHandler.
    * Otherwise result will be parsed to existing aResultDevice object.
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceContentHandler class
    */  
    static CUpnpDeviceContentHandler* NewL( CUpnpContentHandlersController& aController, 
        CUpnpDeviceImplementation* aResultDeviceImpl );
    
    /**
    * Two-phased constructor that will parse CUpnpDeviceImplementation object. If 
    * aResultDeviceImpl is NULL the result object will be created during the parsing, 
    * and will be owned by this CUpnpDeviceContentHandler.
    * Otherwise result will be parsed to existing aResultDevice object.
    * Leaves the object on the CleanupStack
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceContentHandler class
    */  
    static CUpnpDeviceContentHandler* NewLC( CUpnpContentHandlersController& aController, 
        CUpnpDeviceImplementation* aResultDeviceImpl );  
        
        
    /**
    * Two-phased constructor that will parse CUpnpSilentDeviceImplementation object. If 
    * aResultDeviceImpl is NULL the result object will be created during the parsing, 
    * and will be owned by this CUpnpDeviceContentHandler.
    * Otherwise result will be parsed to existing aResultDevice object.
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceContentHandler class
    */  
    static CUpnpDeviceContentHandler* NewL( CUpnpContentHandlersController& aController, 
        CUpnpSilentDeviceImplementation* aResultSilentDeviceImpl );
    
    /**
    * Two-phased constructor that will parse CUpnpSilentDeviceImplementation object. If 
    * aResultDeviceImpl is NULL the result object will be created during the parsing, 
    * and will be owned by this CUpnpDeviceContentHandler.
    * Otherwise result will be parsed to existing aResultDevice object.
    * Leaves the object on the CleanupStack
    * @since Series60 3.2 
    * @return instance of CUpnpDeviceContentHandler class
    */  
    static CUpnpDeviceContentHandler* NewLC( CUpnpContentHandlersController& aController, 
        CUpnpSilentDeviceImplementation* aResultSilentDeviceImpl );    
    
    /**
    * Destructor of CUpnpDeviceContentHandler class
    * @since Series60 3.2     
    */	
    virtual ~CUpnpDeviceContentHandler();
    
    /**
    * Returns parsed CUpnpDevice object, and pass ownership of it to the caller.
    * One shouldn't call this method if CUpnpDeviceContentHandler dosn't have
    * ownership CUpnpDevice object.
    * @see GetResultDevice( CUpnpDevice*& aResultDevice )
    * 
    * @param aResultDevice result
    */
    void GetResultDevice( CUpnpDevice*& aResultDevice );

    /**
    * Returns parsed CUpnpDeviceImplamentation object, and pass ownership of it to the caller.
    * One shouldn't call this method if CUpnpDeviceContentHandler dosn't have
    * ownership CUpnpDeviceImplementation object.
    * @see GetResultDevice( CUpnpDevice*& aResultDevice )
    * 
    * @param aResultDeviceImpl result
    */
    void GetResultDevice( CUpnpDeviceImplementation*& aResultDeviceImpl );
    
    /**
    * Returns parsed CUpnpSilentDeviceImplamentation object, and pass ownership of it to the caller.
    * One shouldn't call this method if CUpnpDeviceContentHandler dosn't have
    * ownership CUpnpSilentDeviceImplementation object.
    * @see GetResultDevice( CUpnpDevice*& aResultDevice )
    * 
    * @param aResultDeviceImpl result
    */
    void GetResultDevice( CUpnpSilentDeviceImplementation*& aResultSilentDeviceImpl );
    
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
    * @since Series60 3.2     
    */  
    CUpnpDeviceContentHandler( CUpnpContentHandlersController& aController,
        CUpnpDevice* aResultDevice );

    /**
    * Default C++ constructor 
    * @since Series60 3.2     
    */  
    CUpnpDeviceContentHandler( CUpnpContentHandlersController& aController, 
        CUpnpDeviceImplementation* aResultDeviceImpl );
        
    /**
    * Default C++ constructor 
    * @since Series60 3.2     
    */  
    CUpnpDeviceContentHandler( CUpnpContentHandlersController& aController, 
        CUpnpSilentDeviceImplementation* aResultSilentDeviceImpl );
	
private:
    CUpnpDevice* iResultDevice;
    CUpnpDeviceImplementation* iResultDeviceImpl;
    CUpnpSilentDeviceImplementation* iResultSilentDeviceImpl;
    TBool iIsDeviceOwned;
    TParseType iParseType;
    
};

#endif //__UPNPDEVICECONTENTHANDLER_H__