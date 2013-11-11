/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

// INCLUDE FILES
#include "HttpConnUtils.h"
#include "httpfiltercommonstringsext.h"
#include <httpstringconstants.h>
#include <http/rhttpsession.h>
#include <BrCtlDefs.h>
#include <flogger.h>
#include <sslerr.h>
#include <httperr.h>
#include <inet6err.h>
#include <in_sock.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// THttpConnUtils::PanicLoader
//
//
// -----------------------------------------------------------------------------
//
void THttpConnUtils::PanicLoader(
    TInt aError )
    {
    User::Panic( _L( "HTTP Resource Loader" ), aError );
    }

// -----------------------------------------------------------------------------
// THttpConnUtils::WriteLog
//
//
// -----------------------------------------------------------------------------
//
void THttpConnUtils::WriteLog(
    TPtrC aBuf, TInt aStatus )
    {
#ifdef __WINSCW__
    RFileLogger::WriteFormat(_L("Browser"), _L("resloader.txt"), EFileLoggingModeAppend,
            _L("%S %d"), &aBuf, aStatus );
#endif //__WINSCW__
    }

// -----------------------------------------------------------------------------
// THttpConnUtils::WmlContent
//
// -----------------------------------------------------------------------------
//
TBool THttpConnUtils::WmlContent(
    const TDesC& aContentType )
    {
    // very simple check. need something more
    // sofisticated.
    return( ( aContentType.Find( _L("wml") ) != KErrNotFound || aContentType.Find( _L("wap") ) != KErrNotFound )
     && aContentType.Find( _L( "application/vnd.wap.xhtml+xml" ) ) == KErrNotFound );
    }


// -----------------------------------------------------------------------------
// THttpConnUtils::MapErrors
// Map the Symbian errors to rainbow errors.
// -----------------------------------------------------------------------------
//
TInt THttpConnUtils::MapHttpErrors(
    TInt aError )
    {
    if (aError == KErrSSLAlertUserCanceled)
        {
        return KBrowserCancelled;
        }
    // All SSL errors are mapped into this one, which gets displayed to the user
    if (aError <= SSL_ERROR_BASE && aError > SSL_ERROR_BASE - 200 ||
        aError == KErrHttpCannotEstablishTunnel)
        {
        return KErrSSLAlertHandshakeFailure;
        }

    // Deal with DNS lookup errors
    if ((aError <= KErrInet6NoDestination) && (aError > (KErrInet6NoDestination - 200)))
        {
        return KBrowserHTTP502;
        }

    // Deal with HTTP errors
    if (aError <= KHttpErrorBase && aError > KHttpErrorBase - 200)
        {
        // Encode errors
        if (aError <= KErrHttpEncodeDefault && aError >= KErrHttpEncodeCookie2)
            {
            return KBrowserFailure;
            }
        // Auth errors
        if (aError == KErrHttpDecodeWWWAuthenticate ||
            aError == KErrHttpDecodeUnknownAuthScheme ||
            aError == KErrHttpDecodeBasicAuth ||
            aError == KErrHttpDecodeDigestAuth)
            {
            return KBrowserMissingAuthHeader;
            }

        // Decode errors
        if (aError <= KErrHttpDecodeMalformedDate && aError >= KErrHttpDecodeCookie)
            {
            return KBrowserBadContent;
            }
        switch (aError)
            {
            case KErrHttpRedirectExceededLimit:
                {
                return KBrowserTooManyRedirects;
                }
            case KErrHttpRedirectNoLocationField:
            case KErrHttpRedirectUseProxy:
                {
                return KBrowserBadRedirect;
                }
            case KErrHttpInvalidUri:
                {
                return KBrowserMalformedUrl;
                }
            default:
                {
                return KBrowserFailure;
                }
            }
        }
    switch (aError)
        {
        case KErrHostUnreach:
            {
            return KBrowserTcpHostUnreachable;
            }
        case KErrAbort: //User has interrupted the loading process. We treat it as cancellation so that no error message shown.
        case KErrCancel:
            {
            return KBrowserCancelled;
            }
        case KErrBadName:
        case KErrPathNotFound:
            {
            return KBrowserFileNotFound;
            }
        case KErrCommsLineFail:
        case KErrNotReady:
            {
            return KBrowserConnFailed;
            }
        case KErrDiskFull:
            {
            return KBrowserFileDiskFullError;
            }
        case KErrTimedOut:
            {
            return KBrowserWaitTimeout;
            }
        case KErrCouldNotConnect:
            {
            return KBrowserMalformedUrl;
            }
        case KErrDisconnected:
            {
            return KBrowserHTTP504;
            }
        default:
            {
            return KBrowserFailure;
            }
        } // end of switch
    }

