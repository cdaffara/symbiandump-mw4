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
#include "config.h"
#include "MultipartContentHandler.h"
#include "multipartparser.h"
#include "bodypart.h"
#include "HttpSessionManager.h"
#include "httpcachemanager.h"
#include <http/rhttptransaction.h>
#include <uri16.h>
#include "httpfiltercommonstringsext.h"
#include "WebCharsetData.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "HttpConnUtils.h"

// CONSTANTS
_LIT ( KContentTypeMultipartMixed,  "multipart/mixed");
_LIT8( KContentTypeMultipartMixed8, "multipart/mixed");
_LIT8( KContentTypeTextHtml8, "text/html");
_LIT8( KContentTypeAppXhtml8, "application/xhtml+xml");
_LIT8( KContentTypeAppWapXhtml8, "application/vnd.wap.xhtml+xml");
_LIT8( KContentTypeAppOctetStream, "application/octet-stream");
_LIT8( KContentTypeTextWml8, "text/vnd.wap.wml");
_LIT8( KContentTypeImages8, "image/");
_LIT8( KContentTypeScript8, "application/x-javascript");
_LIT8( KContentTypeCss8, "text/css");
_LIT8( KErrPage, "<html><body><h1>ERROR:</h1><h2>Invalid Content</h2><br/></body></html>");
_LIT8( KBoundaryEndTag, "--");
_LIT8( KContentTypeStr, "Content-Type: ");
_LIT8( KCharsetStr, "; charset=");
_LIT8( KCrLf, "\r\n");
_LIT8( KEmptyStr, "");
_LIT ( KExpiresFormat,"Expires: %F%*E, %D %*N %Y, %H:%T:%S GMT\r\n");
_LIT8( KCacheControl, "Cache-Control: ");
_LIT8( KMaxAge, "max-age=");
_LIT8( KExpires, "Expires: ");
//_LIT ( KExtHtml, ".html");
//_LIT ( KExtHtm, ".htm");
//_LIT ( KExtAsp, ".asp");
//_LIT ( KExtPhp, ".php");
//_LIT ( KExtJsp, ".jsp");

const TInt KDefaultRespBufferSize = 20 * 1024; // if no content-length header, alloc buffer this size
const TInt KRespBufferExtraMargin = 5 * 1024;  // extra length of buffer to avoid realloc()
const TInt KExpiresHeaderLength = 1024;        // default length for Expires header
const TInt KExpiresInterval = 5;               // num minutes for expiration of multipart content parts
const TInt KBoundaryEndMarkerLen = 2;          // length of the the "--" on last multipart part
const TUint KCharsetUidDef = KCharacterSetIdentifierIso88591;

using namespace WebCore;

