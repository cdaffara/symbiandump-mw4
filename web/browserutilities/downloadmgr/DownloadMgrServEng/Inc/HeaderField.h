/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef CHEADERFIELD_H
#define CHEADERFIELD_H

//  INCLUDES
#include <e32base.h>
#include <HttpDownloadMgrCommon.h>
#include <http.h>

// CONSTANTS
const TInt KResponseHeaderConvTable[][2] = {
    {EDlAttrResponseCharSet, HTTP::ECharset},
    {EDlAttrResponseAge, HTTP::EAge},
    {EDlAttrResponseETag, HTTP::EETag},
    {EDlAttrResponseLocation, HTTP::ELocation},
    {EDlAttrResponseRetryAfter, HTTP::ERetryAfter},
    {EDlAttrResponseServer, HTTP::EServer},
    {EDlAttrResponseVary, HTTP::EVary },
    {0,0},
    };

const TInt KRequestHeaderConvTable[][2] = {
    {EDlAttrRequestAccept, HTTP::EAccept },
    {EDlAttrRequestAcceptCharSet, HTTP::EAcceptCharset },
    {EDlAttrRequestAcceptLanguage, HTTP::EAcceptLanguage },
    {EDlAttrRequestExpect, HTTP::EExpect},
    {EDlAttrRequestFrom, HTTP::EFrom },
    {EDlAttrRequestHost, HTTP::EHost },
    {EDlAttrRequestMaxForwards, HTTP::EMaxForwards },
    {EDlAttrRequestPragma, HTTP::EPragma },
    {EDlAttrRequestReferer, HTTP::EReferer },
    {EDlAttrRequestUserAgent, HTTP::EUserAgent },
    {EDlAttrRequestVary, HTTP::EVary },
    {0,0},
    };

const TInt KEntityHeaderConvTable[][2] = {
    {EDlAttrEntityAllow, HTTP::EAllow },
    {EDlAttrEntityContentEncoding, HTTP::EContentEncoding },
    {EDlAttrEntityContentLanguage, HTTP::EContentLanguage },
    {EDlAttrEntityContentLocation, HTTP::EContentLocation },
    {EDlAttrEntityExpires, HTTP::EExpires },
    {EDlAttrEntityLastModified, HTTP::ELastModified },
    {0,0},
    };

const TInt KGeneralHeaderConvTable[][2] = {
    {EDlAttrGeneralCacheControl, HTTP::ECacheControl },
    {EDlAttrGeneralDate, HTTP::EDate },
    {EDlAttrGeneralPragma, HTTP::EPragma },
    {EDlAttrGeneralVia, HTTP::EVia },
    {EDlAttrGeneralWarning, HTTP::EWarning },
    {0,0},
    };

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
void Trim( TPtrC8& aTrim );

// FORWARD DECLARATIONS
class RFile;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/
NONSHARABLE_CLASS( CHeaderField ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHeaderField* NewL( const TDesC8* aFieldName = NULL,
                                   const TDesC8* aFieldRawData = NULL,
                                   TInt aFieldIndex = KErrNotFound );
        
        /**
        * Destructor.
        */
        virtual ~CHeaderField();

//        static TDesC8   FieldNameByAttr( TInt aFieldAttr );
//        static THttpDownloadMgrAttrib   FieldAttrByName( RStringPool& aStrPool, TDesC8& aFieldName );

    public: // New functions
        
        /**
        * Returns pointer to field name.
        * @since Series 60 v2.8
        * @return pointer to field name.
        */
        inline HBufC8* FieldName() const;

        /**
        * Returns pointer to raw data of the field
        * @since Series 60 v2.8
        * @return pointer to raw data of the field
        */
        inline HBufC8* FieldRawData() const;

        /**
        * Returns the 
        */
        inline TInt FieldIndex() const;

        /**
        * Returns the header value converted to TInt
        * @return raw header value converted to TInt
        */
        TInt Int() const;

        /**
        * Loads header field from file.
        * @since Series 60 v2.8
        * @param aInFile file to read header field from
        * @return none. Leaves on error.
        */
        void LoadHeaderInfoL( RFile& aInFile );

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHeaderField( TInt aFieldIndex );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8* aFieldname,
                         const TDesC8* aFieldRawData );

        // Prohibit copy constructor if not deriving from CBase.
        // CHeaderField( const CHeaderField& );
        // Prohibit assigment operator if not deriving from CBase.
        // CHeaderField& operator=( const CHeaderField& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        TInt    iFieldIndex;    // Index value of the given field in the 
                                // related header id array. (e.g. KResponseHeaderConvTable )
        HBufC8* iFieldName;
        HBufC8* iFieldRawData;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CHEADERFIELD_H   

inline TInt CHeaderField::FieldIndex() const
    {
    return iFieldIndex;
    }

inline HBufC8* CHeaderField::FieldName() const
    {
    return iFieldName;
    }

inline HBufC8* CHeaderField::FieldRawData() const
    {
    return iFieldRawData;
    }

// End of File
