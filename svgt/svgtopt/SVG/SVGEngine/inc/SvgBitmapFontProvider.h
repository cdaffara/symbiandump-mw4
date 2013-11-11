/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Bitmap Font Provider header file
 *
*/


#ifndef SVGBITMAPFONTPROVIDER_H
#define SVGBITMAPFONTPROVIDER_H


#include <fbs.h>
#include <gdi.h>

/**
 * Class description 
 */
NONSHARABLE_CLASS( CSvgBitmapFontProvider ) : public CBase
    {
    
    private:

		/*
		* First Step of two phase construction for this class
		* 
		*/
		CSvgBitmapFontProvider( const TFontSpec& aFontSpec ) ;
		
		/*
		* Second Step of two phase construction for this class
		* 
		*/
    	void ConstructL();
    	
    	    	
    public:
        
        /*
		* Destructor
		* 
		*/
    	~CSvgBitmapFontProvider();
    	
        /*
		* Creates & returns object of this class
		* 
		*/
        static CSvgBitmapFontProvider* NewL( const TFontSpec& aFontSpec );

		/*
		* Creates & returns object of this class
		* 
		*/
        static CSvgBitmapFontProvider* NewLC( const TFontSpec& aFontSpec );
        
        /**
         * Releases a Font
         *
         * @since 1.0
         * @param aFont - Pointer to the font of type CFont* to be released
         */
        void ReleaseFont(CFont* aFont);
        
        /**
         * Releases a Font
         *
         * @since 1.0
         * @param aFont - Pointer to the font where the new font information has to be filed in
         * @param aFontSpec - Object representing Font Specification(s)
         */    
        TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, TFontSpec aFontSpec);
         
        
    //Data Members    
    private:
    		CFbsTypefaceStore *iTypeFaceStore;
    		TFontSpec iFontSpec;
    };

#endif

