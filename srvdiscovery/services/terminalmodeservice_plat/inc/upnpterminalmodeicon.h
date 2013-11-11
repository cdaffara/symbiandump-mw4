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
* Description: CUpnpTerminalModeIcon class declaration
*
*/

#ifndef __UPNPTERMINALMODEICON_H__
#define __UPNPTERMINALMODEICON_H__

#include <e32base.h>   

// CLASS DECLARATION
/**
* This class encapsulates an icon used in any operation of the 
* Terminal Mode Service implemented.
* This represents device and application icons (as provided by Automotive Server).
*/
class CUpnpTerminalModeIcon : public CBase
    {
public:    
    /**
    * Standard Symbian Two-Phase constructor. Returns a pointer to the new element.
    * @param aPath     icon path
    * @param aWidth    icon width
    * @param aHeight   icon height
    * @param aDepth    icon depth
    * @param aMimeType icon mime type
    */    
    IMPORT_C static CUpnpTerminalModeIcon* NewL(const TDesC16& aPath, const TInt aWidth,  
                        const TInt aHeight,const TInt aDepth, const TDesC8& aMimeType );
    /**
    * Destructor. 
    */
    ~CUpnpTerminalModeIcon();
     
public: 
    /**
    * Returns the icon filename
    * @return Returns the icon filename along with complete filepath.
    */
    inline const TDesC& IconFilename()const;
    
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
    * Returns the mime type.
    * @return Returns Icon mime type
    */
    inline const TDesC8& MimeType()const;
    
private:    
    /**
    * C++ default constructor.
    * @param aWidth    icon width
    * @param aHeight   icon height
    * @param aDepth    icon depth 
    */
    CUpnpTerminalModeIcon( const TInt aWidth,const TInt aHeight,const TInt aDepth );
    /**
     * 2nd phase constructor
     * @param aPath     icon path
     * @param aMimeType icon mime type
     */ 
    void ConstructL( const TDesC16& aPath,const TDesC8& aMimeType );

private:    	
    HBufC*                 iFilename;
    TInt                   iIconWidth;
    TInt                   iIconHeight;
    TInt                   iIconDepth;
    HBufC8*                iIconMimeType;
    };

#include <upnpterminalmodeicon.inl>

#endif  //__UPNPTERMINALMODEICON_H__

// End of File
