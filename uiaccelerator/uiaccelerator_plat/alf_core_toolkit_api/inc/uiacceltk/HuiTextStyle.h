/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   A Text style encapsulates a unique text style 
*                in Hitchcock UI Toolkit usage.
*
*/



#ifndef __THUITEXTSTYLE_H__
#define __THUITEXTSTYLE_H__

#include <e32base.h>
#include <uiacceltk/HuiFont.h>
#include <uiacceltk/HuiTexture.h>

class CEikonEnv; // needed to work around inclusion dependency in AknFontProvider.h

/**
 * Constants that are uset to set/reset local definition flags.
 * Local definition flags describe which text style attributes are set/defined
 * locally in this particular text style, and which come from the parent.
 */
const TInt  KTextColorDefined =         0x00000001;
const TInt  KBackgroundColorDefined =   0x00000002;
const TInt  KTextSizeDefined =          0x00000004;
const TInt  KUnderlineDefined =         0x00000008;
const TInt  KStrikeThroughDefined =     0x00000010;
const TInt  KStrokeWeightDefined =      0x00000020;
const TInt  KPostureDefined =           0x00000040;
const TInt  KFontDefined =              0x00000080;

/**
 * THuiTextStyle presents an abstract text style implementation.
 * Its main purpose is to act as an encapsulation of a particular text style.
 *
 * Derived platform dependant classes will implement their own version of this class
 */
class THuiTextStyle 
{
public:
        
    /* Constructors and destructor. */

    /** @beginAPI */
      
    /**
     * Copy constructor.
     */
    IMPORT_C THuiTextStyle( const THuiTextStyle& aTextStyle );
    
	
    /**
     * Returns THuiFont associated with this logical text style.
     * 
     * @return THuiFont associated with this logical text style. 
     */
    IMPORT_C THuiFont& Font();
	
    /**
     * Returns THuiFont associated with this logical text style.
     * 
     * @return THuiFont associated with this logical text style. 
     */
    IMPORT_C const THuiFont& Font() const;
	
    /**
     * Sets a THuiFont object associated with this logical text style.
     * 
     * @param aFont A HuiFont that willl be used in this text style. 
     */
    IMPORT_C virtual void SetFont(THuiFont aFont);

    /**
     * Retrieves a toolkit specific ID of the parent text style. The text styles 
     * are cascaded. The parent of a text style is EHuiTextStyleNormal by default.
     * 
     * @see CHuiTextStyleManager::CreatePlatformTextStyleL
     * 
     * @return A toolkit specific ID of the parent text style.
     */
    IMPORT_C TInt ParentId() const;
	
    /**
     * Sets a toolkit specific ID of the parent text style. The text styles are cascaded.
     * Setting a parent ID will cause the text style to get all the text style attributes
     * that are not locally defined from the parent.
     * 
     * @param aParentId A toolkit specific ID of the parent text style.
     */
    IMPORT_C void SetParentId(TInt aParentId);
	
    /**
     * A unique toolkit specific ID assigned by the text style manager. The text style can be
     * accessed anytime through the CHuiTextStyleManager::TextStyle - method based on this
     * unique ID. The default text style has always ID 0.
     * 
     * @see CHuiTextStyleManager::CreatePlatformTextStyleL
     * 
     * @return A unique toolkit specific ID for this text style.
     */
    IMPORT_C TInt Id() const;
	
	/**
	 * Retrieves the text style text color attribute.
	 * 
	 * @return The text style text color.
	 */
	IMPORT_C TRgb TextColor() const;
	
	/**
	 * Sets the text style text color attribute.
	 * 
	 * @param aTextColour A new text color of the text style.
	 */
	IMPORT_C void SetTextColor(const TRgb& aTextColour);
	
	/**
	 * @todo remove!!!
	 */
	IMPORT_C TRgb BackgroundColor() const;
	
	/**
	 * @todo remove!!!
	 */
	IMPORT_C void SetBackgroundColor(const TRgb& aBackgroundColor);
	
	/**
	 * Retrieves text style text size attribute in twips.
	 * 
 	 * @param aIsDecoratedSize If true, the decoration size (the area reserved for decoration) 
 	 *                         is added to the actual text size. The decoration here means
 	 *                         various effects that can be added to the text, for example a 
 	 *                         shadow.
 	 *
	 * @return The The actual text size in twips plus the decoration size if decorated size
	 *         was requested.      .
	 */
	IMPORT_C TInt TextSizeInTwips(TBool aIsDecoratedSize = EFalse);
	
