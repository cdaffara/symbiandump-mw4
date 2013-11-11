/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of HttpCacheUtil
*
*/

// INCLUDE FILES
#include "HttpCacheUtil.h"
#include <http/rhttpheaders.h>
#include <http/rhttptransaction.h>
#include <http.h>
#include <stdlib.h>
#include <string.h>
#include <stringpool.h>
#include <flogger.h>
#include "tinternetdate.h"
#include "httpfiltercommonstringsext.h"
#include "HttpCacheEntry.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT8( KHttpNewLine, "\r\n" );
// if you change it here, change in HeadersToBufferL too
_LIT8( KHttpFieldSeparator, ": " );
_LIT8( KHttpValueSep, "," );
const TInt KMaxHeaderStrLen = 1024;
_LIT8( KHttpContentType, "Content-Type" );

#ifdef __CACHELOG__
_LIT( KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
_LIT( KHttpCacheGeneralFileName, "cachehandler.txt" );
_LIT( KHttpCacheHashFileName, "hash.txt" );
const TInt KCurrentLogLevel = 0;
#endif // __CACHELOG__

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HttpCacheUtil::SetEntryOnTransL
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::SetEntryOnTransL(
    CHttpCacheEntry& aEntry,
    RHTTPTransaction& aTrans,
    RStringF& aCacheEntryStr )
    {
    RHTTPTransactionPropertySet propSet = aTrans.PropertySet();

    // set transaction property with the event handler callback functions' pointer
    THTTPHdrVal tokenVal = (TInt)(CHttpCacheEntry*)&aEntry;
    aTrans.PropertySet().RemoveProperty( aCacheEntryStr );
    aTrans.PropertySet().SetPropertyL( aCacheEntryStr, tokenVal );
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::EntryOnTransL
//
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* HttpCacheUtil::EntryOnTransL(
    RHTTPTransaction& aTrans,
    RStringF& aCacheEntryStr )
    {
    //
    CHttpCacheEntry* entry = NULL;
    //
    THTTPHdrVal entryPtr;
    RHTTPTransactionPropertySet propSet = aTrans.PropertySet();
    // this is a transaction, already forwarded to download manager
    if( propSet.Property( aCacheEntryStr, entryPtr ) )
        {
        entry = REINTERPRET_CAST( CHttpCacheEntry*, entryPtr.Int() );
        }
    return entry;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::HeadersToBufferLC
//
// -----------------------------------------------------------------------------
//
HBufC8* HttpCacheUtil::HeadersToBufferLC(
    RHTTPHeaders& aHttpHeaders,
    RStringPool& aStrP )
    {
    // Get an iterator for the collection of response headers
    HBufC8* headerString = HBufC8::NewL( KMaxHeaderStrLen );
    TPtr8 headerPtr( headerString->Des() );
    TInt newLength( 0 );
    THTTPHdrFieldIter it = aHttpHeaders.Fields();

    while( it.AtEnd() == EFalse )
        {
        TPtrC8 rawData;
        // Get name of next header field
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = aStrP.StringF( fieldName );
        aHttpHeaders.GetRawField( fieldNameStr, rawData );

        newLength+=( fieldNameStr.DesC().Length() + KHttpFieldSeparator().Length() + rawData.Length() +
            KHttpNewLine().Length() );

        if( headerPtr.MaxLength() < newLength )
            {
            // realloc
            HBufC8* temp = HBufC8::New( newLength + KMaxHeaderStrLen );
            if( !temp )
                {
                delete headerString;
                User::LeaveNoMemory();
                }
            temp->Des().Copy( headerPtr );
            delete headerString;
            headerString = temp;
            headerPtr.Set( headerString->Des() );
            }
        // append
        headerPtr.Append( fieldNameStr.DesC() );
        headerPtr.Append( KHttpFieldSeparator );
        headerPtr.Append( rawData );
        headerPtr.Append( KHttpNewLine );
        ++it;
        }
    CleanupStack::PushL( headerString );
    return headerString;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::BufferToHeadersL
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::BufferToHeadersL(
    const TDesC8& aHeaderStr,
    RHTTPHeaders& aHttpHeaders,
    RStringPool& aStrP )
    {
    // take header str line by line
    TInt sepPos( 0 );
    TInt lineEnd( 0 );
    TPtrC8 headerStr( aHeaderStr );

    while( lineEnd != KErrNotFound )
        {
        lineEnd = headerStr.Find( KHttpNewLine );
        sepPos = headerStr.Find( KHttpFieldSeparator );

        if( lineEnd != KErrNotFound && sepPos != KErrNotFound &&
            lineEnd > sepPos )
            {
            // ETag: "9be043c1-175-793-41419a44"
            TPtrC8 line( headerStr.Left( lineEnd ) );
            // ETag
            TPtrC8 key( line.Left( sepPos ) );
            // "9be043c1-175-793-41419a44"
            TInt sepEnd( sepPos + KHttpFieldSeparator().Length() );
            TPtrC8 value( line.Mid( sepEnd ) );
            // add it to the http header
            RStringF nameStr = aStrP.OpenFStringL( key );
            CleanupClosePushL( nameStr );
            // remove if exists
            THTTPHdrVal headerValue;
            if( aHttpHeaders.GetField( nameStr, 0, headerValue ) == KErrNone )
                {
                //
                aHttpHeaders.RemoveField( nameStr );
                }
            TRAPD( err, aHttpHeaders.SetRawFieldL( nameStr, value, KHttpValueSep ) );
            // leave OOM only
            if( err == KErrNoMemory )
                {
                User::Leave( err );
                }
            CleanupStack::PopAndDestroy(); // namestr

            // move string to the end of the line
            headerStr.Set( headerStr.Mid( lineEnd + KHttpNewLine().Length() ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::BodyToBufferL
//
// -----------------------------------------------------------------------------
//
HBufC8* HttpCacheUtil::BodyToBufferL(
    MHTTPDataSupplier& aBodySupplier )
    {
    // get it from the transaction
    TPtrC8 ptr;
    aBodySupplier.GetNextDataPart( ptr );
    //
    return ptr.AllocL();
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::MergeHeadersLC
//
// -----------------------------------------------------------------------------
//
HBufC8* HttpCacheUtil::MergeHeadersLC(
    const TDesC8& aCachedHeaderStr,
    RHTTPHeaders& aResponseHeaders,
    RStringPool& aStrP )
    {
    // FYI: it uses the response header to merge
    // take header str line by line
    TInt sepPos( 0 );
    TInt lineEnd( 0 );
    TPtrC8 headerStr( aCachedHeaderStr );

    while( lineEnd != KErrNotFound )
        {
        lineEnd = headerStr.Find( KHttpNewLine );
        sepPos = headerStr.Find( KHttpFieldSeparator );

        if( lineEnd != KErrNotFound && sepPos != KErrNotFound &&
            lineEnd > sepPos )
            {
            // ETag: "9be043c1-175-793-41419a44"
            TPtrC8 line( headerStr.Left( lineEnd ) );
            // ETag
            TPtrC8 key( line.Left( sepPos ) );
            // "9be043c1-175-793-41419a44"
            TInt sepEnd( sepPos + KHttpFieldSeparator().Length() );
            TPtrC8 value( line.Mid( sepEnd ) );
            // add it to the http header
            RStringF nameStr = aStrP.OpenFStringL( key );
            CleanupClosePushL( nameStr );
            // avoid corrupted content type from the stack in case of 304
            if(key == KHttpContentType)
                {
                aResponseHeaders.RemoveField(nameStr);
                }
            THTTPHdrVal tempVal;
            // add this field unless it already exists
            if( aResponseHeaders.GetField( nameStr, 0, tempVal ) == KErrNotFound )
                {
                TRAPD( err, aResponseHeaders.SetRawFieldL( nameStr, value, KHttpValueSep ) );
                // leave OOM only
                if( err == KErrNoMemory )
                    {
                    User::Leave( err );
                    }
                }
            CleanupStack::PopAndDestroy(); // namestr
            // move string to the end of the line
            headerStr.Set( headerStr.Mid( lineEnd + KHttpNewLine().Length() ) );
            }
        }
    // we've got the merged headers, let's covert it to buffer
    return HeadersToBufferLC( aResponseHeaders, aStrP );
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::AddHeaderLC
//
// -----------------------------------------------------------------------------
//
HBufC8* HttpCacheUtil::AddHeaderLC(
    const TDesC8& aName,
    const TDesC8& aValue,
    const TDesC8& aHeaders )
    {
    HBufC8* newHeaders = NULL;
    // first check if the header exists so
    // then all we need to do is to update the value
    TInt pos( aHeaders.FindF( aName ) );
    //
    newHeaders = HBufC8::NewLC( aHeaders.Length() + aName.Length() + KHttpFieldSeparator().Length() +
        aValue.Length() + KHttpNewLine().Length() );
    //
    if( pos != KErrNotFound )
        {
        // replace old header with this new one
        TPtr8 newHeadersPtr( newHeaders->Des() );
        // copy headers
        newHeadersPtr.Copy( aHeaders );
        // check what we need to replace
        TPtrC8 leftover( aHeaders.Mid( pos ) );
        // headers are terminated by \r\n
        TInt endPos( leftover.Find( KHttpNewLine ) );
        if( endPos != KErrNotFound )
            {
            // value pos
            TInt valuePos( pos + aName.Length() + KHttpFieldSeparator().Length() );
            newHeadersPtr.Replace( valuePos, pos + endPos - valuePos, aValue );
            }
        }
    else
        {
        // add new header
        // new header = old header\r\n aName: aValue\r\n
        TPtr8 newHeadersPtr( newHeaders->Des() );
        // old headers
        newHeadersPtr.Copy( aHeaders );
        // Cache Control
        newHeadersPtr.Append( aName );
        // :
        newHeadersPtr.Append( KHttpFieldSeparator );
        // max age=180
        newHeadersPtr.Append( aValue );
        // \r\n
        newHeadersPtr.Append( KHttpNewLine );
        }
    return newHeaders;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::HeaderFieldFromBufferLC
//
// -----------------------------------------------------------------------------
//
HBufC8* HttpCacheUtil::HeaderFieldFromBufferLC(
    const TDesC8& aHeaders,
    const TDesC8& aKey )
    {
    HBufC8* value = NULL;
    //
    TInt pos( aHeaders.Find( aKey ) );
    if( pos != KErrNotFound )
        {
        // line: Cache-Control: max-age=180
        TPtrC8 linePtr = aHeaders.Mid( pos );
        // line end \r\n
        TInt lineEnd( linePtr.Find( KHttpNewLine ) );
        // separator :
        TInt sepPos( linePtr.Find( KHttpFieldSeparator ) );
        //
        if( sepPos != KErrNotFound )
            {
            // fix line end
            lineEnd = lineEnd == KErrNotFound ? linePtr.Length(): lineEnd;
            // max-age=180
            TInt valueLen( lineEnd - ( sepPos + KHttpFieldSeparator().Length() ) );
            value = HBufC8::NewLC( valueLen );
            value->Des().Copy( linePtr.Mid( sepPos + KHttpFieldSeparator().Length(), valueLen ) );
            }
        }
    return value;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::MethodFromStr
//
// -----------------------------------------------------------------------------
//

TCacheLoadMethod HttpCacheUtil::MethodFromStr(
    RStringF aMethodStr,
    RStringPool aStrP )
    {
    if( aMethodStr == aStrP.StringF( HTTP::EGET, RHTTPSession::GetTable() ) )
        {
        return EMethodGet;
        }
    if( aMethodStr == aStrP.StringF(HTTP::ECONNECT, RHTTPSession::GetTable() ) )
        {
        return EMethodConnect;
        }
    if( aMethodStr == aStrP.StringF(HTTP::EDELETE, RHTTPSession::GetTable() ) )
        {
        return EMethodDelete;
        }
    if( aMethodStr == aStrP.StringF(HTTP::EHEAD, RHTTPSession::GetTable() ) )
        {
        return EMethodHead;
        }
    if( aMethodStr == aStrP.StringF(HTTP::EOPTIONS, RHTTPSession::GetTable() ) )
        {
        return EMethodOptions;
        }
    if( aMethodStr == aStrP.StringF(HTTP::EPUT, RHTTPSession::GetTable() ) )
        {
        return EMethodPut;
        }
    if( aMethodStr == aStrP.StringF(HTTP::EPOST, RHTTPSession::GetTable() ) )
        {
        return EMethodPost;
        }
    if( aMethodStr == aStrP.StringF(HTTP::ETRACE, RHTTPSession::GetTable() ) )
        {
        return EMethodTrace;
        }
    return EMethodGet;
    }


// -----------------------------------------------------------------------------
// HttpCacheUtil::ProtectedEntry
//
// Returns true if the entry is protected
//
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::ProtectedEntry(
    RHTTPHeaders& aHttpHeaders,
    RStringPool& aStrP )
    {
    TBool protectedEntry( EFalse );
    //
    THTTPHdrVal contType;
    RStringF fieldName = aStrP.StringF( HTTP::EContentType, RHTTPSession::GetTable() );
    //
    if( aHttpHeaders.GetField( fieldName, 0, contType ) == KErrNone )
        {
        if( contType.Type() == THTTPHdrVal::KStrFVal )
            {
            RStringF fieldValStr = aStrP.StringF( contType.StrF() );
            // css and javascrip are protected entries
            protectedEntry = fieldValStr.DesC().Find( _L8("text/css") ) != KErrNotFound ||
                fieldValStr.DesC().Find( _L8("javascript") ) != KErrNotFound;
            }
        }
    return protectedEntry;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::CacheTimeIsFresh
//
// Returns true if the entry is fresh.
//
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::CacheTimeIsFresh(
    const RHTTPHeaders& aRequestHeaders,
    const RHTTPHeaders& aCachedHeaders,
    RStringPool aStrP )
    {
    TInt status( KErrNone );
    TBool isFresh( EFalse );
    TInt64 freshness;
    TInt64 age;
    TInt64 maxAge;
    TInt64 minFresh;
    TInt64 maxStale;
    TTime cachedRequestTime;
    TTime cachedResponseTime;

    TTime date;
    TInt err;
    THTTPHdrVal dateValue;
    RStringF fieldName;

    // Get the date from the headers
    fieldName = aStrP.StringF( HTTP::EDate, RHTTPSession::GetTable() );
    err = aCachedHeaders.GetField( fieldName, 0, dateValue );
    if( err == KErrNotFound || dateValue.Type() != THTTPHdrVal::KDateVal )
        {
        date = 0;
        }
    else
        {
        date = TTime( dateValue.DateTime() );
        }
    cachedRequestTime = date;
    cachedResponseTime = date;

    // Get Max Age header. If maxAge = 0, then must revalidate.
    status = GetCacheControls( aCachedHeaders, &maxAge, NULL, NULL, NULL, NULL, NULL, aStrP );
    if( status == KErrNone &&  maxAge == 0 )
        {
        return EFalse; //Must revalidate
        }

    // Get the freshness and age of the cachedHeaders
    freshness = Freshness( aCachedHeaders, cachedResponseTime, aStrP );
    age = Age( aCachedHeaders, cachedRequestTime, cachedResponseTime, aStrP );
#ifdef __CACHELOG__
        TBuf<50> dateString;
        TTime fr( freshness );

        TRAP(err, fr.FormatL( dateString, KDateString ) );
        if ( err == KErrNone ) {
            HttpCacheUtil::WriteLog( 0, _L( "freshness" ) );
            HttpCacheUtil::WriteLog( 0, dateString );
            }

        TTime ca( age );
        TRAP( err, ca.FormatL( dateString, KDateString ) );
        if ( err == KErrNone ) {
            HttpCacheUtil::WriteLog( 0, _L( "age" ) );
            HttpCacheUtil::WriteLog( 0, dateString );
            }
#endif // __CACHELOG__

    // Get useful cache-control directives from the requestHeaders
    status = GetCacheControls( aRequestHeaders, &maxAge, &minFresh, &maxStale, NULL, NULL, NULL, aStrP );
    if( status == KErrNone )
        {
        // Above values for maxAge, minFresh and maxStale are in seconds
        // Convert to micro seconds before using it.

        // If maxStale is present it means that it is willing to
        // accept a response maxStale seconds after it expires.  To
        // allow this freshness is extended by maxStale seconds.
        if( maxStale != -1 )
            {
            if( maxStale == MAKE_TINT64( KMaxTInt, KMaxTInt ) )
                {
                freshness = MAKE_TINT64( KMaxTInt, KMaxTInt );
                }
            else
                {
                freshness += maxStale * 1000 * 1000;
                }
            }
        // If minFresh is present reject it if it would expire
        // within minFresh seconds.
        if( minFresh != -1 )
            {
            if( ( age + minFresh * 1000 * 1000 ) > freshness )
                {
                return EFalse;
                }
            }
        // If the age > request's maxAge, reject it
        if( maxAge != -1 )
            {
            if( age > maxAge * 1000 * 1000 )
                {
                return EFalse;
                }
            }
        // If age is less than freshness its fresh
    // If age == 0 and freshness == 0 - revalidation is needed
        if( freshness && age <= freshness )
    {
    isFresh = ETrue;
    }
    }
    return isFresh;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::AddValidationHeaders
//
// Adds appropriate headers to make the given "request" conditional.
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::AddValidationHeaders(
    const RHTTPHeaders aCachedHeaders,
    RHTTPHeaders& aRequestHeaders,
    RStringPool& aStrP )
    {
    THTTPHdrVal lastMod;
    THTTPHdrVal eTag;
    RStringF fieldName;
    TInt err;

    // Get the entry's last-modified header
    fieldName = aStrP.StringF( HTTP::ELastModified, RHTTPSession::GetTable() );
    err = aCachedHeaders.GetField( fieldName, 0, lastMod );

    // If last-Modified is present add an If-Modified-Since header
    if( err != KErrNotFound )
        {
        fieldName = aStrP.StringF( HTTP::EIfModifiedSince, RHTTPSession::GetTable() );
        TRAP_IGNORE( aRequestHeaders.SetFieldL( fieldName, lastMod ) );
        }

    // Get the entry's ETag header
    fieldName = aStrP.StringF( HTTP::EETag, RHTTPSession::GetTable() );
    err = aCachedHeaders.GetField( fieldName, 0, eTag );

    // If etag is present add an etag header
    if( err != KErrNotFound )
        {
        fieldName = aStrP.StringF( HTTP::EIfNoneMatch, RHTTPSession::GetTable() );
        TRAP_IGNORE( aRequestHeaders.SetFieldL( fieldName, eTag ) );
        }
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::OperatorCacheContent
// This function checks if aUrl is part of the operator domain. for examp: if aUrl->http://www.opdomain.com/pub/index.html
// and operator domain is http://www.opdomain.com/pub then it returns ETrue.
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::OperatorCacheContent(
    const TDesC8& aOpDomain,
    const TDesC8& aUrl )
    {
    TUriParser8 opDomain;
    TUriParser8 url;
    TBool found( EFalse );

    // check if both URIs are correct.
    if( opDomain.Parse( aOpDomain ) == KErrNone && url.Parse( aUrl ) == KErrNone )
        {
        // host value must be the same. for examp: www.operator.co is different from www.operator.com
        // note: check if compare is case sensitive!
        // note2: if http:// is missing from operator domain then TUriParser8 takes the host part as path.
        if( opDomain.Compare( url, EUriHost ) == 0 )
            {
            // further checking on the path value. domain path must be a substring of the given url.
            // for examp: op path: /news
            // url path: /news/local_news.html
            const TDesC8& opPath = opDomain.Extract( EUriPath );
            if( opPath.Length() )
                {
                const TDesC8& urlPath = url.Extract( EUriPath );
                // we don't take the content if the url path is not defined (while domain path is) or the
                // domain path is not a substring of url path. actually url path must start with domain path
                // op path: /news url path: /media/news/local_news.html is not valid.
                found = ( urlPath.Length() && urlPath.FindF( opPath ) == 0 );
                }
            else
                {
                // no opPath means that we take every content on this host.
                found = ETrue;
                }
            }
        }
    return found;
    }


// -----------------------------------------------------------------------------
// HttpCacheUtil::VSSCacheContent
//
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::VSSCacheContent( const TDesC8& aUrl, const HBufC8* aWhiteList )
    {
    TBool found( EFalse );

    if ( aWhiteList )
        {
        TUint8* bufPtr = (TUint8* ) aWhiteList->Ptr();
        TUint8* startBufPtr = bufPtr;
        TUint len = aWhiteList->Length();
        TInt i, startUri, uriLen;
        TPtrC8 uri ( aUrl );
        for ( i=0; i < len; i++ )
            {
            startUri = i;

            for ( ;( ( *bufPtr != ';' ) && ( i<len ) ) ; i++, bufPtr++ ) ;

            uriLen = i - startUri;

            if ( i == len )
                {
                //For getting total length
                uriLen += 2;
                }

            TPtrC8 uriDomain( startBufPtr, uriLen);
            if ( OperatorCacheContent( uriDomain, uri ) )
                {
                found = ETrue;
                break;
                }

            startBufPtr = ++bufPtr;
            i++;
            } //end for()
        } //end if ( aWhiteList )

    return found;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::PragmaNoCache
//
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::PragmaNoCache(
    RHTTPTransaction& aTrans )
    {
    //
    RHTTPHeaders requestHeaders = aTrans.Request().GetHeaderCollection();
    RStringPool strP = aTrans.Session().StringPool();
    RStringF fieldName = strP.StringF( HTTP::ENoCache, RHTTPSession::GetTable() );

    // check no-cache
    THTTPHdrVal headerVal;
    TInt noCacheField( requestHeaders.GetField( fieldName, 0, headerVal ) );
    // check no-store
    fieldName = strP.StringF( HTTP::ENoStore, RHTTPSession::GetTable() );
    TInt noStoreField( requestHeaders.GetField( fieldName, 0, headerVal ) );
    //
    return( noCacheField == KErrNone || noStoreField == KErrNone );
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::GetHeaderFileName
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::GetHeaderFileName(
    const TDesC& aBodyFileName,
    TDes& aHeaderFileName )
    {
    // body file name = foo
    // header file name = foo.h
    aHeaderFileName.Copy( aBodyFileName );
    // take the filename and append the new extension
    aHeaderFileName.Append( KHttpCacheHeaderExt() );
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::AdjustExpirationTime
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::AdjustExpirationTimeL(
    RHTTPHeaders& aResponseHeaders,
    RStringPool& aStrP )
    {
    const TStringTable& stringTable = RHTTPSession::GetTable();
    THTTPHdrVal hdrVal;

    if( aResponseHeaders.GetField( aStrP.StringF( HTTP::EExpires, stringTable ), 0, hdrVal ) == KErrNone )
        {
        TTime expDate( hdrVal.DateTime() );

#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "adjust expiration time from" ) );

        TBuf<50> dateString;
        TTime expTime( hdrVal.DateTime() );

        expTime.FormatL( dateString, KDateString );
        HttpCacheUtil::WriteLog( 0, dateString );
#endif // __CACHELOG__

        // double it
        TTimeIntervalMinutes minutes;
        TTimeIntervalHours hours;
        TTime now;
        now.UniversalTime();

        if( expDate.MinutesFrom( now, minutes ) == KErrNone )
            {

#ifdef __CACHELOG__
        now.FormatL( dateString, KDateString );

        HttpCacheUtil::WriteLog( 0, _L( "current time" ) );
        HttpCacheUtil::WriteLog( 0, dateString );
        //
        now.FormatL( dateString, KDateString );
        HttpCacheUtil::WriteLog( 0, _L( "expires in (minutes)" ), minutes.Int() );
#endif // __CACHELOG__

            expDate+=minutes;
            }
        // minutes owerflow? take hours instead
        else if( expDate.HoursFrom( now, hours ) == KErrNone )
            {
            expDate+=hours;
            }
        else
            {
            // last resort
            TTimeIntervalDays days( expDate.DaysFrom( now ) );
            expDate+=days;
            }
        // set new date on the response header
        aResponseHeaders.RemoveField( aStrP.StringF( HTTP::EExpires, stringTable ) );
        // add it
        hdrVal.SetDateTime( expDate.DateTime() );
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "to" ) );
        //
        TTime newExptime( hdrVal.DateTime() );
        newExptime.FormatL( dateString, KDateString );
        HttpCacheUtil::WriteLog( 0, dateString );

        TTimeIntervalMinutes min;

        newExptime.MinutesFrom( now, min );

        HttpCacheUtil::WriteLog( 0, _L( "now it expires in (minutes)" ), min.Int() );
#endif // __CACHELOG__
        aResponseHeaders.SetFieldL( aStrP.StringF( HTTP::EExpires, stringTable ), hdrVal );
        }
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::CacheNeedsUpdateL
//
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::CacheNeedsUpdateL(
    RHTTPHeaders& aResponseHeaders,
    const TDesC8& aCachedHeadersStr,
    RStringPool& aStrP )
    {
    HBufC8* valueStr;
    TTime cachedDate( 0 );
    TTime responseDate( 0 );
    TTime cachedLastModified( 0 );
    TTime responseLastModified( 0 );
    RStringF cachedETag;
    RStringF responseETag;
    RStringF fieldName;
    TInternetDate value;
    THTTPHdrVal hdrValue;
    TBool eTag( EFalse );
    TBool eLastModified( EFalse );
    TBool eDate( EFalse );
    TInt status;
    TBool update( ETrue );

     // date
    fieldName = aStrP.StringF( HTTP::EDate, RHTTPSession::GetTable() );
    // new
    status = aResponseHeaders.GetField( fieldName, 0, hdrValue );
    if( status == KErrNone )
        {
        responseDate = TTime( hdrValue.DateTime() );
        // orig
        valueStr = HttpCacheUtil::HeaderFieldFromBufferLC( aCachedHeadersStr, fieldName.DesC() );
        if( valueStr )
            {
            eDate = ETrue;
            value.SetDateL( valueStr->Des() );
            cachedDate = TTime( value.DateTime() );
            CleanupStack::PopAndDestroy(); // valueStr
            }
        }

    // last modified
    fieldName = aStrP.StringF( HTTP::ELastModified, RHTTPSession::GetTable() );
    // new
    status = aResponseHeaders.GetField( fieldName, 0, hdrValue );
    if( status == KErrNone )
        {
        responseLastModified = TTime( hdrValue.DateTime() );
        // orig
        valueStr = HttpCacheUtil::HeaderFieldFromBufferLC( aCachedHeadersStr, fieldName.DesC() );
        if( valueStr )
            {
            eLastModified = ETrue;
            value.SetDateL( valueStr->Des() );
            cachedLastModified  = TTime( value.DateTime() );
            CleanupStack::PopAndDestroy(); // valueStr
            }
        }

    // Etag
    fieldName = aStrP.StringF( HTTP::EETag, RHTTPSession::GetTable() );
    // new
    status = aResponseHeaders.GetField( fieldName, 0, hdrValue );
    if( status == KErrNone )
        {
        responseETag = hdrValue.StrF();
        // orig
        valueStr = HttpCacheUtil::HeaderFieldFromBufferLC( aCachedHeadersStr, fieldName.DesC() );
        if( valueStr )
            {
            TRAP( status, cachedETag = aStrP.OpenFStringL( valueStr->Des() ) );
            CleanupStack::PopAndDestroy(); // valueStr
            eTag = ( status == KErrNone );
            //
            if( eTag )
                {
                CleanupClosePushL( cachedETag );
                }
            }
        }

    // If the entry's date is greater than the responses's date,
    // ignore the response.
    if( eDate && cachedDate > responseDate )
        {
        update  = EFalse;
        }
    // If the entry's ETag does not equal the responses's ETag,
    // replace the cached entry with the response.
    else if( eTag && cachedETag != responseETag )
        {
        update  = ETrue;
        }
    // If the entry's lastMod is greater than the responses's lastMod,
    // ignore the response.
    else if( eLastModified && cachedLastModified > responseLastModified )
        {
        update  = EFalse;
        }
    // If the entry's lastMod is equal to the responses's lastMod,
    // update the cached headers with the new response headers.
    else if( eLastModified && cachedLastModified == responseLastModified )
        {
        update  = ETrue;
        }
    // If the entry's lastMod is less than the responses's lastMod,
    // replace the cached entry with the response.
    else if( eLastModified && cachedLastModified < responseLastModified )
        {
        update  = ETrue;
        }
    else
        {
        // If we get here the headers need to be replaced
        update  = ETrue;
        }
    //
    if( eTag )
        {
        CleanupStack::PopAndDestroy(); // cachedETag
        }
    return update;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::IsCacheable
//
// -----------------------------------------------------------------------------
//
TBool HttpCacheUtil::IsCacheable(
    RHTTPTransaction& aTrans,
    TUint aMaxSize,
    TBool& aProtectedEntry )
    {
    TBool isCacheable( ETrue );
    TBool hasExplicitCache( EFalse );
    TInt64 maxAge;
    TBool noCache;
    TBool noStore;
    TTime now;
    TTime expires;
    RStringF fieldName;
    THTTPHdrVal expiresVal;
    TInt err;
    RStringPool strP = aTrans.Session().StringPool();
    RHTTPHeaders respHeaders = aTrans.Response().GetHeaderCollection();

    // not protected by default
    aProtectedEntry = EFalse;
    // 1. do not cache sesnitive content (DRM)
    // 2. do not cache content bigger than the cache
    // 3. check if the content is protected
    // 4. check normal cache directives

    // init the field name
    THTTPHdrVal contType;
    fieldName = strP.StringF( HTTP::EContentType, RHTTPSession::GetTable() );

    // check if this is a noncacheable content type
    if( respHeaders.GetField( fieldName, 0, contType ) == KErrNone &&
        contType.StrF() == strP.StringF( HttpFilterCommonStringsExt::EApplicationVndOmaDrm,
        HttpFilterCommonStringsExt::GetTable() ) )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "HttpCacheUtil::IsCacheable - sensitive content. do not cache" ) );
#endif
        // drm == nocache
        isCacheable = EFalse;
        }
    else
        {
        // check if the content is bigger than the cache
        THTTPHdrVal contLen;
        fieldName = strP.StringF( HTTP::EContentLength, RHTTPSession::GetTable() );

        if( respHeaders.GetField( fieldName, 0, contLen ) != KErrNotFound &&
            ( contLen.Type() == THTTPHdrVal::KTIntVal && contLen.Int() > aMaxSize ) )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "oversized content. do not cache" ) );
#endif
            // oversized content
            return EFalse;
            }
        // check if this is a proteced entry
        aProtectedEntry = ProtectedEntry( respHeaders, strP );
        // check various cache controls
        if( GetCacheControls( respHeaders, &maxAge, NULL, NULL, NULL,
            &noCache, &noStore, strP ) == KErrNone )
            {
            // There are several header "conditions" that make a resource
            // non-cachable.  Reject if they are present.
            //
            // If no-cache or no-store directives exist -> don't cache.
            if( noCache || noStore )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLog( 0, _L( "HttpCacheUtil::IsCacheable - no cache/no store header. do not cache" ) );
#endif
                // no protection on this entry
                aProtectedEntry = EFalse;
                return EFalse;
                }
            // Get the current time
            now.UniversalTime();

            // Get the expires from the respHeaders
            fieldName = strP.StringF( HTTP::EExpires, RHTTPSession::GetTable() );
            err = respHeaders.GetField( fieldName, 0, expiresVal );
            if( err == KErrNone )
                {
                expires = TTime( expiresVal.DateTime() );
                }
            else
                {
                expires = now;
                }

            // if past-expire date do not cache. According to RFC2616, section 13.2.4/14.9.3,
            // if maxage is present, then ignore expires
            if (!maxAge && now > expires)
                {
                return EFalse;
                }

            if( err == KErrNone || maxAge > 0 )
                {
                hasExplicitCache = ETrue;
                }
            // Reject if the http status code doesn't equal 200, 304, 301, 410.
            // Note: We accept status codes, 307, 302 if some cache
            //       control directives exist.
            switch( aTrans.Response().StatusCode() )
                {
                case HTTPStatus::EOk:
                case HTTPStatus::ENotModified:
                case HTTPStatus::EMovedPermanently:
                case HTTPStatus::EGone:
                    {
                    break;
                    }
                case HTTPStatus::EFound:
                case HTTPStatus::ETemporaryRedirect:
                    {
                    if( !hasExplicitCache )
                        {
                        isCacheable = EFalse;
                        }
                    break;
                    }
                default:
                    {
                    isCacheable = EFalse;
                    break;
                    }
                }

            // Check method specific conditions.
            switch( HttpCacheUtil::MethodFromStr( aTrans.Request().Method(), strP ) )
                {
                case EMethodGet:
                    {
                    // Reject if the url contains a "query" part unless there
                    // are caching directives.
                    TBool isQuery;
                    isQuery = aTrans.Request().URI().IsPresent( EUriQuery );

                    if( isQuery && !hasExplicitCache )
                        {
                        isCacheable = EFalse;
                        }
                    break;
                    }
                case EMethodPost:
                    {
                    // Reject unless there are caching directives
                    if( !hasExplicitCache )
                        {
                        isCacheable = EFalse;
                        }
                    break;
                    }
                case EMethodHead:
                    {
                    // Don't cache Head responses...we don't need to, and the
                    // cache implementation can't currently handle it anyway.
                    // So just fall through to the default case.
                    // Reject all other methods
                    }
                default:
                    {
                    isCacheable = EFalse;
                    break;
                    }
                }
            }
        }
    return isCacheable;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::WriteLog
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::WriteLog(
    TInt aLogLevel,
    TPtrC aBuf,
    TInt aAny )
    {
#ifdef __CACHELOG__
    TBool log( aLogLevel <= KCurrentLogLevel );
    TPtrC fileName( KHttpCacheGeneralFileName );

    if ( aLogLevel == 1 )
        {
        // write logging to hash.txt
        fileName.Set( KHttpCacheHashFileName );
        log = ETrue;
        }

    if ( log )
        {
        if ( aAny != 0xffff )
            {
            RFileLogger::WriteFormat(_L("Browser"), fileName, EFileLoggingModeAppend,
                    _L("%S %d"), &aBuf, aAny );
            }
        else
            {
            RFileLogger::WriteFormat(_L("Browser"), fileName, EFileLoggingModeAppend,
                    _L("%S"), &aBuf );
            }
        }
#else // __CACHELOG__
    (void)aLogLevel;
    (void)aBuf;
    (void)aAny;
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::WriteFormatLog
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::WriteFormatLog(
    TInt aLogLevel,
    TRefByValue<const TDesC> aBuf, ... )
    {
#ifdef __CACHELOG__
    TBool log( aLogLevel <= KCurrentLogLevel );
    TPtrC fileName( KHttpCacheGeneralFileName );

    if ( aLogLevel == 1 )
        {
        // write logging to hash.txt
        fileName.Set( KHttpCacheHashFileName );
        log = ETrue;
        }

    if ( log )
        {
        VA_LIST args;
        VA_START(args, aBuf);
        TBuf16<1024> string;
        TDes16IgnoreOverflow overflow;
        string.AppendFormatList(aBuf, args, &overflow);
        RFileLogger::WriteFormat(_L("Browser"), fileName, EFileLoggingModeAppend, string);
        VA_END(args);
        }
#else // __CACHELOG__
    (void)aLogLevel;
    (void)aBuf;
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::WriteLogFilenameAndUrl
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::WriteLogFilenameAndUrl(
    TInt aLogLevel,
    TPtrC aMethodName,
    const TPtrC aFilename,
    const TDesC8& aUrl,
    TInt aAny,
    TLogItemType aItemType )
    {
#ifdef __CACHELOG__
    // Create a buffer for method name, filename, and url string
    _LIT(KColonSpace, " : ");
    _LIT(KSpace, " ");
    TInt itemTypeStringLen( 30 );
    TInt tmpLen( aMethodName.Length() + aFilename.Length() +
                 3*KColonSpace().Length() + aUrl.Length() + itemTypeStringLen );
    HBufC* tmp = HBufC::New( tmpLen );

    if ( tmp ) {
        TPtr tmpPtr( tmp->Des() );
        tmpPtr.Copy( aMethodName );
        tmpPtr.Append( KColonSpace );

        TChar backSlash('\\');
        TInt filenamePos( aFilename.LocateReverse( TChar('\\') ) );
        if ( filenamePos > 0 ) {
            tmpPtr.Append( aFilename.Right( aFilename.Length() - filenamePos - 1 ) );
            tmpPtr.Append( KColonSpace );
        }

        // Convert url to TPtr
        HBufC* tmpUrl = HBufC::New( aUrl.Length() );
        if ( tmpUrl ) {
            TPtr tmpUrlPtr( tmpUrl->Des() );
            tmpUrlPtr.Copy( aUrl );
            TInt urlPos( tmpUrlPtr.LocateReverse( TChar('/') ) );
            if ( urlPos > 0 && (aUrl.Length()-2 > urlPos) ) {
                tmpPtr.Append( tmpUrlPtr.Right( aUrl.Length() - urlPos - 1 ) );
                tmpPtr.Append( KColonSpace );
            }
            else {
                tmpPtr.Append( tmpUrlPtr );
                tmpPtr.Append( KColonSpace );
            }
        }

        // Append the bucketIndex, lookup table pos, etc...
        switch ( aItemType )
        {
            case ELogItemTypeNone:
                break;

            case ELogBucketIndex:
                tmpPtr.Append( _L("bucketIndex =") );
                break;

            case ELogEntrySize:
                tmpPtr.Append( _L("entrySize =") );
                break;

            case ELogLookupTablePos:
                tmpPtr.Append( _L("lookupTable pos =") );
                break;

            case ELogFileErrorCode:
                tmpPtr.Append( _L("file errorCode =") );
                break;

            default:
                break;
        }

        HttpCacheUtil::WriteLog( aLogLevel, tmpPtr, aAny );

        delete tmp;
        delete tmpUrl;
        }
#else // __CACHELOG__
    (void)aLogLevel;
    (void)aMethodName;
    (void)aFilename;
    (void)aUrl;
    (void)aAny;
    (void)aItemType;
#endif
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::WriteUrlToLog
//
// Get the freshness of the "entry".
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::WriteUrlToLog(
    TInt aLogLevel,
    const TDesC8& aUrl,
    TInt aAny )
    {
#ifdef __CACHELOG__
    HBufC* tmp = HBufC::New( aUrl.Length() );
    if( tmp )
        {
        TPtr tmpPtr( tmp->Des() );
        tmpPtr.Copy( aUrl );
        HttpCacheUtil::WriteLog( aLogLevel, tmpPtr, aAny );
        delete tmp;
        }
#else // __CACHELOG__
    (void)aLogLevel;
    (void)aUrl;
    (void)aAny;
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::WriteUrlToLog
//
// Get the freshness of the "entry".
//
// -----------------------------------------------------------------------------
//
void HttpCacheUtil::WriteUrlToLog(
    TInt aLogLevel,
    const TDesC& aTxt,
    const TDesC8& aUrl,
    TInt aAny )
    {
#ifdef __CACHELOG__
    TBool log( aLogLevel <= KCurrentLogLevel );
    TPtrC fileName( KHttpCacheGeneralFileName );

    if( aLogLevel == 1 )
        {
        // hash
        fileName.Set( KHttpCacheHashFileName );
        log = ETrue;
        }
    if( log )
        {    //
        HBufC* tmp = HBufC::New( aUrl.Length() );
        if( tmp )
            {
            TPtr tmpPtr( tmp->Des() );
            tmpPtr.Copy( aUrl );
            if( aAny != 0xffff )
                {
                RFileLogger::WriteFormat( _L("Browser"), fileName, EFileLoggingModeAppend,
                        _L("%S %S %d"), &aTxt, &tmpPtr, aAny );
                }
            else
                {
                RFileLogger::WriteFormat(_L("Browser"), fileName, EFileLoggingModeAppend,
                        _L("%S %S"), &aTxt, &tmpPtr );
                }
            delete tmp;
            }
        }
#else // __CACHELOG__
    (void)aLogLevel;
    (void)aTxt;
    (void)aUrl;
    (void)aAny;
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::GenerateNameLC
// Given a URL, generates fully qualified Symbian path for storing HTTP response.
// The format is <cache base dir> + <subdirectory> + <file name>.
// Caller must free the returned HBufC* when done.
// -----------------------------------------------------------------------------
HBufC* HttpCacheUtil::GenerateNameLC( const TDesC8& aUrl, const TDesC& aBaseDir )
    {
    TUint32 crc (0);

    // Use the entire URL for CRC calculation: maximizes source
    // entropy/avoids collisions. Returns 8 char hex filename, use
    // KMaxFilenameLength to internalize/externalize.
    Mem::Crc32(crc, aUrl.Ptr(), aUrl.Size());

    // extract least significant 4 bits (nibble) for subdirectory
    TUint32 nibble( crc & (KCacheSubdirCount-1) );

    // filename has full path, drive:\directory\sub-directory\8-char filename
    // e.g. c:\system\cache\E\078AFEFE, where E is calculated by nibble()
    // and 078AFEFE is calculated by crc32()
    HBufC* fileName = HBufC::NewLC( KMaxPath );
    // Note in KFormat, the %08x : a 32-bit value can represented as 0xFFFFFFFF
    _LIT(KFormat,"%S%x%c%08x");
    fileName->Des().Format(KFormat, &aBaseDir, nibble, KPathDelimiter, crc);

    return fileName;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::Freshness
//
// Get the freshness of the "entry".
//
// -----------------------------------------------------------------------------
//
TInt64 HttpCacheUtil::Freshness(
    const RHTTPHeaders& aHeaders,
    TTime aResponseTime,
    RStringPool aStrP )
    {
    TInt status( KErrNone );
    TInt64 maxAge;
    TTime expires;
    TTime lastModified( 0 );
    TTime date;
    RStringF fieldName;
    THTTPHdrVal hdrValue;
    TInt err;
    TInt64 freshness( 0 );

    // Get the date from the headers
    fieldName = aStrP.StringF( HTTP::EDate, RHTTPSession::GetTable() );
    err = aHeaders.GetField( fieldName, 0, hdrValue );
    if ( err == KErrNotFound || hdrValue.Type() != THTTPHdrVal::KDateVal )
        {
        date = aResponseTime;
        }
    else
        {
        date = TTime( hdrValue.DateTime() );
        }

    // Get useful cache-control directives
    status = GetCacheControls( aHeaders, &maxAge, NULL, NULL, NULL, NULL, NULL, aStrP );
    if ( status == KErrNone )
        {
        // max-age is in delta-seconds. Convert it to micro seconds.
        // All our calculations are in micro-seconds
        // If maxAge is present, use it
        if ( maxAge != -1 )
            {
            freshness = maxAge * 1000 * 1000;

            return freshness;
            }

        // Get the expires from the headers
        fieldName = aStrP.StringF( HTTP::EExpires, RHTTPSession::GetTable() );
        err = aHeaders.GetField( fieldName, 0, hdrValue );
        if ( err == KErrNotFound || hdrValue.Type() != THTTPHdrVal::KDateVal )
            {
            expires = 0;
            }
        else
            {
            expires = TTime( hdrValue.DateTime() );
            }

        // Otherwise, if the expires is present use it
        if ( err != KErrNotFound )
            {
            freshness = expires.Int64() - date.Int64();
            return freshness;
            }

        // Get the last modified from the headers
        fieldName = aStrP.StringF( HTTP::ELastModified, RHTTPSession::GetTable() );
        err = aHeaders.GetField( fieldName, 0, hdrValue );
        if( err == KErrNotFound || hdrValue.Type() != THTTPHdrVal::KDateVal )
            {
            hdrValue = 0;
            }
        else
            {
            lastModified = TTime( hdrValue.DateTime() );
            }

        // Otherwise, if last-modified is present use it
        if ( err != KErrNotFound )
            {
            if( aResponseTime > lastModified )
                {
                freshness = aResponseTime.Int64() - lastModified.Int64();
                freshness += ((TInt64)( freshness * 0.10 ) );
                }
            return freshness;
            }
        // If it get here then we have no way to determine if it is
        // fresh.  So we set freshness to zero, which will most likely
        // cause the resource to be validated.
        freshness = 0;
        }
    return freshness;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::Age
//
// Get the age of the "entry".
//
// -----------------------------------------------------------------------------
//
TInt64 HttpCacheUtil::Age(
    const RHTTPHeaders& aHeaders,
    TTime aRequestTime,
    TTime aResponseTime,
    RStringPool aStrP )
    {
    TTime now;
    TDateTime date;
    TTime time;

    TInt64 correctedRecvAge;
    TInt64 apparentAge;
    TInt64 responseDelay;
    TInt64 correctedInitialAge;
    TInt64 residentTime;
    TInt64 currentAge;
    TInt64 age;


    RStringF fieldName;
    THTTPHdrVal dateValue;
    TInt err;

    // Get the current time. All internet dates are GMT
    now.UniversalTime();
#ifdef __CACHELOG__
    TBuf<50> dateString;
    //
    TRAP( err, now.FormatL( dateString, KDateString ) );
    if ( err == KErrNone ) {
        HttpCacheUtil::WriteLog( 0, _L( "current time" ) );
        HttpCacheUtil::WriteLog( 0, dateString );
        }
#endif // __CACHELOG__

    // The aRequestTime is same as that of headers time.

    time = aRequestTime;

    // Do a sanity check
    if( aRequestTime > now )
        {
        aRequestTime = now;
        }
    if( aResponseTime > now )
        {
        aResponseTime = now;
        }
    if( aRequestTime > aResponseTime )
        {
        aRequestTime = aResponseTime;
        }
    if( time > aResponseTime )
        {
        time = aResponseTime;
        }

    // Get the age from the headers.  If age is missing it equals
    // 0, which is a safe value to use below.
    fieldName = aStrP.StringF( HTTP::EAge, RHTTPSession::GetTable() );
    err = aHeaders.GetField( fieldName, 0, dateValue );
    if( err == KErrNotFound )
        {
        age = 0;
        }
    else
        {
        // Age should be in delta seconds
        if( dateValue.Type() == THTTPHdrVal::KTIntVal )
            {
            age = dateValue;
            }
        else
            {
            age = 0;
            }
        }

    // Get the "current" age
    apparentAge = Max( TInt64( 0 ), aResponseTime.Int64() - time.Int64() );
    // The TTime::Int64() gives the time in micro seconds. And the age field is in
    // delta seconds.
    correctedRecvAge = Max( apparentAge, age * 1000 * 1000 );
    responseDelay = aResponseTime.Int64() - aRequestTime.Int64();
    correctedInitialAge = correctedRecvAge + responseDelay;
    residentTime = now.Int64() - aResponseTime.Int64();

    currentAge = correctedInitialAge + residentTime;

    //the current age in micro seconds.
    return currentAge;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::GetCacheControls
//
// Description: Returns various cache controls.
//
// -----------------------------------------------------------------------------
//
TInt HttpCacheUtil::GetCacheControls(
    const RHTTPHeaders& aHeaders,
    TInt64* aMaxAge,
    TInt64* aMinFresh,
    TInt64* aMaxStale,
    TBool* aMustRevalidate,
    TBool* aNoCache,
    TBool* aNoStore,
    RStringPool aStrP )
    {
    TInt status( KErrNone );
    TInt i;
    TInt cacheCount( 0 );
    RStringF directive;
    TInt64 value;
    char* extraValues = NULL;
    RStringF fieldName;


    if( aMaxAge )
        {
        *aMaxAge = -1;
        }
    if( aMinFresh )
        {
        *aMinFresh = -1;
        }
    if( aMaxStale )
        {
        *aMaxStale = -1;
        }
    if( aMustRevalidate )
        {
        *aMustRevalidate = EFalse;
        }
    if( aNoCache )
        {
        *aNoCache = EFalse;
        }
    if( aNoStore )
        {
        *aNoStore = EFalse;
        }

    // init the field name
    fieldName = aStrP.StringF( HTTP::ECacheControl, RHTTPSession::GetTable() );
    TRAP( status, cacheCount = aHeaders.FieldPartsL( fieldName ) );
    if( status == KErrNone )
        {
        for( i = 0; i < cacheCount; i++ )
            {
            status = GetCacheControlDirective( aHeaders, i, directive, &value, &extraValues, aStrP );
            if( status == KErrNone )
                {
                if( directive == aStrP.StringF( HTTP::EMaxAge, RHTTPSession::GetTable() ) )
                    {
                    if( aMaxAge )
                        {
                        *aMaxAge = value;
                        }
                    }
                else if( directive == aStrP.StringF( HTTP::EMinFresh, RHTTPSession::GetTable() ) )
                    {
                    if( aMinFresh )
                        {
                        *aMinFresh = value;
                        }
                    }
                else if( directive == aStrP.StringF( HTTP::EMaxStale, RHTTPSession::GetTable() ) )
                    {
                    if( aMaxStale )
                        {
                        *aMaxStale = value;
                        }
                    }
                else if( directive == aStrP.StringF( HTTP::EMustRevalidate, RHTTPSession::GetTable() ) )
                    {
                    if( aMustRevalidate )
                        {
                        *aMustRevalidate = ETrue;
                        }
                    }
                else if( directive == aStrP.StringF( HTTP::ENoCache, RHTTPSession::GetTable() ) )
                    {
                    if( aNoCache )
                        {
                        *aNoCache = ETrue;
                        }
                    }
                else if( directive == aStrP.StringF( HTTP::ENoStore, RHTTPSession::GetTable() ) )
                    {
                    if( aNoStore )
                        {
                        *aNoStore = ETrue;
                        }
                    }
                directive.Close();
                // It ignores extraValues, just free it
                User::Free( extraValues );
                extraValues = NULL;
                }
            else
                {
                break;
                }
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::GetCacheControlDirective
//
// -----------------------------------------------------------------------------
//
TInt HttpCacheUtil::GetCacheControlDirective(
    const RHTTPHeaders& aHeaders,
    TInt aIndex,
    RStringF& aDirective,
    TInt64* aDirectiveValue,
    char** aExtraValue,
    RStringPool aStrP )
    {
    RStringF fieldName;
    THTTPHdrVal hdrVal;
    TInt err;

    // Get the cache-control from the headers
    fieldName = aStrP.StringF( HTTP::ECacheControl, RHTTPSession::GetTable() );
    aHeaders.GetField( fieldName, aIndex, hdrVal );

    if( hdrVal.Type() == THTTPHdrVal::KStrVal || hdrVal.Type() == THTTPHdrVal::KStrFVal )
        {
        RStringF cacheDir = hdrVal.StrF();

        TInt endPos;
        _LIT8(KFind, "=");

        endPos = cacheDir.DesC().Find( KFind );
        if( endPos != -1 )
            {
            TRAP( err, aDirective = aStrP.OpenFStringL( cacheDir.DesC().Left( endPos ) ) );
            if( err != KErrNone )
                {
                return err;
                }
            TPtrC8 value = cacheDir.DesC().Right( cacheDir.DesC().Length() - endPos - 1 );
            err = ExtractCacheControlDirectiveValue( aStrP, aDirective, value, aDirectiveValue, aExtraValue );
            if( err != KErrNone )
                {
                aDirective.Close();
                return err;
                }
            }
        else
            {
            aDirective = cacheDir.Copy();
            // Directives which MUST have values
            if( ( aDirective == aStrP.StringF( HTTP::EMaxAge, RHTTPSession::GetTable() ) ) ||
                ( aDirective == aStrP.StringF( HTTP::EMinFresh, RHTTPSession::GetTable() ) ) ||
                ( aDirective == aStrP.StringF( HTTP::ESMaxAge, RHTTPSession::GetTable() ) ) )
                {
                aDirective.Close();
                return KErrGeneral;
                }
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// HttpCacheUtil::ExtractCacheControlDirectiveValue
//
// -----------------------------------------------------------------------------
//
TInt HttpCacheUtil::ExtractCacheControlDirectiveValue(
    RStringPool aStrP,
    RStringF& aDirective,
    TDesC8& aValue,
    TInt64* aDirectiveValue,
    char** aExtraValue )
    {
    TInt status( KErrNone );
    TInt temp;
    char* errorIfNull;

    *aDirectiveValue = -1;
    *aExtraValue = NULL;
    char* valuestr = (char*)User::Alloc( aValue.Length() + 1 );
    if( !valuestr )
        {
        return KErrNoMemory;
        }
    memcpy( valuestr, aValue.Ptr(), aValue.Length() );
    valuestr[ aValue.Length() ] = '\0';


    if( ( aDirective == aStrP.StringF( HTTP::EMaxAge, RHTTPSession::GetTable() ) ) ||
        ( aDirective == aStrP.StringF( HTTP::EMinFresh, RHTTPSession::GetTable() ) ) ||
        ( aDirective == aStrP.StringF( HTTP::ESMaxAge, RHTTPSession::GetTable() ) ) )
        {
        // Cases with directiveValues
        temp = strtoul( valuestr, &errorIfNull, 10 );
        if( !errorIfNull )
            {
            status = KErrGeneral;
            }
        else
            {
            *aDirectiveValue = temp;
            }
        User::Free( valuestr );
        return status;
        }

    if( ( aDirective == aStrP.StringF( HTTP::EMaxStale, RHTTPSession::GetTable() ) ) )
        {
        // Cases with optional directiveValues
        temp = strtoul( valuestr, &errorIfNull, 10 );
        if( errorIfNull )
            {
            *aDirectiveValue = temp;
            }
        User::Free( valuestr );
        return status;
        }

    if( ( aDirective == aStrP.StringF( HTTP::ENoCache, RHTTPSession::GetTable() ) ) ||
        ( aDirective == aStrP.StringF( HTTP::EPrivate, RHTTPSession::GetTable() ) ) )
        {
        *aExtraValue = valuestr;
        return status;
        }
    User::Free( valuestr );
    return status;
    }


// -----------------------------------------------------------------------------
// FilePathHash
// Hash function for Symbian file paths: discards case first
// -----------------------------------------------------------------------------
static TUint32 FilepathHash(const TDesC& aDes)
{
    //since this function is intensively used by the HashMap,
    //keeping (slow) heap allocation out of it.
    TBuf<KMaxPath> normalized ( aDes );

    normalized.LowerCase();
    return DefaultHash::Des16( normalized );
}

// -----------------------------------------------------------------------------
// FilepathIdent
// Comparator for Symbian file paths: Use case-insensitive compare
// -----------------------------------------------------------------------------
static TBool FilepathIdent(const TDesC& aL, const TDesC& aR)
{
    return ( aL.CompareF(aR) == 0 );
}


void HttpCacheUtil::GenerateCacheContentHashMapL(CHttpCacheFileHash*& aHashMap, RFs& aRfs, const TDesC& aCacheFolder, const TInt aInitialValue)
    {
    // need to be able to initialise hash here, but use it sensibly in calling function hence rather strange object construction
    aHashMap = CHttpCacheFileHash::NewLC(aInitialValue);
    
    CDirScan* scanner = CDirScan::NewLC( aRfs );

    //Configure CDirScan to tell you all contents of a particular directory hierarchy
    scanner->SetScanDataL( aCacheFolder, KEntryAttNormal, ESortNone );
    CDir* matchingFiles( 0 );

    //Step 1. Find out all files on disk: by walking the directory hierarchy, one directory at a time
    for (;;)
        {
        //1a. Get list of files in current directory, NULL if no directory left in tree
        scanner->NextL( matchingFiles );
        if ( !matchingFiles )
            break;

        TPtrC dir( scanner->FullPath() );

        //1b. Add any files found to the HashTable
        const TInt nMatches = matchingFiles->Count();
        for ( TInt i = 0; i < nMatches; i++ )
            {
            TEntry entry ( (*matchingFiles)[i] ) ;
            aHashMap->InsertAndStoreL( entry, dir );
            }

        delete matchingFiles;
        } // End of step 1: adding all known files on disk to Map

    CleanupStack::PopAndDestroy( 1, scanner );
    CleanupStack::Pop( aHashMap );
    
#ifdef __CACHELOG__
    {
    HttpCacheUtil::WriteFormatLog(0, _L("-----------START PRINTING MAP OF SIZE %d---------"), aHashMap->HashMap().Count());
    THttpCacheFileHashIter iter(aHashMap->HashMap());
    const TDesC* key;
    while ((key = iter.NextKey()) != 0)
        {
        const TFileInfo* val = iter.CurrentValue();
        HttpCacheUtil::WriteFormatLog(0, _L("MAP WALK: %S, with size = %d, value = %d "), key, val->iFileSize, val->iUserInt);
        }
    HttpCacheUtil::WriteFormatLog(0, _L("-----------DONE PRINTING MAP-------------"));
    }
#endif
    }

void HttpCacheUtil::EnsureTrailingSlash( TDes& aPath )
    {
    // fix folder by appending trailing \\ to the end -symbian thing
    // unless it is already there
    if (aPath.LocateReverse('\\') != aPath.Length() - 1)
        {
        aPath.Append(_L("\\"));
        }
    
    }


CHttpCacheFileHash::CHttpCacheFileHash(const TInt aInitialValue, const THashFunction32<TDesC>& aHash, const TIdentityRelation<TDesC>& aIdentity) : iHashMap(aHash, aIdentity), iInitialValue(aInitialValue)
    {
    }

CHttpCacheFileHash* CHttpCacheFileHash::NewLC(const TInt iInitialValue)
    {
    CHttpCacheFileHash* obj = new (ELeave) CHttpCacheFileHash(iInitialValue, &FilepathHash, &FilepathIdent);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    return obj;
    }

CHttpCacheFileHash::~CHttpCacheFileHash()
    {
    iStringStorage.ResetAndDestroy();
    delete iFileInfoStorage;
    iHashMap.Close();
    }

void CHttpCacheFileHash::ConstructL()
    {
    iFileInfoStorage = new (ELeave) CArrayFixSeg<TFileInfo>(64);    // 64 objects at a time
    }

void CHttpCacheFileHash::InsertAndStoreL(const TEntry& aEntry, const TDesC& aDir)
    {
    TFileInfo& info = iFileInfoStorage->ExtendL();
    info.iFileSize = aEntry.iSize;
    info.iUserInt = iInitialValue;
    HBufC* fullPath = HBufC::NewL( aDir.Length() + aEntry.iName.Length() );
    iStringStorage.AppendL( fullPath ); //keep object safe for later destruction
    TPtr path(fullPath->Des());
    path.Copy( aDir );
    path.Append( aEntry.iName );
    iHashMap.Insert( fullPath , &info );
    }

//  End of File