// -----------------------------------------------------------------------------
// MultipartContentHandler::MultipartContentHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
MultipartContentHandler::MultipartContentHandler()
    : m_ResponseUrl(NULL),
      m_TopLevelBoundary(NULL),
      m_TopLevelBoundaryEnd(NULL),
      m_ParsingCompleted(false),
      m_CacheExpirationHeader(NULL)
{
    m_MarkupContent.Set(KEmptyStr);
    m_MarkupCharset.Set(KEmptyStr);
    m_MarkupContentType.Set(KEmptyStr);
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void MultipartContentHandler::ConstructL()
{
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MultipartContentHandler* MultipartContentHandler::NewL()
{
    MultipartContentHandler* self = new(ELeave)MultipartContentHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
MultipartContentHandler::~MultipartContentHandler()
{
    for(TInt n = 1; n<m_BodyPartsArray.Count(); n++) {
        delete m_BodyPartsArray[n];
    }
    m_BodyPartsArray.Reset();
    delete m_ResponseBuffer;
    delete m_ResponseUrl;
    delete m_TopLevelBoundary;
    delete m_TopLevelBoundaryEnd;
    delete m_CacheExpirationHeader;
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::IsSupported
// -----------------------------------------------------------------------------
bool MultipartContentHandler::IsSupported(
    const ResourceResponse& response)
{
    TInt pos = response.mimeType().des().Find(KContentTypeMultipartMixed);
    //
    return (pos != KErrNotFound);
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::HandleResponseHeadersL
// -----------------------------------------------------------------------------
TInt MultipartContentHandler::HandleResponseHeadersL(
    const ResourceResponse& response,
    RHTTPTransaction& httpTransaction)
{
    // initialize response body buffer
    TUint contentLength = response.expectedContentLength();
    contentLength = contentLength > 0 ? contentLength : KDefaultRespBufferSize;
    // add extra size to buffer to avoid realloc calls
    m_ResponseBuffer = HBufC8::NewL(contentLength + KRespBufferExtraMargin) ;
    TPtrC8 respUrl = response.url().des();
    delete m_ResponseUrl;
    m_ResponseUrl = HBufC::NewL(respUrl.Length());
    m_ResponseUrl->Des().Copy(respUrl);

    TPtrC8 boundPtr;
    THTTPHdrVal hdrVal;
    // save multipart boundary string
    RHTTPHeaders hdrs = httpTransaction.Response().GetHeaderCollection();
    RHTTPSession session = httpTransaction.Session();
    RStringPool stringPool = session.StringPool();
    const TStringTable& stringTable = session.GetTable();
    if(hdrs.GetParam(stringPool.StringF( HTTP::EContentType, stringTable ),
        stringPool.StringF( HttpFilterCommonStringsExt::EBoundary,
        HttpFilterCommonStringsExt::GetTable()), hdrVal) == KErrNone) {
        boundPtr.Set(hdrVal.StrF().DesC());
    }
    // locate any cache control headers in top-level response
    TPtrC8 expiresPtr;
    TPtrC8 cacheControlPtr;
    hdrs.GetRawField(stringPool.StringF(HTTP::EExpires, stringTable), expiresPtr);
    hdrs.GetRawField(stringPool.StringF(HTTP::ECacheControl, stringTable), cacheControlPtr);
    if(expiresPtr.Length() > 0) {
        // found header matching "Expires: . . ."
        delete m_CacheExpirationHeader;
        m_CacheExpirationHeader = HBufC8::NewL(KExpires().Length() +
            expiresPtr.Length() + KCrLf().Length());
        TPtr8 expPtr = m_CacheExpirationHeader->Des();
        expPtr.Copy(KExpires);
        expPtr.Append(expiresPtr);
        expPtr.Append(KCrLf);
    } else if(cacheControlPtr.FindF(KMaxAge) != KErrNotFound) {
        // found header matching "Cache-Control: max-age= . . ."
        delete m_CacheExpirationHeader;
        m_CacheExpirationHeader = HBufC8::NewL(KCacheControl().Length() +
            KMaxAge().Length() + cacheControlPtr.Length() + KCrLf().Length());
        TPtr8 expPtr = m_CacheExpirationHeader->Des();
        expPtr.Copy(KCacheControl);
        expPtr.Append(cacheControlPtr);
        expPtr.Append(KCrLf);
    } else {
        // generate Expires header with default expiration
        m_CacheExpirationHeader = GenerateExpiresHeaderL();
    }
    // save boundary strings to member variable buffers
    m_TopLevelBoundary = HBufC8::NewL(boundPtr.Length());
    m_TopLevelBoundary->Des().Copy(boundPtr);
    //
    m_TopLevelBoundaryEnd = HBufC8::NewL(boundPtr.Length() + KBoundaryEndMarkerLen );
    m_TopLevelBoundaryEnd->Des().Copy( boundPtr);
    m_TopLevelBoundaryEnd->Des().Append(KBoundaryEndTag);
    //
    return KErrNone;
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::HandleResponseBodyL
// -----------------------------------------------------------------------------
TInt MultipartContentHandler::HandleResponseBodyL(
    TDesC8& aRespData)
{
    TInt status(KErrNone);
    if (aRespData.Length()) {
        // do we have enough capacity left in response buffer to handle this chunk?
        TInt sizeNeeded = m_ResponseBuffer->Length() + aRespData.Length();
        if (sizeNeeded > m_ResponseBuffer->Des().MaxLength()) {
            // add extra length to avoid future realloc's
            m_ResponseBuffer = m_ResponseBuffer->ReAllocL( sizeNeeded +
            KRespBufferExtraMargin);
        }
        m_ResponseBuffer->Des().Append(aRespData);
    }
    return status;
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::ResponseCompleteL
// -----------------------------------------------------------------------------
TInt MultipartContentHandler::ResponseComplete()
{
    TRAPD(ret, HandleMultipartMixedL());
    return ret;
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::MarkupContent
// -----------------------------------------------------------------------------
const TDesC8& MultipartContentHandler::MarkupContent()
{
    __ASSERT_DEBUG(m_ParsingCompleted, User::Panic(_L("MultipartContentHandler"), KErrNotReady));
    return m_MarkupContent;
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::HandleMultipartMixedL
// -----------------------------------------------------------------------------
TInt MultipartContentHandler::HandleMultipartMixedL()
{
    TInt status(KErrNone);
    // parse complete response
    MultipartParser::ParseL(*m_ResponseBuffer, KContentTypeMultipartMixed8,
                            *m_TopLevelBoundary, *m_ResponseUrl,
                            m_BodyPartsArray, -1);
    TInt  partCount(ValidateMultipartArrayL());
    //
    if (partCount > 0) {
        TUint charsetId(KCharsetUidDef);
        m_MarkupCharset.Set(m_BodyPartsArray[0]->Charset());
        m_MarkupContentType.Set(m_BodyPartsArray[0]->ContentType());
        CHttpCacheManager* cacheMgr = StaticObjectsContainer::instance()->
                                      resourceLoaderDelegate()->
                                      httpSessionManager()->cacheManager(); 
        // push non-markup parts to the two caches
        // this assumes that response follows convention for first
        // part to be the markup and following parts to be the 
        // resources referenced by the markup
        for(TInt n = 1; n<m_BodyPartsArray.Count(); n++) {
            PushToHttpCacheL(*m_BodyPartsArray[n], cacheMgr);
            // PushToMemoryCacheL(*m_BodyPartsArray[n]);
            delete m_BodyPartsArray[n];
            m_BodyPartsArray[n] = NULL;
        }
        m_MarkupContent.Set(m_BodyPartsArray[0]->Body());
    } else {
        __DEBUGGER();  // fixme: remove once all testing complete                
        m_MarkupContentType.Set(KContentTypeTextHtml8);        
        m_MarkupContent.Set(KErrPage);
        status = KErrGeneral;
    }
    m_ParsingCompleted = true;
    return status;
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::PushToMemoryCache
// -----------------------------------------------------------------------------
/*void MultipartContentHandler::PushToMemoryCacheL( CBodyPart& aPart )
{
    // not implemented
}*/

// -----------------------------------------------------------------------------
// CHttpLoaderUtils::PushToHttpCacheL
// -----------------------------------------------------------------------------
void MultipartContentHandler::PushToHttpCacheL(
    CBodyPart& aPart,
    CHttpCacheManager* aCacheMgr)
{
    TBool status(EFalse);
    __ASSERT_DEBUG(aCacheMgr, THttpConnUtils::PanicLoader(KErrGeneral));

    // convert the part's url to ascii
    TPtrC8 asciiUrl;
    TUint8* retPtr = NULL;
    THttpConnUtils::ConvertPtrUsc2ToAscii( aPart.Url(), asciiUrl, &retPtr );
    if(aCacheMgr->Find(asciiUrl)) {
        aCacheMgr->RemoveL(asciiUrl);
    }
    // generate content-type header
    TInt contTypeLen = aPart.ContentType().Length();
    TInt charsetLen = aPart.Charset().Length();
    TInt hdrsLen(0);
    if(contTypeLen > 0) {
        hdrsLen += KContentTypeStr().Length() + contTypeLen + KCrLf().Length();
        if(charsetLen > 0) {
            hdrsLen += KCharsetStr().Length() + charsetLen;
        }
    }
    // generate Expires header
    HBufC8* expirationBuf(NULL);
    TPtrC8 expPtrActual;
    TInt expStart(aPart.Headers().FindF(KExpires));
    TInt cacheControlStart(aPart.Headers().FindF(KCacheControl));
    TInt cleanupCount(0);
    //
    if(expStart != KErrNotFound) {
        // found part header matching "Expires: . . ."
        const TPtrC8 expPtr = aPart.Headers().Mid(expStart);
        TInt hdrEnd = expPtr.FindF(KCrLf);
        hdrEnd = (hdrEnd == KErrNotFound) ? expPtr.Length() : hdrEnd;
        expirationBuf = HBufC8::NewLC(hdrEnd + KCrLf().Length());
        cleanupCount++;
        expirationBuf->Des().Copy(expPtr.Left(hdrEnd));
        expirationBuf->Des().Append(KCrLf);
        expPtrActual.Set(*expirationBuf);
    } else if((cacheControlStart != KErrNotFound) && 
            (aPart.Headers().Find(KMaxAge) != KErrNotFound)) {
        // found part header matching "Cache-Control: max-age= . . ."
        const TPtrC8 expPtr = aPart.Headers().Mid(cacheControlStart);
        TInt hdrEnd = expPtr.FindF(KCrLf);
        hdrEnd = (hdrEnd == KErrNotFound) ? expPtr.Length() : hdrEnd;
        expirationBuf = HBufC8::NewLC(hdrEnd + KCrLf().Length());
        cleanupCount++;
        expirationBuf->Des().Copy(expPtr.Left(hdrEnd));
        expirationBuf->Des().Append(KCrLf);
        expPtrActual.Set(*expirationBuf);
    } else {
        // generate Expires header with default expiration
        expPtrActual.Set(*m_CacheExpirationHeader);
    }
    hdrsLen += expPtrActual.Length();
    HBufC8* headers = HBufC8::NewLC(hdrsLen);
    cleanupCount++;
    TPtr8 headersPtr = headers->Des();
    headersPtr.Copy(expPtrActual);
    //
    if(contTypeLen > 0) {
        headersPtr.Append(KContentTypeStr);
        headersPtr.Append(aPart.ContentType());
        if( charsetLen > 0 ) {
            headersPtr.Append(KCharsetStr);
            headersPtr.Append(aPart.Charset());
        }
        headersPtr.Append( KCrLf );
    }
    //
    status = aCacheMgr->SaveL(asciiUrl, *headers, aPart.Body(), aPart.Headers());
    User::Free( retPtr );
    CleanupStack::PopAndDestroy(cleanupCount);  //headers, expirationBuf
}

// -----------------------------------------------------------------------------
// MultipartContentHandler::IsExtensionTextType
// -----------------------------------------------------------------------------
/*TInt MultipartContentHandler::IsExtensionTypeText(const TDesC16& aResponseUrl)
{

    // special case for application/octet-stream. some web servers
    // are miscofigured and send application/octet-stream even for html
    // page so we need to pass it to webcore as if it was text/html. however
    // some binary content also comes as application/octet-stream (such as
    // sisx files ). so here we che(ha)ck if the response url has .html .htm .asp
    // extension. though luck for the rest. feel free to extend the list.
    TUriParser parser;
    if( parser.Parse( aResponseUrl ) == KErrNone )
        {
        //
        TPtrC path = parser.Extract( EUriPath );
        // path == 1 means only / (no filename)
        if( path.Length() > 1 )
            {

              if (!( path.Find( KExtHtml ) != -1 ||
                  path.Find( KExtHtm ) != -1 ||
                  path.Find( KExtAsp ) != -1 ||
                  path.Find( KExtPhp ) != -1 ||
                  path.Find( KExtJsp ) != -1 ))
                  {
                return KErrGeneral;
                  }
            }
        }

  return KErrNone;
}*/

// -----------------------------------------------------------------------------
// MultipartContentHandler::ValidateMultipartArrayL
// -----------------------------------------------------------------------------
TInt MultipartContentHandler::ValidateMultipartArrayL()
{
    // set the validation rules mask for multipart/mixed
    TInt rulesMask = KRulePartUrlMatchesRespUrl |
                     KRuleUrlMatchingOnHostname |
                     KRuleFirstPartIsMarkup |
                     KRuleOnlyWebContentAllowed;
    //
    TUriParser respUriParser;
    User::LeaveIfError(respUriParser.Parse(*m_ResponseUrl));
    //    
    if (rulesMask & KRulePartUrlMatchesRespUrl) {
        // compare specific portions of part url with multipart response url
        TUriParser partUri;
        //
        if(rulesMask & KRuleUrlMatchingOnScheme) {
            // not yet implemented
            __ASSERT_DEBUG(EFalse, User::Panic(_L("MultipartContentHandler"), KErrArgument));
        }
        //
        if(rulesMask & KRuleUrlMatchingOnHostname) {
            TInt m(0);
            while(m < m_BodyPartsArray.Count()) {
                if((partUri.Parse(m_BodyPartsArray[m]->Url()) != KErrNone)
                    || (respUriParser.Compare(partUri, EUriHost ) != 0)) {
                    // invalid part url
                    delete m_BodyPartsArray[m];
                    m_BodyPartsArray[m] = NULL;
                    m_BodyPartsArray.Remove(m);
                } else {
                    ++m;
                }
            }
        }
        if(rulesMask & KRuleUrlMatchingOnPort) {
            // not yet implemented
            __ASSERT_DEBUG(EFalse, User::Panic(_L("MultipartContentHandler"), KErrArgument));
        }
    }
    if((rulesMask & KRuleFirstPartIsMarkup) && (m_BodyPartsArray.Count() > 0)) {
        // ensure that content-type of first part is either HTML, XHTML or WML
        const TDesC8& contType = m_BodyPartsArray[0]->ContentType();
        if((contType.Find(KContentTypeTextHtml8) == KErrNotFound) &&        // html
           (contType.Find(KContentTypeAppXhtml8) == KErrNotFound) &&      // xhtml
           (contType.Find(KContentTypeAppWapXhtml8) == KErrNotFound) &&   // wap??
           (contType.Find(KContentTypeTextWml8) == KErrNotFound) &&        // wml
           (contType.Find(KContentTypeAppOctetStream) == KErrNotFound) /*||
           IsExtensionTypeText(m_BodyPartsArray[0]->Url()) == KErrGeneral)*/) {
            m_BodyPartsArray.ResetAndDestroy();
            return 0;
        }
    }
    if((rulesMask & KRuleOnlyWebContentAllowed) && (m_BodyPartsArray.Count() > 0)) {
        // remove all unrecognized body parts
        // ensure that content-type of all parts are from the following
        // list: markup, image, script, css, or sound types
        TInt m(0);
        while(m < m_BodyPartsArray.Count()) {
            const TDesC8& contType = m_BodyPartsArray[m]->ContentType();
            if((contType.Find(KContentTypeTextHtml8) == KErrNotFound ) &&     // html
               (contType.Find(KContentTypeImages8 ) == KErrNotFound ) &&       // images
               ( contType.Find( KContentTypeCss8 ) == KErrNotFound ) &&          // css
               ( contType.Find( KContentTypeScript8 ) == KErrNotFound ) &&       // javascript
               ( contType.Find( KContentTypeAppXhtml8 ) == KErrNotFound ) &&     // xhtml
               ( contType.Find( KContentTypeTextWml8 ) == KErrNotFound ) &&      // wml
               ( contType.Find( KContentTypeAppWapXhtml8 ) == KErrNotFound ) &&  // wap??
               ( contType.Find( KContentTypeAppOctetStream ) == KErrNotFound )) { // octet-stream??
                delete m_BodyPartsArray[m];
                m_BodyPartsArray[m] = NULL;
                m_BodyPartsArray.Remove(m);
            } else {
                ++m;
            }
        }
    }
    return m_BodyPartsArray.Count();
}


// -----------------------------------------------------------------------------
// MultipartContentHandler::GenerateExpiresHeader
// -----------------------------------------------------------------------------
HBufC8* MultipartContentHandler::GenerateExpiresHeaderL()
{
    HBufC* expiresBuf = HBufC::NewLC(KExpiresHeaderLength);
    TPtr expiresPtr = expiresBuf->Des();
    TTime expireTime;
    expireTime.UniversalTime();
    expireTime += TTimeIntervalMinutes(KExpiresInterval);
    //
    expireTime.FormatL(expiresPtr, KExpiresFormat);
    // now convert to 8-bit
    HBufC8* retBuf = HBufC8::NewL(expiresBuf->Length());
    retBuf->Des().Copy(*expiresBuf);
    CleanupStack::PopAndDestroy(expiresBuf);
    return retBuf;
}
//  End of File
