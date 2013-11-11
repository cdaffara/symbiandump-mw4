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
* Description:  Declares the CUpnpContentHandler class
 *
*/


#ifndef __UPNPCONTENTHANDLER_H__
#define __UPNPCONTENTHANDLER_H__

#include <xml/contenthandler.h>
#include <babitflags.h>
    
class CUpnpContentHandlersController;

/**
 * Abstract base class for all upnp content handlers used by CUpnpController.
 * It uses events similar to Xml::MContentHandler, but excludes not used events
 * and parameters.
 *
 */
    
using namespace Xml;

NONSHARABLE_CLASS(CUpnpContentHandler) : public CBase
    {
public:

    //parsing type
    enum TParseType 
        {
        EDevice,
        EDeviceImpl,
        ESilentDeviceImpl
        };
        
    /**
     * This method is a callback to indicate an element has been parsed.
     * @param				aElement is a handle to the element's details.
     * @param				aAttributes contains the attributes for the element.
     */
    virtual void OnStartElementL( const RTagInfo& aElement,
            const RAttributeArray& aAttributes ) = 0;

    /**
     * This method is a callback to indicate the end of the element has been reached.
     * @param				aElement is a handle to the element's details.
     */
    virtual void OnEndElementL( const RTagInfo& aElement ) = 0;

    /**
     * This method is a callback that sends the content of the element.
     * @param				aBytes is the raw content data for the element. 
     * The client is responsible for converting the data to the 
     * required character set if necessary.
     * In some instances the content may be binary and must not be converted.
     */
    virtual void OnContentL( const TDesC8& aBytes ) = 0;

    /**
     * Returns value if class is interested in tags all namespaces 
     * 
     * @return ETrue when the class is interested in tags in all namespaces 
     */
    virtual TBool InterestedInAllNamespaces();
    
     /**
     * Reset internal state
     */
    virtual void ResetState();
protected:

    /**
     * Set controller's current content handler to IgnoreContentHandler object
     * 
     **/
    void SetIgnoreHandlerL();

    /**
     * Returns ETrue if tag on aPosition has been already found.
     * If not set that now it is found and return EFalse.
     * @param              aPosition position of tag in iFoundTags
     * @param              aFoundTags flags with found tags set
     * @return             ETrue is found, EFalse otherwise
     */
    template<class T>
    inline TBool IsTagRepeated( TInt aPosition, TBitFlagsT<T>& aFoundTags );

    /**
     * Leaves if tag on aPosition has been already found.
     * If not just set that now it is found.
     * @param              aPosition position of tag in iFoundTags
     * @param              aFoundTags flags with found tags set
     */
    template<class T>
    inline void RepeatedTagCheckL( TInt aPosition, TBitFlagsT<T>& aFoundTags );

    /**
     * Constructor 
     * 
     * @param aController reference to controller of the object
     */
    CUpnpContentHandler( CUpnpContentHandlersController& aController );

protected:
    CUpnpContentHandlersController& iController;

    };

#include "upnpcontenthandler.inl"
#endif //__UPNPCONTENTHANDLER_H__
