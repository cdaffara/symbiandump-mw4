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
* Description:  Declares the CUpnpSoapContentHandler class
*
*/



#ifndef __UPNPSOAPCONTENTHANDLER_H__
#define __UPNPSOAPCONTENTHANDLER_H__

#include <e32def.h>
#include "upnpcontenthandler.h"
#include "upnpdescriptionproperty.h"
#include "upnpsoapliterals.h"

using namespace Xml;

/**
*  This class handles xml tree in action (soap) messages (requests/responses)
*  It extends CUpnpContentHandler class
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*/
NONSHARABLE_CLASS( CUpnpSoapContentHandler ) : public CUpnpContentHandler
{
public:

    /**
    * Two-phased constructor
    * @return instance of CUpnpSoapContentHandler class
    */
    static CUpnpSoapContentHandler* NewL(
            CUpnpContentHandlersController& aController,
            RPointerArray<CUpnpDescriptionProperty>& aParsedValues );

    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @return instance of CUpnpSoapContentHandler class
    */
    static CUpnpSoapContentHandler* NewLC(
            CUpnpContentHandlersController& aController,
            RPointerArray<CUpnpDescriptionProperty>& aParsedValues );

    /**
    * Destructor of CUpnpSoapContentHandler class
    *
    */
    virtual ~CUpnpSoapContentHandler();

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
    
    /**
    * This method clears iIsInsideEnvelope flag
    */
    virtual void ResetState();

protected:

    /**
    * Default C++ constructor
    */
    CUpnpSoapContentHandler( CUpnpContentHandlersController& aController,
        RPointerArray<CUpnpDescriptionProperty>& aParsedValues );

    /**
     * Second phase constructor
     */
    void ConstructL();

private:

    RPointerArray<CUpnpDescriptionProperty>& iParsedValues;

    TBool iIsInsideEnvelope;

};

#endif //__UPNPSOAPCONTENTHANDLER_H__
