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
* Description:  Declares CUpnpIcon class
*
*/


#ifndef C_CUPNPICON_H
#define C_CUPNPICON_H

// INCLUDES

#include <badesca.h>
#include <upnpdescriptionproperty.h>
#include <s32mem.h>
// CONSTANTS

_LIT8( KUrl, "url" );
_LIT8( KWidth, "width" );
_LIT8( KDepth, "depth" );
_LIT8( KHeight, "height" );
_LIT8( KMimeType, "mimetype" );

const TInt KMaxNumberPropertySize=10;
const TInt KMaxMimeTypeSize = 20;
const TInt KDefBuffer = 100;

// CLASS DECLARATION

/**
*  Declares CUpnpIcon class
*
*  @since Series60 2.6
*/
class CUpnpIcon : public CBase
{
public: // Constructors and destructor

    /**
    * Two-phased constructor. returns a pointer to the new element.
    * @param aElement This reference contains the state variable element
    */
    IMPORT_C static CUpnpIcon* NewL( );
    
    /**
    * Two-phased constructor. returns a pointer to the new element.
    * @param aPath icon path
    * @param aUrl icon url
    * @param aWidth icon width
    * @param aHeight icon height
    * @param aDepth icon depth
    * @param aMimeType icon mime type
    */    
    IMPORT_C static CUpnpIcon* NewL(const TDesC16& aPath, const TDesC8& aUrl, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType);
    
    /**
    * Two-phased constructor. returns a pointer to the new element.
    * @param aPath icon path
    * @param aWidth icon width
    * @param aHeight icon height
    * @param aDepth icon depth
    * @param aMimeType icon mime type
    */    
    IMPORT_C static CUpnpIcon* NewL(const TDesC16& aPath, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType);
    
    /**
    * Destructor. Deletes the element from memory. 
    */
    IMPORT_C virtual ~CUpnpIcon();
    
    
public: // New functions

    /**
    * Returns the mime type.
    * @since Series60 2.6
    * @return return icon mime type
    */
    IMPORT_C const TPtrC8 MimeType() const;
    
    /**
    * Returns the width of the icon.
    * @since Series60 2.6
    * @return width in pixels or KErrNotFound
    */
    IMPORT_C TInt Width() const;
    
    /**
    * Returns the width of the icon.
    * @since Series60 2.6
    * @return icon width 
    */
    TPtrC8 WidthTPtrC8() const;
    
    /**
    * Returns the height of the icon.
    * @since Series60 2.6
    * @return height in pixels or KErrNotFound
    */
    IMPORT_C TInt Height() const;
    
    /**
    * Returns height of the icon.
    * @since Series60 2.6
    * @return icon height 
    */
    TPtrC8 HeightTPtrC8() const;
    
    /**
    * Returns the color depth of the icon.
    * @since Series60 2.6
    * @return icon depth or KErrNotFound
    */
    IMPORT_C TInt Depth() const;
    
    /**
    * Returns color depth of the icon.
    * @since Series60 2.6
    * @return icon depth
    */    
    TPtrC8 DepthTPtrC8() const;
    
    /**
    * Returns the url address.
    * @since Series60 2.6
    * @return url address or NULL 
    */
    IMPORT_C const TPtrC8 Url() const;

    /**
    * Returns the icon buffer
    * @since Series60 2.6
    * @return icon buffer
    */
    IMPORT_C const TPtr8& GetIconBufferPtr() const;
    
    /**
    * Get bitmap filename
    * @since Series60 2.6
    * @return bitmap filename
    */
    IMPORT_C TFileName BitmapFilename() const;
    
    /**
    * Set bitmap filename
    * @since Series60 2.6
    * @param aName the bitmap filename (path?)
    */
    IMPORT_C void SetBitmapFilename( const TDesC& aName );
    
    /**
    * Sets icon mime type
    * @since Series60 2.6
    * @param aMimeType icon mime type
    */
    void SetMimeTypeL( const TDesC8& aMimeType );
    
    /**
    * Sets icon url
    * @since Series60 2.6
    * @param aUrl icon url
    */
    IMPORT_C void SetUrlL( const TDesC8& aUrl );
    
    /**
    * Sets icon width
    * @since Series60 2.6
    * @param aWidth icon width
    */
    void SetWidthL( const TInt aWidth );
    
    /**
    * Sets icon height
    * @since Series60 2.6
    * @param aHeight icon height
    */
    void SetHeightL( const TInt aHeight );
    
    /**
    * Sets icon depth
    * @since Series60 2.6
    * @param aDepth icon depth
    */
    void SetDepthL( const TInt aDepth );
    
    /**
    * Add description property
    * @since Series60 3.2
    * @param aProperty
    */
    void AddPropertyL( CUpnpDescriptionProperty* aProperty );
    
    /**
    * Deserializes icon
    * @since Series60 3.2
    * @param aStream
    */
    void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Serializes icon
    * @since Series60 3.2
    * @param aStream
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
    * @since Series60 3.2
    * @return serialized icon
    */
    IMPORT_C HBufC8* ToDes8L() const;
    
private: // Constructors

    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aElement
    */
    void ConstructL( );
    
    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aPath icon path
    * @param aUrl icon url
    * @param aWidth icon width
    * @param aHeight icon height
    * @param aDepth icon depth
    * @param aMimeType icon mime type
    */
    void CUpnpIcon::ConstructL(const TDesC16& aPath, const TDesC8& aUrl, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType);
    
    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aPath icon path
    * @param aWidth icon width
    * @param aHeight icon height
    * @param aDepth icon depth
    * @param aMimeType icon mime type
    */
    void CUpnpIcon::ConstructL(const TDesC16& aPath, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType);
        
    
    /**
    * C++ default constructor.
    * @param aElement
    */
    CUpnpIcon();

    /**
    * Deletes icon's file if exists.
    * @since Series60 2.6
    */
    void DeleteIconFromFsL();

private: // Data
    
    // Buffer to pass data to server
    HBufC8* iSendBuffer;           
    TPtr8 iSendBufferPtr;
 	
    // XML element containing icon description
    RPointerArray<CUpnpDescriptionProperty> iProperties;
    	
    // iFileName the bitmap filename, owned
    TFileName   iFilename;
    };
	
#endif  //C_CUPNPICON_H

// End of File