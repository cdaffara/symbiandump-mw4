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
* Description:  A misc. char-encoding related utilities.
*
*/


#ifndef XML_ENCODING_H
#define XML_ENCODING_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCnvCharacterSetConverter;

// CLASS DECLARATION


/**
*  A misc. char-encoding related utilities.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CXmlEncoding: public CBase
    {
    private: // Data types
        struct SupportedEncodings
            {
            TDesC8*  charEncoding;
            TUint    uid;
            };


    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXmlEncoding* NewL();
        
        /**
        * Destructor.
        */        
        virtual ~CXmlEncoding();
        
        
    public: // New methods
        /**
        * Resolves the given char-encoding into its uid.
        *
        * @since 3.0
        * @param aCharEncoding The encoding.
        * @param aUid The resulting uid for the given encoding.
        * @return ETrue if the encoding was resolved.
        */
        TBool ResolveCharEncodingL(const TDesC8& aCharEncoding, TUint& aUid) const;

        /**
        * Determine the char-encoding.
        *
        * @since 3.0
        * @param aBuffer The native buffer.
        * @param aCharSet The char-set provided by the orignal source
        * @param aEncoding The resulting encoding.
        * @return ETrue if a encoding was determined.
        */
        IMPORT_C TBool DetermineCharEncodingL(const TDesC8& aBuffer, 
                const TDesC& aCharSet, TUint& aEncoding) const;

        /**
        * Determine the char-encoding from the BOM.
        *
        * @since 3.0
        * @param aBuffer The native buffer.
        * @param aEncoding The resulting encoding.
        * @return ETrue if a encoding was determined.
        */
        TBool DetermineCharEncodingFromBom(const TDesC8& aBuffer, TUint& aEncoding) const;

        /**
        * Determine the char-encoding from the char-encoding in the xml-prolog.
        *
        * @since 3.1
        * @param aBuffer The native buffer.
        * @param aEncoding The resulting encoding.
        * @return ETrue if a encoding was determined.
        */
        TBool DetermineCharEncodingFromXmlProlog(const TDesC8& aBuffer, TUint& aEncoding) const;

        /**
        * Erase the char-encoding attribute (if any) from the xml-prolog.
        *
        * @since 3.0
        * @param aUtf8Buffer The utf8 buffer.
        * @return void
        */
        void StripCharEncoding(HBufC8& aUtf8Buffer) const;

        /**
        * Erase the char-encoding attribute (if any) from the xml-prolog.
        *
        * @since 3.0
        * @param aUtf8Buffer The utf8 buffer.
        * @return void
        */
        void StripCharEncoding(HBufC16& aUcs2Buffer) const;

        /**
        * Converts the given native buffer into utf8.
        *
        * @since 3.0
        * @param aEncoding The encoding.
        * @param aBuffer The native buffer.
        * @return The converted buffer.
        */
        IMPORT_C HBufC8* ConvertToUtf8L(TUint aEncoding, const TDesC8& aBuffer) const;

        /**
        * Converts the given native buffer into ucs2.
        *
        * @since 3.0
        * @param aEncoding The encoding.
        * @param aBuffer The native buffer.
        * @return The converted buffer.
        */
        IMPORT_C HBufC16* ConvertToUcs2L(TUint aEncoding, const TDesC8& aBuffer) const;
        

    private:
        /**
        * C++ default constructor.
        */
        CXmlEncoding();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Converts the given ucs2 buffer into utf8.
        *
        * @since 3.0
        * @param aUsc2Buffer The ucs2 buffer.
        * @return The converted buffer.
        */
        HBufC8* ConvertToUtf8L(HBufC16& aUsc2Buffer) const;
        
        /**
        * Finds the location of the encoding attribute in the xml-prolog.
        *
        * @since 3.0
        * @param aBuffer The native buffer.
        * @param aBegin The beginning of the attribute.
        * @param aEnd  The end of the attribute.
        * @param aValueBegin The beginning of the encoding's value.
        * @param aValueEnd The end of the encoding's value
        * @return ETrue if the encoding was found.
        */
        TBool CXmlEncoding::FindEncoding(const TDesC8& aBuffer, 
                TInt& aBegin, TInt& aEnd, TInt& aValueBegin, TInt& aValueEnd) const;

        /**
        * Finds the location of the encoding attribute in the xml-prolog.
        *
        * @since 3.0
        * @param aBuffer The ucs2 buffer.
        * @param aBegin The beginning of the attribute.
        * @param aEnd  The end of the attribute.
        * @param aValueBegin The beginning of the encoding's value.
        * @param aValueEnd The end of the encoding's value
        * @return ETrue if the encoding was found.
        */
        TBool CXmlEncoding::FindEncoding(const TDesC& aBuffer, 
                TInt& aBegin, TInt& aEnd, TInt& aValueBegin, TInt& aValueEnd) const;


    private:  // Data
        RFs                         iRfs;
        CCnvCharacterSetConverter*  iConverter;

        RArray<SupportedEncodings>  iCharEncodings;
    };
        
#endif      // XML_ENCODING_H
            
// End of File
