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
*      Implementation of class CDdEng.   
*      
*
*/


// INCLUDE FILES

#include "DdEng.h"
#include "CodData.h"
#include "CodError.h"
#include "CodPanic.h"
#include "CodStatus.h"
#include "DdParser.h"
#include "CodLogger.h"
#include "CodSaver.h"
#include "CodUtil.h"
#include "CodDefs.h"
#include <DocumentHandler.h>
#include <e32math.h>
#include <RoapDef.h>

// CONSTANTS

_LIT8( KLicenseStartTag, "<license" );
_LIT8( KLicenseEndTag, "</license>" );

const TInt KElementEnd( '>' );

/// Supported DD major version (only major number needs to be checked, 1.0)
LOCAL_D const TUint32 KDdSupportedMajorVersion = 2;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDdEng::NewL()
// ---------------------------------------------------------
//
EXPORT_C CDdEng* CDdEng::NewL
( CEikProcess* aProcess, MCodLoadObserver* aObserver )
    {
    CDdEng* model = new (ELeave) CDdEng( aObserver );
    CleanupStack::PushL( model );
    model->ConstructL( aProcess );
    CleanupStack::Pop( model );
    return model;
    }

// ---------------------------------------------------------
// CDdEng::~CDdEng()
// ---------------------------------------------------------
//
EXPORT_C CDdEng::~CDdEng()
    {
    CLOG(( ECodEng, 2, _L("CDdEng::~CDdEng") ));
    }

// ---------------------------------------------------------
// CDdEng::CDdEng()
// ---------------------------------------------------------
//
CDdEng::CDdEng( MCodLoadObserver* aObserver ): CCodEngBase( aObserver )
    {
    CLOG(( ECodEng, 2, _L("CDdEng::CDdEng") ));
    }

// ---------------------------------------------------------
// CDdEng::CheckDataL()
// ---------------------------------------------------------
//
void CDdEng::CheckDataL()
    {
    CLOG(( ECodEng, 2, _L("-> CDdEng::CheckDataL") ));
    if( !iData->IsValid() )
        {
        // Syntactically OK, but mandatory attributes are missing.
        User::Leave( KErrCodInvalidDescriptor );
        }
    CLOG(( ECodEng, 4, _L("CDdEng::CheckDataL: data valid") ));
    CheckVersionL( iData->Version() );
    CLOG(( ECodEng, 4, _L("CDdEng::CheckDataL: version OK") ));
    CLOG(( ECodEng, 2, _L("<- CDdEng::CheckDataL") ));
    }

// ---------------------------------------------------------
// CDdEng::ParseDataL()
// ---------------------------------------------------------
//
void CDdEng::ParseDataL()
    {
    CLOG(( ECodEng, 2, _L("-> CDdEng::ParseDataL") ));

    __ASSERT_DEBUG( iCodBuf, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iData, CodPanic( ECodInternal ) );

    TDdParser parser;
    TBool isLicenseTag (EFalse);
    parser.ParseL( *iCodBuf, *iData, iIsDd2, isLicenseTag );
    if (isLicenseTag && iIsDd2)
	{	
	  iIsLicense = ETrue;
	  CreateSaverL(KDd2DataType);
	  ParseLicenseL();
	}
    CLOG(( ECodEng, 4, _L("CDdEng::SetL: parsed OK") ));
    CLOG(( ECodEng, 2, _L("<- CDdEng::ParseDataL") ));
    }

