/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CUpnpServiceImplementation
*
*/



// INCLUDE FILES
#include <e32def.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>
#include "upnpserviceimplementation.h"
#include "upnphttpmessage.h"
#include "upnpsoapmessage.h"
#include "upnpsoapmessagefactory.h"
#include "upnpgenamessage.h"
#include "upnpgenamessagefactory.h"
#include "upnpstring.h"
#include "upnpcommonupnplits.h"
#include "upnphttpmessagefactory.h"
#include "upnpdevice.h"
#include "upnpdeviceimplementationbase.h"
#include "upnpcommonstructs.h"
#include "upnplist.h"
#include "upnpfileutils.h"
#include "upnpdispatcher.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpeventcontroller.h"
#include "upnpsoapparser.h"

#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"

// -----------------------------------------------------------------------------
// CUpnpService::SetDispatcher
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::SetDispatcher( CUpnpDispatcher* aDispatcher )
    {
    if(iDispatcher)
        {
        iDispatcher->RemoveCustomer(*this, EFalse);
        }

    iDispatcher = aDispatcher;
    }

// -----------------------------------------------------------------------------
// CUpnpService::SendL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::SendL( CUpnpHttpMessage *aMessage )
    {
    if ( iDispatcher )
        {
        iDispatcher->SendMessageL( aMessage, *this );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpService::Path
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpServiceImplementation::Path()
    {
    return *iPath;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::CUpnpServiceImplementation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpServiceImplementation::CUpnpServiceImplementation( CUpnpDevice& aDevice )
: CUpnpService(aDevice)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::~CUpnpServiceImplementation
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpServiceImplementation::~CUpnpServiceImplementation()
    {
    DeviceImpl().DetachService( this );
    iEventedVariables.Reset();
    delete iEventController;
    delete iSaxController;
    delete iSoapParser;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::BaseConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::BaseConstructL( const TDesC& aFileName,
        const TDesC8& aServiceType )
    {
    //subscriber library initialization
    LOGS("CUpnpServiceImplementation::BaseConstructL");

    HBufC8* descr = UpnpFileUtil::ReadFileL( aFileName );
    CleanupStack::PushL(descr);
    // File ready

    this->ConstructL( *descr, aServiceType );

    CleanupStack::PopAndDestroy(descr);

    iEventController = CUpnpEventController::NewL( *this );

    // from device
    DeviceImpl().AttachServiceL( this );
    iComplete = ETrue;
    iDispatcher->AddCustomerL( *this, EFalse );

    iSaxController = CUpnpContentHandlersController::NewL();
    iSoapParser = new (ELeave) TUpnpSoapParser( *iSaxController );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::MessageReceivedLD
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::MessageReceivedLD( CUpnpHttpMessage* aMessage )
    {
    LOGS("CUpnpServiceImplementation::MessageReceivedL(CUpnpHttpMessage*)");

    if ( !aMessage )
        {
        return;
        }

    CleanupStack::PushL( aMessage );
    if ( aMessage->IsSoap() )
        {
        // checking content type of action. Checking that content type exists.
        TPtrC8 tempHeaderValue = aMessage->GetHeaderValue( UpnpGENA::KContentType() );
        TPtrC8 headerValue;
        TInt posOfSemicolon = tempHeaderValue.Find( UpnpString::KSemiColon() );

        // checking if content type contains semicolon
        if ( posOfSemicolon != KErrNotFound )
            {
            tempHeaderValue.Set( tempHeaderValue.Left( posOfSemicolon ) );
            TPtrC8 trimmed( UpnpString::Trim( tempHeaderValue, EFalse ) );
            headerValue.Set( trimmed );
            }
        else
            {
            headerValue.Set( tempHeaderValue );
            }

        if ( headerValue.CompareC( UpnpGENA::KDefaultContentType ) != 0 )
            {
            // content type of action is not text/xml. Invalid action.
            CUpnpHttpMessage* messageOut
            = RUpnpHttpMessageFactory::HttpResponseErrorL(aMessage,
                    EHttpUnsupportedMediaType
            );
            CleanupStack::PushL( messageOut );
            SendL( messageOut );
            CleanupStack::PopAndDestroy( messageOut );
            CleanupStack::PopAndDestroy( aMessage );
            return;
            }

        TRAPD( err, HandleActionL( reinterpret_cast<CUpnpSoapMessage*>( aMessage ) ) );
        if ( err )
            {
            SendL( reinterpret_cast<CUpnpSoapMessage*>( aMessage ),
                    MapGenericErrorToUpnp( err ) );
            }
        }
    else if ( aMessage->IsGena() )
        {
        TRAPD( err, HandleActionL( reinterpret_cast<CUpnpGenaMessage*>( aMessage ) ) );
        if ( err )
            {
            // send proper errormessage
            CUpnpHttpMessage* messageOut
            = RUpnpHttpMessageFactory::UpnpResponseErrorL(aMessage,
                    EActionFailed
            );
            CleanupStack::PushL( messageOut );
            SendL( messageOut );
            CleanupStack::PopAndDestroy( messageOut );
            }
        }
    else
        {
        //should never happen        
        LOGS1( "CUpnpServiceImplementation::MessageReceivedLD msg type %d", aMessage->Type() );
        ASSERT( EFalse );       
        }
    CleanupStack::PopAndDestroy( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::MapGenericErrorToUpnp
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpServiceImplementation::MapGenericErrorToUpnp( TInt aError )
    {
    if ( aError == KErrArgument )
        {
        return EInvalidArgs;
        }
    else
        {
        return EActionFailed;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::HandleActionL
// -----------------------------------------------------------------------------
//
void CUpnpServiceImplementation::HandleActionL(CUpnpGenaMessage* aMessage)
    {
    LOGS("CUpnpServiceImplementation::HandleActionL(CUpnpGenaMessage*)");

    if ( !aMessage )
        {
        return;
        }

    TPtrC8 method = aMessage->Method();

    if (method.Compare(UpnpGENA::KGenaSubscribe) == 0)
        {
        // Subscription or renewing:
        iEventController->SubscribeL( aMessage );
        }
    else if (method.Compare(UpnpGENA::KGenaUnSubscribe) == 0)
        {
        // UnSubscription:
        iEventController->UnSubscribeL( aMessage );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::HandleActionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpServiceImplementation::HandleActionL( CUpnpSoapMessage* aMessage )
    {
    LOGS("CUpnpServiceImplementation::HandleActionL(CUpnpSoapMessage*)");
    if ( !aMessage )
        {
        return;
        }

    // parse SOAP to CUpnpAction
    CUpnpAction* action = CreateActionLC( aMessage->ActionName() );

    if ( !action )
        {
        // send errormessage
        SendL( aMessage, EInvalidAction );
        }
    else
        {
        iSoapParser->UpdateActionWithRequestL( aMessage, action );
        TInt actionError( action->Error() );
        if ( ( EUpnpOk == actionError ) || ( EHttp200Ok == actionError ) )
            {
            CleanupStack::Pop( action );
            // a valid action received
            ActionReceivedLD( action );
            }
        else
            {
            TUpnpErrorCode upnpError;
            // error
            if ( actionError < 0 ) //is generic symbian error

                {
                upnpError = MapGenericErrorToUpnp( actionError );
                }
            else
                {
                upnpError = (TUpnpErrorCode) actionError;
                }
            SendL( aMessage, upnpError );
            CleanupStack::PopAndDestroy( action );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::SendL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::SendL( CUpnpAction* aAction,
        TUpnpErrorCode aError)
    {
    LOGS1("CUpnpServiceImplementation::SendL(CUpnpAction*, TUpnpErrorCode: %d)", aError);

    CUpnpHttpMessage* msg( NULL );
    if ( EHttpOk == aError )
        {
        msg = reinterpret_cast<CUpnpHttpMessage*>
        (RUpnpSoapMessageFactory::SoapResponseL( aAction ));
        }
    else
        {
        LOGS1("Error: %i", aError);
        msg = reinterpret_cast<CUpnpHttpMessage*>
        (RUpnpSoapMessageFactory::SoapResponseL( aAction, aError ));
        }
    CleanupStack::PushL( msg );
    SendL( msg );
    CleanupStack::PopAndDestroy( msg );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::SendL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::SendL(CUpnpSoapMessage* aMessage,
        TUpnpErrorCode aError)
    {
    LOGS1("CUpnpServiceImplementation::SendL(CUpnpSoapMessage*, TUpnpErrorCode: %d)", aError);

    CUpnpHttpMessage* msg = reinterpret_cast<CUpnpHttpMessage*>(
            RUpnpSoapMessageFactory::SoapResponseL(aMessage, aError));
    CleanupStack::PushL(msg);
    SendL(msg);
    CleanupStack::PopAndDestroy(msg);
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::SetStateVariableL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::SetStateVariableL( const TDesC8& aName,
        const TDesC8& aValue,
        TBool aIsModerated )
    {
    LOGS("CUpnpServiceImplementation::SetStateVariableL");
    TInt totalDesLength(0);
    totalDesLength = KGenaXml().Length() + KGenaPropertysetStart().Length()+
    KGenaPropertysetEnd().Length();
    for ( TInt i(0);i<iStateVariables.Count();i++ )
        {
        if ( iStateVariables[i]->Eventable().CompareF(KYes) == 0 )
            {
            totalDesLength+=KGenaPropertyStart().Length();
            totalDesLength+=iStateVariables[i]->Name().Length();
            totalDesLength+=KCloseBracket().Length();
            if ( aName.Compare( iStateVariables[i]->Name() ) == 0 )
                {
                totalDesLength+=aValue.Length();
                }
            else
                {
                totalDesLength+=iStateVariables[i]->Value().Length();
                }
            totalDesLength+=KOpenBracket().Length();
            totalDesLength+=( UpnpString::KSlash )().Length();
            totalDesLength+=iStateVariables[i]->Name().Length();
            totalDesLength+=KGenaPropertyEnd().Length();
            }
        }
    if (totalDesLength<KMaxGenaMessageLength)
        {
        CUpnpStateVariable* variable = StateVariable( aName );
        if( !variable )
            {
            User::Leave( KErrBadHandle );
            }

        variable->SetValueL( aValue, aIsModerated );
        }
    else
        {
        iEventController->SendModeratedNotification();
        }
    }
 
// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::StateVariableValue
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpServiceImplementation::StateVariableValue(
        const TDesC8& aVariableName )
    {
    CUpnpStateVariable* variable = StateVariable(aVariableName);
    if (variable)
        {
        return variable->Value();
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::StateVariable
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpStateVariable* CUpnpServiceImplementation::StateVariable(
        const TDesC8& aVariableName )
    {
    CUpnpStateVariable* variable = NULL;
    variable = CUpnpService::StateVariable(aVariableName);
    if (variable)
        {
        variable->SetParentImplementation(*this);
        }
    return variable;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::AddEventVariable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::AddEventVariable
(const CUpnpStateVariable& aVariable,
        TBool aIsModerated )
    {
    LOGS("CUpnpServiceImplementation::AddEventVariable");

    if (iEventedVariables.Find(&aVariable) == KErrNotFound)
        {
        iEventedVariables.Append(&aVariable);
        }
    if (aIsModerated)
        {
        iEventController->SendModeratedNotification();
        }
    else
        {
        iEventController->SendNonModeratedNotification();
        }

    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::SetVariableMaxEventRate
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::SetVariableMaxEventRate
(const TDesC8& aVariable, TInt aMaxRate)

    {
    LOGS("CUpnpServiceImplementation::SetVariableMaxEventRate");

    for(TInt i = 0; i < iEventedVariables.Count(); i++)
        {
        if(iEventedVariables[i]->Name().Compare(aVariable)==0)
            {
            iEventedVariables[i]->SetMaxEventRate(aMaxRate, KMaxEventRate);
            }
        }

    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::StateVariableEvented
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpServiceImplementation::StateVariableEvented(
        const TDesC8& /*aVariableName*/)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::SubcribersAmount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpServiceImplementation::SubcribersAmount()
    {
    return iEventController->SubscribersAmount();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::DeviceImpl
// -----------------------------------------------------------------------------
//
CUpnpDeviceImplementationBase& CUpnpServiceImplementation::DeviceImpl()
    {
    return (CUpnpDeviceImplementationBase&) Device();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceImplementation::EventedStateVariables
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpStateVariable>&
CUpnpServiceImplementation::EventedStateVariables()
    {
    return iEventedVariables;
    }

//  End of File