	/**
	 * Sets the text style text size attribute in twips.
	 * 
	 * @param aTextSizeInTwips The new text style text size in twips.
 	 * @param aIsDecoratedSize If true, the decoration size is subtracted from the text size
 	 *                         to ensure that the rasterized text will fit into the reserved
 	 *                         area including the decoration. The decoration here means various
 	 *                         effects that can be added to the text, for example a shadow .
	 */
	IMPORT_C void SetTextSizeInTwips(TInt aTextSizeInTwips, TBool aIsDecoratedSize = EFalse);

	/**
	 * Retrieves text style text size attribute in pixels.
	 * 
 	 * @param aIsDecoratedSize If true, the decoration size (the area reserved for decoration) 
 	 *                         is added to the actual text size. The decoration here means
 	 *                         various effects that can be added to the text, for example a 
 	 *                         shadow.
 	 *
	 * @return The actual text size in pixels plus the decoration size if decorated size
	 *         was requested.
	 */
	IMPORT_C TInt TextSizeInPixels(TBool aIsDecoratedSize = EFalse);
	
	/**
	 * Sets the text style text size attribute in pixels.
	 * 
	 * @param aTextSizeInPix   The new text style text size in pixels.
 	 * @param aIsDecoratedSize If true, the decoration size is subtracted from the text size
 	 *                         to ensure that the rasterized text will fit into the reserved
 	 *                         area including the decoration. The decoration here means various
 	 *                         effects that can be added to the text, for example a shadow .
	 */
	IMPORT_C void SetTextSizeInPixels(TInt aTextSizeInPix, TBool aIsDecoratedSize = EFalse);
	
	/**
	 * Retrieves text style text stroke weight attribute.
	 * 
	 * @return The text style text stroke weight. If ETrue, the text is bold,
	 *         otherwise normal.
	 */
	IMPORT_C TBool StrokeWeight();
	
	/**
	 * Sets the text style text stroke weight attribute.
	 * 
	 * @param aIsBold The new text style text stroke weight. If ETrue, the text is bold,
	 *                otherwise normal. 
	 */
	IMPORT_C void SetStrokeWeight(TBool aIsBold);
	
	/**
	 * Retrieves text style text posture attribute.
	 * 
	 * @return The text style text posture. If ETrue, the text is bold,
	 *         otherwise normal.
	 */
	IMPORT_C TBool Posture();
	
	/**
	 * Sets the text style text posture attribute.
	 * 
	 * @param aIsItalic The new text style text posture. If ETrue, the text is italic,
	 *                  otherwise upright. 
	 */
	IMPORT_C void SetPosture(TBool aIsItalic);
	
	/**
	 * Retrieves text style text underline attribute.
	 * 
	 * @return The text style text underlining. If ETrue, the text is underlined,
	 *         otherwise normal.
	 */
	IMPORT_C TBool Underline() const;
	
	/**
	 * Sets the text style text underline attribute.
	 * 
	 * @param aIsUnderline The new text style text underline attribute. If ETrue, 
	 * the text is underlined, otherwise normal. 
	 */
	IMPORT_C void SetUnderline(TBool aIsUnderline);
	
	/**
	 * Retrieves text style text strike through attribute.
	 * 
	 * @return The text style text strike through attribute. If ETrue, the text is 
	 *         strike through, otherwise normal.
	 */
	IMPORT_C TBool StrikeThrough() const;
	
	/**
	 * Sets the text style text strike through attribute.
	 * 
	 * @param aIsStrikeThrough The new text style text strike through attribute. 
	 *                         If ETrue, the text is strike through, otherwise normal. 
	 */
	IMPORT_C void SetStrikeThrough(TBool aIsStrikeThrough);	
    
	/**
	 * Retrieves font style id.
	 * 
	 * @return The text font style id.
	 */            
    IMPORT_C TInt FontStyleId() const;
    
    /**
    * Get the typeface that would be used for this text style
    *
    * @param  aTypeface  Returned typeface structure
    */
    IMPORT_C void GetTypeface( TTypeface& aTypeface ) const;

