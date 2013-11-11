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
* Description:  Declares the CUpnpIgnoreContentHandler class
 *
*/


#ifndef __UPNPIGNORECONTENTHANDLER_H__
#define __UPNPIGNORECONTENTHANDLER_H__

#include "upnpcontenthandler.h"

class CUpnpContentHandlersController;

/**
 *  Dummy implementation of MContentHandler (by derivation from CUpnpContentHandler) - ignore all subtree.
 * After correct use of it (correct methods calls series) it's ready to be reused again.
 * It is designed to be used by upnp controler that delegates to it events from xml sax parser
 *
 */

NONSHARABLE_CLASS(CUpnpIgnoreContentHandler) : public CUpnpContentHandler
    {
public:

    /**
     * Two-phased constructor
     * @since Series60 3.2 
     * @return instance of CUpnpOldServiceContentHandler class
     */
    static CUpnpIgnoreContentHandler* NewL( 
        CUpnpContentHandlersController& aController );

    /**
     * Two-phased constructor. Leaves the object on CleanupStack
     * @since Series60 3.2 
     * @return instance of CUpnpOldServiceContentHandler class
     */
    static CUpnpIgnoreContentHandler* NewLC( 
        CUpnpContentHandlersController& aController );

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

private:

    /**
     * Default C++ constructor 
     * @since Series60 3.2     
     */
    CUpnpIgnoreContentHandler( CUpnpContentHandlersController& aController );

private:
    TUint iOpenedTagCounter;

    };

#endif //__UPNPIGNORECONTENTHANDLER_H__
