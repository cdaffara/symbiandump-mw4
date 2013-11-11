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
* Description:  Implements the CUpnpService class
*
*/


//  INCLUDE FILES
#include "upnpcommonstructs.h"
#include "upnpstring.h"

#include "upnphttpmessage.h"
#include "upnphttpmessagefactory.h"
#include "upnpserviceimplementation.h"
#include "upnpgenamessage.h"
#include "upnpdevice.h"
#include "upnpservice.h"
#include "upnpfileutils.h"
#include "upnpargument.h"

//for sax

#include "upnpcontenthandlerscontroller.h"

using namespace UpnpFileUtil;

static const TInt KMaxServiceDescriptionLength( 51200 );



// ============================ MEMBER FUNCTIONS ===============================

CUpnpService* CUpnpService::NewL( CUpnpDevice* aDevice )
    {
    CUpnpService* self = new (ELeave) CUpnpService( *aDevice );
    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpService::AddAction
// Add new action to action list
// -----------------------------------------------------------------------------
//        
void CUpnpService::AddActionL( CUpnpAction& aAction )
    {
    iActions.AppendL( &aAction );
    }
// -----------------------------------------------------------------------------
// CUpnpService::CUpnpService
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpService::CUpnpService( CUpnpDevice& aDevice )
: iParentDevice( aDevice )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::ConstructL( const TDesC8& aDescription,
        const TDesC8& /*aServiceType*/)
    {
    if ( aDescription.Length() > KMaxServiceDescriptionLength )
        {
        User::Leave( KErrTooBig );
        }
    CUpnpContentHandlersController* controller = 
        CUpnpContentHandlersController::NewLC();
    controller->ParseServiceL( aDescription, this );
    CleanupStack::PopAndDestroy( controller );
    }

// -----------------------------------------------------------------------------
// CUpnpService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpService* CUpnpService::NewL( const TDesC8& aDescription,
        const TDesC8& aServiceType,
        CUpnpDevice& aDevice )
    {
    CUpnpService* self = new (ELeave) CUpnpService( aDevice );

    CleanupStack::PushL( self );
    self->ConstructL( aDescription, aServiceType );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpService::~CUpnpService
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpService::~CUpnpService()
    {
    iActions.ResetAndDestroy();
    iActions.Close();
    iStateVariables.ResetAndDestroy();
    iStateVariables.Close();

    delete iType;
    delete iPath;
    delete iSubscriptionUrl;
    delete iControlUrl;
    delete iUrl;
    }
// -----------------------------------------------------------------------------
// CUpnpService::CreateActionLC
// Creating an action by Control Point
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAction* CUpnpService::CreateActionLC( const TDesC8& aActionName )
    {
    TInt index( 0 );

    while ( index < iActions.Count() )
        {
        CUpnpAction& action = *iActions[index];

        if ( aActionName.Compare( action.Name() ) == KErrNone )
            {
            CUpnpAction* act = CUpnpAction::NewLC( action, *this );

            act->SetDestinationAddr( iParentDevice.Address() );
            act->SetDestinationPathL( ControlUrl() );

            return act;
            }

        index++;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpService::GetActionList
// Build a list of actions provided by the CUpnpService.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::GetActionList( RPointerArray<CUpnpAction>& aActionList )
    {
    aActionList = iActions;
    }

// -----------------------------------------------------------------------------
// CUpnpService::GetAction
// Returns an Action with the given name
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TBool CUpnpService::GetAction( CUpnpAction*& aAction, 
    const TDesC8& aActionName )
    {
    TBool result = EFalse;
    aAction = NULL;

    for ( TInt i=0; i<iActions.Count(); i++ )
        {
        if ( iActions[i]->Name().Compare( aActionName ) == 0 )
            {
            result = ETrue;
            aAction = iActions[i];
            break;
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpService::StateVariableList
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpStateVariable> CUpnpService::StateVariableList() const
    {
    return iStateVariables;
    }
// -----------------------------------------------------------------------------
// CUpnpService::AddStateVariableL
// Add new state to state list
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::AddStateVariableL( CUpnpStateVariable* aStateVariable )
    {
    iStateVariables.AppendL( aStateVariable );
    }

// -----------------------------------------------------------------------------
// CUpnpService::SetStateVariableL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::SetStateVariableL( const TDesC8& aName,
        const TDesC8& aValue )
    {

    TInt i( 0 );

    for ( i=0; i < iStateVariables.Count() &&
            iStateVariables[i]->Name() != aName; i++ )
        {
        }

    if ( i != iStateVariables.Count() )
        { 
        iStateVariables[i]->SetValueL( aValue );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpService::StateVariable
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpStateVariable* CUpnpService::StateVariable( const TDesC8& aName )
    {
    TInt index = 0;

    while ( index < iStateVariables.Count() )
        {
        CUpnpStateVariable* var = iStateVariables[index];

        if ( aName.Compare( var->Name() ) == KErrNone )
            {
            return var;
            }

        index++;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpService::ServiceType
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpService::ServiceType()
    {
    if (iType)
        {
        return *iType;
        }
    else
        {
        return KNullDesC8;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpService::SetServiceTypeL
// Get service type of this (local or remote) service
// -----------------------------------------------------------------------------
//
void CUpnpService::SetServiceTypeL( const TDesC8& aServiceType )
    {
    HBufC8* tmp = aServiceType.AllocL();
    delete iType;
    iType = tmp;
    }
// -----------------------------------------------------------------------------
// CUpnpService::Path
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpService::Path()
    {
    return *iPath;
    }
// -----------------------------------------------------------------------------
// CUpnpService::SetPathL
// Get message dispatching path. For internal use only
// -----------------------------------------------------------------------------
//
void CUpnpService::SetPathL( const TDesC8& aPath )
    {
    HBufC8* tmp = aPath.AllocL();
    delete iPath;
    iPath = tmp;
    }

// -----------------------------------------------------------------------------
// CUpnpService::Address
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpService::Address()
    {
    return iParentDevice.Address();
    }

// -----------------------------------------------------------------------------
// CUpnpService::SubscriptionUrl
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpService::SubscriptionUrl()
    {
    if ( iSubscriptionUrl )
        {
        return *iSubscriptionUrl;
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpService::SetSubscriptionUrl
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::SetSubscriptionUrl( HBufC8* aUrl )
    {
    iSubscriptionUrl = aUrl;
    }

// -----------------------------------------------------------------------------
// CUpnpService::ControlUrl
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpService::ControlUrl()
    {
    if (iControlUrl)
        {
        return *iControlUrl;
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpService::SetControlUrl
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::SetControlUrl( HBufC8* aUrl )
    {
    iControlUrl = aUrl;
    }
// -----------------------------------------------------------------------------
// CUpnpService::SetServiceUrl
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpService::SetServiceDescriptionUrl( HBufC8* aUrl )
    {
    iUrl = aUrl;
    }
// -----------------------------------------------------------------------------
// CUpnpService::ControlUrl
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpService::ServiceDescriptionUrl()
    {
    if ( iUrl )
        {
        return *iUrl;
        }
    else
        {
        return KNullDesC8();
        }
    }
// -----------------------------------------------------------------------------
// CUpnpService::Device
// Gets parent device
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice& CUpnpService::Device()
    {
    return iParentDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpService::SetArgumentTypes
// Sets argument types 
// -----------------------------------------------------------------------------
//
void CUpnpService::SetArgumentTypes()
    {
    for ( TInt actionIndex(0); actionIndex < iActions.Count() ; actionIndex++ )
        {
        CUpnpAction* action = iActions[ actionIndex ];
        TInt actCount = action->ArgumentList().Count();
        for ( TInt argumentIndex(0); argumentIndex < actCount; argumentIndex++ )
            {
            CUpnpArgument* argument = action->ArgumentList()[ argumentIndex ];
            CUpnpStateVariable* stateVariable =
                    StateVariable( argument->RelatedStateVariable() );
            if ( stateVariable )
                {
                argument->SetType( stateVariable->Type() );
                }
            else
                argument->SetType( EUnknown );
            }
        }
    }
// -----------------------------------------------------------------------------
// CUpnpService::Added
// Set true value
// -----------------------------------------------------------------------------
//
void CUpnpService::Added()
    {
    iIsAdded = ETrue;
    }
// -----------------------------------------------------------------------------
// CUpnpService::IsAdded
// Get boolean value if added
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CUpnpService::IsAdded()
    {
    return iIsAdded;
    }
// -----------------------------------------------------------------------------
// CUpnpService::IsComplete
// Get boolean value if complete
// -----------------------------------------------------------------------------
// 
TBool CUpnpService::IsComplete()
    {
    return iComplete;
    }
//  End of File 
