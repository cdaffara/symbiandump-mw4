/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cookie access component for script
*
*/



// INCLUDES
#include "CookieHandler.h"
#include <cookiemanagerclient.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KExpires, "expires");
_LIT(KPath, "path");
_LIT(KDomain, "domain");

_LIT8(KCookieSeperator, "; "); // ';' and a space
_LIT8(KCookieNameValueSeperator, "="); // ';' part seperator


// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

TPtrC stripWhiteSpace(const TDesC& aString)
{
    const TUint16* s = aString.Ptr();
    const TUint16* e = aString.Ptr()+aString.Length()-1;
    while (s<e && TChar(*s).IsSpace()) {
        s++;
    }
    while (s<e && TChar(*e).IsSpace()) {
        e--;
    }
    return TPtrC(s,e-s+1);
}


// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================
//
// Creates a new object.
// @return The constructed session.
//
CookieHandler* CookieHandler::init()
{
    CookieHandler* self = new CookieHandler;
    if (self) {
        TRAPD(err, self->constructL());
        if (err) {
            delete self;
            self = NULL;
        }
    }
    return self;
}

TText* CookieHandler::cookiesForUrl(const TDesC8& aUrl, TInt32& aLen)
{
    TText* text = NULL;
    TRAP_IGNORE(text = cookiesForUrlL(aUrl, aLen));
    return text;
}

TText* CookieHandler::cookiesForUrlL(const TDesC8& aUrl, TInt32& aLen)
{
    m_getCookies.ResetAndDestroy(); // frees all prev memory allocated
    RCookieManager& cookieManager = *m_cookieManager;
    TBool cookie2Reqd;
    
    TUriParser8 uriParser;
    User::LeaveIfError( uriParser.Parse( aUrl ));
    // possible leave
    cookieManager.GetCookiesL(uriParser.UriDes(), m_getCookies, cookie2Reqd);
    
    TText* cookieString = NULL;
    
    HBufC8* cookieString8 = NULL;
    TInt totalCookieSize = 0;
    
    cookiesToStringL(NULL, totalCookieSize);
    if (totalCookieSize > 0) {
        cookieString8 = HBufC8::NewLC(totalCookieSize) ;
        cookiesToStringL(cookieString8 , totalCookieSize);
        // convert it to 16 bit
        cookieString = (TText*)User::AllocL(cookieString8->Des().Length() * sizeof(TText));
        TPtr(cookieString, cookieString8->Des().Length()).Copy(cookieString8->Des());
        CleanupStack::PopAndDestroy(); //cookieString8
    }
    m_getCookies.ResetAndDestroy(); // frees all prev memory allocated
    aLen = totalCookieSize;
    return cookieString ;
}

//
// adds cookie ,
// @param aUrl ,document.url
// @param aCookieData,cookie buffer
//
void CookieHandler::addCookieL(const TDesC& aCookieData, const TDesC8& aUrl, const TDesC8& /*aPolicyBaseURL*/)
{
    RStringF name;
    RStringF nameValue;
    RStringF expiresAttrib;
    RStringF pathAttrib;
    RStringF domainAttrib;
    RStringF versionAttrib;
    
    TInt popAndDestroyCount = 0;
    
    CookieRecord cookieRecord;
    
    RCookieManager& cookieManager = *m_cookieManager;
    
    // parse the cookie buffer and cookieRecord is assign with cookie field and value
    parseCookieFieldsL( aCookieData, cookieRecord);
    
    TUriParser8 uriParser;
    // possible leave
    User::LeaveIfError( uriParser.Parse( aUrl ) );
    
    // if the domain is not specified then extract the domain name from the Url
    if(!cookieRecord.m_domainName.Length()) {
        const TDesC8& domainName = uriParser.Extract(EUriHost);
        cookieRecord.m_domainName.Set(TPtrC(asciiToUnicodeLC(domainName)->Des()));
        popAndDestroyCount ++;
    }
    
    // if the domain is not specified then extract the domain name from the Url
    if(!cookieRecord.m_pathName.Length()) {
        const TDesC8& pathName= uriParser.Extract(EUriPath);
        cookieRecord.m_pathName.Set(TPtrC(asciiToUnicodeLC(pathName)->Des()));
        popAndDestroyCount ++;
    }
    
    // initialize the strings , to be stored in string pool
    // note the function 6 items on the cleanupstack , so increment popAndDestroyCount by 6
    initAttributesForLongCookieLC(name, nameValue, expiresAttrib, pathAttrib,
        domainAttrib, versionAttrib,
        cookieRecord);
    popAndDestroyCount +=6;
    // create a cookie
    CCookie* cookie = CCookie::NewL( (*m_stringPool) );
    
    CleanupStack::PushL( cookie );
    popAndDestroyCount ++;
    
    // set the attributes on the cookie
    User::LeaveIfError( cookie->SetAttribute( CCookie::EName, name) );
    User::LeaveIfError( cookie->SetAttribute( CCookie::EValue, nameValue) );
    User::LeaveIfError( cookie->SetAttribute( CCookie::EDomain, domainAttrib ) );
    User::LeaveIfError( cookie->SetAttribute( CCookie::EPath, pathAttrib ) );
    
    if(cookieRecord.m_expires.Length()) {
        User::LeaveIfError( cookie->SetAttribute( CCookie::EExpires, expiresAttrib ) );
    }
    if(cookieRecord.m_secure.Length()) {
        User::LeaveIfError( cookie->SetAttribute( CCookie::EVersion, versionAttrib ) );
    }
    
    cookieManager.StoreCookie(*cookie, uriParser);
    CleanupStack::PopAndDestroy(popAndDestroyCount); // RCookieManager.Close(), InitAttributesForLongCookieLC(6),cookie,unicodeToAsciiLC,asciiToUnicodeLC(2)
}

