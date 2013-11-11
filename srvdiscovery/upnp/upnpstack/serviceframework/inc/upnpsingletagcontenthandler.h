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
* Description:  Declares the CUpnpSingleTagContentHandler class
*
*/



#ifndef __UPNPSINGLETAGCONTENTHANDLER_H__
#define __UPNPSINGLETAGCONTENTHANDLER_H__

#include "upnpcontenthandler.h"

using namespace Xml;

/**
*  This class handles single tag of xml subtree. It stores name of tag, and value found inside
*  in descriptors passed to it. It allows only single level values and ignores any tag inside.
*  <acceptedtag>
*  acceptedValue<ignoredtag1 \><ignoredtag2>ignoredValue (inside ignoredtag)</ignoredtag2>
*  </acceptedtag>
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*/
NONSHARABLE_CLASS( CUpnpSingleTagContentHandler ) : public CUpnpContentHandler
{
public:

    /**
    * Two-phased constructor
    * @param aControler controler of parsing process
    * @param aTagName out parameter where name of found tag will be stored
    * @param aValue out patameter where value found inside tag will be stored
    * @return instance of CUpnpSingleTagContentHandler class
    */
    static CUpnpSingleTagContentHandler* NewL(
            CUpnpContentHandlersController& aController,
            RBuf8& aTagName, RBuf8& aValue );

    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @param aControler controler of parsing process
    * @param aTagName out parameter where name of found tag will be stored
    * @param aValue out patameter where value found inside tag will be stored
    * @return instance of CUpnpSingleTagContentHandler class
    */
    static CUpnpSingleTagContentHandler* NewLC(
            CUpnpContentHandlersController& aController,
            RBuf8& aTagName, RBuf8& aValue );

    /**
    * Destructor of CUpnpSingleTagContentHandler class
    *
    */
    virtual ~CUpnpSingleTagContentHandler();

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
    CUpnpSingleTagContentHandler( CUpnpContentHandlersController& aController,
        RBuf8& aTagName, RBuf8& aValue );

public:
    /**
     * Copy bytes from aSrc to aDest, and realloc aDest if its length is too less.
     * @param aDest descriptor to make copy to
     * @param aSrc data to copy
     */
    static void SafeCopyL( RBuf8& aDest, const TDesC8& aSrc );

private:

    //not owned
    RBuf8& iTagName;
    //not owned
    RBuf8& iValue;

    TBool iIsInsideOfTag;

};

#endif //__UPNPSINGLETAGCONTENTHANDLER_H__
