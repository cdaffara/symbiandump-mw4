/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of class CDrmHandler.   
*
*/


// INCLUDE FILES

#include <DRMCommon.h>
#include <DocumentHandler.h>
#include "DrmHandler.h"
#include "CodLogger.h"
#include "CodPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDrmHandler::NewL()
// ---------------------------------------------------------
//
CDrmHandler* CDrmHandler::NewL( const TDesC& aFname )
    {
    CDrmHandler* handler = new (ELeave) CDrmHandler();
    CleanupStack::PushL( handler );
    handler->ConstructL( aFname );
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CDrmHandler::~CDrmHandler()
// ---------------------------------------------------------
//
CDrmHandler::~CDrmHandler()
    {
    CLOG(( ECodEng, 2, _L("-> *** CDrmHandler::~CDrmHandler") ));
    delete iDrmCommon;
    iRightsClient.Close();
    delete iContentUri;
    delete iType;
    CLOG(( ECodEng, 2, _L("<- *** CDrmHandler::~CDrmHandler") ));
    }

// ---------------------------------------------------------
// CDrmHandler::Cleanup()
// ---------------------------------------------------------
//
void CDrmHandler::Cleanup()
    {
    CLOG(( ECodEng, 2, _L("-> CDrmHandler::Cleanup") ));
    if ( iContentUri && iRightsClient.Handle() )
        {
#ifdef __TEST_COD_LOG
        TInt err =  // ('Log-only' variable.)
#endif /* def __TEST_COD_LOG */
        iRightsClient.DeleteDbEntry( *iContentUri );
        CLOG(( ECodEng, 3, _L("  err(%d)"), err ));
        delete iContentUri;
        iContentUri = NULL;
        }
    CLOG(( ECodEng, 2, _L("<- CDrmHandler::Cleanup") ));
    }

// ---------------------------------------------------------
// CDrmHandler::Type()
// ---------------------------------------------------------
//
TPtrC8 CDrmHandler::Type() const
    {
    TPtrC8 ptr;
    if ( iType )
        {
        ptr.Set( *iType );
        }
    return ptr;
    }
    
// ---------------------------------------------------------
// CDrmHandler::CDrmHandler()
// ---------------------------------------------------------
//
CDrmHandler::CDrmHandler()
    {
    CLOG(( ECodEng, 2, _L("*** CDrmHandler::CDrmHandler") ));
    }

// ---------------------------------------------------------
// CDrmHandler::ConstructL()
// ---------------------------------------------------------
//
void CDrmHandler::ConstructL( const TDesC& aFname )
    {
    CLOG(( ECodEng, 2, _L("-> CDrmHandler::ConstructL") ));
    TBool isDcf( EFalse );
    iDrmCommon = DRMCommon::NewL();
    User::LeaveIfError( iDrmCommon->Connect() );
    User::LeaveIfError( iDrmCommon->IsProtectedFile( aFname, isDcf ) );
    if ( !isDcf )
        {
        CLOG(( ECodEng, 4, _L("  not DCF") ));
        User::Leave( KErrNotSupported );
        }
    DRMCommon::TContentProtection prot;
    TUint length;
    User::LeaveIfError( iDrmCommon->GetFileInfo
        ( aFname, prot, iType, iContentUri, length ) );
#ifdef __TEST_COD_LOG
    TPtrC8 mime( iType->Des() );
    CLOG(( ECodEng, 4, \
        _L8("  inside DCF: MIME==<%S>, size(%u), prot(%d)"), \
        &mime, length, prot ));
#endif /* def __TEST_COD_LOG */
    CheckTypeL( TDataType( *iType ) );
    delete iDrmCommon;
    iDrmCommon = NULL;    
    // For Forward-Locked and Combined Delivery messages, the
    // saved rights need to be cleaned up if something goes wrong.
    // Prepare for cleanup.
    if( prot != DRMCommon::ESuperDistributable )
        {
        CLOG(( ECodEng, 4, _L("  connect DRM Rights Client") ));
        User::LeaveIfError( iRightsClient.Connect() );
        }
    else
        {
        // No cleanup is required, delete uneeded members now.
        delete iContentUri;
        iContentUri = NULL;
        }
    CLOG(( ECodEng, 2, _L("<- CDrmHandler::ConstructL") ));
    }

// ---------------------------------------------------------
// CDrmHandler::CheckTypeL
// ---------------------------------------------------------
//
void CDrmHandler::CheckTypeL( const TDataType& aType ) const
    {
#ifdef __TEST_COD_LOG
    TPtrC8 mime( aType.Des8() );
    CLOG(( ECodEng, 4, _L8("-> CDrmHandler::CheckTypeL(%S)"), &mime ));
#endif /* def __TEST_COD_LOG */
    __ASSERT_DEBUG( iDrmCommon, CodPanic( ECodInternal ) );
    TBool supported( EFalse );
    TDataType type;
    TInt i;
    User::LeaveIfError( iDrmCommon->DataTypesCount( i ) );
    for( i--; i >= 0; i-- )
        {
        User::LeaveIfError( iDrmCommon->SupportedDataType( i, type ) );
        if( type == aType )
            {
            supported = ETrue;
            break;
            }
        }
    if( !supported )
        {
        User::Leave( KMimeNotSupported );
        }
    CLOG(( ECodEng, 4, _L("<- CDrmHandler::CheckTypeL") ));
    }
