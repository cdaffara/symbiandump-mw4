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
#include <http.h>
#include <BrowserUiSDKCRKeys.h>
#include "HttpRequestHeaderManager.h"
#include "httpfiltercommonstringsext.h"
#include "ResourceRequest.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include <syslangutil.h>
#include "WebCharsetData.h"
#include <tinternetdate.h>

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KNullStr, "" );
_LIT ( KStarStr, "*" );
_LIT ( KCommaSeperator, "," );
_LIT8( KStarSlashStar8, "*/*" );
_LIT ( KStarSlashStar, "*/*" );
_LIT ( KQualPtSeven, ";q=0.7" );
_LIT8( KNoCache, "no-cache" );
_LIT8( KOnlyIfCached, "only-if-cached" );
_LIT8( KQHalfValue, ";q=0.5" );
_LIT8( KQWholeValue, ";q=1.0" );
const TUint KDefBufSize = 512;
const TInt KLangStrLen = 10;
const TUint KCharsetUidDef = KCharacterSetIdentifierIso88591;
// LOCAL FUNCTIONS

using namespace WebCore;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::HttpRequestHeaderManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
HttpRequestHeaderManager::HttpRequestHeaderManager(
    RHTTPSession& aSession )
    : m_Session(aSession),
      m_AcceptHdrMimesTopLevel(NULL),
      m_UserCharset(KCharsetUidDef),
      m_CharsetBuf(NULL),
      m_UserSelectedLang(TLanguage(0))
{
}