// ---------------------------------------------------------
// CDdEng::StatusCode
// ---------------------------------------------------------
//
TInt CDdEng::StatusCode( TInt aError, TState aState ) const
    {
    // Meaningless to fiddle with statuses after we are done.
    __ASSERT_DEBUG( aState <= ENotify, CodPanic( ECodInternal ) );

    TInt code( KHttp954LoaderError );

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
            code = KHttp902UserCancelled;
            break;
            }

        case KErrCodInvalidDescriptor:
            {
            code = KHttp906InvalidDescriptor;
            break;
            }

        case KErrCodInvalidType:
            {
            code = KHttp907InvalidType;
            break;
            }

        case KErrCodUnsupportedVersion:
            {
            code = KHttp951InvalidDdVersion;
            break;
            }

        case KErrCodAttributeMismatch:
            {
            code = KHttp905AttributeMismatch;
            break;
            }

        case KBadMimeType:
        case KMimeNotSupported:
            {
            code = aState == ESet ?
                KHttp952DeviceAborted :      // ESet -> COD MIME bad.
                KHttp953NonAcceptableContent;// EFetch -> content MIME bad.
            break;
            }
        case KErrCodHttpLicenseFailed:
        case KErrRoapGeneral:
        case KErrRoapUnsupported:
        case KErrRoapServer:
        case KErrRoapServerFatal:
        case KErrRoapDevice:
        case KErrRoapInvalidDomain:
        case KErrRoapDomainFull:
        case KErrRoapNotRegistered:
            {
            code = KHttp957LicenseFailed;
            break;
            }

        case KErrTimedOut:
        case KErrCodHttpCommsFailed:
        case KErrCodHttpNoResponse:
            {
            code = KHttp903LossOfService;
            break;
            }

        case KErrCodHttpRequestedRangeNotSatisfiable:
            {
            code = KHttp909RequestedRangeNotSatisfiable;
            break;
            }
 
        case KErrCodHttpPreconditionFailed:
            {
            code = KHttp955PreconditionFailed;
            break;
            }

       	case KErrCodHttpBadUrl:
        default:
            {
            code = KHttp954LoaderError;
            break;
            }
        }
    return code;
    }

// ---------------------------------------------------------
// CDdEng::StatusText()
// ---------------------------------------------------------
//
const TDesC8& CDdEng::StatusText( TInt aCode ) const
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
        case KHttp951InvalidDdVersion:     return KText951InvalidDdVersion;
        case KHttp952DeviceAborted:        return KText952DeviceAborted;
        case KHttp953NonAcceptableContent: return KText953NonAcceptableContent;
        case KHttp954LoaderError:          return KText954LoaderError;
        case KHttp956LicenseSuccess:       return KText956LicenseSuccess;
        case KHttp957LicenseFailed:        return KText957LicenseFailed;
        case KHttp910NoMemory:             return KText910NoMemory;
        case KHttp955PreconditionFailed:   return KText955MediaObjectUpdated;
        case KHttp970MixedStatus:          return KText970MixedStatus; 
        case KHttp981DownloadCompletion:   return KText981DownloadCompletion; 
        default:                           CodPanic( ECodInternal );
        }
    return KNullDesC8;  /* NOTREACHED */
    }

// ---------------------------------------------------------
// CDdEng::CheckVersionL()
// ---------------------------------------------------------
//
void CDdEng::CheckVersionL( const TDesC& aVersion ) const
    {
    TUint32 majorVer = 1;   // If version is not specified, it defaults to 1.0.

    if( aVersion.Length() )
        {
        // Version is specified, parse it to get get major version.
        TLex lex( aVersion );
        TReal realVer;
        TInt32 intVer;
        if( lex.Val( realVer ) != KErrNone )
            {
            // Could not parse version as a real number, this indicates that
            // the DD-Version is some rubbish string.
            User::Leave( KErrCodInvalidDescriptor );
            }
        User::LeaveIfError( Math::Int( intVer, realVer ) );
        if( intVer < 0 )
            {
            // Negative DD-Version?
            User::Leave( KErrCodInvalidDescriptor );
            }
        majorVer = STATIC_CAST( TUint32, intVer );
        }

    // For code clarity, we always check major version (even if not given and
    // defaults to supported 1.0).
    if( majorVer > KDdSupportedMajorVersion )
        {
        User::Leave( KErrCodUnsupportedVersion );
        }
    }

// ---------------------------------------------------------
// CDdEng::ParseLicenseL()
// ---------------------------------------------------------
//
void CDdEng::ParseLicenseL()
{
	HBufC8* descriptorBuf = CodUtil::ConvertLC( iCodBuf->Des());
	TPtrC8 license;
	TPtrC8 descriptor (descriptorBuf->Ptr());
	TInt startTag = descriptor.Find(KLicenseStartTag); // "<license"
	if (startTag != KErrNotFound)
	{
		descriptor.Set(descriptor.Right(descriptor.Length()- startTag -1));
		TInt endTag = descriptor.Locate(KElementEnd); //'>'
		if (endTag != KErrNotFound)
		{
			license.Set(descriptor.Right(descriptor.Length()-endTag-1));
			TInt licenseTagEnd = license.Find(KLicenseEndTag); // "</license"
			if (licenseTagEnd != KErrNotFound)
			{
				license.Set(license.Left(licenseTagEnd));
			}
		}
	}
	iSaver->AppendData( license );
	CleanupStack::PopAndDestroy( descriptorBuf );
}
