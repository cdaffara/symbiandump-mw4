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
*      Declaration of class CMediaObjectData.   
*      
*
*/


#ifndef MEDIA_OBJECT_H
#define MEDIA_OBJECT_H

// INCLUDES

#include <e32base.h>
#include <bamdesca.h>
#include "CodDefs.h"
#include <f32file.h>
#include "HttpDownloadData.h"

// ================= CONSTANTS =======================
/// OMA 1 DD MIME type.
_LIT8( KDdDataType, DD_MIME_TYPE );
// OMA 2 DD MIME type.
_LIT8( KDd2DataType, DD2_MIME_TYPE );

// FORWARD DECLARATION

class CDesC8ArrayFlat;
class CMediaObjectData;
typedef RPointerArray<CMediaObjectData> RMediaObjectArray;

class CHeaderField;

// CLASS DECLARATION

/**
* COD / DD Data.
*/
NONSHARABLE_CLASS( CMediaObjectData ): public CBase
    {

    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The created COD data.
        */      
        IMPORT_C static CMediaObjectData* NewL();
        
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CMediaObjectData();
        
    public:     // New methods

        /**
        * Check whether COD data is valid (all mandatory values are filled).
        * @return ETrue if data is valid.
        */
        IMPORT_C TBool IsValid() const;

        /**
        * Clear everything. Does not free memory, simply sets all descriptors
        * to zero length.
        */
        IMPORT_C void Reset();

    public:     // Getters
    
        /**
        * Get Name attribute.
        * @return Name.
        */
        IMPORT_C TPtrC Name() const;

        /**
        * Get Vendor attribute.
        * @return Vendor.
        */
        IMPORT_C TPtrC Vendor() const;

        /**
        * Get Description attribute.
        * @return Description.
        */
        IMPORT_C TPtrC Description() const;

        /**
        * Get Url attribute.
        * @return Url.
        */
        IMPORT_C TPtrC8 Url() const;

        /**
        * Get Size attribute.
        * @return Size.
        */
        IMPORT_C TInt Size() const;
        
        /**
        * Get Size attribute.
        * @return Size.
        */
        IMPORT_C TInt DownloadedSize() const;

        /**
        * Get media state attribute.
        * @return Size.
        */
        IMPORT_C TMediaObjectState State() const;
                
        /**
        * Get Type attribute.
        * @return Type.
        */
        IMPORT_C TPtrC8 Type() const;

        /**
        * Get Type attribute list.
        * @return Type list.
        */
        IMPORT_C const MDesC8Array& Types() const;

        /**
        * Get Install-Notify attribute.
        * @return Install-Notify.
        */
        IMPORT_C TPtrC8 InstallNotify() const;

        /**
        * Get Next-URL attribute.
        * @return Next-URL.
        */
        IMPORT_C TPtrC8 NextUrl() const;

        /**
        * Get Next-URLatError attribute.
        * @return URLatError.
        */
        IMPORT_C TPtrC8 NextUrlAtError() const;

        /**
        * Get Info-URL attribute.
        * @return Info-URL.
        */
        IMPORT_C TPtrC8 InfoUrl() const;

        /**
        * Get Price attribute.
        * @return Price.
        */
        IMPORT_C TPtrC Price() const;

        /**
        * Get Icon attribute.
        * @return Icon.
        */
        IMPORT_C TPtrC8 Icon() const;

        /**
        * Get Version attribute (DD specific).
        * @return Version.
        */
        IMPORT_C TPtrC Version() const;

        /**
        * Get source URI.
        * @return Source URI.
        */
        IMPORT_C TPtrC8 SourceUri() const;

        /**
        * Get Order.
        * @return ETrue if order tag is "Post", EFails iin case of "any".
        */
        IMPORT_C TBool IsPostOrder() const;

        /**
        * ProgressiveDownload (DD specific).
        * @return iProgressiveDl.
        */
        IMPORT_C TBool ProgressiveDownload() const;

        /**
        * Check if aType appears in the type list.
        * @param aType Type to find.
        * @return ETrue if aType appears in the type list.
        */
        TBool HasType( const TDesC8& aType ) const;
				
        /**
        * Get number of elements in the type list.
        * @return Number of elements.
        */
        TInt TypesCount() const;

       /**
        * Get updated DD URI attribute. 
        * @return Size.
        */
        IMPORT_C TPtrC8 UpdatedDDUriL() const;
        
        /**
        * Get Full Name (with path) of media object. 
        * @return iFullName.
        */
        TPtrC FullName() const;
        
        /**
        * Get Pausable attribute. 
        * @return iPausable.
        */
        TBool Pausable() const;
        
        /**
        * Check if unique filename is set or not.
        * @return IsUniqueFilenameSet.
        */
        TBool IsUniqueFilenameSet() const;

    public:		
    
        /**
        * Append information to stored in info file.
        * @return void
        * @Added in OMA pause-resume feature
        */
	    void StoreMediaInfoL( TPtr8& aBuf, const CArrayPtrFlat<CHeaderField>* aHeaders ) const;
		
		/**
        * Load the information from info file
        * @return void
        * @Added in OMA pause-resume feature
        */
	    void LoadMediaInfoL( RFile& aInFile, RFs &aFs, CArrayPtrFlat<CHeaderField>*& aHeaders );
	    
	    /**
        * Re-arrange the types so that TYPE at index 0 should match the content-type mentioned in the header of content download
        * @return void
        */
        void ReArrangeTypesL( TInt aIndex );
                

    protected:  // Constructors and destructor

        /**
        * Constructor.
        */      
        CMediaObjectData();
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

    protected:  // Setters
    
        /**
        * Set Name attribute.
        * @param Name.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetNameL( const TDesC& aName );

        /**
        * Set Description attribute.
        * @param Description.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetDescriptionL( const TDesC& aDescription );

        /**
        * Set Url attribute.
        * @param Url.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetUrlL( const TDesC& aUrl );

        /**
        * Set Size attribute.
        * @param Size.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetSize( TInt aSize );

        /**
        * Set Downloaded size attribute.
        * @param Size.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetDownloadedSize( TInt aSize );

        /**
        * Set state attribute.
        * @param TMediaObjectState.
        * @return void.
        */
        void SetState( TMediaObjectState aState );

        /**
        * Set Type attribute (replace all).
        * @param Type.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetTypeL( const TDesC& aType );

        /**
        * Set Type attribute.
        * @param Type.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool AddTypeL( const TDesC& aType );

        /**
        * Set Install-Notify attribute.
        * @param Install-Notify.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetInstallNotifyL( const TDesC& aInstallNotify );

        /**
        * Set Info-URL attribute.
        * @param Info-URL.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetInfoUrlL( const TDesC& aInfoUrl );

        /**
        * Set Price attribute.
        * @param Price.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetPriceL( const TDesC& aPrice );

        /**
        * Set Icon attribute.
        * @param Icon.
        */
        TBool SetIconL( const TDesC& aIcon );

        /**
        * Set Version attribute.
        * @param Version.
        */
        TBool SetVersionL( const TDesC& aVersion );

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
        * Set Order attribute.
        * @param aIsPostOrder.
        */
        void SetOrderIsPost( TBool aIsPostOrder );

        /**
        * Set updated DD URI attribute. 
        */
        TBool SetUpdatedDDURI( const TDesC& aUrl );
        
        /**
        * Set Unique-Filename-Set attribute.
        * @param aUniqueFilenameSet (ETrue / EFalse).
        */
        void UniqueFilenameSet( TBool aUniqueFilenameSet );
        
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
        * Sets the currently downloaded media content size
        * @return void
        */		
        void SetDownloadedFileSizeL( RFs &aFs );
		
		/**
        * Creates a buffer holding content types
        * @return void
        * @Added in OMA pause-resume feature
        */
	    void AppendContentTypesL(TPtr8& abuf,CDesC8ArrayFlat* aTypes)const;

		/**
        * Creates an array of content types.
        * @return void
        * @Added in OMA pause-resume feature
        */
	    void LoadContentTypesL(RFile& aInFile,CDesC8ArrayFlat* aTypes);	

        /**
        * Append the HTTP Response of this media object.
        * @param aBuf Buffer to be appended with header info.
        * @return void.
        */
        void AppendHeadersL( TPtr8& aBuf, const CArrayPtrFlat<CHeaderField>* aHeaders ) const;

        /**
        * Load the header fields from the info file.
        * @param aFile Info file.
        * @param aHeaders Headers fields to be filled into.
        * @return void.
        */
        void LoadHeadersL( RFile& aFile, CArrayPtrFlat<CHeaderField>*& aHeaders );

    private:    // friends

        /// Parsers can set the data.
        friend class TCodParser;

        /// Parser can set the data.
        friend class TDdParser;
        
        /// Engine can set source URI.
        friend class CCodEngBase;
        
        /// Saver needs to access the paths.
        friend class CFileSaver;        

    private:    // data

        HBufC* iName;               ///< COD-Name. Owned.
        HBufC* iDescription;        ///< COD-Description. Owned.
        HBufC8* iUrl;               ///< COD-URL. Owned.
        TUint iSize;                 ///< COD-Size.
        CDesC8ArrayFlat* iTypes;    ///< COD-Types. Owned.
        HBufC8* iInstallNotify;     ///< COD-Install-Notify. Owned.
        HBufC8* iInfoUrl;           ///< COD-Info-URL. Owned.
        HBufC* iPrice;              ///< COD-Price. Owned.
        HBufC8* iIcon;              ///< COD-Icon. Owned.
        HBufC8* iSourceUri;         ///< Source URI. Owned.
        TBool iProgressiveDl;       ///< Progressive download. //for OMA 2
        TBool iIsPostOrder;			///< Order (True if Post or False if Any) // for OMA 2
        
        //Media specific data required for CodEngBase
        
        TInt iDownloadedSize;       ///< Currently downloaded size of the media.
        TMediaObjectState iState;   ///< Media object state
      	TInt iStatusCode;	        ///< HTTP code sent in install-notify.
        TInt iResult;               ///< Resulting error code (overall result).
        TFileName iFileName;        ///< File name of saved content.
        HBufC* iFullName;           ///< File name based on the Temp file path and url file name
        TFileName iTempPath;        ///< Temp file path (downloading).
        TFileName iRootPath;        ///< Root path (saving content).
        TBool iPausable;            ///< Pausable
        TBool iUniqueFilenameSet;   ///< ETrue if unique name for this track is already set.
                                    ///< Required to prevent over-writing of existing media file.
        
    };

#endif /* def MEDIA_OBJECT_H */