//-----------------------------------------------------------------------------
// HttpRequestHeaderManager::ConstructL
// Epoc Constructor
//-----------------------------------------------------------------------------
//
void HttpRequestHeaderManager::ConstructL()
{
    m_StringPool = m_Session.StringPool();
    m_StringPool.OpenL( HttpFilterCommonStringsExt::GetTable() );

    m_HttpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    //
	CRepository* repository = CRepository::NewLC(KCRUidBrowser);
	// initialize accept header MIME types
    ReadCenRepMimeTypesL(*repository);
    CleanupStack::PopAndDestroy(repository);
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
HttpRequestHeaderManager* HttpRequestHeaderManager::NewL(
    RHTTPSession& aSession )
{
    HttpRequestHeaderManager* self = new( ELeave ) HttpRequestHeaderManager( aSession );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
HttpRequestHeaderManager::~HttpRequestHeaderManager()
{
    delete m_AcceptMimeTypesCenRep;
    m_AcceptMimeTypesCenRep = NULL;
    delete m_AcceptHdrMimesTopLevel;
    m_AcceptHdrMimesTopLevel = NULL;
    delete m_AcceptLangStr;
    m_AcceptLangStr = NULL;
    delete m_InstalledLangBuf;
    m_InstalledLangBuf = NULL;
    delete m_CharsetBuf;
    m_CharsetBuf = NULL;
}

//-----------------------------------------------------------------------------
// HttpRequestHeaderManager::AddAllHeadersL
// Add all Request headers
//-----------------------------------------------------------------------------
void HttpRequestHeaderManager::AddAllHeadersL(
    RHTTPHeaders& aHeaders,
    const ResourceRequest& aRequest )
{
    SetAcceptHeaderL(aHeaders, aRequest);
	SetClientAcceptHeaderL(aHeaders, aRequest);
    SetContentTypeHeaderL(aHeaders, aRequest);
    SetCacheControlHeadersL(aHeaders, aRequest);
    SetRefererHeaderL(aHeaders, aRequest);
    SetAcceptLanguageHeaderL(aHeaders, aRequest);
    SetAcceptCharsetHeaderL(aHeaders, aRequest);
    SetConnectionHeaderL(aHeaders, aRequest);
    AddXMLHttpHeadersL(aHeaders,aRequest );
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetAcceptHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetAcceptHeaderL(
    RHTTPHeaders& aHeaders,
    const ResourceRequest& aRequest )
{
    const TStringTable& stringTable = m_Session.GetTable();
    TPtrC8 acceptMimeTypes;
    HBufC8* tmpBuf = NULL;
	TInt pushCounter( 0 );

    if(aRequest.mainLoad()) {
        acceptMimeTypes.Set(*TopLevelAcceptStringL(aRequest));
    } else {
        // for requests that are not top-level, WebCore may or may not
        // set accept MIME types.  If it does, we will send that string
        // as it, if not - we will add * / *
        TPtrC reqMimeTypes(aRequest.httpAccept().des());
        if(reqMimeTypes.Length() > 0) {
            // WebCore has set accept MIME type - leave alone
            tmpBuf = HBufC8::NewL(reqMimeTypes.Length());
			CleanupStack::PushL(tmpBuf);
			++pushCounter;
            tmpBuf->Des().Copy(reqMimeTypes);
            acceptMimeTypes.Set(*tmpBuf);
        } else {
            // no MIME type - add * / *
            acceptMimeTypes.Set(KStarSlashStar8);
        }
    }
    RStringF str = m_StringPool.OpenFStringL(acceptMimeTypes);
    CleanupClosePushL(str);
	++pushCounter;
    aHeaders.SetFieldL(m_StringPool.StringF(HTTP::EAccept, stringTable), str);
    CleanupStack::PopAndDestroy(pushCounter);
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetAcceptHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetClientAcceptHeaderL(
    RHTTPHeaders& aHeaders,
    const ResourceRequest& aRequest )
{
    // add client request headers
    const RPointerArray<HBufC8>& clientHeaders = m_HttpSessionMgr->ClientAcceptHeaders();
    int i=0;
    // clientHeaders.Count()-1 for the double i++ to make sure that we 
    // have both the key and the value string
    while (i < clientHeaders.Count()-1)
        {
        TPtrC8 headerName = *(clientHeaders[i++]);
        TPtrC8 headerValue = *(clientHeaders[i++]);
        // header name
        RStringF headerNameStr = m_StringPool.OpenFStringL(headerName);
        CleanupClosePushL(headerNameStr);
        
        // date type
        if (headerNameStr == m_StringPool.StringF(HTTP::EIfModifiedSince, RHTTPSession::GetTable()) ||
            headerNameStr == m_StringPool.StringF(HTTP::EIfUnmodifiedSince, RHTTPSession::GetTable()))
            {
            TInternetDate date;
            date.SetDateL(headerValue);
            TDateTime modifyTime(date.DateTime());
            aHeaders.SetFieldL(headerNameStr, modifyTime);
            }
        // string    
        else     
            {
            RStringF headerValueStr = m_StringPool.OpenFStringL( headerValue ); 
            CleanupClosePushL(headerValueStr);
            
            aHeaders.SetFieldL( headerNameStr, headerValueStr );
            CleanupStack::PopAndDestroy(); // headerNameStr
            }
        //
        CleanupStack::PopAndDestroy(); // headerNameStr
        }
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetContentTypeHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetContentTypeHeaderL(
    RHTTPHeaders& aHeaders,
    const WebCore::ResourceRequest& aRequest)
{
    /* todo: can we avoid this string comparison? e.g. method enum */
    if (aRequest.httpMethod() == "POST" || aRequest.httpMethod() == "PUT") {
        const TStringTable& stringTable = m_Session.GetTable();
        TPtrC contTypePtr(aRequest.httpContentType().des());
        __ASSERT_DEBUG(contTypePtr.Length(), User::Leave(KErrArgument));
        HBufC8* contTypeBuf = HBufC8::NewLC(contTypePtr.Length());
        contTypeBuf->Des().Copy(contTypePtr);
        RStringF str = m_StringPool.OpenFStringL( contTypeBuf->Des() );
        CleanupClosePushL( str );
        aHeaders.SetFieldL( m_StringPool.StringF( HTTP::EContentType, stringTable ), str );
        CleanupStack::PopAndDestroy(2); // str, contTypeBuf
    }
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetCacheControlHeadersL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetCacheControlHeadersL(
    RHTTPHeaders& aHeaders,
    const WebCore::ResourceRequest& aRequest)
{
    const TStringTable& stringTable = m_Session.GetTable();
    TPtrC8 cacheStrPtr;
    TBool setPragma(EFalse);
    //
    switch(aRequest.cachePolicy()) {
        case UseProtocolCachePolicy:
            // normal load - do nothing
            return;
        case ReloadIgnoringCacheData:
            // reload - add no-cache header
            cacheStrPtr.Set(KNoCache);
            setPragma = ETrue;
            break;
        case ReturnCacheDataElseLoad:
            // back/forward or encoding change - allow stale data
            return;
        case ReturnCacheDataDontLoad:
            // results of a post - allow stale data and only use cache
            cacheStrPtr.Set(KOnlyIfCached);
            break;
        default:
            User::Leave(KErrArgument);
    }
    //
    if(cacheStrPtr.Length() > 0) {
        RStringF str = m_StringPool.OpenFStringL(cacheStrPtr);
        CleanupClosePushL(str);
        aHeaders.SetFieldL(m_StringPool.StringF( HTTP::ECacheControl, stringTable), str);
        if(setPragma) {
            // this is only for HTTP 1.0 support
            aHeaders.SetFieldL(m_StringPool.StringF(HTTP::EPragma, stringTable), str);
        }
        CleanupStack::PopAndDestroy(); // str
    }
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetRefererHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetRefererHeaderL(
    RHTTPHeaders& aHeaders,
    const WebCore::ResourceRequest& aRequest)
{
    if(m_HttpSessionMgr->refererEnabled()) {
        String refererStr = aRequest.httpReferrer();
        if(!refererStr.isNull()) {
            const TStringTable& stringTable = m_Session.GetTable();
            TPtrC refererPtr(refererStr);
            HBufC8* refererBuf = HBufC8::NewLC(refererPtr.Length());
            refererBuf->Des().Copy(refererPtr);
            RStringF str = m_StringPool.OpenFStringL(refererBuf->Des());
            CleanupClosePushL(str);
            aHeaders.SetFieldL(m_StringPool.StringF( HTTP::EReferer, stringTable ), str );
            CleanupStack::PopAndDestroy(2); // str, refererBuf
        }
    }
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetAcceptLanguageHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetAcceptLanguageHeaderL(
    RHTTPHeaders& aHeaders,
    const WebCore::ResourceRequest& aRequest)
{
    // need to put user selected Language and other installed language in the format as below:
    // en;q=1.0, es;q=0.5, fr;q=0.5
    //
    const TStringTable& stringTable = m_Session.GetTable();
    TLanguage currentUserSelectedLang(User::Language());
    //
    if(currentUserSelectedLang != m_UserSelectedLang) {
        m_UserSelectedLang = currentUserSelectedLang;
        ComposeMultipleLanguagesStringL(m_UserSelectedLang);
    }
    RStringF str = m_StringPool.OpenFStringL(*m_InstalledLangBuf);
    CleanupClosePushL(str);
    aHeaders.SetFieldL(m_StringPool.StringF(HTTP::EAcceptLanguage, stringTable), str);
    CleanupStack::PopAndDestroy(1); // str
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetAcceptCharsetHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetAcceptCharsetHeaderL(
    RHTTPHeaders& aHeaders,
    const WebCore::ResourceRequest& aRequest)
{
    const TStringTable& stringTable = m_Session.GetTable();
    //
    RStringF str = m_StringPool.OpenFStringL(*AcceptCharsetStringL());
    CleanupClosePushL(str);
    aHeaders.SetFieldL(m_StringPool.StringF(HTTP::EAcceptCharset, stringTable), str);
    CleanupStack::PopAndDestroy(1); // str
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::SetConnectionHeaderL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::SetConnectionHeaderL(
    RHTTPHeaders& aHeaders,
    const WebCore::ResourceRequest& aRequest)
{
    const TStringTable& stringTable = m_Session.GetTable();
    //
    RStringF str = m_StringPool.OpenFStringL(_L8("keep-alive"));
    CleanupClosePushL(str);
    aHeaders.SetFieldL(m_StringPool.StringF(HTTP::EConnection, stringTable), str);
    CleanupStack::PopAndDestroy(1); // str
}
// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::ComposeAcceptCharsetStringL()
// -----------------------------------------------------------------------------
HBufC8* HttpRequestHeaderManager::AcceptCharsetStringL()
{
    TUint currentUserCharset = m_HttpSessionMgr->charset();
    if(currentUserCharset == 0) {
        currentUserCharset = KCharsetUidDef;
    }
    if((m_UserCharset == currentUserCharset) && m_CharsetBuf) {
        // charset unchanged - use existing string
        return m_CharsetBuf;
    }
    //
    m_UserCharset = currentUserCharset;
    delete m_CharsetBuf;
    m_CharsetBuf = NULL;
    HBufC* tmpBuf = HBufC::NewLC(KDefBufSize);
    TPtr tmpBufPtr = tmpBuf->Des();
    // add user-selected charset
    const TText *pCharset = charsetForUid(m_UserCharset);
    if(pCharset){
       TPtrC userCharset(pCharset);
       tmpBufPtr.Append(userCharset);
    }
    // add Latin1 charset, if not same as user-selected
    if(m_UserCharset != KCharacterSetIdentifierIso88591) {
        tmpBufPtr.Append(KCommaSeperator);
        TPtrC latinCharset(charsetForUid(KCharacterSetIdentifierIso88591));
        tmpBufPtr.Append(latinCharset);
        tmpBufPtr.Append(KQualPtSeven);
    }
    // add Utf8 charset, if not same as user-selected
    if(m_UserCharset != KCharacterSetIdentifierUtf8) {
        tmpBufPtr.Append(KCommaSeperator);
        TPtrC utf8Charset(charsetForUid(KCharacterSetIdentifierUtf8));
        tmpBufPtr.Append(utf8Charset);
        tmpBufPtr.Append(KQualPtSeven);
    }
    // add * with q-factor
    tmpBufPtr.Append(KCommaSeperator);
    tmpBufPtr.Append(KStarStr);
    tmpBufPtr.Append(KQualPtSeven);
    //   
    // move string to 8-bit buffer
    m_CharsetBuf = HBufC8::NewL(tmpBufPtr.Length());
    m_CharsetBuf->Des().Copy(tmpBufPtr);
    CleanupStack::PopAndDestroy(tmpBuf);
    return m_CharsetBuf;
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::ComposeMultipleLanguagesStringL()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::ComposeMultipleLanguagesStringL(TLanguage aUserLanguge)
{
    CArrayFixFlat<TInt>* systemEpocLanguageCodes = NULL;

    User::LeaveIfError(SysLangUtil::GetInstalledLanguages(systemEpocLanguageCodes));
    CleanupStack::PushL(systemEpocLanguageCodes);
    
    // Compose the String such as "en;q=1.0, es;q=0.5, fr;q=0.5"
    RStringF  userSelectedLangString = HttpFilterCommonStringsExt::GetLocaleLanguage(m_StringPool);
    TPtrC8 userSelectedLangPtr(userSelectedLangString.DesC());
    //
    TInt xtraDataPerLang = KCommaSeperator().Length() + KLangStrLen + KQHalfValue().Length();
    TInt reqdBufSize = ((userSelectedLangPtr.Length() + xtraDataPerLang)
	                     * (systemEpocLanguageCodes->Count()));

    delete m_InstalledLangBuf;
    m_InstalledLangBuf = NULL;
    m_InstalledLangBuf = HBufC8::NewL(reqdBufSize);
    TPtr8 langBufPtr(m_InstalledLangBuf->Des());
    langBufPtr.Copy(userSelectedLangPtr);

    if(systemEpocLanguageCodes->Count() > 1) {
        // If we have more than one one language installed, do...
        // ;q=1.0
        langBufPtr.Append(KQWholeValue);
        // Append the other installed languages
        for (TInt i = 0; i < systemEpocLanguageCodes->Count(); ++i) {
            TLanguage language((TLanguage)systemEpocLanguageCodes->At(i));
            //
            if( language != aUserLanguge ) {
                RStringF extraLang = HttpFilterCommonStringsExt::GetLanguageString( m_StringPool, language );
                TInt newLength(langBufPtr.Length() + KCommaSeperator().Length() +
                    extraLang.DesC().Length() + KQHalfValue().Length());
                // how much too short is our buffer?
                TInt addLen = langBufPtr.MaxLength() - newLength;
                if(addLen > 0) {
                    // resize buffer
                    m_InstalledLangBuf = m_InstalledLangBuf->ReAllocL(newLength + addLen);
                    langBufPtr.Set( m_InstalledLangBuf->Des() );
                }
                // ,
                langBufPtr.Append( KCommaSeperator );
                // extra language
                langBufPtr.Append( extraLang.DesC() );
                // ;q=0.5
                langBufPtr.Append( KQHalfValue );
            }
        }
    }
    // else: Don't append q value if we only have one language installed
    CleanupStack::PopAndDestroy(systemEpocLanguageCodes);
}

//-----------------------------------------------------------------------------
// HttpRequestHeaderManager::ReadMimeTypesL
// Query the supported MIME types from the central repository
//-----------------------------------------------------------------------------
void HttpRequestHeaderManager::ReadCenRepMimeTypesL(
    CRepository& aRepository)
{
	HBufC* mimeTemp = HBufC::NewLC(NCentralRepositoryConstants::KMaxBinaryLength);
    TPtr mimeTempPtr = mimeTemp->Des();
    //
    User::LeaveIfError(aRepository.Get(KBrowserNGMimeTypes, mimeTempPtr));
    m_AcceptMimeTypesCenRep = HBufC8::NewL(mimeTempPtr.Length());
    m_AcceptMimeTypesCenRep->Des().Copy(mimeTempPtr);
    CleanupStack::PopAndDestroy(mimeTemp);
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::TopLevelAcceptStringL()
// -----------------------------------------------------------------------------
HBufC8* HttpRequestHeaderManager::TopLevelAcceptStringL(
    const WebCore::ResourceRequest& aRequest)
{
    // we construct this string only once, during first top-level load of the
    // browsing session.  this assumes that the string for MIME types set by 
    // WebCore for top-level requests is constant
    if(!m_AcceptHdrMimesTopLevel) {
        TPtrC reqMimeTypes(aRequest.httpAccept().des());
        TInt reqMimeTypesLen(reqMimeTypes.Length()); 
        TInt headerMimeTypesLen = m_AcceptMimeTypesCenRep->Length() + reqMimeTypesLen +
                                  KStarSlashStar().Length() + KQHalfValue().Length() + 1;
        m_AcceptHdrMimesTopLevel = HBufC8::NewL(headerMimeTypesLen);
        TPtr8 acceptHdrMimesPtr = m_AcceptHdrMimesTopLevel->Des();
        //
        if (reqMimeTypesLen > 0) {
            // if request MIME types string contains trailing * / * then we need to 
            // insert the cenrep MIME types before it
            TInt starSlashStarPos = reqMimeTypes.Find(KStarSlashStar);
            if(starSlashStarPos == KErrNotFound) {
                acceptHdrMimesPtr.Copy(reqMimeTypes);
                acceptHdrMimesPtr.Append(KCommaSeperator);
            } else {
                acceptHdrMimesPtr.Copy(reqMimeTypes.Left(starSlashStarPos));
            }
        }
        acceptHdrMimesPtr.Append(*m_AcceptMimeTypesCenRep);
        delete m_AcceptMimeTypesCenRep; // we don't need this anymore now that
        m_AcceptMimeTypesCenRep = NULL; // we have completely constructed string
        acceptHdrMimesPtr.Append(KCommaSeperator);    
        acceptHdrMimesPtr.Append(KStarSlashStar);
        acceptHdrMimesPtr.Append(KQHalfValue);

    }
    return m_AcceptHdrMimesTopLevel;
}

// -----------------------------------------------------------------------------
// HttpRequestHeaderManager::AddXMLHttpHeaders()
// -----------------------------------------------------------------------------
void HttpRequestHeaderManager::AddXMLHttpHeadersL(RHTTPHeaders& aHeaders, 
    const WebCore::ResourceRequest& aRequest )
{
    const TStringTable& stringTable = m_Session.GetTable();
    HTTPHeaderMap headerFields = aRequest.httpHeaderFields();
    HTTPHeaderMap::const_iterator end = headerFields.end();
    for (HTTPHeaderMap::const_iterator it = headerFields.begin(); it != end; ++it)
        {
        WebCore::String headerName = it->first;
        WebCore::String headerValue = it->second;

        TPtrC strNamePtr(headerName.des());
        HBufC8* strNameBuf = HBufC8::NewLC(strNamePtr.Length());
        strNameBuf->Des().Copy(strNamePtr);
        RStringF strName = m_StringPool.OpenFStringL( strNameBuf->Des() );
        CleanupClosePushL(strName );

        TPtrC strValuePtr(headerValue.des());
        HBufC8* strValueBuf = HBufC8::NewLC(strValuePtr.Length());
        strValueBuf->Des().Copy(strValuePtr);
        RStringF strValue = m_StringPool.OpenFStringL( strValueBuf->Des() );
        CleanupClosePushL(strValue );

        aHeaders.RemoveField(strName);

        if (strName == m_StringPool.StringF(HTTP::EIfModifiedSince, RHTTPSession::GetTable()) ||
            strName == m_StringPool.StringF(HTTP::EIfUnmodifiedSince, RHTTPSession::GetTable()))
            { 
            TPtrC8 strval(*strValueBuf);
            TInternetDate date;
            date.SetDateL(strval);
            TDateTime modifyTime(date.DateTime());
            aHeaders.SetFieldL(strName, modifyTime);
            }
        else
            {
            aHeaders.SetFieldL(strName, strValue);    
            }
        CleanupStack::PopAndDestroy(4);
        }
}
//  End of File
