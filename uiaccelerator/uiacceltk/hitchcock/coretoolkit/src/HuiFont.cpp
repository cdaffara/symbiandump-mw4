/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   THuiFont implementation
*
*/



#include <coemain.h>
#include <w32std.h>
#include <AknBidiTextUtils.h>
#include <AknFontAccess.h>
#include <AknLayoutFont.h>

#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiUtil.h"

const TInt KHuiFontVerticalShiftInPixels = 1;

/**
 * Structure containing Symbian Font instance and reference count.
 */
NONSHARABLE_STRUCT( THuiFont::TFontRef )
    {
    /**
     * Constructor. Initializes both member variables to zero.
     */
    inline TFontRef();

    /** Symbian Font instance. */
    CFont* iFontInstance;
    /** Reference count */
    TInt iRefCount;
    };

EXPORT_C THuiFont::THuiFont(TInt aId, const TFontSpec& aFontSpec)
        : iId(aId), iFontSpec(aFontSpec), iFont(NULL), iTextPaneHeight(0), iCategory(EAknFontCategoryUndefined)
    {
    }

EXPORT_C THuiFont::THuiFont()
		: iId(0), iFont(NULL), iTextPaneHeight(0), iCategory(EAknFontCategoryUndefined)
	{
	}
     
EXPORT_C THuiFont::~THuiFont()
    {
	ReleaseFont();
	iFont = NULL;
    }

EXPORT_C THuiFont::THuiFont( const THuiFont& aFont )
    : iId( aFont.iId ), iFontSpec( aFont.iFontSpec ), iFont( aFont.iFont ), 
      iTextPaneHeight( aFont.iTextPaneHeight ), iCategory( aFont.iCategory )  
    {
    if ( iFont )
        {
        iFont->iRefCount++;
        }
    }
            
EXPORT_C THuiFont& THuiFont::operator=( const THuiFont& aFont )
    {
    if ( this == &aFont )
        {
        return *this;
        }
        
    ReleaseFont();
        
    iId = aFont.iId;
    iFontSpec = aFont.iFontSpec;
    iFont = aFont.iFont;
    if ( iFont )
        {
        iFont->iRefCount++;
        }

    iTextPaneHeight = aFont.iTextPaneHeight;
    iCategory = aFont.iCategory;
    return *this;
    }


EXPORT_C TInt THuiFont::Id() const
    {
    return iId;
    }


EXPORT_C TAknFontCategory THuiFont::Category() const
    {
    return iCategory;
    }


EXPORT_C void THuiFont::SetCategory(const TAknFontCategory aCategory)
    {
    iCategory = aCategory;
    ReleaseFont();
    }

EXPORT_C TInt THuiFont::TextPaneHeight() const
    {
    return iTextPaneHeight;
    }

EXPORT_C void THuiFont::SetTextPaneHeight(TInt aTextPaneHeight)
    {
    iTextPaneHeight = aTextPaneHeight;
    ReleaseFont();
    }

EXPORT_C void THuiFont::SetFontSpec(const TFontSpec& aFontSpec)
    {
    iFontSpec = aFontSpec;
    ReleaseFont();
    }


EXPORT_C TFontSpec THuiFont::FontSpec(MGraphicsDeviceMap* /*aMap*/) const
    {
    // @Todo need to implement the zooming here
    return iFontSpec;
    }


EXPORT_C CFont* THuiFont::NearestFontL(TReal32 aTextMeshYScale) __SOFTFP
	{
    /// @todo  Accessing the screen device during a display resizing event may
    ///        result in a font that is suitable for the display size that
    ///        was in use prior to the resize. Probably we should use
    ///        AknLayoutUtils here.

    if(!iFont)
	    {
        CWsScreenDevice* screenDev = 0;    
        if (CCoeEnv::Static())
            {
            screenDev = CCoeEnv::Static()->ScreenDevice();
            }
        else
            {
            screenDev = CHuiStatic::ScreenDevice();
            }
	    User::LeaveIfNull(screenDev);
	    
        TFontSpec spec = iFontSpec;
                   
        if(aTextMeshYScale != 1)
            {
            spec.iHeight = HUI_ROUND_FLOAT_TO_INT( TReal32(iFontSpec.iHeight) * aTextMeshYScale );
            }

        TFontRef* font = new (ELeave) TFontRef;
        CleanupStack::PushL( font );
        font->iRefCount++;
        
        if (!CCoeEnv::Static())
             {
             _LIT(KS60Digital, "Series 60 ZDigi"); // we assume that only FPS counter uses texts inside hui
             spec = TFontSpec(KS60Digital, 150);         
             User::LeaveIfError( screenDev->GetNearestFontInTwips(font->iFontInstance, spec) );
             iTextPaneHeight = screenDev->VerticalTwipsToPixels(spec.iHeight);
             }
        else
            {
            TAknFontSpecification aknFs(Category(), spec, screenDev);
            aknFs.SetTextPaneHeight( screenDev->VerticalTwipsToPixels(spec.iHeight));
            aknFs.SetTextPaneHeightIsDesignHeight( ETrue );
            font->iFontInstance = 
                AknFontAccess::CreateLayoutFontFromSpecificationL( 
                        *screenDev, 
                        spec.iTypeface,
                        aknFs );
            }    
        CleanupStack::Pop( font );
        iFont = font;
		}
    return iFont->iFontInstance;
	}


