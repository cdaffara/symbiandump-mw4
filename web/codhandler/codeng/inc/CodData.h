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
*      Declaration of class CCodData.   
*      
*
*/


#ifndef COD_DATA_H
#define COD_DATA_H

// INCLUDES

#include <e32base.h>
#include <bamdesca.h>
#include "CodDefs.h"
#include <f32file.h>

#include "MediaObject.h"
// ================= CONSTANTS =======================

// FORWARD DECLARATION

// CLASS DECLARATION

/**
* COD / DD Data.
*/
NONSHARABLE_CLASS( CCodData ): public CBase
    {

    public:     // types
    
        enum TSupressConfirm     /// Confirm supress 
            {
            ENever = 0,  ///< Never suppress confirmation.
            EUserConfirmStepOnly,    ///< Suppress user confirmation step only.
            EAlways  ///< Always suppress confirmation.
            };


    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The created COD data.
        */      
        IMPORT_C static CCodData* NewL();
        
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CCodData();
        
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
   public://Methods for managing info files

		/**
        * Append information to stored in info file.
        * @return void
        * @Added in OMA pause-resume feature
        */
	    void AppendStorageInfoL(TPtr8& aBuf)const;
		
		/**
        * Load the information from info file
        * @return void
        * @Added in OMA pause-resume feature
        */
	    void LoadStorageInfoL( RFile& aInFile );        
    
    public://Methods to manage media objects
    
        /**
        * Append given media object to the array
        * @return index of the media object
        */
        TInt AppendMediaObjectL( CMediaObjectData *aMO );
        
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
        * Get Size attribute.
        * @return Size.
        */
        IMPORT_C TInt Size() const;
        
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
        * Get Confirm attribute. 
        * @return Size.
        */
        IMPORT_C TInt SuppressConfirm() const;

       /**
        * Get updated DD URI attribute. 
        * @return Size.
        */
        IMPORT_C TPtrC8 UpdatedDDUriL() const;
        
        
        /**
        * Append given media object to the array
        * @return media object
        */
        IMPORT_C CMediaObjectData *operator[]( TInt aIndex );

        /**
        * Append given media object to the array
        * @return media object
        */
        IMPORT_C CMediaObjectData *operator[]( TInt aIndex ) const;

        /**
        *Returns the currently Active Download
        */
        IMPORT_C TInt ActiveDownload() const;
        
        
        /**
        * Get the Count of number of Media Objects.
        */
        inline TInt  Count( ) const { return iMediaObjects.Count(); }            

    protected:  // Constructors and destructor

        /**
        * Constructor.
        */      
        CCodData();
        
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
        * Set Vendor attribute.
        * @param Vendor.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetVendorL( const TDesC& aVendor );

        /**
        * Set Description attribute.
        * @param Description.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetDescriptionL( const TDesC& aDescription );

        /**
        * Set Size attribute.
        * @param Size.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetSize( TUint aSize );

        /**
        * Set suppressConfirm attribute.
        * @param .
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetSuppressConfirm( TInt aSuppressConfirm );

        /**
        * Set Install-Notify attribute.
        * @param Install-Notify.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetInstallNotifyL( const TDesC& aInstallNotify );

        /**
        * Set Next-URL attribute.
        * @param Next-URL.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetNextUrlL( const TDesC& aNextUrl );

        /**
        * Set Next-URLatError attribute.
        * @param URLatError.
        * @return ETrue if set, EFalse for invalid data.
        */
        TBool SetNextUrlAtErrorL( const TDesC& aNextUrlAtError );

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
        * Set Order attribute.
        * @param aIsPostOrder.
        */
        void SetOrderIsPost( TBool aIsPostOrder );

		/**
        * Set updated DD URI attribute. 
        */
        TBool SetUpdatedDDURI( const TDesC& aUrl );
       
        /**
        * Set updated DD URI attribute. 
        */
        void SetActiveDownload( const TInt aMediaObjectId );


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

    private:    // friends

        /// Parsers can set the data.
        friend class TCodParser;

        /// Parser can set the data.
        friend class TDdParser;
        
        /// Engine can set source URI.
        friend class CCodEngBase;

    private:    // data
    
		HBufC* iName;               ///< COD-Name. Name ( of the album ).
		HBufC* iVendor;             ///< COD-Vendor. Vendor ( info of album ).
		HBufC* iDescription;        ///< COD-Description. Description ( of album ).
		TUint iSize;                ///< COD-Size. Total size ( of all tracks in album ).
		HBufC8* iInstallNotify;     ///< COD-Install-Notify. Owned.
		HBufC8* iNextUrl;           ///< COD-Next-URL. Owned.
		HBufC8* iNextUrlAtError;    ///< COD-Next-UrlatError. Owned.
		HBufC8* iInfoUrl;           ///< COD-Info-URL. Owned.
		HBufC* iPrice;              ///< COD-Price. Owned.
		HBufC8* iIcon;              ///< COD-Icon. Owned.
		HBufC* iVersion;            ///< DD-Version. Owned.
		TBool iIsPostOrder;         ///< Order (True if Post or False if Any) // for OMA 2
		TInt  iSuppressConfirm;     ///< Suppress the confirmation for OMA 2 
		HBufC8* iSourceUri;         ///< Source Uri owned.
        HBufC8* iUpdatedDDUri;        ///< Updated DD URI
        TInt    iActiveDownload;	///< Currently Active Media Object 
		RMediaObjectArray iMediaObjects;	///< Media objects
    };

#endif /* def COD_DATA_H */
