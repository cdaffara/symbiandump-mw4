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
* Description:  Declares the CUpnpErrorCodeSeekerContentHandler class
*
*/



#ifndef __UPNPERRORCODESEEKERCONTENTHANDLER_H__
#define __UPNPERRORCODESEEKERCONTENTHANDLER_H__

#include "upnpcontenthandler.h"

using namespace Xml;

/**
*  This class handles soap xml subtree starting with fault element.
*  The only data we're interested in this subtree is upnp errorCode
*  Rest of subtree is ignored
*  <s:Fault>
*  <faultcode>s:Client</faultcode>
*  <faultstring>UPnPError</faultstring>
*  <detail>
*  <UPnPError xmlns="urn:schemas-upnp-org:control-1-0">
*  <errorCode>error code</errorCode>
*  <errorDescription>error string</errorDescription>
*  </UPnPError>
*  </detail>
*  </s:Fault>
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*/
NONSHARABLE_CLASS( CUpnpErrorCodeSeekerContentHandler ) : public CUpnpContentHandler
{
public:

    /**
    * Two-phased constructor
    * @param aControler controler of parsing process
    * @param aErrorValue out parameter where error value found will be stored
    * @return instance of CUpnpErrorCodeSeekerContentHandler class
    */
    static CUpnpErrorCodeSeekerContentHandler* NewL(
            CUpnpContentHandlersController& aController,
            RBuf8& aErrorValue );

    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @param aControler controler of parsing process
    * @param aErrorValue out parameter where error value found will be stored
    * @return instance of CUpnpErrorCodeSeekerContentHandler class
    */
    static CUpnpErrorCodeSeekerContentHandler* NewLC(
            CUpnpContentHandlersController& aController,
            RBuf8& aErrorValue );

    /**
    * Destructor of CUpnpErrorCodeSeekerContentHandler class
    *
    */
    virtual ~CUpnpErrorCodeSeekerContentHandler();

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
    CUpnpErrorCodeSeekerContentHandler( CUpnpContentHandlersController& aController,
        RBuf8& aErrorCode );


    /**
     * Set next state of parsing. If we're interested in particular subtree
     * state will be handeled by this class, otherwise by ignore handler
     * @param aFoundTagName name of currently analyzed tag
     */
    void SetNextStateL( const TDesC8& aFoundTagName );

    /**
     * Set previous state of parsing. When particular tag is ended
     * state of parsind process should be updated.
     */
    void SetPreviousState();

    /**
     * Set particular next state of parsing. If we're interested in particular subtree
     * state will be handeled by this class, otherwise by ignore handler.
     * @param aNextAcceptedState next state that should be set if current tag is correct
     * @param aFoundTagName name of currently analyzed tag
     */
    void SetParticularNextStateL(
        const TDesC8& aNextAcceptedState, const TDesC8& aFoundTagName );

private:

    RBuf8& iErrorValue;

    TPtrC8 iCurrentState;

};

#endif //__UPNPERRORCODESEEKERCONTENTHANDLER_H__
