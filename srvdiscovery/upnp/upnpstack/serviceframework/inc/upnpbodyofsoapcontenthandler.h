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
* Description:  Declares the CUpnpBodyOfSoapContentHandler class
*
*/



#ifndef __UPNPBODYOFSOAPCONTENTHANDLER_H__
#define __UPNPBODYOFSOAPCONTENTHANDLER_H__

#include <e32def.h>
#include "upnppropertysetcontenthandler.h"

using namespace Xml;

/**
*  This class handles body subtree of soap messages
*  It extends CUpnpContentHandler class
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*/
NONSHARABLE_CLASS( CUpnpBodyOfSoapContentHandler ) : public CUpnpPropertysetContentHandler
{
public:

    /**
    * Two-phased constructor
    * @return instance of CUpnpBodyOfSoapContentHandler class
    */
    static CUpnpBodyOfSoapContentHandler* NewL(
            CUpnpContentHandlersController& aController,
            RPointerArray<CUpnpDescriptionProperty>& aResultSetOfArguments );

    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @return instance of CUpnpBodyOfSoapContentHandler class
    */
    static CUpnpBodyOfSoapContentHandler* NewLC(
            CUpnpContentHandlersController& aController,
            RPointerArray<CUpnpDescriptionProperty>& aResultSetOfArguments );

    /**
    * Destructor of CUpnpBodyOfSoapContentHandler class
    *
    */
    virtual ~CUpnpBodyOfSoapContentHandler();

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
     * Returns value if class is interested in tags all namespaces
     *
     * @return ETrue when the class is interested in tags in all namespaces
     */
    virtual TBool InterestedInAllNamespaces();

protected:

    /**
    * Default C++ constructor
    */
    CUpnpBodyOfSoapContentHandler( CUpnpContentHandlersController& aController,
        RPointerArray<CUpnpDescriptionProperty>& aResultSetOfArguments );

    /**
     * Second phase constructor
     */
    void ConstructL();

private:
    /**
     * Returns true if content handler is in inside argument state
     */
    TBool InsideOfArgument();

private:

    TBool iIsInsideOfAction;

};

#endif //__UPNPBODYOFSOAPCONTENTHANDLER_H__
