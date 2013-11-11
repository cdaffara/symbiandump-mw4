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
 * Description:  CUpnpAction
 *
 */

// INCLUDE FILES
#include "upnpaction.h"
#include "upnpargument.h"
#include "upnpstring.h"
#include "upnpsoapmessage.h"
#include "upnpservice.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpAction::CUpnpAction
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CUpnpAction::CUpnpAction( CUpnpService& aParentService ) :
    iParentService( aParentService )
    {
    iError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAction::ConstructL( CUpnpAction& aAction )
    {
    RPointerArray<CUpnpArgument>& argumentList = aAction.ArgumentList();

    for ( TInt index = 0; index < argumentList.Count(); index++ )
        {
        CUpnpArgument* arg = CUpnpArgument::NewL( *argumentList[index],
            iParentService );
        CleanupStack::PushL( arg );
        iArguments.AppendL( arg );
        CleanupStack::Pop( arg );
        }

    iName = aAction.Name().AllocL();
    iDestinationPath = HBufC8::NewL( 0 );

    iSessionId = CUpnpHttpMessage::NewSessionIdL();
    }

// -----------------------------------------------------------------------------
// CUpnpAction::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAction* CUpnpAction::NewLC( CUpnpAction& aAction,
    CUpnpService& aParentService )
    {
    CUpnpAction* self = new (ELeave) CUpnpAction( aParentService );

    CleanupStack::PushL( self );
    self->ConstructL( aAction );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::~CUpnpAction
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAction::~CUpnpAction()
    {
    iArguments.ResetAndDestroy();
    iArguments.Close();

    delete iName;
    delete iDestinationPath;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SetArgumentL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAction::SetArgumentL( const TDesC8& aName,
    const TDesC8& aValue )
    {
    for ( TInt i = 0; i < iArguments.Count(); i++ )
        {
        CUpnpArgument* arg = iArguments[i];

        if ( aName.Compare( arg->Name() ) == 0 )
            {
            arg->SetValueL( aValue );
            return KErrNone;
            }
        }
    return KErrBadName;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::ArgumentValue
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpAction::ArgumentValue( const TDesC8& aName )
    {
    for ( TInt i = 0; i < iArguments.Count(); i++ )
        {
        CUpnpArgument* arg = iArguments[i];

        if ( aName.Compare( arg->Name() ) == 0 )
            {
            return arg->Value();
            }
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAction::Argument
//
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpArgument* CUpnpAction::Argument( const TDesC8& aName )
    {
    for ( TInt i = 0; i < iArguments.Count(); i++ )
        {
        CUpnpArgument* arg = iArguments[i];

        if ( aName.Compare( arg->Name() ) == 0 )
            {
            return arg;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SetDestinationPathL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAction::SetDestinationPathL( const TDesC8& aDestination )
    {
    delete iDestinationPath;
    iDestinationPath = NULL;
    iDestinationPath = aDestination.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpAction::DestinationPath
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpAction::DestinationPath()
    {
    return *iDestinationPath;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SetSender
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAction::SetSender( const TInetAddr & aSender )
    {
    iSender = aSender;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::Sender
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr& CUpnpAction::Sender()
    {
    return iSender;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::Name
//
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpAction::Name()
    {
    return *iName;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::ArgumentList
//
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpArgument>& CUpnpAction::ArgumentList()
    {
    return iArguments;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::DestinationAddr
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpAction::DestinationAddr() const
    {
    return iDestinationAddr;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::ArgumentType
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAction::ArgumentType( const TDesC8 &aArgumentName )
    {
    for ( TInt i = 0; i < iArguments.Count(); i++ )
        {
        CUpnpArgument* arg = iArguments[i];

        if ( aArgumentName.Compare( arg->Name() ) == 0 )
            {
            return arg->Type();
            }
        }

    return EUndefined;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::ServiceType
//
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpAction::ServiceType()
    {
    return const_cast<TDesC8&>( iParentService.ServiceType() );
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SessionId
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAction::SessionId() const
    {
    return iSessionId;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SessionId
//
// -----------------------------------------------------------------------------
//
void CUpnpAction::SetSessionId( TInt aId )
    {
    iSessionId = aId;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SetDestinationAddr
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAction::SetDestinationAddr( const TInetAddr & aAddr )
    {
    iDestinationAddr = aAddr;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::Error
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAction::Error() const
    {
    return iError;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::Service
//
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpService& CUpnpAction::Service()
    {
    return iParentService;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SetError
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAction::SetError( TInt aError )
    {
    iError = aError;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::Local
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpAction::Local() const
    {
    return iLocal;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::SetLocal
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAction::SetLocal( TBool aLocal )
    {
    iLocal = aLocal;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::NewL
// for TESTS only
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAction* CUpnpAction::NewL( const TDesC8& aActionName )
    {
    CUpnpAction* self = new (ELeave) CUpnpAction();
    CleanupStack::PushL( self );
    self->SetNameL( aActionName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::NewL
// -----------------------------------------------------------------------------
//
CUpnpAction* CUpnpAction::NewL( CUpnpService& aParentService )
    {
    CUpnpAction* self = new (ELeave) CUpnpAction( aParentService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::NewL
// -----------------------------------------------------------------------------
//
void CUpnpAction::SetNameL( const TDesC8& aName )
    {
    HBufC8* tmp = aName.AllocL();
    delete iName;
    iName = tmp;
    }

// -----------------------------------------------------------------------------
// CUpnpAction::NewL
// -----------------------------------------------------------------------------
//
void CUpnpAction::AddArgumentL( CUpnpArgument& aAction )
    {
    iArguments.AppendL( &aAction );
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpAction::ConstructL()
    {
    iSessionId = CUpnpHttpMessage::NewSessionIdL();
    iDestinationPath = HBufC8::NewL( 0 );
    iName = HBufC8::NewL( 0 );
    }

// -----------------------------------------------------------------------------
// CUpnpAction::CUpnpAction
// for TESTS only. Object isn't completely valid. 
// -----------------------------------------------------------------------------
//
CUpnpAction::CUpnpAction() 
: iParentService(*(CUpnpService*)1)//tests only iParentService shouldn't be used
    {
    }

// End of File
