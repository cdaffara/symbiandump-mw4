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
* Description:  SVG Implementation header file
 *
*/


#include "SvgBitmapFontProvider.h"


// ---------------------------------------------------------------------------
// Two phase constructor for this class
// 
// ---------------------------------------------------------------------------
CSvgBitmapFontProvider* CSvgBitmapFontProvider::NewL( const TFontSpec& aFontSpec )
    {
    CSvgBitmapFontProvider* self = NewLC( aFontSpec );
    CleanupStack::Pop();
    return self;
    }


// --------------------------------------------------------------------------
// CSvgBitmapFontProvider* CSvgBitmapFontProvider::NewLC()
// ---------------------------------------------------------------------------
CSvgBitmapFontProvider* CSvgBitmapFontProvider::NewLC( const TFontSpec& aFontSpec )
    {
    CSvgBitmapFontProvider* self  = new ( ELeave ) CSvgBitmapFontProvider( aFontSpec );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


//----------------------------------------------------------------------------
// First Step of two phase construction for this class
//----------------------------------------------------------------------------
CSvgBitmapFontProvider::CSvgBitmapFontProvider( const TFontSpec& aFontSpec ):
                                                          iTypeFaceStore(NULL),
                                                          iFontSpec(aFontSpec)
	{
	
	}

// --------------------------------------------------------------------------
// CSvgBitmapFontProvider::ConstructL()
// ---------------------------------------------------------------------------
void CSvgBitmapFontProvider::ConstructL()
    {
    	iTypeFaceStore = CFbsTypefaceStore::NewL(NULL);
    }

// --------------------------------------------------------------------------
// CSvgBitmapFontProvider::ConstructL()
// ---------------------------------------------------------------------------
CSvgBitmapFontProvider::~CSvgBitmapFontProvider()
	{
			delete iTypeFaceStore ;				
	}

// --------------------------------------------------------------------------
// CSvgBitmapFontProvider::ReleaseFont(CFont* aFont)
// ---------------------------------------------------------------------------
void CSvgBitmapFontProvider::ReleaseFont(CFont* aFont)
{
	iTypeFaceStore->ReleaseFont(aFont);
}

// --------------------------------------------------------------------------
// CSvgBitmapFontProvider::GetNearestFontToDesignHeightInTwips(CFont* aFont, TFontSpec aFontSpec)
// ---------------------------------------------------------------------------
TInt CSvgBitmapFontProvider::GetNearestFontToDesignHeightInTwips(CFont*& aFont, TFontSpec aFontSpec)
{
    TName fontTypefaceName(aFontSpec.iTypeface.iName);
    
	TInt matchLength = fontTypefaceName.Length();                                
                                
    TInt typefaceCount = iTypeFaceStore->NumTypefaces();
    TTypefaceSupport typefaceSupport;
    
    TInt i;
    for(i=0; i<typefaceCount; i++)
        {
        iTypeFaceStore->TypefaceSupport(typefaceSupport, i);
        TInt findResult = typefaceSupport.iTypeface.iName.Find(fontTypefaceName.Ptr(), matchLength);
        if(!findResult)
            {
            fontTypefaceName = typefaceSupport.iTypeface.iName;
            break;
            }
        }
        
    //If the element typeface couldn't find && didn't set the typeface name.
    if(( i == typefaceCount) && (iFontSpec.iTypeface.iName.Length()))
        {
        fontTypefaceName = iFontSpec.iTypeface.iName;
        }

    aFontSpec.iTypeface.iName = fontTypefaceName;        
	return iTypeFaceStore->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
}

