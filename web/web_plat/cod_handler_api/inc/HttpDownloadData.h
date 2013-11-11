/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base classes for CodData IPC
*
*/



#ifndef HTTPDOWNLOADDATA_H
#define HTTPDOWNLOADDATA_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <s32strm.h>

//  MACROS

// COD Attribute length limits. Use these as maximum size in UI controls
// displaying COD attributes.

/// COD-Name maximum length.
#define COD_NAME_MAX_LEN                40
/// COD-Vendor maximum length.
#define COD_VENDOR_MAX_LEN              40
/// COD-Description maximum length.
#define COD_DESCRIPTION_MAX_LEN         160
/// COD-URL maximum length.
#define COD_URL_MAX_LEN                 1024
/// COD-Size maximum length.
#define COD_SIZE_MAX_LEN                6
/// COD-Type maximum length.
#define COD_TYPE_MAX_LEN                40
/// COD-Install-Notify maximum length.
#define COD_INSTALL_NOTIFY_MAX_LEN      1024
/// COD-Next-URL maximum length.
#define COD_NEXT_URL_MAX_LEN            1024
/// COD-Next-URLatError maximum length.
#define COD_NEXT_URL_AT_ERROR_MAX_LEN   1024
/// COD-Info-URL maximum length.
#define COD_INFO_URL_MAX_LEN            1024
/// COD-Price maximum length.
#define COD_PRICE_MAX_LEN               32
/// COD-Icon maximum length.
#define COD_ICON_MAX_LEN                1024
/// COD-Version maximum length.
#define COD_VERSION_MAX_LEN             40
/// COD-License maximum length (OMA 2)
#define COD_LICENSE_MAX_LEN             1024
/// COD-Order maximum length (OMA 2)
#define COD_ORDER_MAX_LEN               32


// CONSTANTS

/// Granularity of the type list.
LOCAL_D const TInt KTypeGranularity = 1;


// TYPE DEFINITIONS

// Media object states
enum TMediaObjectState
    {
    EQueued = 0,    //Media object download is queued
    EInProgress,    //Download is in progress
    EFailed,        //Download failed
    ESucceeded,     //Download succeeded
	ETobeInstalled	//Download completed but not moved
    };


// FORWARD DECLARATION
class CDesC8ArrayFlat;
class RWriteStream;
class RReadStream;


// CLASS DECLARATION

