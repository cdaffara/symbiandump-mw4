/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CDownloadDataClient.
*      
*
*/


#ifndef DOWNLOAD_DATA_CLIENT_H
#define DOWNLOAD_DATA_CLIENT_H

// INCLUDES

#include <e32base.h>
#include "HttpDownloadData.h"

// ================= CONSTANTS =======================

// FORWARD DECLARATION

class CDesC8ArrayFlat;

// CLASS DECLARATION

/**
* Media Data.
*/
NONSHARABLE_CLASS( CMediaDataClient ) : public CMediaDataBase
    {
    
    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMediaDataClient* NewL();
        
        /**
        * Two-phased constructor.
        */
        static CMediaDataClient* NewLC();
        
        /**
        * Destructor.
        */      
        ~CMediaDataClient();
        
    public:     // Methods
    
    	/**
        * Creates an HBufC8 representation of 'this'.
        */
        HBufC8* MarshalDataL() const;
        
        /**
        * Writes 'this' to the stream.
        */
        void ExternalizeL(RWriteStream& aStream) const;
        
        /**
        * Dummy function - No implementation.
        */
        void InternalizeL(RReadStream& aStream);
        
    public:     // Getters
    
        /**
        * Get size of this class including actual memory occupied by descriptors.
        * @return Bytes.
        */
        TInt Bytes() const;
        
    public:     // Setters
        
        /**
        * Set Name attribute.
        * @param Name.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetNameL( const TDesC& aName );

        /**
        * Set Url attribute.
        * @param Url.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetUrlL( const TDesC8& aUrl );

        /**
        * Set Size attribute.
        * @param Size.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetSize( TUint aSize );

        /**
        * Set Type attribute (replace all).
        * @param Type.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetTypeL( const TDesC8& aType );

        /**
        * Set Type attribute.
        * @param Type.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool AddTypeL( const TDesC8& aType );
        
        /**
        * Reset the Type array.
        * @return Number of elements.
        */
        void ResetTypes();

        /**
        * Set Icon attribute.
        * @param Icon.
        */
        TBool SetIconL( const TDesC8& aIcon );

        /**
        * Set source URI.
        * @param aSourceUri Source URI.
        */
        void SetSourceUriL( const TDesC8& aSourceUri );

        /**
        * Set Progressive download attribute.
        * @param aProgressiveDl Progressive Download setting.
        */
        void SetProgressiveDownload( TBool aProgressiveDl );
        
        /**
        * Set state attribute.
        * @param TMediaObjectState.
        * @return void.
        */
        void SetState( TMediaObjectState aState );

        /**
        * Set result attribute.
        * @param aResult.
        * @return void.
        */
        void SetResult( TInt aResult );
        
        /**
        * Set redir Url attribute.
        * @param aRedirUrl Redirected Url.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetRedirUrlL( const TDesC8& aRedirUrl );
        
        /**
        * Set method attribute.
        * @param aMethod Http request method.
        */
        void SetMethod( TInt aMethod );
        
        /**
        * Set Progressive download attribute.
        * @param aRedirected Whether redirected or not.
        */
        void SetRedirected( TBool aRedirected );
        
        /**
        * Set destination filename attribute.
        * @param aDestFilename Destination filename.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetDestFilenameL( const TDesC& aDestFilename );
        
        /**
        * Set Temp filename attribute.
        * @param aTempFilename Temp filename.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetTempFilenameL( const TDesC& aTempFilename );
        
        /**
        * Set downloaded size attribute.
        * @param aDownloadedSize Downloaded size of the media object.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetDownloadedSize( TInt aDownloadedSize );
        
        /**
        * Set destination removable attribute.
        * @param aDesRemovable Whether removable media drive.
        */
        void SetDesRemovable( TBool aDesRemovable );
        
        /**
        * Set last error id attribute.
        * @param aLastErrorId Last error ID.
        */
        void SetLastErrorId( TInt aLastErrorId );
        
        /**
        * Set global error id attribute.
        * @param aGlobalErrorId Global error ID.
        */
        void SetGlobalErrorId( TInt aGlobalErrorId );
        
        /**
        * Set Pausable attribute.
        * @param aPausable Whether download pausable or not.
        */
        void SetPausable( TBool aPausable );
        
        /**
        * Set StatusCode attribute.
        * @param aStatusCode.
        * @return void.
        */
        void SetStatusCode( TInt aStatusCode );
    private:    // implementation details

        /**
        * Common setter for string attributes, with overflow check.
        * @param aBuf Buffer to set.
        * @param aString String to set.
        * @param aMaxLength Max length to check.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetStringAttrL
            ( HBufC*& aBuf, const TDesC& aString, TInt aMaxLength );

        /**
        * Common setter for string attributes, with overflow check.
        * @param aBuf Buffer to set.
        * @param aString String to set.
        * @param aMaxLength Max length to check.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetStringAttrL
            ( HBufC8*& aBuf, const TDesC& aString, TInt aMaxLength );
        
        /**
        * Common setter for string attributes, with overflow check.
        * @param aBuf Buffer to set.
        * @param aString String to set.
        * @param aMaxLength Max length to check.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetStringAttrL
            ( HBufC8*& aBuf, const TDesC8& aString, TInt aMaxLength );
        
    protected:  // Constructors and destructor
        
        /** 
        * Constructor.
        */
        CMediaDataClient(){}
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

    };
    

