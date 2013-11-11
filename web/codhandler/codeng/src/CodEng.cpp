/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Implementation of class CCodEng.   
*      
*
*/


// INCLUDE FILES

#include "CodEng.h"
#include "CodData.h"
#include "CodError.h"
#include "CodPanic.h"
#include "CodStatus.h"
#include "CodParser.h"
#include "CodLogger.h"
#include "CodSaver.h"
#include <DocumentHandler.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodEng::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodEng* CCodEng::NewL
( CEikProcess* aProcess, MCodLoadObserver* aObserver )
    {
    CCodEng* model = new (ELeave) CCodEng( aObserver );
    CleanupStack::PushL( model );
    model->ConstructL( aProcess );
    CleanupStack::Pop( model );
    return model;
    }

// ---------------------------------------------------------
// CCodEng::~CCodEng()
// ---------------------------------------------------------
//
EXPORT_C CCodEng::~CCodEng()
    {
    CLOG(( ECodEng, 2, _L("CCodEng::~CCodEng") ));
    }

// ---------------------------------------------------------
// CCodEng::CCodEng()
// ---------------------------------------------------------
//
CCodEng::CCodEng( MCodLoadObserver* aObserver ): CCodEngBase( aObserver )
    {
    CLOG(( ECodEng, 2, _L("CCodEng::CCodEng") ));
    }

// ---------------------------------------------------------
// CCodEng::CheckDataL()
// ---------------------------------------------------------
//
void CCodEng::CheckDataL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEng::CheckDataL") ));
    if( !iData->IsValid() )
        {
        // Syntactically OK, but mandatory attributes are missing.
        User::Leave( KErrCodInvalidDescriptor );
        }
    CLOG(( ECodEng, 4, _L("CCodEng::CheckDataL: data valid") ));
    CLOG(( ECodEng, 2, _L("<- CCodEng::CheckDataL") ));
    }

// ---------------------------------------------------------
// CCodEng::ParseDataL()
// ---------------------------------------------------------
//
void CCodEng::ParseDataL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEng::ParseDataL") ));

    __ASSERT_DEBUG( iCodBuf, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iData, CodPanic( ECodInternal ) );

    TCodParser parser;
    parser.ParseL( *iCodBuf, *iData );
    CLOG(( ECodEng, 4, _L("CCodEng::SetL: parsed OK") ));
    CLOG(( ECodEng, 2, _L("<- CCodEng::ParseDataL") ));
    }

// ---------------------------------------------------------
// CCodEng::StatusCode
// ---------------------------------------------------------
//
TInt CCodEng::StatusCode( TInt aError, TState aState ) const
    {
    // Meaningless to fiddle with statuses after we are done.
    __ASSERT_DEBUG( aState <= ENotify, CodPanic( ECodInternal ) );

    TInt code( KHttp924LoaderError );

    switch( aError )
        {
        case KErrNone:
            {
            code = KHttp900Success;
            break;
            }

        case KErrNoMemory:
        case KErrDiskFull:
            {
            code = KHttp910NoMemory;
            break;
            }

        case KErrCodInsufficientSpace:
            {
            code = KHttp901InsufficientMemory;
            break;
            }

        case KErrCancel:
        case KErrAbort:
            {
            code = aState == EReady ?
                KHttp921UserAborted :   // EReady -> COD rejected by user.
                KHttp902UserCancelled;  // other -> something is cancelled.
            break;
            }

        case KErrCodInvalidDescriptor:
        case KErrCodUnsupportedVersion:
            {
            code = KHttp906InvalidDescriptor;
            break;
            }

        case KErrCodAttributeMismatch:
            {
            code = KHttp905AttributeMismatch;
            break;
            }

        case KErrCodInvalidType:
            {
            code = KHttp907InvalidType;
            break;
            }

        case KBadMimeType:
        case KMimeNotSupported:
            {
            code = iState == ESet ?
                KHttp922DeviceAborted :      // ESet -> COD MIME bad.
                KHttp923NonAcceptableContent;// EFetch -> content MIME bad.
            break;
            }

        case KErrTimedOut:
        case KErrCodHttpCommsFailed:
        case KErrCodHttpNoResponse:
            {
            code = KHttp903LossOfService;
            break;
            }

        default:
            {
            code = KHttp924LoaderError;
            break;
            }
        }
    return code;
    }

// ---------------------------------------------------------
// CCodEng::StatusText()
// ---------------------------------------------------------
//
const TDesC8& CCodEng::StatusText( TInt aCode ) const
    {
    switch ( aCode )
        {
        case KHttp900Success:              return KText900Success;
        case KHttp901InsufficientMemory:   return KText901InsufficientMemory;
        case KHttp902UserCancelled:        return KText902UserCancelled;
        case KHttp903LossOfService:        return KText903LossOfService;
        case KHttp905AttributeMismatch:    return KText905AttributeMismatch;
        case KHttp906InvalidDescriptor:    return KText906InvalidDescriptor;
        case KHttp907InvalidType:          return KText907InvalidType;
        case KHttp921UserAborted:          return KText921UserAborted;
        case KHttp922DeviceAborted:        return KText922DeviceAborted;
        case KHttp923NonAcceptableContent: return KText923NonAcceptableContent;
        case KHttp924LoaderError:          return KText924LoaderError;
        case KHttp910NoMemory:             return KText910NoMemory;
        default:                           CodPanic( ECodInternal );
        }
    return KNullDesC8;  /* NOTREACHED */
    }
