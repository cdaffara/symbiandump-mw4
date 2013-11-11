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
* Description:  Declares the CUpnpContentHandlersController class
 *
*/


#ifndef __UPNPCONTENTHANDLERSCONTROLLER_H__
#define __UPNPCONTENTHANDLERSCONTROLLER_H__

#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <cstack.h>
#include "upnpsoapparser.h"

// CLASS DECLARATION
class CUpnpDevice;
class CUpnpDeviceImplementation;
class CUpnpSilentDeviceImplementation;
class CUpnpService;
class CUpnpContentHandler;
class CUpnpDescriptionProperty;
class CUpnpAction;
class CUpnpSoapMessage;

/**
 *  This class implements the interface MContentHandler required by a client of the xml framework
 *  It is designed to be used by xml parser that generates events using callback methods
 *
 *  @since Series60 2.6
 */

using namespace Xml;

NONSHARABLE_CLASS( CUpnpContentHandlersController ) :
    public CBase, public MContentHandler
    {
public:
    /**
     * Destructor
     */
    virtual ~CUpnpContentHandlersController();

    /**
     * Two-phased constructor
     * @since Series60 3.2
     * @return instance of CUpnpContentHandlersController class
     */
    IMPORT_C static CUpnpContentHandlersController* NewL();

    /**
     * Two-phased constructor. Leaves the object on CleanupStack
     * @since Series60 3.2
     * @return instance of CUpnpContentHandlersController class
     */
    IMPORT_C static CUpnpContentHandlersController* NewLC();

    /**
    * For internal use. Update all necessary information from a SOAP-message
    * that should be OK answer (e.g. received by Http 200 OK message).
    * @param aMessage SOAP response message
    * @param aAction an action to update
    */
    IMPORT_C void UpdateActionWithOKResponseL( CUpnpSoapMessage* aMessage,
        CUpnpAction* aAction );

    /**
     * For internal use. Update all necessary information from a SOAP-message
     * that should contain SOAP error (fault) message (eg. received by http
     * 500 InternalServerError message.
     * @param aMessage SOAP response message
     * @param aAction an action to update
     */
    IMPORT_C void UpdateActionWithErrorResponseL( CUpnpSoapMessage* aMessage,
            CUpnpAction* aAction );

    /**
    * For internal use. Attaches the information contained in a GENA buffer
    * to this service. In practice, this means handling of received
    * GENA messages.
    * @since Series60 3.2
    * @param aGenaNotify gena notify body to be attached,
    * @param aService service which gena will be atached to
    */
    IMPORT_C void AttachL( const TDesC8& aGenaNotify, CUpnpService& aService );

    /**
     * Parses an xml document and returns object of CUpnpService class
     * It is used for the first time the xml document is parsed
     * @since Series60 3.2
     * @param aDescription xml content
     * @param aParentDevice parent device
     * @return instance of CUpnpService class
     */
    IMPORT_C CUpnpService* ParseServiceL( const TDesC8& aDescription,
        CUpnpDevice* aParentDevice );

    /**
    * Parses an xml document
    * It is used in case when CUpnpService object already exists
    * @since Series60 3.2
    * @param aDescription xml content
    * @param aService service pointer [in-out param]
    */
    IMPORT_C void ParseServiceL( const TDesC8& aDescription, CUpnpService* aService );

    /**
    * Parses an xml document and returns instance of CUpnpDevice class
    * It is used for the first time the device xml document is parsed
    * @since Series60 3.2
    * @param aDescription xml content
    * @return instance of CUpnpDevice class
    */
    IMPORT_C CUpnpDevice* ParseDeviceL( const TDesC8& aDescription );

    /**
    * Parses an xml document and returns instance of CUpnpDevice class
    * It is used in case when CUpnpDevice object already exists and update of device
    * xml is required
    * @since Series60 3.2
    * @param aDescription xml content
    * @param aDevice pointer [in-out param]
    */
    IMPORT_C void ParseDeviceL( const TDesC8& aDescription, CUpnpDevice* aDevice );

    /**
    * Parses an xml document and returns instance of CUpnpDeviceImplementation class
    * It is used for the first time the device xml document is parsed
    * @since Series60 3.2
    * @param aDescription xml content
    * @return instance of CUpnpDeviceImplementation class
    */
    IMPORT_C CUpnpDeviceImplementation* ParseDeviceImplL( const TDesC8& aDescription );

    /**
    * Parses an xml document and returns instance of CUpnpDeviceImplementation class
    * It is used in case when CUpnpDeviceImplementation object already exists and
    * update of device xml is required
    * @since Series60 3.2
    * @param aDescription xml content
    * @param aDevice pointer [in-out param]
    */
    IMPORT_C void ParseDeviceImplL( const TDesC8& aDescription,
        CUpnpDeviceImplementation* aDeviceImpl );

    /**
    * Parses an xml document and returns instance of CUpnpSilentDeviceImplementation class
    * It is used for the first time the device xml document is parsed
    * @since Series60 3.2
    * @param aDescription xml content
    * @return instance of CUpnpDeviceImplementation class
    */
    IMPORT_C CUpnpSilentDeviceImplementation* ParseSilentDeviceImplL( const TDesC8& aDescription );

    /**
    * Parses an xml document and returns instance of CUpnpSilentDeviceImplementation class
    * It is used in case when CUpnpDeviceImplementation object already exists and
    * update of device xml is required
    * @since Series60 3.2
    * @param aDescription xml content
    * @param aDevice pointer [in-out param]
    */
    IMPORT_C void ParseSilentDeviceImplL( const TDesC8& aDescription,
        CUpnpSilentDeviceImplementation* aDeviceImpl );

    /**
    * Update action with all necessary information from
    * a SOAP request message.
    * @since Series60 3.2
    * @param aMessage SOAP request message
    * @param aAction - an action to update
    */
    void UpdateActionWithRequestL( CUpnpSoapMessage* aMessage, CUpnpAction* aAction );

    /**
    * Parses a xml document and returns set objects of CUpnpDescriptionProperties
    * It is used to parse action messages (soap)
    * @pre description property array should be leave safe
    * (there will be PopAndDestroy called on it in case of leave)
    */
    void ParseSoapL(
            const TDesC8& aDescription,
            RPointerArray<CUpnpDescriptionProperty>& aParsedValues );

    /**
     * Sets ContentHandler argument as a current content handler, so it will
     * receive parsing events. Previous content handler will be push on stack
     * that it could be againt current content handler after calling of
     * SetPreviousContentHandler.
     * @since Series60 3.2
     * @param aNewContentHandler content handler that will receive all parsing events
     *   untill current content handler will be changed
     */
    void SetCurrentContentHandlerL( CUpnpContentHandler* aNewContentHandler );

    /**
     * Deletes current content handler, and sets previous content handler to become current
     * contetnt handler, and receive all sax parsing events.
     * @since Series60 3.2
     */
    void SetPreviousContentHandler();

private: // from MContentHandler

    /**
     * This method is a callback to indicate the start of the document.
     * @param				aDocParam Specifies the various parameters of the document.
     * @arg				aDocParam.iCharacterSetName The character encoding of the document.
     * @param				aErrorCode is the error code.
     * 					If this is not KErrNone then special action may be required.
     */
    virtual void OnStartDocumentL( const RDocumentParameters& aDocParam,
        TInt aErrorCode );

    /**
     * This method is a callback to indicate the end of the document.
     * @param				aErrorCode is the error code.
     * 				If this is not KErrNone then special action may be required.
     */
    virtual void OnEndDocumentL( TInt aErrorCode );

    /**
     * This method is a callback to indicate an element has been parsed.
     * @param              aElement is a handle to the element's details.
     * @param              aAttributes contains the attributes for the element.
     * @param              aErrorCode is the error code.
     *                If this is not KErrNone then special action may be required.
     */
    virtual void OnStartElementL( const RTagInfo& aElement,
        const RAttributeArray& aAttributes, TInt aErrorCode );

    /**
     * This method is a callback to indicate the end of the element has been reached.
     * @param              aElement is a handle to the element's details.
     * @param              aErrorCode is the error code.
     *                If this is not KErrNone then special action may be required.
     */
    virtual void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );

    /**
     * This method is a callback that sends the content of the element.
     * Not all the content may be returned in one go. The data may be sent in chunks.
     * This method just append chunk to chunks previously received. When we get all
     * chunks ChunksMergingEndL method pass merged chunks to current content handler.
     * @param              aBytes is the raw content data for the element.
     *                 The client is responsible for converting the data to the
     *                 required character set if necessary.
     *                 In some instances the content may be binary and must not be converted.
     * @param              aErrorCode is the error code.
     *                If this is not KErrNone then special action may be required.
     */
    virtual void OnContentL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
     * This method is always called before the corresponding OnStartElementL method.
     * @param				aPrefix is the Namespace prefix being declared.
     * @param				aUri is the Namespace URI the prefix is mapped to.
     * @param				aErrorCode is the error code.
     * 				If this is not KErrNone then special action may be required.
     */
    virtual void OnStartPrefixMappingL( const RString& aPrefix,
        const RString& aUri, TInt aErrorCode );

    /**
     * This method is a notification of the end of the scope of a prefix-URI mapping.
     * This method is called after the corresponding DoEndElementL method.
     * @param				aPrefix is the Namespace prefix that was mapped.
     * @param				aErrorCode is the error code.
     * 				If this is not KErrNone then special action may be required.
     */
    virtual void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

    /**
     * This method is a notification of ignorable whitespace in element content.
     * @param				aBytes are the ignored bytes from the document being parsed.
     * @param				aErrorCode is the error code.
     * 				If this is not KErrNone then special action may be required.
     */
    virtual void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * This method is a notification of a skipped entity. If the parser encounters an
     * external entity it does not need to expand it - it can return the entity as aName
     * for the client to deal with.
     * @param				aName is the name of the skipped entity.
     * @param				aErrorCode is the error code.
     * 				If this is not KErrNone then special action may be required.
     */
    virtual void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

    /**
     This method is a receive notification of a processing instruction.
     * @param				aTarget is the processing instruction target.
     * @param				aData is the processing instruction data. If empty none was supplied.
     * @param				aErrorCode is the error code.
     * 				If this is not KErrNone then special action may be required.
     */
    virtual void OnProcessingInstructionL( const TDesC8& aTarget,
        const TDesC8& aData, TInt aErrorCode );

    /**
     * This method indicates an error has occurred.
     * @param				aError is the error code
     */
    virtual void OnError( TInt aErrorCode );

    /**
     * This method obtains the interface matching the specified uid.
     * @return				0 if no interface matching the uid is found.
     * 				Otherwise, the this pointer cast to that interface.
     * @param				aUid the uid identifying the required interface.
     */
    virtual TAny* GetExtendedInterface( const TInt32 aUid );
    
    /**
     * This method removes forbidden characters from description     
     * @param				aDescription to be modified
     */
    void RemoveForbiddenCharacters( TDes8& aDescription );

