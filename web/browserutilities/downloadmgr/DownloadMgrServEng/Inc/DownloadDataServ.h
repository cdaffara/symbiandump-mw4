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
* Description:  Implements CDownloadDataServ
*
*/



#ifndef DOWNLOAD_DATA_SERV_H
#define DOWNLOAD_DATA_SERV_H

//  INCLUDES
#include <e32base.h>
#include "HttpDownloadData.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Media Data Server.
*/
NONSHARABLE_CLASS( CMediaDataServ ) : public CMediaDataBase
    {
    
    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor - create object from stream.
        */
        static CMediaDataServ* NewL(RReadStream& aStreamData);
        
        /**
        * Two-phased constructor - create object from stream.
        */
        static CMediaDataServ* NewL(const TDesC8& aStreamData);
        
        /**
        * Destructor.
        */      
        ~CMediaDataServ();
        
    public:     // Methods
        
        /**
        * Writes 'this' to the stream.
        */
        void ExternalizeL(RWriteStream& aStream) const;
        
        /**
        * Initializes 'this' from stream.
        */
        virtual void InternalizeL(RReadStream& aStream);
        
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
        * Set Temp filename attribute.
        * @param aTempFilename TempPath filename.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetTempFilenameL( const TDesC& aTempFilename );
        
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
        CMediaDataServ(){}
        
    };


/**
* Download Data Server - Abstract Class.
*/ 
NONSHARABLE_CLASS( CDownloadDataServ ) : public CDownloadDataBase
    {
    
    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor - create object from stream.
        */
        static CDownloadDataServ* NewL(const TDesC8& aStreamData);
        
        /**
        * Destructor.
        */      
        ~CDownloadDataServ();
        
    public:     // Methods
        
        /**
        * Initializes 'this' from stream.
        */
        void InternalizeL(RReadStream& aStream);
        
    public:     // Methods to manage media data
    
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
        
    public:		// Getters
    	
    	/**
        * Get the Count of number of elements in Media Array.
        */
        inline TInt Count( ){ return iMediaArray.Count(); }
        
        /**
        * Get Album name attribute.
        * @return Name.
        */
        inline TPtrC Name() const { return *iName; }
        
    public:     // Setters
    
        /**
        * Set Album Name attribute.
        * @param Name.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetNameL( const TDesC& aName );
        
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
        CDownloadDataServ(){}
    
    };


#endif      // DOWNLOAD_DATA_SERV_H
            
// End of File
