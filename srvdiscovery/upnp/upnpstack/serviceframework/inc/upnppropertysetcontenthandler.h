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
* Description:  Declares the CUpnpPropertysetContentHandler class
*
*/



#ifndef __UPNPPROPERTYSETCONTENTHANDLER_H__
#define __UPNPPROPERTYSETCONTENTHANDLER_H__

#include <e32def.h>
#include <e32std.h>
#include "upnpcontenthandler.h"

_LIT8( KUpnpPropertyset,                   "propertyset" );
_LIT8( KUpnpProperty,                      "property" );
_LIT8( KEventNamespaceUri,                 "urn:schemas-upnp-org:event-1-0" );

class CUpnpDescriptionProperty;

using namespace Xml;

/**
*  This class handles xml tree in notify (gena) message
*  It extends CUpnpContentHandler class
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*/
NONSHARABLE_CLASS( CUpnpPropertysetContentHandler ) : public CUpnpContentHandler
{
public:

    /**
    * Two-phased constructor
    * @return instance of CUpnpPropertysetContentHandler class
    */
    static CUpnpPropertysetContentHandler* NewL(
            CUpnpContentHandlersController& aController,
            RPointerArray<CUpnpDescriptionProperty>& aResultPropertyset );

    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @return instance of CUpnpPropertysetContentHandler class
    */
    static CUpnpPropertysetContentHandler* NewLC(
            CUpnpContentHandlersController& aController,
            RPointerArray<CUpnpDescriptionProperty>& aResultPropertyset );

    /**
    * Destructor of CUpnpPropertysetContentHandler class
    *
    */
    virtual ~CUpnpPropertysetContentHandler();

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
    CUpnpPropertysetContentHandler( CUpnpContentHandlersController& aController,
        RPointerArray<CUpnpDescriptionProperty>& aResultPropertyset );

    /**
     * Second phase constructor
     */
    void ConstructL();

protected:
    void StoreCurrentPropertyL();

    //owned
    RBuf8 iCurrentPropertyKey;
    //owned
    RBuf8 iCurrentPropertyValue;

    //not owned
    RPointerArray<CUpnpDescriptionProperty>& iResultPropertyset;

private:
    TBool iIsInsidePropertyset;

};

#endif //__UPNPPROPERTYSETCONTENTHANDLER_H__