/**
* Media Data Base.
*/
NONSHARABLE_CLASS( CMediaDataBase ) : public CBase
    {
    
    public:     // Constructors and destructor
        
        /**
        * Destructor.
        */      
        virtual ~CMediaDataBase() {}
        
    public:     // Methods

        /**
        * Writes 'this' to the stream.
        */
        virtual void ExternalizeL(RWriteStream& aStream) const = 0;
        
        /**
        * Initializes 'this' from stream.
        */
        virtual void InternalizeL(RReadStream& aStream) = 0;
        
    public:     // Getters
        
        /**
        * Get Name attribute.
        * @return Name.
        */
        inline TPtrC Name() const { return *iName; }

        /**
        * Get Url attribute.
        * @return Url.
        */
        inline TPtrC8 Url() const { return *iUrl; }

        /**
        * Get Size attribute.
        * @return Size.
        */
        inline TInt Size() const { return iSize; }

        /**
        * Get media state attribute.
        * @return Size.
        */
        inline TMediaObjectState State() const { return iState; }
                
        /**
        * Get Type attribute.
        * @return Type.
        */
        inline TPtrC8 Type() const
        	{
        	if ( iTypes->MdcaCount() )
        		return iTypes->MdcaPoint( 0 );
    		return TPtrC8( KNullDesC8 );
        	}

        /**
        * Get Type attribute list.
        * @return Type list.
        */
        inline const MDesC8Array& Types() const { return *iTypes; }

        /**
        * Get Icon attribute.
        * @return Icon.
        */
        inline TPtrC8 Icon() const { return *iIcon; }

        /**
        * Get source URI.
        * @return Source URI.
        */
        inline TPtrC8 SourceUri() const { return *iSourceUri; }

        /**
        * ProgressiveDownload (DD specific).
        * @return iProgressiveDl.
        */
        inline TBool ProgressiveDownload() const { return iProgressiveDl; }

        /**
        * Check if aType appears in the type list.
        * @param aType Type to find.
        * @return ETrue if aType appears in the type list.
        */
        inline TBool HasType( const TDesC8& aType ) const
    	    {
		    TInt i;
		    return 0 == iTypes->Find( aType, i, ECmpNormal );
		    }
				
        /**
        * Get number of elements in the type list.
        * @return Number of elements.
        */
        inline TInt TypesCount() const { return iTypes->MdcaCount(); }
        
        /**
        * Get Redirected Url.
        * @return iRedirUrl.
        */
        inline TPtrC8 RedirUrl() const { return *iRedirUrl; }
        
        /**
        * Get Method.
        * @return iMethod.
        */
        inline TInt Method() const { return iMethod; }
        
        /**
        * Is Redirected?
        * @return iRedirected.
        */
        inline TBool Redirected() const { return iRedirected; }
        
        /**
        * Get Destination Filename.
        * @return iDestFilename.
        */
        inline TPtrC DestFilename() const { return *iDestFilename; }
        
        /**
        * Get Downloaded Size.
        * @return iDownloadedSize.
        */
        inline TInt DownloadedSize() const { return iDownloadedSize; }
        
        /**
        * Get Destination Removable.
        * @return iDesRemovable.
        */
        inline TBool DesRemovable() const { return iDesRemovable; }
        
        /**
        * Get Last Error Id.
        * @return iLastErrorId.
        */
        inline TInt LastErrorId() const { return iLastErrorId; }
        
        /**
        * Get Global Error Id.
        * @return iGlobalErrorId.
        */
        inline TInt GlobalErrorId() const { return iGlobalErrorId; }
        
        /**
        * Get Pausable attribute.
        * @return iPausable.
        */
        inline TBool Pausable() const { return iPausable; }
        
        /**
        * Get size of this class including actual memory occupied by descriptors.
        * @return Bytes.
        */
        virtual TInt Bytes() const = 0;
        
        /**
        * Gets the result of Media Object
        * @ return iResult
        */
        inline TInt Result () const { return iResult; } 
        
        /**
        * Get TempPath Filename.
        * @return iDestFilename.
        */
        inline TPtrC TempFilename() const { return *iTempFilename; }
        
        /**
        * Gets the StatusCode of Media Object
        * @ return iStatusCode
        */
        inline TInt StatusCode () const { return iStatusCode; } 
    public:     // Setters
        
        /**
        * Set Name attribute.
        * @param Name.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetNameL( const TDesC& aName ) = 0;

        /**
        * Set Url attribute.
        * @param Url.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetUrlL( const TDesC8& aUrl ) = 0;

        /**
        * Set Size attribute.
        * @param Size.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetSize( TUint aSize ) = 0;

        /**
        * Set Type attribute (replace all).
        * @param Type.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetTypeL( const TDesC8& aType ) = 0;

        /**
        * Set Type attribute.
        * @param Type.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool AddTypeL( const TDesC8& aType ) = 0;
        
        /**
        * Reset the Type array.
        * @return Number of elements.
        */
        virtual void ResetTypes() = 0;

        /**
        * Set Icon attribute.
        * @param Icon.
        */
        virtual TBool SetIconL( const TDesC8& aIcon ) = 0;

        /**
        * Set source URI.
        * @param aSourceUri Source URI.
        */
        virtual void SetSourceUriL( const TDesC8& aSourceUri ) = 0;

        /**
        * Set Progressive download attribute.
        * @param aProgressiveDl Progressive Download setting.
        */
        virtual void SetProgressiveDownload( TBool aProgressiveDl ) = 0;
        
        /**
        * Set state attribute.
        * @param TMediaObjectState.
        * @return void.
        */
        virtual void SetState( TMediaObjectState aState ) = 0;
        
        /**
        * Set result attribute.
        * @param aResult.
        * @return void.
        */
        virtual void SetResult( TInt aResult ) = 0;
        
        /**
        * Set redir Url attribute.
        * @param aRedirUrl Redirected Url.
        * @return void.
        */
        virtual TBool SetRedirUrlL( const TDesC8& aRedirUrl ) = 0;
        
        /**
        * Set method attribute.
        * @param aMethod Http request method.
        */
        virtual void SetMethod( TInt aMethod ) = 0;
        
        /**
        * Set Progressive download attribute.
        * @param aRedirected Whether redirected or not.
        */
        virtual void SetRedirected( TBool aRedirected ) = 0;
        
        /**
        * Set destination filename attribute.
        * @param aDestFilename Destination filename.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetDestFilenameL( const TDesC& aDestFilename ) = 0;
        
        /**
        * Set Temp filename attribute.
        * @param aTempFilename TempPath filename.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetTempFilenameL( const TDesC& aTempFilename ) = 0;
                
        /**
        * Set downloaded size attribute.
        * @param aDownloadedSize Downloaded size of the media object.
        * @return ETrue if set, EFalse for invalid data.
        */
        virtual TBool SetDownloadedSize( TInt aDownloadedSize ) = 0;
        
        /**
        * Set destination removable attribute.
        * @param aDesRemovable Whether removable media drive.
        */
        virtual void SetDesRemovable( TBool aDesRemovable ) = 0;
        
        /**
        * Set last error id attribute.
        * @param aLastErrorId Last error ID.
        */
        virtual void SetLastErrorId( TInt aLastErrorId ) = 0;
        
        /**
        * Set global error id attribute.
        * @param aGlobalErrorId Global error ID.
        */
        virtual void SetGlobalErrorId( TInt aGlobalErrorId ) = 0;
        
        /**
        * Set Pausable attribute.
        * @param aPausable Whether download pausable or not.
        */
        virtual void SetPausable( TBool aPausable ) = 0;
        
        /**
        * Set StatusCode attribute.
        * @param aStatusCode.
        * @return void.
        */
        virtual void SetStatusCode( TInt aStatusCode ) = 0;
    protected:  // data
    
        HBufC* iName;               ///< Media-Name. Owned.
        HBufC8* iUrl;               ///< Media-URL. Owned.
        TUint iSize;                ///< Media-Size.
        CDesC8ArrayFlat* iTypes;    ///< Media-Types. Owned.
        HBufC8* iIcon;              ///< Media-Icon. Owned.
        HBufC8* iSourceUri;         ///< Source URI. Owned.
        TBool iProgressiveDl;       ///< Progressive download. //for OMA 2
        TMediaObjectState iState;   ///< Media object state
        TInt iResult;
        HBufC8* iRedirUrl;			///< Media Redirected-URL.
        TInt iMethod;				///< Media Request-Method.
        TBool iRedirected;			///< Is Redirected?
        HBufC* iDestFilename;		///< Media Destination Filename.
        TInt iDownloadedSize;       ///<The media track downloaded size.Should be equal to iSize when download completed successfully
        TBool iDesRemovable;		///< Is file stored on removable media?
        TInt iLastErrorId;			///< THttpDownloadMgrError.
        TInt iGlobalErrorId;		///< Global Error Id.
        TBool iPausable;			///< Is Media-Download Pausable?
        HBufC* iTempFilename;		///< Media Temp Filename.
        TInt iStatusCode;
    };


/**
* Download Data Base.
*/ 
NONSHARABLE_CLASS( CDownloadDataBase ) : public CBase
    {
    
    public:     // Constructors and destructor
        
        /**
        * Destructor.
        */      
        virtual ~CDownloadDataBase() {}

        /*
        *The MO count
        */
        inline TInt Count() const { return iMediaArray.Count();}
        
    protected:  // data
        
        HBufC* iName;               ///< Download-Name. Name ( of the album ).
        TUint iSize;                ///< Download-Size. Total size ( of all tracks in album ).
        HBufC8* iIcon;              ///< Download-Icon. Owned.
        HBufC8* iUpdatedDDUri;      ///< Updated DD URI
        RPointerArray<CMediaDataBase> iMediaArray;	///< Media objects
    
    };

#endif      // HTTPDOWNLOADDATA_H   

// End of File
