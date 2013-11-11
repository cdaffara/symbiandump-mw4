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
* Description:   Font base class.
*
*/



#ifndef __HUIFONT_H__
#define __HUIFONT_H__

#include <AknFontSpecification.h> 

#include <e32base.h>
#include <gdi.h>
#include <bitstd.h>

/**
 * THuiFont is presents an abstract font implementation. It cannot be used on
 * its own. Its main purpose is to act as a handle to a system font and
 * provide metrics to the layout and drawing routines.
 *
 * Different rendering plugins will implement their own version of this class
 * to suit their own functionality.
 */
class THuiFont
    {
public:

    /** @beginAPI */

    /**
     * Default constructor. This instance should be initialised before use.
     */
    IMPORT_C THuiFont();
    
    /**
     * Constructor from a TFontSpec in twips.
     * Since the metrics are mastered in pixels, the passed-in TFontSpec::Height() is converted to 
     * pixels using the default screen device's twips-to-pixel ratio.
     *
     * @param aId       A handle id set into the THuiFont by the Texture manager or skin
     * @param aFontSpec Symbian OS Font Specification to store
     */
    IMPORT_C THuiFont(TInt aId, const TFontSpec& aFontSpec);

    /**
     * Destructor.
     * @todo: T-class shouldn't have destructor to begin with
     */
    IMPORT_C ~THuiFont();

    /**
     * Copy constructor.
     * @param aFont Font to be copied.
     */
    IMPORT_C THuiFont( const THuiFont& aFont );
            
    /**
     * Assignment operator.
     * @param aFont Font to be copied.
     * @return Reference to this instance.
     */
    IMPORT_C THuiFont& operator=( const THuiFont& aFont );

    /* Methods. */

    /**
     * Determines the font's identifier.
     *
     * @return  Font identifier.
     */
    IMPORT_C TInt Id() const;

    /**
     * Returns the font category.
     */
    IMPORT_C TAknFontCategory Category() const;

    /**
     * Returns the Symbian font specification. Always returned in Twips
     * 
     *  @param aMap  Graphics device map containing desired pixel to twips ratio to use
     *  @return Symbian OS Font specification
     */
    IMPORT_C TFontSpec FontSpec(MGraphicsDeviceMap* aMap = NULL) const;    
    
    /** @endAPI */

    /**
     * Sets the font specification.
     */
    IMPORT_C void SetFontSpec(const TFontSpec& aFontSpec);
   
    /**
     * Sets the category.
     */
    IMPORT_C void SetCategory(const TAknFontCategory aCategory);

    /**
     * Required vertical extend of the font. This value is always returned in pixels.
     * The text will fit within a rectangle of this height
     * 
     * @return  text pane height in pixels into which the text is to fit completely
     */
    IMPORT_C TInt TextPaneHeight() const;
    
    /**
     * Required vertical extend of the font. This value is always specified in pixels.
     * The text will fit within a rectangle of this height
     * 
     * @param aTextPaneHeight  text pane height in pixels into which the text is to fit completely
     */
    IMPORT_C void SetTextPaneHeight(TInt aTextPaneHeight);
    
    /**
     * Returns the nearest CFont object representation of this THuiFont object.
     * Creates the CFont object according to specifications in this THuiFont unless
     * one is already created. If CFont object is already created then the already
     * created instance is returned. Releasing of CFont - object is handled by this
     * class.
     *
     * @param aTextMeshYScale  Height scale factor from LCD resolution to TV resolution. @todo: tell more...
     * 
     * @return Closest CFont - object representing this THuiFont.
     */
    IMPORT_C CFont* NearestFontL(TReal32 aTextMeshYScale = 1) __SOFTFP;
    
    /**
     * Rasterizes the given text string to the target graphics context using this font.
     * The caller has to configure the graphics context with parameters to be used in
     * rasterization before calling this method.
     * 
     * @param aTextString 		The text string to be rasterized.
     * @param aTargetContext 	The target graphics context to be used in rasterization.
     */
    IMPORT_C void RasterizeLineL(const TDesC& aTextString, CFbsBitGc& aTargetContext);

    /**
     * Retrieves the dimensions in pixels occupied by rasterization of the given string using
     * this font.
     * 
     * @param aTextString String that is used in calculation of rasterization dimensions.
     * @return Dimensions occupied by rasterization of the text string in pixels.
     */
    IMPORT_C TSize LineExtentsL(const TDesC& aTextString);
    
	/**
	 * Release the Symbian font representation.
	 */
	IMPORT_C void ReleaseFont();
	
private:

    /**
    * This is backporting AknBidiTextUtils::MeasureTextBoundsWidth from S60 3.1 to have that
    * functionality also when running hitchcock on S60 3.0 platform
    *
    * Measures the full horizontal width in pixels of the passed-in text using a particular font, 
    * including in the width any side-bearings of the glyphs at the ends of the text, and any
    * declared "advance" of the run of glyphs. 
    * 
    * It cannot be used for vertical text measurement.
    * 
    * Side-bearings are parts of glyphs that extend left or right from the normal width
    * or "advance" of the glyph. A left side-bearing, for instance, will overlap with a glyph
    * to its left. Another way of thinking about this is that the origin (0,0) of the glyph is 
    * not at its bottom left. 
    *
    * The advance of a run of glyphs is the sum of the advances - once in visual ordering and 
    * shaping has been performed - of all the glyphs. It is defined relative to a drawing origin.
    * 
    * Within runs of text, side-bearings do not contribute to the width of the text. However,
    * at the (visual) ends of text, they are likely to need to be counted, depending upon the
    * exact use case.
    * 
    * This method returns the width of the horizontal envelope of the text by taking the extreme 
    * horizontal extents of the text bounds rectangle (which includes side-bearings on either end)
    * and the extent of the advance. Thus it returns the width of :
    *   Min(<left_text_bound>, 0), Max( <right_text_bound>, <advance>)
    *   
    * This method should be used when the proposed text is going to be drawn using any horizontal
    * CGraphicsContext::DrawText drawing API. 
    *
    * The text can be in visual or logical order.
    *
    * @since 3.1
    * @param aFont  Font to use
    * @param aText  Text to be measured
    * @param aOrder Whether the text provided is in visual or logical order
    * @return       width of the text in pixels.
    */
    TInt THuiFont::MeasureBidiTextBoundsWidth(const CFont& aFont,
                                          const TDesC& aText,
                                          CFont::TMeasureTextInput::TFlags aOrder) const;


    /** Identifier number. Assigned by the texture manager. */
    TInt iId;

    /** Symbian Font specification. */
    TFontSpec iFontSpec;

    // Structure containing Symbian Font instance and reference count.
    struct TFontRef;

    /** Nearest Font object. */
    TFontRef* iFont;
    
    /** This is the vertical space in which the font will fit.
    This measure is alwasy in pixels.
    It may be necessary to use this request the font, rather than via the Fontspec*/
    TInt iTextPaneHeight;
    
    TAknFontCategory iCategory;
    
    TInt iSpare1;
    TInt iSpare2;
    };


#endif // __HUIFONT_H__