/**
* Download Data.
*/  
NONSHARABLE_CLASS( CDownloadDataClient ) : public CDownloadDataBase
    {
    
    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDownloadDataClient* NewL();
        
        /**
        * Two-phased constructor.
        */
        static CDownloadDataClient* NewLC();
        
        /**
        * Destructor.
        */      
        ~CDownloadDataClient();
        
    public:     // Methods
        
        /**
        * Creates an HBufC8 representation of 'this'.
        */
        HBufC8* MarshalDataL() const;
        
        /**
        * Writes 'this' to the stream.
        */
        void ExternalizeL(RWriteStream& aStream) const;
        
    public:     // Methods to manage media data
    
        /**
        * Append given media object to the array
        * @return index of the media object
        */
        TInt AppendMediaData( CMediaDataClient *aMO );
        
        /**
        * Append given media object to the array
        * @return media object
        */
        CMediaDataBase* operator[]( TInt aIndex );

        /**
        * Append given media object to the array
        * @return media object
        */
        CMediaDataBase* operator[]( TInt aIndex ) const;
        
    public:     // Getters
    
        /**
        * Get size of this class including actual memory occupied by descriptors.
        * @return Bytes.
        */
        TInt Bytes() const;
        
    public:     // Setters
    
        /**
        * Set Name attribute.
        * @param Name.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetNameL( const TDesC& aName );

        /**
        * Set Size attribute.
        * @param Size.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetSize( TUint aSize );

        /**
        * Set Icon attribute.
        * @param Icon.
        */
        TBool SetIconL( const TDesC8& aIcon );

       /**
        * Set updated DD URI attribute. 
        * @param Updated DD Uri.
        */
        TBool SetUpdatedDDURI( const TDesC8& aUrl );
        
    private:    // implementation details

        /**
        * Common setter for string attributes, with overflow check.
        * @param aBuf Buffer to set.
        * @param aString String to set.
        * @param aMaxLength Max length to check.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetStringAttrL
            ( HBufC*& aBuf, const TDesC& aString, TInt aMaxLength );

        /**
        * Common setter for string attributes, with overflow check.
        * @param aBuf Buffer to set.
        * @param aString String to set.
        * @param aMaxLength Max length to check.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetStringAttrL
            ( HBufC8*& aBuf, const TDesC& aString, TInt aMaxLength );
            
        /**
        * Common setter for string attributes, with overflow check.
        * @param aBuf Buffer to set.
        * @param aString String to set.
        * @param aMaxLength Max length to check.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetStringAttrL
            ( HBufC8*& aBuf, const TDesC8& aString, TInt aMaxLength );
        
    protected:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        CDownloadDataClient(){}
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

    };

#endif /* def DOWNLOAD_DATA_CLIENT_H */
