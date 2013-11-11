/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Text style class.
*
*/



#ifndef C_ALFTEXTSTYLE_H
#define C_ALFTEXTSTYLE_H

#include <e32base.h>
#include <gdi.h>    // for TRgb
#include <alf/alftexture.h>

class CAlfEnv;
class CAlfGenComponent;
class THuiFont;
class CAlfTextStyleManager;

/**
 * CAlfTextStyle presents an abstract text style implementation.
 * Its main purpose is to act as an encapsulation of a particular text style.
 * Usage:
 * @code
 * 
 *  //Get Textstyle manager from environment
 *  CAlfTextStyleManager* styleMgr = &iEnv->TextStyleManager();
 * 
 *  //Get textstyle
 *  TInt styleId = styleMgr->CreatePlatformTextStyleL();
 * 	CAlfTextstyle* style =  styleMgr->TextStyle( styleId );
 * 
 *  //Customise textstyle
 *  style->SetStrikeThrough( ETrue );
 *  style->SetUnderline( ETrue );
 * 
 *  //Assign style to text visual
 *  textVisual->SetTextStyle( style->Id() );
 * 
 *  //Cleanup 
 *  styleMgr->DeleteTextStyle(  styleId );
 *  delete iEnv;
 * 
 * @endcode
 * 
 * @see CAlfTextStyleManager
 * Derived platform dependant classes will implement their own version of this class
 */