    /**
     * Sets the text style text pane height in pixels. 
     *
     * @note This means that the text size (i.e. the return value from calling @c TextSizeInPixels)
     *     will be in general smaller than the text pane height, as the text pane is intended to match 
     *     the font's maximum extent.
     * @note In order to convert a height from a metric value into pixels, use @c CHuiVisual::LocalToDisplay.
     * 
     * @param aTextPaneHeight   The new text style text pane height in pixels.
     * @param aIsDecoratedSize If true, the decoration size is subtracted from the text pane height
     *      to ensure that the rasterized text will fit into the reserved area including the 
     *      decoration. The decoration here means various effects that can be added to the 
     *      text, for example a shadow.
     */    
     IMPORT_C void SetTextPaneHeightInPixels(TInt aTextPaneHeight, TBool aIsDecoratedSize = EFalse);
    
    /** @endAPI */
    
    /**
     * Constructs a text style that uses font according to font ID provided by font 
     * manager. Uses a default text style when parameters are not set.
     *
     * @param aFontStyleId 	Font ID provided by font manager. If not set the default font
     * 						is used.
     * @param aParentId		The parent text style. Root style is created if the ID is not set.
     */
    THuiTextStyle( TInt aFontStyleId = 0, TInt aParentId = -1 );
       
    /**
     * Rasterizes a single line of text using this style.
     * Rasterization is done to the texture given as a parameter.
     * Caller has to assure that the the texture is of correct dimentions
     * and pixel format.
     * 
     * @todo Text shadow should be a property of the text style (or even a separate brush, maybe?)
     * and not configured in text mesh.
     *
     * @param aTextLine  	 The line of text to rasterize.
     * @param aTargetTexture Reference to a texture that the text is going 
     * 						 to be rasterized to.
     */
    void RasterizeLineL(const TDesC& aTextLine, CHuiTexture& aTargetTexture);

    /**
     * Retrieves the dimensions in pixels occupied by rasterization of the given string using
     * this text style.
     * 
     * @param aTextLine   String that is used in calculation of rasterization dimensions.
     * @return Dimensions occupied by rasterization in pixels.
     */
    TSize LineExtentsL(const TDesC& aTextLine);    
	
    /**
     * Text style manager is setting this ID when a new text style is created.
     * @see CHuiTextStyleManager::CreatePlatformTextStyleL
     * 
     * @param aId A Unique toolkit specific ID for this text style.
     *            This text style can be accessed anytime through the 
     *            CHuiTextStyleManager::TextStyle - method using this ID.
     */
    void SetId(TInt aId);
	
    /**
     * Retrieves the flags that describe which text style attributes are set/defined
     * locally in this particular text style, and which come from the parent.
     * Each flag can be used to set/reset a bit in definitions flags member variable.
     * If a flag, KTextColorDefined for example, is set, it means that the respective 
     * text style attribute is defined locally. If the flag is not set, the respective 
     * attribute value will be looked from the parent.
     * 
     * @see KTextColorDefined as an example.
     * 
     * @return Current text style attribute flags.
     */
    TInt LocalDefinitionFlags() const;

    /**
     * Sets the flags that describe which text style attributes are set/defined
     * locally in this particular text style, and which come from the parent.
     * @see SetId
     * 
     * @param aLocalDefinitionFlags Will set all the text style attribute flags.
     */
    void SetLocalDefinitionFlags(TInt aLocalDefinitionFlags);
    
    void  EnableClipping(TRect aClipRect);
	void  DisableClipping();
	
protected:

    THuiFont iFont;
	
    /**
     * Font style ID
     */
    TInt  iFontStyleId; 
    
    /**
     * Local definition flags
     */
    TInt  iLocalDefinitionFlags;

private:

    /**
     * Toolkit specific ID of the parent text style. The text styles are cascaded.
     * The parent of a text style is EHuiTextStyleNormal by default.
     */
    TInt  iParentId;
    
    /**
     * A unique toolkit specific ID for the text style. The text style can be accessed 
     * anytime through the CHuiTextStyleManager based on this unique ID. 
     * The default text style has always ID 0.
     */
    TInt  iId;
    
    TRgb  iTextColor;
    TRgb  iBackgroundColor;
    TBool iIsUnderline;
    TBool iIsStrikeThrough;
    
    TBool iClippingEnabled;
    TRect iClipRect;
};

#endif  //__THUITEXTSTYLE_H__