EXPORT_C void THuiFont::RasterizeLineL(const TDesC& aTextString, CFbsBitGc& aTargetContext)
	{
    // Retrieve the CFont object used when rasterizing this text mesh.
    CFont* font = NearestFontL();

    // Draw the text
    aTargetContext.UseFont(font);
    
    TInt avkonMaxAscent = 0;
    if (CCoeEnv::Static())
        {
        const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( font );
        avkonMaxAscent = layoutFont->MaxAscent() + KHuiFontVerticalShiftInPixels;
        }
    else
        {
        avkonMaxAscent = iTextPaneHeight; 
        }
    aTargetContext.DrawText(aTextString, TPoint(0,  avkonMaxAscent ));
    aTargetContext.DiscardFont();
	}

EXPORT_C TSize THuiFont::LineExtentsL(const TDesC& aTextString)
	{
	if (aTextString.Length() == 0)
    	{
    	// Zero length strings have extents of zero.
        return TSize(0, 0);
    	}
	
    // Retrieve the CFont object used when rasterizing this text mesh.
	CFont* font = NearestFontL();
    TSize textSize( MeasureBidiTextBoundsWidth(*font, aTextString, CFont::TMeasureTextInput::EFVisualOrder), 
                    font->FontMaxHeight());
    
    const CAknLayoutFont* layoutFont = 0;
    if (CCoeEnv::Static())
        {
        layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( font );
        }
    
    if ( layoutFont )
        {
        textSize.iHeight = font->HeightInPixels();
        TInt textPaneHeight = layoutFont->TextPaneHeight();
        TInt textPaneTopToBaseline = layoutFont->TextPaneTopToBaseline();
        
        textSize.iHeight += textPaneHeight - textPaneTopToBaseline;
        textSize.iHeight += KHuiFontVerticalShiftInPixels;
        }
    else
        {
        textSize.iHeight = Max(textSize.iHeight, font->HeightInPixels());
        textSize.iHeight += 3; // the best approximation - fails on big (>=72) fonts
        }
        
    // Return the calculated text size.
    return textSize;
	}

TInt THuiFont::MeasureBidiTextBoundsWidth(
    const CFont& aFont,
    const TDesC& aText,
    CFont::TMeasureTextInput::TFlags aOrder) const
    {
    CFont::TMeasureTextInput input; 
    input.iFlags = aOrder;
    CFont::TMeasureTextOutput output; 
    
    TInt textAdvance = aFont.MeasureText( aText, &input, &output );
	
	TRect bounds = output.iBounds;
	bounds.iTl.iX = Min(bounds.iTl.iX, 0);
	bounds.iBr.iX = Max(bounds.iBr.iX, textAdvance);    
    return bounds.Width();
    }

EXPORT_C void THuiFont::ReleaseFont()
	{
	if(iFont)
		{
        iFont->iRefCount--;
        if ( !iFont->iRefCount )
            {
            // No more references to iFont instance, we can release 
            // Symbian Font instance & delete TFontRef object.

            const CAknLayoutFont* layoutFont = 0;
            if (CCoeEnv::Static())
                {
                layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( iFont->iFontInstance );
                }
            
            if ( layoutFont )
                {
                delete layoutFont;
                }
            else
                {
                CWsScreenDevice* screenDev = 0;    
                if (CCoeEnv::Static())
                    {
                    screenDev = CCoeEnv::Static()->ScreenDevice();
                    }
                else
                    {
                    screenDev = CHuiStatic::ScreenDevice();
                    }

	            screenDev->ReleaseFont( iFont->iFontInstance );
                }
            delete iFont;
            }
        iFont = NULL;
		}		
	}

// Implementation of THuiFont::TFontRef:

inline THuiFont::TFontRef::TFontRef()
    : iFontInstance( NULL ), iRefCount( 0 )
    {
    }