NONSHARABLE_CLASS( CAlfTextStyle ): public CBase
    {

public:

    /* Constructors and destructor. */
	
	/**
	 * Destructor.
	 */
	~CAlfTextStyle();
	
	/**
	 * Constructs a new CAlfTextStyle object.
	 *
     * @param aEnv					Current Alf environment.
     * @param aId					The unique ID of this text style. Created by CAlfTextStyleManager.
     * @param aImplementationId 	Describes the type of text style that is created (platform text style, etc.)
     * @param aConstructionParams 	Construction parameters.
	 */
	static CAlfTextStyle* NewL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams);

	/**
	 * Constructs a new CAlfTextStyle object and leaves it into the cleanup stack.
	 *
     * @param aEnv					Current Alf environment.
     * @param aId					The unique ID of this text style. Created by CAlfTextStyleManager.
     * @param aImplementationId 	Describes the type of text style that is created (platform text style, etc.)
     * @param aConstructionParams 	Construction parameters.
	 */
	static CAlfTextStyle* NewLC(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams);
	
    /**
     * Returns the S60 font style id of this text style. Ids are defined in avkon.hrh in 
     * TAknLogicalFontId enumeration.
     * 
     * @return The S60 font style id of this text style.
     */
    TInt FontStyleId() const;

    /**
     * Sets the S60 font style id for this text style. Alf representation of the text style
     * stores this attribute for its own reference. Setting font style id here only
     * sets the font style id in alf private data. Does not set the font style id in
     * hitchcock core. 
     * Ids are defined in avkon.hrh in TAknLogicalFontId enumeration.
     * 
     * @param aFontStyleId The S60 font style id.
     */
    void SetFontStyleId(TInt aFontStyleId);

    /**
     * Returns the parent id of this text style. Text styles can be cascaded
     * so that the child text styles use the parameters of their parents unless
     * the parameters are explicitly overridden in the child.
     * 
     * @return the parent id of this text style.
     */
    IMPORT_C TInt ParentId() const;
    
    /**
     * Sets the parent style for this text style. The parent style has to be created
     * through the CAlfTextStyleManager. Parent Id of a preconfigured text style
     * cannot be changed.
     * @See TAlfPreconfiguredTextStyle
     * 
     * @param aParentId the parent style for this text style
     * @see CAlfTextStyleManager
     */
    IMPORT_C void SetParentId(TInt aParentId);
    
    /**
     * Returns the id of the text style. This id is set by the CAlfTextStyleManager.
     * 
     * @return The unique id of this text style.
     */
    IMPORT_C TInt Id() const;
    
    /**
     * Returns the text color of this text style.
     * 
     * @return A TRgb object that represents the color of the text used with this
     *         text style.
     */
    IMPORT_C TRgb TextColor() const;
    
    /**
     * Sets the color of the text rasterized with this style. Text color of a preconfigured 
     * text style cannot be changed.
     * @See TAlfPreconfiguredTextStyle     
     * 
     * @param aTextColor The color of the text to be set.
     */
    IMPORT_C void SetTextColor(const TRgb& aTextColor);
    
	/**
	 * Gets text style text size attribute in twips.
	 * 
 	 * @param aIsDecoratedSize If true, the decoration size (the area reserved for decoration) 
 	 *                         is added to the actual text size. The decoration here means
 	 *                         various effects that can be added to the text, for example a 
 	 *                         shadow.
 	 *
	 * @return The actual text size in twips plus the decoration size if decorated size
	 *         was requested.      .
	 */
	IMPORT_C TInt TextSizeInTwips(TBool aIsDecoratedSize = EFalse) const;
	
    /**
     * Sets the text size of this style in screen size independent units (twips). Text size
     * of a preconfigured text style cannot be changed.
     * @See TAlfPreconfiguredTextStyle     
     * 
     * @param aTextSizeInTwips Size of the text in twips.
 	 * @param aIsDecoratedSize If true, the decoration size is subtracted from the text size
 	 *                         to ensure that the rasterized text will fit into the reserved
 	 *                         area including the decoration. The decoration here means various
 	 *                         effects that can be added to the text, for example a shadow .
     */    
    IMPORT_C void SetTextSizeInTwips(TInt aTextSizeInTwips, TBool aIsDecoratedSize = EFalse);
    
	/**
	 * Gets text style text size attribute in pixels.
	 * 
 	 * @param aIsDecoratedSize If true, the decoration size (the area reserved for decoration) 
 	 *                         is added to the actual text size. The decoration here means
 	 *                         various effects that can be added to the text, for example a 
 	 *                         shadow.
 	 *
	 * @return The actual text size in pixels plus the decoration size if decorated size
	 *         was requested.
	 */
	IMPORT_C TInt TextSizeInPixels(TBool aIsDecoratedSize = EFalse) const;
	
    /**
     * Sets the text size of this style in pixels. Text size of a preconfigured 
     * text style cannot be changed.
     * @See TAlfPreconfiguredTextStyle
     * 
     * @param aTextSizeInPixels Size of the text in pixels.
 	 * @param aIsDecoratedSize If true, the decoration size is subtracted from the text size
 	 *                         to ensure that the rasterized text will fit into the reserved
 	 *                         area including the decoration. The decoration here means various
 	 *                         effects that can be added to the text, for example a shadow .
     */    
    IMPORT_C void SetTextSizeInPixels(TInt aTextSizeInPixels, TBool aIsDecoratedSize = EFalse);
    
    /**
     * Indicates whether the text is rasterized in bold or normal.
     * 
     * @return Boolean indicating whether text is bold or not.
     */    
    IMPORT_C TBool IsBold() const;
    
    /**
     * Sets the bold on and off in the text style. Setting bold on and off is
     * not possible for a preconfigured text style.
     * @See TAlfPreconfiguredTextStyle
     * 
     * @param aIsBold True for bold and false normal.
     */    
    IMPORT_C void SetBold(TBool aIsBold);
    
    /**
     * Indicates whether the text is rasterized in italic or normal.
     * 
     * @return Boolean indicating whether text is italic or not.
     */    
    IMPORT_C TBool IsItalic() const;
    
    /**
     * Sets the italic on and off in the text style. Setting italic on and off is
     * not possible for a preconfigured text style.
     * @See TAlfPreconfiguredTextStyle
     * 
     * @param aIsItalic True for italic and false for normal.
     */    
    IMPORT_C void SetItalic(TBool aIsItalic);
    
    /**
     * Indicates whether the text is underlined or not.
     * 
     * @return Boolean indicating whether text is underlined.
     */    
    IMPORT_C TBool IsUnderline() const;
    
    /**
     * Sets the underlining on and off in the text style. Setting underline on and off is
     * not possible for a preconfigured text style.
     * @See TAlfPreconfiguredTextStyle
     * 
     * @param aIsUnderline True for underlined and false for normal text.
     */    
    IMPORT_C void SetUnderline(TBool aIsUnderline);
    
    /**
     * Indicates whether the text is struck through or not.
     * 
     * @return Boolean indicating whether text has a strike through or not.
     */        
    IMPORT_C TBool IsStrikeThrough() const;
    
    /**
     * Sets the strike through on and off in the text style. Setting strike through on 
     * and off is not possible for a preconfigured text style.
     * @See TAlfPreconfiguredTextStyle
     * 
     * @param aIsStrikeThrough True for text with strike through and false for normal text.
     */    
    IMPORT_C void SetStrikeThrough(TBool aIsStrikeThrough);

    /**
     * Gets the typeface in use for the text style.
     * The typeface may be blank.
     * 
     * @param on return, contains the typeface information.
     */    
    void GetTypeface(TTypeface& aTypeface) const;

    /**
     * Gets the typeface in use for the text style.
     * The typeface may be zero length, but the pointer returned will not be NULL
     * 
     * @return a descriptor on the heap containing the typeface name, owned by the caller.
     */    
    IMPORT_C HBufC* TypefaceNameL() const;
    
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
     *          to ensure that the rasterized text will fit into the reserved area including the 
     *          decoration. The decoration here means various effects that can be added to the 
     *          text, for example a shadow.
     */    
     IMPORT_C void SetTextPaneHeightInPixels(TInt aTextPaneHeight, TBool aIsDecoratedSize = EFalse);
            
    /**
     * Return associated CAlfGenComponent object.
     */
    CAlfGenComponent* Comms() const;
    
    /**
     * Return associated serverside object.
     */
    TInt ServerHandle() const;

#ifdef ALF_RASTER_TEXT    
public: // internal utils

    THuiFont* Font() const; // returns used font, either own ir parent style
    THuiFont* OwnFont(); // ensures that style has its own font override and returns that
    void SetFont(THuiFont* aFont);
    void ReportChanged(); // propagate changes on mesh (asynch)
    void RasterizeLineL(const TDesC& aTextLine, CAlfTexture** aTargetTexture); // uploads rasterized texture to scene
    TSize LineExtentsL(const TDesC& aTextLine); // measures the extents for given text
    void SetManager(CAlfTextStyleManager* aManager);
#endif    
protected:
	/**
	 * Constructor.
	 */
	CAlfTextStyle();	
	
    /**
     * Second phase constructor.
     */
    void ConstructL(CAlfEnv& aEnv, TInt aId, TInt aImplementationId, const TDesC8& aConstructionParams);	
	    
private:
	
    // Private data. Own. 
    struct TPrivateData;
    TPrivateData* iData;	
	
    };

#endif // C_ALFTEXTSTYLE_H