// -----------------------------------------------------------------------------
// THttpConnUtils::ConvertCharset
// Convert the settings characterset from TUint to THTTPHdrVal.
// -----------------------------------------------------------------------------
//
THTTPHdrVal THttpConnUtils::ConvertCharset(
    RStringPool aStringPool,
    TUint aCharset )
    {
    THTTPHdrVal charsetHdr;

    switch (aCharset)
        {
        case EUTF8:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EUtf8,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_1:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88591,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EGb2312:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EGb2312,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EBig5:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EBig5,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_2:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88592,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_4:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88594,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_5:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88595,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_6:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88596,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_7:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88597,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_8:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88598,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_9:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso88599,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_15:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso885915,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EISO8859_8i:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1255,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1255:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1255,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1256:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1256,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1250:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1250,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1251:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1251,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1252:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1252,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1253:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1253,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1254:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1254,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows1257:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows1257,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case ETis620: // This value can be in the settings only if tis-620 is supported, no variation needed
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::ETis620,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EShiftJis: // This value can be in the settings only if Japanese is supported, no variation needed
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EShiftJIS,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EJisX0201_1997: // This value can be in the settings only if Japanese is supported, no variation needed
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EJisX0201_1997,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EJisX0208_1997: // This value can be in the settings only if Japanese is supported, no variation needed
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EJisX0208_1997,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EEucJp: // This value can be in the settings only if Japanese is supported, no variation needed
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EEucJp,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EEucKr:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EEucKr,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }

        case EKsc5601:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EKsc5601,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EIso2022Jp: // This value can be in the settings only if Japanese is supported, no variation needed
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EIso2022Jp,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        case EWindows874:
            {
            charsetHdr = aStringPool.StringF(HttpFilterCommonStringsExt::EWindows874,
                HttpFilterCommonStringsExt::GetTable());
            break;
            }
        }
    return charsetHdr;
    }

// -----------------------------------------------------------------------------
// THttpConnUtils::OpenStringFromUnicode
// Create an RString object from a UCS2 buffer.
// -----------------------------------------------------------------------------
//
TInt THttpConnUtils::OpenStringFromUnicode(
    RStringPool aStringPool,
    TUint16* aStr,
    RString& aString )
    {
    __ASSERT_DEBUG( aStr != NULL, THttpConnUtils::PanicLoader( KErrGeneral ) );

    TInt len = User::StringLength( aStr ) + 1;
    TPtr16 ptr16( aStr, len - 1, len );
    TUint8* str8 = new TUint8 [ len ];
    if( !str8 )
        {
        return KErrNoMemory;
        }
    TPtr8 ptr8( str8, len );
    ptr8.Copy( ptr16 );
    TRAPD( ret, aString = aStringPool.OpenStringL( ptr8 ) );
    delete[] str8;
    return ret;
    }


// -----------------------------------------------------------------------------
// THttpConnUtils::ConvertPtrUsc2ToAscii
// Convert a UCS2 buffer to ascii.
// -----------------------------------------------------------------------------
//
TInt THttpConnUtils::ConvertPtrUsc2ToAscii(
    const TPtrC& aUsc2Ptr,
    TPtrC8& aAsciiPtr,
    TUint8** retPtr )
    {
    TUint len = aUsc2Ptr.Length() + 1; // 1 for NULL terminator
    TUint8* asciiBuf = new TUint8 [len];
    if (asciiBuf != NULL)
        {
        TPtr8 asciiPtr(asciiBuf, 0, len);
        asciiPtr.Copy(aUsc2Ptr);
        asciiPtr.ZeroTerminate();
        aAsciiPtr.Set(asciiPtr);
        *retPtr = asciiBuf;
        }
    else
        {
        return KErrNoMemory;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// THttpConnUtils::CompareUrls
//
// -----------------------------------------------------------------------------
//
TBool THttpConnUtils::IsIntraPageNavigation(
    const TDesC& aUrl1,
    const TDesC& aUrl2 )
    {
    TBool intranavi( ETrue );
    // remove '#' frag
    TInt pos1( aUrl1.LocateReverse('#') );
    TInt pos2( aUrl2.LocateReverse('#') );

    // from no fragment to no fragment
    // www.foo.com -> www.foo.com -load
    // www.foo.com -> www.fakefoo.com -load
    // does not matter what the url is. this cannot be
    // intrapage navigation
    if( pos1 == KErrNotFound && pos2 == KErrNotFound )
        {
        intranavi = EFalse;
        }
    // from fragment to fragment
    else if( pos1 != KErrNotFound && pos2 != KErrNotFound )
        {
        //
        TPtrC url1( aUrl1.Left( pos1 ) );
        TPtrC url2( aUrl2.Left( pos2 ) );

        if( url1.Compare( url2 ) != KErrNone )
            {
            // urls are different
            // www.foo.com/index.html#a -> www.fakefoo.com/index.html#a -load
            // www.foo.com/index.html#a -> www.foo.com/pub/index.html#a -load
            intranavi = EFalse;
            }
        else
            {
            // same url. same fragment?
            // www.foo.com/index.html#a -> www.foo.com/index.html#a -intrapage navi(nothing)
            // www.foo.com/index.html#a -> www.foo.com/index.html#b -intrapage navi
            intranavi = ETrue;
            }
        }
    // from fragment to no fragment
    else if( pos2 == KErrNotFound )
        {
        // www.foo.com/index.html#a -> www.foofake.com -load
        // www.foo.com/index.html#a -> www.foo.com/index.html -load
        intranavi = EFalse;
        }
    // from no fragment to fragment
    else
        {
        TPtrC url1( pos1 == KErrNotFound ? aUrl1 : aUrl1.Left( pos1 ) );
        TPtrC url2( pos2 == KErrNotFound ? aUrl2 : aUrl2.Left( pos2 ) );

        // www.foo.com/index.html -> www.foo.com/index.html#a -intrapage navi
        // www.foo.com/index.html -> www.foofake.com/index.html#a -load
        intranavi = url1.Compare( url2 ) == KErrNone;
        }
    return intranavi;
    }

//  End of File
