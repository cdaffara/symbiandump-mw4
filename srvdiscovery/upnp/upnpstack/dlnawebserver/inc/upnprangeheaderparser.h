/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpRangeHeaderParser is a class used to range header prsing
*
*/


#ifndef C_CUPNPRANGEHEADERPARSER_H
#define C_CUPNPRANGEHEADERPARSER_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>
#include "upnpcons.h"
#include "upnphttpservertransaction.h"

class CUpnpHttpMessage;

static const TInt KIntegerOccurance = 3;
static const TInt KSlashSpaceAndMinusSize = 3;
static const TInt KMaxIntegerInContentLength = 2;


// CLASS DECLARATION

NONSHARABLE_CLASS (CUpnpRangeHeaderParser) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aHeaderBuffer should be leave safe
        */
        static CUpnpRangeHeaderParser* NewL( CUpnpHttpServerTransaction& aTransaction,
                                              TInt& aStartPos, TInt& aEndPos );
        static CUpnpRangeHeaderParser* NewLC( CUpnpHttpServerTransaction& aTransaction,
                                                       TInt& aStartPos, TInt& aEndPos );
        
        /**
        * Destructor.
        */
        virtual ~CUpnpRangeHeaderParser();

    public: // New functions

        /**
        * Returns parsing status as HTTP error code (e.g. EHttpPartialContent, -EHttpBadRequest) if Range header exist or
        * EHttpOk if doesn't
        * If request is corrected and Range header exist all references passed into construction phase
        * gots new value
        */        
        TInt ParseRangeL( CUpnpHttpMessage* aMsg, TInt aFileSize );
 
    private:

        /**
        * C++ default constructor.
        */

        CUpnpRangeHeaderParser( CUpnpHttpServerTransaction &aTransaction,
                                TInt& aStartPos, TInt& aEndPos );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Checks syntax of Range header
        * EHttpPartialContent if Range header exists and syntax is proper 
        * EHttpOk if Range header doesn't exist
        * -EHttpBadRequest if Range syntax is improper
        */        
        TInt CheckRangeHeaderSyntax( CUpnpHttpMessage* aMsg );
        
        /**
        * Checks Range value
        * EHttpPartialContent if values there are proper
        * - HTTP error code if not
        */        
        TInt CheckRangeValues( TInt aFileSize );

        /**
        * Parses HTTP Range header, according to rules described in DLNA specyfication
        *  and if header is correct, it returns first and last byte positions. If header
        *  is not correct, those values are random and should not be used.
        * @param aRangeHeader Range header.
        * @param aStartPos reference which will be filled with first byte position.
        * @param aEndPos reference which will be filled with last byte position or
        *  with KErrNotFound if such position was not specified in the header which is correct.
        * @return ETrue if header syntax is correct, EFalse if not.
        **/
        static TBool ParseRangeHeader( TDesC8& aRangeHeader, TInt& aStartPos, TInt& aEndPos );
        
        /**
        * Appends Content-Length header to response
        */        
        void AppendContentLengthToResponseL( TInt aFileSize, TInt rangeStatus );
        
           
        /**
        * Appends Bytes header to response
        */        
        void AppendBytesToResponseL( TInt aFileSize );
               
               
        /**
        * Creates response for 206 Partial Content
        */        
        void CreatePartialContentResponseL( 
            TInt aFileSize, TInt aHttpCode );
        
        /**
        * Creates response for 204 No Content
        */        
        void CreateNoContentResponseL( TInt aFileSize, TInt aHttpCode  );

        /**
        * Checks number format
        * Returns ETrue if aNumber is improper
        */   
        static TBool HasImproperChars( const TLex8& aNumber );
        
        /**
        * Moves parser position to first not white space
        */ 
        static void MovePosition( const TDesC8& aString, TInt& aPos );
        
        /**
        * Handles conversion exception
        * Returns EFalse if error cannot be handled
        */ 
        static TBool HandleConversionException( TLex8& aNumber, TInt& aRangePos, TInt aError );
        
        /**
        Members
        */
    private:
        // Reference to response header, not owned
    	CUpnpHttpServerTransaction &iTransaction;
        // Reference to start position
        TInt& iStartPos;
        // Reference to end position
        TInt& iEndPos; 
        //size parsing buffer
        TBuf8<KMaxIntegerInContentLength*KMaxIntegerLength> iSizeBuffer;

    };


#endif