void CookieHandler::addCookie(const TDesC& aCookieData, const TDesC8& aUrl, const TDesC8& aPolicyBaseURL)
{
    TRAP_IGNORE(addCookieL(aCookieData, aUrl, aPolicyBaseURL));
}

//
// Destructor.
//
CookieHandler::~CookieHandler()
{
    // frees all memory allocated ,
    // including the objects whose
    // pointers are contained by the array
    m_getCookies.ResetAndDestroy();
    
    m_getCookies.Close();
    if(m_cookieManager) {
        m_cookieManager->Close();
        delete m_cookieManager;
    }
    
    if(m_stringPool) {
        m_stringPool->Close();
        delete m_stringPool;
    }
}

//
// Constructor.
//
CookieHandler::CookieHandler()
{}

//
// ConstructL.
// @return None.
//
void CookieHandler::constructL( )
{
    m_stringPool = new (ELeave)RStringPool;
    m_stringPool->OpenL();
    m_cookieManager = new (ELeave) RCookieManager( *m_stringPool );
    User::LeaveIfError( m_cookieManager->Connect() );
}


//
// Auxillary function to convert 16 bit data to 8 bit.
// @return HBufC8 - 8 bit data.
//
HBufC8* CookieHandler::unicodeToAsciiLC( const TDesC16& aString ) const
{
    HBufC8* buf = HBufC8::NewLC( aString.Length()  + 1); // +1 for zero terminate
    buf->Des().Copy( aString );
    TPtr8 bufDes8 = buf->Des();
    bufDes8.ZeroTerminate();
    return buf;
}

//
// Auxillary function to convert 8 bit data to 16 bit.
// @return HBufC16 - 16 bit data.
//
HBufC16* CookieHandler::asciiToUnicodeLC( const TDesC8& aString ) const
{
    HBufC16* buf = HBufC16::NewLC( aString.Length()  + 1); // +1 for zero terminate
    buf->Des().Copy( aString );
    TPtr16 bufDes16 = buf->Des();
    bufDes16.ZeroTerminate();
    return buf;
}

//
// @param   the strings that are to be stored in stringpool
//                                     aName,
//                                     aNameValue,
//                                     aExpires,
//                                     aPath,
//                                     aDomain,
//                                     aVersion

//   @param const CookieRecord& aCookieRecord  , parsed cookie fields

// Initializes the string with values from CookieRecord structure.
//
void CookieHandler::initAttributesForLongCookieLC( RStringF& aName,
                                                  RStringF& aNameValue,
                                                  RStringF& aExpires,
                                                  RStringF& aPath,
                                                  RStringF& aDomain,
                                                  RStringF& aVersion,
                                                  const CookieRecord& aCookieRecord) const
{
    
    // Setting name attribute
    aName = m_stringPool->OpenFStringL(unicodeToAsciiLC(aCookieRecord.m_name)->Des());
    CleanupStack::PopAndDestroy(); //unicodeToAsciiLC
    CleanupClosePushL<RStringF>( aName);
    
    // Setting name attribute
    aNameValue = m_stringPool->OpenFStringL(unicodeToAsciiLC(aCookieRecord.m_nameValue)->Des());
    CleanupStack::PopAndDestroy(); //unicodeToAsciiLC
    CleanupClosePushL<RStringF>( aNameValue);
    
    aExpires = m_stringPool->OpenFStringL( unicodeToAsciiLC(aCookieRecord.m_expires)->Des());
    CleanupStack::PopAndDestroy(); //unicodeToAsciiLC
    CleanupClosePushL<RStringF>( aExpires);
    // Setting Path attribute
    aPath = m_stringPool->OpenFStringL( unicodeToAsciiLC(aCookieRecord.m_pathName)->Des());
    CleanupStack::PopAndDestroy(); //unicodeToAsciiLC
    CleanupClosePushL<RStringF>( aPath );
    
    // Setting Domain attribute
    aDomain = m_stringPool->OpenFStringL( unicodeToAsciiLC(aCookieRecord.m_domainName)->Des() );
    CleanupStack::PopAndDestroy(); //unicodeToAsciiLC
    CleanupClosePushL<RStringF>( aDomain );
    
    // Setting Domain attribute
    aVersion = m_stringPool->OpenFStringL( _L8("1"));
    CleanupClosePushL<RStringF>( aVersion );
}

