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
* Description:  Declares the CUpnpAllowedValueRangeContentHandler class
 *
*/


#ifndef __UPNPALLOWEDVALUERANGECONTENTHANDLER_H__
#define __UPNPALLOWEDVALUERANGECONTENTHANDLER_H__

#include <babitflags.h>
#include "upnpcontenthandler.h"

class CUpnpStateVariable;

using namespace Xml;

/**
 *  This class handles allowedvaluerange subtree of service descriptions
 *  It extends CUpnpContentHandler class, thus it is subsequent 
 *  implementation of MContentHandler interface. It is designed to be 
 *  used by upnp controler that delegates to it events from xml sax parser   
 *
 *  @since Series60 2.6
 */
NONSHARABLE_CLASS( CUpnpAllowedValueRangeContentHandler ) : 
    public CUpnpContentHandler
    {
protected:
    enum TState
        {
        EInitial,
        EMinimum,
        EMaximum,
        EStep
        };

public:

    /**
     * Two-phased constructor
     * @since Series60 3.2 
     * @return instance of CUpnpAllowedValueRangeContentHandler class
     */
    static CUpnpAllowedValueRangeContentHandler* NewL(
            CUpnpContentHandlersController& aController, 
            CUpnpStateVariable& aResultAction );

    /**
     * Two-phased constructor. Leaves the object on CleanupStack
     * @since Series60 3.2 
     * @return instance of CUpnpAllowedValueRangeContentHandler class
     */
    static CUpnpAllowedValueRangeContentHandler* NewLC(
            CUpnpContentHandlersController& aController, 
            CUpnpStateVariable& aResultAction );

    /**
     * Destructor of CUpnpAllowedValueRangeContentHandler class
     * @since Series60 3.2     
     */
    virtual ~CUpnpAllowedValueRangeContentHandler();

public: // from MContentHandler

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
    CUpnpAllowedValueRangeContentHandler( CUpnpContentHandlersController& aController,
            CUpnpStateVariable& aResultAction );

private:
    CUpnpStateVariable& iResultStateVariable;
    TState iCurrentState;
    TBitFlags8 iFoundTags;

    };

#endif //__UPNPALLOWEDVALUERANGECONTENTHANDLER_H__
