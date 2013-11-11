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
* Description:  CUpnpContentHandlersController
 *
*/


#include "upnpcontenthandlerscontroller.h"

#include <xml/parser.h>
#include <xml/xmlparsererrors.h>
#include <xml/matchdata.h>

#include "upnpstring.h"
#include "upnpservice.h"
#include "upnpdevice.h"
#include "upnpdeviceimplementation.h"
#include "upnpsilentdeviceimplementation.h"

#include "upnpserviceliterals.h"
#include "upnpdeviceliterals.h"

#include "upnpcontenthandler.h"
#include "upnpignorecontenthandler.h"
#include "upnpsoapcontenthandler.h"
#include "upnppropertysetcontenthandler.h"
#include "upnpservicecontenthandler.h"
#include "upnpdevicecontenthandler.h"

using namespace Xml;

// CONSTANTS
const TUint KUtfLowMax = 0x1F;
const TUint KUtfHighMin = 0x7F;
const TUint KUtfHighMax = 0xA0;

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::~CUpnpContentHandlersController
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpContentHandlersController::~CUpnpContentHandlersController()
    {
    delete iParser;
    ASSERT( NULL == iStack || iStack->IsEmpty() );
    delete iStack;
    
    iContent.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentHandlersController* CUpnpContentHandlersController::NewL()
    {
    CUpnpContentHandlersController* controller =
        CUpnpContentHandlersController::NewLC();    
    CleanupStack::Pop( controller );
    return controller;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::NewLC
// Two-phased constructor. Leaves the object on the CleanupStack
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentHandlersController* CUpnpContentHandlersController::NewLC()
    {
    CUpnpContentHandlersController* controller =
        new (ELeave) CUpnpContentHandlersController();
    CleanupStack::PushL( controller );
    controller->ConstructL();
    return controller;
    }

// -----------------------------------------------------------------------------
// CleanupResetAndDestroy
// Releases array, and object keapt in it.
// Used as method passed to TCleanupItem object.
// -----------------------------------------------------------------------------
//
LOCAL_C void CleanupResetAndDestroy(TAny* aRPointerArray)
    {
    reinterpret_cast<RPointerArray<CUpnpDescriptionProperty>* >(aRPointerArray)
        ->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::UpdateActionWithOKResponseL
// For internal use. Update all necessary information from a SOAP-message
// that should be OK answer (e.g. received by Http 200 OK message).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::UpdateActionWithOKResponseL(
    CUpnpSoapMessage* aMessage, CUpnpAction* aAction )
    {
    iSoapParser.UpdateActionWithOKResponseL( aMessage, aAction );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::UpdateActionWithErrorResponseL
// For internal use. Update all necessary information from a SOAP-message
// that should contain SOAP error (fault) message (eg. received by http
// 500 InternalServerError message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::UpdateActionWithErrorResponseL(
    CUpnpSoapMessage* aMessage, CUpnpAction* aAction )
    {
    iSoapParser.UpdateActionWithErrorResponseL( aMessage, aAction );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::UpdateActionWithRequestL
// Update action with all necessary information from
// a SOAP request message.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::UpdateActionWithRequestL(
    CUpnpSoapMessage* aMessage, CUpnpAction* aAction )
    {
    iSoapParser.UpdateActionWithRequestL( aMessage, aAction );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::AttachL
// For internal use. Attaches the information contained in a GENA buffer
// to this service. In practice, this means handling of received GENA messages.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::AttachL( const TDesC8& aGenaNotify,
                                                       CUpnpService& aService )
    {
    RPointerArray<CUpnpDescriptionProperty> parsedValues;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &parsedValues ) );

    TRAPD( parseError, ParseGenaL( aGenaNotify, parsedValues ) );
    if ( KErrNone != parseError )
        {
        User::Leave( KErrGeneral );
        }
    for ( TInt i(0); i < parsedValues.Count(); ++i )
        {
        CUpnpDescriptionProperty* property = parsedValues[i];
        aService.SetStateVariableL( property->Name(), UpnpString::Trim( property->Value()) );
        }
    CleanupStack::PopAndDestroy( &parsedValues );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseSoapL
// Parses a xml document and returns set objects of CUpnpDescriptionProperties
// It is used to parse action messages (soap)
// @pre description property array should be leave safe
// (there will be PopAndDestroy called on it in case of leave)
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::ParseSoapL(
        const TDesC8& aDescription,
        RPointerArray<CUpnpDescriptionProperty>& aParsedValues )
    {
    CUpnpSoapContentHandler* soapCH =
        CUpnpSoapContentHandler::NewL( *this, aParsedValues );
    iCorrectUri.Set( KSoapNamespaceUri );
    ParseXmlL( aDescription, soapCH );
    delete soapCH;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseParseDescriptionPropertiesL
// Parses a xml document and returns set objects of CUpnpDescriptionProperties
// It is used to parse event notification
// @pre description property array should be leave safe
// (there will be PopAndDestroy called on it in case of leave)
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::ParseGenaL(
        const TDesC8& aDescription,
        RPointerArray<CUpnpDescriptionProperty>& aParsedValues )
    {
    CUpnpPropertysetContentHandler* propsetCH =
        CUpnpPropertysetContentHandler::NewL( *this, aParsedValues );
    iCorrectUri.Set( KEventNamespaceUri );
    ParseXmlL( aDescription, propsetCH );
    delete propsetCH;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseServiceL
// Parses a xml document and returns object of CUpnpService class
// It is used for the first time the xml document is parsed
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpService* CUpnpContentHandlersController::ParseServiceL(
        const TDesC8& aDescription, CUpnpDevice* aParentDevice )
    {
    CUpnpServiceContentHandler* serviceCH =
        CUpnpServiceContentHandler::NewL( *this, aParentDevice );
    iCorrectUri.Set( KServiceNamespaceUri );
    ParseXmlL( aDescription, serviceCH );
    CUpnpService* result = serviceCH->ResultService();
    delete serviceCH;
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseServiceL
// Parses an xml document
// It is used in case when CUpnpService object already exists
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::ParseServiceL(
        const TDesC8& aDescription, CUpnpService* aService )
    {
    CUpnpServiceContentHandler* serviceCH =
        CUpnpServiceContentHandler::NewL( *this, *aService );
    iCorrectUri.Set( KServiceNamespaceUri );
    ParseXmlL(aDescription, serviceCH);
    delete serviceCH;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseDeviceL
// Parses an xml document and returns instance of CUpnpDevice class
// It is used for the first time the device xml document is parsed
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice* CUpnpContentHandlersController::ParseDeviceL(
    const TDesC8& aDescription )
    {
    return DoParseDeviceL( aDescription, (CUpnpDevice*)NULL );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseDeviceL
// Parses an xml document and returns instance of CUpnpDevice class
// It is used in case when CUpnpDevice object already exists and update of
// device xml is required
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::ParseDeviceL(
    const TDesC8& aDescription, CUpnpDevice* aDevice )
    {
    ASSERT( NULL != aDevice );
    DoParseDeviceL( aDescription, aDevice );
    }

template<class T> T* CUpnpContentHandlersController::DoParseDeviceL(
    const TDesC8& aDescription, T* aDevice )
    {
    CUpnpDeviceContentHandler* deviceCH = CUpnpDeviceContentHandler::NewL(
        *this, aDevice );
    iCorrectUri.Set( KDeviceNamespaceUri );
    ParseXmlL( aDescription, deviceCH );
    T* result(NULL);
    if ( !aDevice )
        {
        deviceCH->GetResultDevice( result );
        }
    delete deviceCH;
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseDeviceImplL
// Parses an xml document and returns instance of CUpnpDevice class
// It is used for the first time the device xml document is parsed
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceImplementation* CUpnpContentHandlersController::ParseDeviceImplL(
    const TDesC8& aDescription )
    {
    return DoParseDeviceL( aDescription, (CUpnpDeviceImplementation*)NULL );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseDeviceImplL
// Parses an xml document and returns instance of CUpnpDevice class
// It is used in case when CUpnpDeviceImplementation object already exists and
// update of device xml is required
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::ParseDeviceImplL(
    const TDesC8& aDescription, CUpnpDeviceImplementation* aDeviceImpl )
    {
    ASSERT( NULL != aDeviceImpl );
    DoParseDeviceL( aDescription, aDeviceImpl );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseSilentDeviceImplL
// Parses an xml document and returns instance of CUpnpDevice class
// It is used for the first time the device xml document is parsed
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSilentDeviceImplementation* CUpnpContentHandlersController::ParseSilentDeviceImplL(
    const TDesC8& aDescription )
    {
    return DoParseDeviceL( aDescription, (CUpnpSilentDeviceImplementation*)NULL );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseSilentDeviceImplL
// Parses an xml document and returns instance of CUpnpDevice class
// It is used in case when CUpnpSilentDeviceImplementation object already exists and
// update of device xml is required
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentHandlersController::ParseSilentDeviceImplL(
    const TDesC8& aDescription, CUpnpSilentDeviceImplementation* aDeviceImpl )
    {
    ASSERT( NULL != aDeviceImpl );
    DoParseDeviceL( aDescription, aDeviceImpl );
    }

// -----------------------------------------------------------------------------
// ReleaseHandlersOnStack
// Releases all handlers that are on CStack<CUpnpContentHandler, ETrue> object
// Used as method passed to TCleanupItem object.
// -----------------------------------------------------------------------------
//
LOCAL_C void ReleaseHandlersOnStack(TAny* aStack)
    {
    reinterpret_cast<CStack<CUpnpContentHandler, ETrue>*>(aStack)->Clear();
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ParseXmlL
// Parses a xml document by passed content handler,
// there is a guarantee that aHandlerToUse will be deleted in case of leave
// precondition: iStack is empty
// postcondition: iStack is empty
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::ParseXmlL( const TDesC8& aDescription,
    CUpnpContentHandler* aHandlerToUse )
    {
    ASSERT( iStack->IsEmpty() );
    iCurrentContentHandler = aHandlerToUse;
    iStack->PushL( aHandlerToUse );
    CleanupStack::PushL( TCleanupItem( ReleaseHandlersOnStack, iStack ) );
    iDocStarted = EFalse;
    TRAPD( parseError, Xml::ParseL( *iParser, aDescription ) );
    if ( EXmlSyntax == parseError || EXmlInvalidToken == parseError ) 
        {
        while ( iStack->Count() > 1 )
            {
            delete iStack->Pop();
            }
        aHandlerToUse->ResetState();
        iCurrentContentHandler = aHandlerToUse;   
        iDocStarted = EFalse;
        RBuf8 fixedDes;
        fixedDes.CreateL( aDescription );
        CleanupClosePushL( fixedDes );                
        RemoveForbiddenCharacters( fixedDes );        
        TRAP( parseError, Xml::ParseL( *iParser, fixedDes ) );
        CleanupStack::PopAndDestroy( &fixedDes );
        }
    
    if ( EXmlJunkAfterDocElement != parseError ) 
        {
        User::LeaveIfError( parseError );
        }
    else
        {
        //EXmlJunkAfterDocElement must not be ignored when root isn't complete
        if ( !iDocStarted || iStack->Count() != 1 )
            {
            User::Leave( KErrArgument );     //no valid root element were found
            }
        }
    ASSERT( iStack->Count() == 1 );
    ASSERT( iStack->Head() == iCurrentContentHandler && iCurrentContentHandler == aHandlerToUse );
    iStack->Pop();
    CleanupStack::Pop( iStack );
    ASSERT( iStack->IsEmpty() );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::RemoveForbiddenCharacters
// Removes forbidden characters
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::RemoveForbiddenCharacters( TDes8& aDescription )
    {    
    for( TInt i=0; i<aDescription.Length(); i++ )
        {
        if ( ( aDescription[i] <= KUtfLowMax ) ||
            ( aDescription[i] >= KUtfHighMin && aDescription[i] <= KUtfHighMax ) )
            {
            aDescription.Delete( i, 1 );
            i--;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::CUpnpContentHandlersController
// Default C++ constructor
// -----------------------------------------------------------------------------
//
CUpnpContentHandlersController::CUpnpContentHandlersController()
    : iSoapParser( *this )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ConstructL
// 2nd phase constructor, dedicated to be used by inherited classs
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::ConstructL()
    {
    _LIT8( KXmlMimeType, "text/xml" );
    _LIT8( KLibxml2, "libxml2" );
    CMatchData *matchData = CMatchData::NewL();
    CleanupStack::PushL( matchData );
    matchData->SetMimeTypeL( KXmlMimeType );
    matchData->SetVariantL( KLibxml2 );
    iParser = CParser::NewL( *matchData, *this );
    CleanupStack::PopAndDestroy( matchData );
    iStack = new (ELeave) CStack<CUpnpContentHandler,ETrue>();
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnStartDocumentL
// This method is a callback to indicate the start of the document.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnStartDocumentL(
    const RDocumentParameters& /*aDocParam*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    iDocStarted = ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnEndDocumentL
// This method is a callback to indicate the end of the document.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnEndDocumentL( TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& aAttributes,
    TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    ChunksMergingEndL();
    if ( iCurrentContentHandler->InterestedInAllNamespaces()
            || aElement.Uri().DesC().Compare( iCorrectUri ) == 0 )
        {
        iCurrentContentHandler->OnStartElementL( aElement, aAttributes );
        }
    else
        {
        SetCurrentContentHandlerL( CUpnpIgnoreContentHandler::NewL( *this ) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnEndElementL( const RTagInfo& aElement,
    TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    ChunksMergingEndL();
    iCurrentContentHandler->OnEndElementL( aElement );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnContentL
// This method is a callback that sends the content of the element.
// Not all the content may be returned in one go. The data may be sent in chunks.
// This method just append chunk to chunks previously received. When we get all
// chunks ChunksMergingEndL method pass merged chunks to current content handler.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnContentL( const TDesC8& aBytes,
    TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    const TInt newLength = iContent.Length() + aBytes.Length();
    if ( iContent.MaxLength() < newLength )
        {
        iContent.ReAllocL( newLength );
        }
    iContent.Append( aBytes );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::ChunksMergingEndL
// Method that is called when all chunks that contain current content
// were received, so we can pass merged content to current content handler
// as an argument of OnContentL method
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::ChunksMergingEndL()
    {
    if ( iContent.Length() > 0 )
        {
        TLex8 lex(iContent);
        lex.SkipSpace();
        if ( !lex.Eos() ) 
            {
            iCurrentContentHandler->OnContentL( iContent );
            }
        iContent.Zero();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnStartPrefixMappingL
// This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
// This method is always called before the corresponding OnStartElementL method.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnStartPrefixMappingL(
    const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnEndPrefixMappingL
// This method is a notification of the end of the scope of a prefix-URI mapping.
// This method is called after the corresponding DoEndElementL method.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnEndPrefixMappingL(
    const RString& /*aPrefix*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnIgnorableWhiteSpaceL
// This method is a notification of ignorable whitespace in element content.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnSkippedEntityL
// This method is a notification of a skipped entity. If the parser encounters an
// external entity it does not need to expand it - it can return the entity as aName
// for the client to deal with.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnSkippedEntityL(
    const RString& /*aName*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnProcessingInstructionL
// This method is a receive notification of a processing instruction.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    ChunksMergingEndL();
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::OnError
// This method indicates an error has occurred.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::OnError( TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::GetExtendedInterface
// This method obtains the interface matching the specified uid.
// -----------------------------------------------------------------------------
//
TAny* CUpnpContentHandlersController::GetExtendedInterface( const TInt32 /*aUid*/)
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::SetCurrentContentHandlerL
// Sets ContentHandler argument as a current content handler, so it will
// receive parsing events. Previous content handler will be push on stack
// that it could be againt current content handler after calling of
// SetPreviousContentHandler.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::SetCurrentContentHandlerL(
    CUpnpContentHandler* aNewContentHandler )
    {
    ASSERT( NULL != aNewContentHandler );
    iStack->PushL( aNewContentHandler );
    iCurrentContentHandler = aNewContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandlersController::SetPreviousContentHandler
// Deletes current content handler, and set previous content handler as current
// contetnt handler.
// -----------------------------------------------------------------------------
//
void CUpnpContentHandlersController::SetPreviousContentHandler()
    {
    ASSERT( iStack->Count() > 1 );
    delete iStack->Pop();
    iCurrentContentHandler = iStack->Head();
    }

//  End of File