//
// Auxillary function to parse the cookie buffer and to extract the fields .
// @param aCookieName cookie field to be retrived .
// @param aCookieString cookie buffer.
// @param aEndIndex cosumed bytes from the buffer.
// @return TPtrC - descriptor to the field value.
//
TPtrC CookieHandler::getCookieFieldValue(const TDesC& aCookieFieldName,
                                         const TPtrC& aCookieString ,
                                         TInt& aEndIndex) const
{
    TInt endIndex = aCookieString.Locate(';');
    TPtrC cookieFieldval(NULL, 0);
    // access the name of the cookie
    TPtrC cookieField(aCookieString.Ptr(),
        endIndex != KErrNotFound ? endIndex : aCookieString.Length());
    
    // extract the field value
    TInt assingmentIndex = cookieField.Locate('=');
    if (assingmentIndex != KErrNotFound ) {
        TPtrC tempCookieFieldName(stripWhiteSpace(TPtrC(cookieField.Ptr(), assingmentIndex)));
        // if the cookie field is found
        if (!aCookieFieldName.CompareF( tempCookieFieldName )) {
            if (endIndex != KErrNotFound) {
                // skip ';'
                aEndIndex += endIndex + 1 ;
            }
            else {
                // consume the rest of the characters
                aEndIndex += cookieField.Length();
            }
            // set the cookie value pointer
            cookieFieldval.Set(cookieField.Ptr() + assingmentIndex + 1 , cookieField.Length() - assingmentIndex - 1); // cookie field value
        }
    }
    return cookieFieldval;
}

//
// Auxillary function to parse and assigns the cookie field structure.
// @param aCookieString cookie buffer.
// @param aCookieFeilds cookie fields structure.
//

void CookieHandler::parseCookieFieldsL( const TPtrC& aCookieString,
                                       CookieRecord& aCookieRecord) const
{
    TInt endIndex = 0;
    TInt cookieLength = aCookieString.Length();
    if( cookieLength ) {
        // get the cookie the name
        TInt assingmentIndex = aCookieString.Locate('=');
        if (assingmentIndex == KErrNotFound) {
            // name is the entire cookie string. value is empty.
            assingmentIndex = cookieLength - 1;
        }
        
        aCookieRecord.m_name.Set( stripWhiteSpace(TPtrC(aCookieString.Ptr(), assingmentIndex )) );
        aCookieRecord.m_nameValue.Set(getCookieFieldValue(aCookieRecord.m_name, TPtrC(aCookieString.Ptr() + endIndex , cookieLength - endIndex), endIndex));
        
        if (aCookieRecord.m_nameValue.Length() == 0) {
            // both firefox and ie allow empty value.
            
            // the name of the cookie should be available , else the cookie string is invalid
            // User::Leave(KErrArgument) ;
        }
        aCookieRecord.m_expires.Set(getCookieFieldValue(KExpires, TPtrC(aCookieString.Ptr() + endIndex , cookieLength - endIndex), endIndex));
        aCookieRecord.m_pathName.Set(getCookieFieldValue(KPath, TPtrC(aCookieString.Ptr() + endIndex , cookieLength - endIndex), endIndex));
        aCookieRecord.m_domainName.Set(getCookieFieldValue(KDomain, TPtrC(aCookieString.Ptr() + endIndex , cookieLength - endIndex), endIndex));
    }
}

//
// Auxillary function to convert the cookie class to string . If the buffer is NULL
// then the value is not copied
// @param aCookieBuffer cookie buffer.to append the cooke  field strings
// @param aCookiesSize total size of the cookie string
//
void CookieHandler::cookiesToStringL(HBufC8* aCookieBuffer, TInt& aCookiesSize)
{
    aCookiesSize = 0;
    for (TInt cookieIndex = 0; cookieIndex < m_getCookies.Count(); cookieIndex++) {
        CCookie* cookie = m_getCookies[cookieIndex];
        // we are interested only in the name of the cookie and its value attr
        // append the name and value pairs on to the descriptor
        THTTPHdrVal attributeName;
        THTTPHdrVal attributeNameVal;
        // if the cookie name and value are found then append the cookie else ignore
        if (cookie->Attribute(CCookie::EName, attributeName) != KErrNotFound &&
            cookie->Attribute(CCookie::EValue, attributeNameVal) != KErrNotFound) {
            TDesC8 name = attributeName.Str().DesC();
            TDesC8 value = attributeNameVal.Str().DesC();
            
            aCookiesSize += name.Length() + value.Length();
            // if the cookie buffer is not NULL then append to the buffer
            if (aCookieBuffer) {
                aCookieBuffer->Des().Append(attributeName.Str().DesC());
                aCookieBuffer->Des().Append(KCookieNameValueSeperator);
                aCookieBuffer->Des().Append(attributeNameVal.Str().DesC());
                aCookieBuffer->Des().Append(KCookieSeperator);
            }
        }
    }
    // cookies delimiter
    aCookiesSize += (KCookieSeperator().Length() + KCookieNameValueSeperator().Length()) * m_getCookies.Count();
}
