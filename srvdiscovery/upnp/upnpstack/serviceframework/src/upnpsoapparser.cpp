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
* Description:  TUpnpSoapParser
 *
*/


#include "upnpsoapparser.h"

#include "upnpsoapmessage.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpaction.h"
#include "upnpsoapliterals.h"

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
TUpnpSoapParser::TUpnpSoapParser(
    CUpnpContentHandlersController& aParsingController )
    : iParsingController( aParsingController )
    {
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
// TUpnpSoapParser::UpdateActionWithOKResponseL
// For internal use. Update all necessary information from a SOAP-message
// that should be OK answer (e.g. received by Http 200 OK message).
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::UpdateActionWithOKResponseL( CUpnpSoapMessage* aMessage,
    CUpnpAction* aAction )
    {
    if ( NULL == aMessage )
        {
        return;
        }
    RPointerArray<CUpnpDescriptionProperty> parsedValues;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &parsedValues ) );

    UpdateActionL( aMessage, aAction, parsedValues );
    if ( parsedValues[0]->Name() == KUpnpSoapFault )
        {
        UpdateActionFaultL( parsedValues[0], aAction );
        }
    else
        {
        ProcessActionArgumentsL( parsedValues, aAction );
        }
    CleanupStack::PopAndDestroy( &parsedValues );
    }

// -----------------------------------------------------------------------------
// TUpnpSoapParser::UpdateActionWithErrorResponseL
// For internal use. Update all necessary information from a SOAP-message
// that should contain SOAP error (fault) message (eg. received by http
// 500 InternalServerError message.
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::UpdateActionWithErrorResponseL(
    CUpnpSoapMessage* aMessage, CUpnpAction* aAction )
    {
    if ( NULL == aMessage )
        {
        return;
        }
    RPointerArray<CUpnpDescriptionProperty> parsedValues;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &parsedValues ) );

    UpdateActionL( aMessage, aAction, parsedValues );
    if ( parsedValues[0]->Name() == KUpnpSoapFault )
        {
        UpdateActionFaultL( parsedValues[0], aAction );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( &parsedValues );
    }

// -----------------------------------------------------------------------------
// TUpnpSoapParser::UpdateActionWithRequestL
// Update action with all necessary information from
// a SOAP request message.
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::UpdateActionWithRequestL(
    CUpnpSoapMessage* aMessage, CUpnpAction* aAction )
    {
    if ( NULL == aMessage )
        {
        return;
        }
    RPointerArray<CUpnpDescriptionProperty> parsedArguments;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &parsedArguments ) );

    UpdateActionL( aMessage, aAction, parsedArguments );
    ProcessActionArgumentsL( parsedArguments, aAction );
    CleanupStack::PopAndDestroy( &parsedArguments );
    }

// -----------------------------------------------------------------------------
// TUpnpSoapParser::UpdateActionL
// Common code used by 3 particular soap update action methods methods
// @post if there were no leave aParsedValues has at least 1 object within
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::UpdateActionL( CUpnpSoapMessage* aMessage,
    CUpnpAction* aAction, RPointerArray<CUpnpDescriptionProperty>& aParsedValues )
    {
    aAction->SetSender( aMessage->Sender() );
    aAction->SetLocal( aMessage->Local() );
    aAction->SetSessionId( aMessage->SessionId() );

    iParsingController.ParseSoapL( aMessage->Body(), aParsedValues );
    if ( aParsedValues.Count() == 0 )    //error of parsing, nothing retrieved
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// TUpnpSoapParser::ProcessActionArgumentsL
// Process parsed description property, and the rest of parsed properties
// and update action arguments with them.
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::ProcessActionArgumentsL(
    RPointerArray<CUpnpDescriptionProperty>& aProperties, CUpnpAction* aAction )
    {
    TRAPD( err, UpdateActionArgumentsL( aProperties, aAction ) );
    if( KErrNone != err )
        {
        if ( EInvalidArgs == err )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// TUpnpSoapParser::UpdateActionArgumentsL
// Update all action arguments with previously parsed properties.
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::UpdateActionArgumentsL(
    RPointerArray<CUpnpDescriptionProperty>& aProperties, CUpnpAction* aAction )
    {
    aAction->SetError( EHttpOk );
    for ( TInt i(1); i < aProperties.Count(); ++i )
        {
        CUpnpDescriptionProperty* property = aProperties[i];
        aAction->SetArgumentL( property->Name(), property->Value() );
        }
    }

// -----------------------------------------------------------------------------
// TUpnpSoapParser::UpdateActionFaultL
// Update action error value from parsed description property
// -----------------------------------------------------------------------------
//
void TUpnpSoapParser::UpdateActionFaultL(
    CUpnpDescriptionProperty* aProperty, CUpnpAction* aAction )
    {
    TLex8 code( aProperty->Value() );
    TInt errNumber( aAction->Error() );
    TInt parseErr = code.Val( errNumber );
    aAction->SetError( errNumber );
    User::LeaveIfError( parseErr );
    }

//  End of File