private:

    /**
     * Default C++ constructor
     */
    CUpnpContentHandlersController();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /**
    * Parses a xml document and returns set objects of CUpnpDescriptionProperties
    * It is used to parse event notification
    * @pre description property array should be leave safe
    * (there will be PopAndDestroy called on it in case of leave)
    */
    void ParseGenaL(
            const TDesC8& aDescription,
            RPointerArray<CUpnpDescriptionProperty>& aParsedValues );

    /**
    * Parses a xml document by passed content handler
    * there is a guarantee that aHandlerToUse will be deleted in case of leave
    * @param aDescription description in xml
    * @param aHandlerToUse content handler that will be used to parsing
    * @pre: iStack is empty
    * @post: iStack is empty
    */
    void ParseXmlL( const TDesC8& aDescription, CUpnpContentHandler* aHandlerToUse );

    /**
     * Method that is called when all chunks that contain current content
     * were received, so we can pass merged content to current content handler
     * as an argument of OnContentL method
     */
    void ChunksMergingEndL();

    /**
    * Common code for CUpnpDevice, and CUpnpDeviceImplementation objects
    */
    template<class T>
    T* DoParseDeviceL( const TDesC8& aDescription, T* aDevice );

private:
    CUpnpContentHandler* iCurrentContentHandler;
    //must always be equals to iStack.Head() kept here for performance and clarity reasons

    CParser* iParser;
    CStack<CUpnpContentHandler, ETrue>* iStack;
    RBuf8 iContent;
    TPtrC8 iCorrectUri;

    /**
     * The flag contains information if OnStartDocumentL were invoked
     * It isn't invoked f.e if xml document is empty
     */
    TBool iDocStarted;

    /**
     * Object responsible for soap parsing logic
     */
    TUpnpSoapParser iSoapParser;

    };

#endif //__UPNPCONTENTHANDLERSCONTROLLER_H__
