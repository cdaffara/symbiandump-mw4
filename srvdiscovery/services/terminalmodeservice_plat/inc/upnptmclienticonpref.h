/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CUpnpTmClientIconPref class declaration.
*
*/

#ifndef __UPNPTMCLIENTICONPREF_H__
#define __UPNPTMCLIENTICONPREF_H__

#include <e32base.h>   

const TUint KDefaultDepthValue = 24;

// CLASS DECLARATION
/**
 * The class specifies the icon preferences corresponding to each Client
 * Profile.It describes the mime type and the dimensions that client adhering to
 * a particular profile supports. AutomotiveServer sets the icon preferences and
 * sends the same to TM Service when requested.
 */
class CUpnpTmClientIconPref: public CBase
    {
public: 
    /**
    * Two-phased constructor. returns a pointer to the new element.
    * @param aProfileId Client profile ID
    * @param aWidth     icon width
    * @param aHeight    icon height
    * @param aDepth     icon depth
    * @param aMimeType  icon mime type
    */    
    IMPORT_C static CUpnpTmClientIconPref* NewL( TUint aProfileId );
    /**
    * Destructor. 
    */
    ~CUpnpTmClientIconPref();
    
public: 
    // inlined "setter" and "getter" methods
    /**
    * Returns the profileID.
    * @return Returns the profileID of the client profile.
    */
    inline TUint ClientProfileId()const;
    /**
    * Returns the mime type.
    * @return Returns icon mime type
    */
    inline const TDesC8& MimeType()const;
    /**
    * Returns the width of the icon.
    * @return Returns width in pixels or KErrNotFound
    */
    inline TInt Width()const; 
    /**
    * Returns the height of the icon.
    * @return Returns height in pixels or KErrNotFound
    */
    inline TInt Height()const;
    /**
    * Returns the color depth of the icon.
    * @return Returns icon depth or KErrNotFound
    */
    inline TInt Depth()const;
    /**
    * Method is used to provide the preferred mime type of the client.
    * @param aMimeType icon mime type
    */
    IMPORT_C void SetMimeTypeL( const TDesC8& aMimeType ); 
    /**
    * Sets icon width
    * @param aWidth icon width
    */
    inline void SetWidth( const TInt aWidth );
    /**
    * Sets icon height
    * @param aHeight icon height
    */
    inline void SetHeight( const TInt aHeight );
    /**
    * Sets icon depth
    * @param aDepth icon depth
    */
    inline void SetDepth( const TInt aDepth );
  
private:  
    /**
    * C++ default constructor.
    */
    CUpnpTmClientIconPref( TUint aProfileId );
    /**
     * 2nd phase constructor
     */ 
    void ConstructL( );

private: // Data   	
    TUint                  iClientProfileId;
    HBufC8*                iIconMimeType;
    TInt                   iIconWidth;
    TInt                   iIconHeight;
    TInt                   iIconDepth;
    };

#include <upnptmclienticonpref.inl>

#endif  //__UPNPTMCLIENTICONPREF_H__

// End of File
