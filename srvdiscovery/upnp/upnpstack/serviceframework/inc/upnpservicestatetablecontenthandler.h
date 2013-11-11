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
* Description:  Declares the CUpnpServiceStateTableContentHandler class
 *
*/


#ifndef __UPNPSERVICESTATETABLECONTENTHANDLER_H__
#define __UPNPSERVICESTATETABLECONTENTHANDLER_H__

#include "upnpcontenthandler.h"

class CUpnpService;
class CUpnpStateVariable;

using namespace Xml;

/**
 *  This class handles service state table subtree of service descriptions
 *  It extends CUpnpContentHandler class, thus it is subsequent implementation of 
 *  MContentHandler interface.
 *  It is designed to be used by upnp controler that delegates to it events from 
 *  xml sax parser   
 *
 *  @since Series60 2.6
 */
NONSHARABLE_CLASS(CUpnpServiceStateTableContentHandler) : 
    public CUpnpContentHandler
    {
public:

    /**
     * Two-phased constructor
     * @since Series60 3.2 
     * @return instance of CUpnpServiceStateTableContentHandler class
     */
    static CUpnpServiceStateTableContentHandler* NewL(
            CUpnpContentHandlersController& aController, 
            CUpnpService& aResultService );

    /**
     * Two-phased constructor. Leaves the object on CleanupStack
     * @since Series60 3.2 
     * @return instance of CUpnpServiceStateTableContentHandler class
     */
    static CUpnpServiceStateTableContentHandler* NewLC(
            CUpnpContentHandlersController& aController, 
            CUpnpService& aResultService );

    /**
     * Destructor of CUpnpServiceStateTableContentHandler class
     * @since Series60 3.2     
     */
    virtual ~CUpnpServiceStateTableContentHandler();

public: // from MContentHandler

    /**
     * This method is a callback to indicate an element has been parsed.
     * @param				aElement is a handle to the element's details.
     * @param				aAttributes contains the attributes for the element.
     */
    virtual void OnStartElementL( const RTagInfo& aElement, 
        const RAttributeArray& aAttributes );

    /**
     This method is a callback to indicate the end of the element has been reached.
     @param              aElement is a handle to the element's details.
     */
    virtual void OnEndElementL( const RTagInfo& aElement );

    /**
     This method is a callback that sends the content of the element.
     @param              aBytes is the raw content data for the element in one chunk
     */
    virtual void OnContentL( const TDesC8& aBytes );

protected:

    /**
     * Default C++ constructor 
     * @since Series60 3.2     
     */
    CUpnpServiceStateTableContentHandler( CUpnpContentHandlersController& aController,
            CUpnpService& aResultService );

    /**
     * Parse attributes, and set correct eventable values to aResultStateVariable.
     * @param aResultStateVariable result to change eventable values in
     * @param aAttributes attributes we analyze
     */
    void ParseAttributesL( CUpnpStateVariable& aResultStateVariable,
            const RAttributeArray& aAttributes );

private:
    CUpnpService& iResultService;
    TBool iStateVariableFound;

    };

#endif //__UPNPSERVICESTATETABLECONTENTHANDLER_H__
