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
* Description:  Implementation of the Multipart Body Part
*
*/



#ifndef BODYPART_H
#define BODYPART_H

#warning The Multipart Parser API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//  INCLUDES
#include <e32std.h>
#include <e32base.h>


//  CONSTANTS


// DATA TYPES


// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
*  Implementation of the Multipart Body Part
*
*  \b Library: MultipartParser.lib
*
*  @since 3.0
*/
class CBodyPart : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CBodyPart* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CBodyPart();

    public: // New functions

        /**
        * @since 3.0
        * @return The body of this body part
        */
	    inline const TDesC8& Body() { return iBody; }

        /**
        * @since 3.0
        * @return The content type of this body part
        */
	    inline const TDesC8& ContentType() { return iContentType; }

        /**
        * @since 3.0
        * @return The charset of this body part
        */
	    inline const TDesC8& Charset() { return iCharset; }	

        /**
        * @since 3.0
        * @return The content ID of this body part
        */
	    inline const TDesC8& ContentID() { return iContentID; }

        /**
        * @since 3.0
        * @return The url of this body part
        */
	    inline const TDesC16& Url() { return *iUrl; }

        /**
        * @since 3.0
        * @return The headers of this body part
        */
	    inline const TDesC8& Headers() { return iHeaders; }

        /**
        * @since 3.0
        * @return The boundary of this body part
        */
        const TDesC8& Boundary();

        /**
        * @since 3.0
        * @return The content-base of this body part
        */
        const TDesC8& ContentBase();

        /**
        * @since 3.0
        * @return The content-location of this body part
        */
        const TDesC8& ContentLocation();

        /**
        * @since 3.0
        * @return The content-transfer-encoding of this body part
        */
        const TDesC8& ContentTransferEncoding();

        /**
        * @since 3.0
        * @return If the body of the body part is decoded or unzipped
        */
        TBool IsDecodedBody();

        /**
        * @since 3.0
        * @param aBody   The body of the body part
        * @return void
        */
        inline void SetBody( const TDesC8& aBody ) { iBody.Set( aBody ); }

        /**
        * @since 3.0
        * @param aBoundary   The boundary of the body part
        * @return void
        */
        void SetBoundary( const TDesC8& aBoundary );

        /**
        * @since 3.0
        * @param aCharset   The charset of the body part
        * @return void
        */
        void SetCharset( const TDesC8& aCharset );

        /**
        * @since 3.0
        * @param aContentBase   The content-base of the body part
        * @return void
        */
        void SetContentBase( const TDesC8& aContentBase );

        /**
        * @since 3.0
        * @param aContentLocation   The content-location of the body part
        * @return void
        */
        void SetContentLocation( const TDesC8& aContentLocation );

        /**
        * @since 3.0
        * @param aContentTransferEncoding   The content-transfer-encoding of the body part
        * @return void
        */
        void SetContentTransferEncoding( const TDesC8& aContentTransferEncoding );

        /**
        * @since 3.0
        * @param aContentType   The content-type of the body part
        * @return void
        */
        void SetContentType( const TDesC8& aContentType );

        /**
        * @since 3.0
        * @param aContentID   The content-ID of the body part
        * @return void
        */
        void SetContentID( const TDesC8& aContentID );

        /**
        * @since 3.0
        * @param aHeaders   The headers of the body part
        * @return void
        */
        inline void SetHeaders( const TDesC8& aHeaders ) { iHeaders.Set( aHeaders ); }

        /**
        * @since 3.0
        * @param aIsDecodedBody   If the body of the body part is decoded or unzipped
        * @return void
        */
        void SetIsDecodedBody( TBool aIsDecodedBody );

        /**
        * @since 3.0
        * @param aUrl   The URL of the body part
        * @return void
        */
        void SetUrl( HBufC16* aUrl );


        
    public: // Functions from base classes
        

    protected:  // Functions from base classes
        

    private:

        /**
        * C++ default constructor.
        */
        CBodyPart();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data
        TPtrC8   iBody;
        TPtrC8   iBoundary;
        TPtrC8   iCharset;
        TPtrC8   iContentBase;
        TPtrC8   iContentLocation;
        TPtrC8   iContentTransferEncoding;
        TPtrC8   iContentType;
        TPtrC8   iContentID;
        TPtrC8   iHeaders;
        TBool    iIsDecodedBody;  ///< if true, this class responsible for freeing the memory of iBody
        HBufC16* iUrl;
    };

#endif      // BODYPART_H   
            
// End of File
