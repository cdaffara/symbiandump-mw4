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
* Description:  class definition for accessing cookies from script
*
*/


#ifndef COOKIE_HANDLER_H
#define COOKIE_HANDLER_H

// System includes
#include <e32base.h>

// CONSTANTS

const TUint8 KMaxNumOfTypes = 30;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RStringPool;
class CCookie;
class RStringF;
class RCookieManager;
// CLASS DECLARATION

struct CookieRecord
    {
    CookieRecord():
                  m_name(KNullDesC),m_nameValue(KNullDesC),m_expires(KNullDesC),
                  m_pathName(KNullDesC),m_domainName(KNullDesC),m_secure(KNullDesC) {}
    TPtrC m_name;
    TPtrC m_nameValue;
    TPtrC m_expires;
    TPtrC m_pathName;
    TPtrC m_domainName;
    TPtrC m_secure;
    };
/**
* This class encapsulates a cookie handler functions to be accessed from
* ecma script, The following cookie fields are supported.
*  "cookieName=cookieData;
*  [;expires=timeInGmtString]
*  [;path=pathName]
*  [;domain=domainName]
*  [;secure]"
*/
class CookieHandler
    {
    public :    // construction, destruction
        /**
        * Creates a new object.
        * @return The constructed session.
        */
        static CookieHandler* init();

        /**
        * adds cookie ,
        * @param aUrl ,document.url
        * @param aCookieData,cookie buffer
        */
        void addCookie(const TDesC& aCookieData, const TDesC8& aUrl, const TDesC8& aPolicyBaseURL);

        /**
        * retrives all the cookies name and value pairs delimited by ';' and ' '
        * @param aUrl ,document.url
        * @return TText* cookies flattened on to the buffer.
        */
        TText* cookiesForUrl(const TDesC8& aUrl, TInt32& aLen);

        /**
        * Destructor.
        */
        virtual ~CookieHandler();

    private :    // constructors
        /**
        * Constructor.
        */
        CookieHandler( );

        /**
        * ConstructL.
        * @return None.
        */
        void constructL( );

        void addCookieL(const TDesC& aCookieData, const TDesC8& aUrl, const TDesC8& aPolicyBaseURL);
        TText* cookiesForUrlL(const TDesC8& aUrl, TInt32& aLen);

        /**
        * Auxillary function to convert 16 bit data to 8 bit.
        * @return HBufC8 - 8 bit data.
        */
        HBufC8* unicodeToAsciiLC( const TDesC16& aString ) const;

        /**
        * Auxillary function to convert 8 bit data to 16 bit.
        * @return HBufC16 - 16 bit data.
        */
        HBufC16* asciiToUnicodeLC( const TDesC8& aString ) const;

        /**
        * @param   the strings that are to be stored in stringpool
                                            RStringF& aName,
                                            RStringF& aNameValue,
                                            RStringF& aExpires,
                                            RStringF& aPath,
                                            RStringF& aDomain,
                                            RStringF& aVersion

          @param const CookieRecord& aCookieRecord  , parsed cookie fields

        * Initializes the string with values from CookieRecord structure.
        */
        void initAttributesForLongCookieLC( RStringF& aName,
                                            RStringF& aNameValue,
                                            RStringF& aExpires,
                                            RStringF& aPath,
                                            RStringF& aDomain,
                                            RStringF& aVersion,
                                            const CookieRecord& aCookieRecord) const;
        /**
        * Auxillary function to convert the cookie class to string . If the buffer is NULL
        * then the value is not copied
        * @param aCookieBuffer cookie buffer.to append the cooke  field strings
        * @param aCookiesSize total size of the cookie string
        */
        void cookiesToStringL(HBufC8* aCookieBuffer, TInt& aCookiesSize);

        /**
        * Auxillary function to parse the cookie buffer and to extract the fields .
        * @param aCookieField cookie field to be retrived .
        * @param aCookieString cookie buffer.
        * @param aEndIndex cosumed bytes from the buffer.
        * @return TPtrC - descriptor to the field value.
        */
        TPtrC getCookieFieldValue(const TDesC& aCookieField,
                                  const TPtrC& aCookieString ,
                                  TInt& aEndIndex) const ;

        /**
        * Auxillary function to parse and assigns the cookie field structure.
        * @param aCookieString cookie buffer.
        * @param aCookieRecord cookie fields structure.
        */
        void parseCookieFieldsL( const TPtrC& aCookieString,
                                 CookieRecord& aCookieRecord) const;
    private :    // data members
        RStringPool* m_stringPool ;// <<has
        RPointerArray<CCookie> m_getCookies;// <<has
        RCookieManager* m_cookieManager; // NOTE: should not use pointers to r-class, but RCookieManager
                    // class doesn't provide a default constructor, too bad :(
    };

#endif
