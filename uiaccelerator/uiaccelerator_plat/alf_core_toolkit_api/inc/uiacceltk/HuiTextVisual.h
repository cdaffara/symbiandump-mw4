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
* Description:   Definition of CHuiTextVisual. CHuiTextVisual is a visual
*                that draws text.
*
*/



#ifndef __HUITEXTVISUAL_H__
#define __HUITEXTVISUAL_H__


#include <e32base.h>
#include <AknsItemID.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>


/* Forward declarations. */
class CHuiControl;
class CHuiTextMesh;
class THuiTextStyle;
class CAknPictographInterface;
class CHuiPictographAnimator;


/**
 * Text visual shows one or more lines of text.
 *
 * @todo  Escape sequences for formatting (font, colors) will be supported.
 * @todo  Changing text style with escape sequences.
 */
NONSHARABLE_CLASS(CHuiTextVisual) : public CHuiVisual
    {
public:

    /* Types. */
    
    enum TLineWrap
        {
        /** Lines must be broken manually with line break characters. 
            If the visual does not use clipping, long text lines will 
            go outside the visual. This is the default mode. */
        ELineWrapManual,
        
        /** Lines that do not fit in the visual's content area are truncated 
            with an ellipsis (...). */
        ELineWrapTruncate,
        
        /** Lines are broken to the visual's content area. */
        ELineWrapBreak
        };

    enum TLineSpacingUnits
        {
        /** Line spacing is given in pixels */
        EPixels,
        
        /** Line spacing is given in twips */
        ETwips
        };


    /* Constructors and destructor. */

    /** @beginAPI */

    /**
     * Constructs and appends a new text visual to the owner control.
     *
     * @param aOwnerControl  Control that will own the new visual.
     *
     * @return  The new visual.
     */
    IMPORT_C static CHuiTextVisual* AddNewL(CHuiControl& aOwnerControl,
                                            CHuiLayout* aParentLayout = 0);

    /** @endAPI */
    
    
    /**
     * Constructor.
     */
    CHuiTextVisual(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CHuiTextVisual();


    /* Methods. */

    /** @beginAPI */
    
    /**
     * Set the text shown in the visual. The text may contain formatting
     * codes to change the size, color, and alignment of the text.
     *
     * @param aText  Descriptor containing the text.
     */
    IMPORT_C void SetTextL(const TDesC& aText);

    /**
     * Returns the text of the visual.
     */
    IMPORT_C const TDesC& Text() const;

    /**
     * Sets the alignment of the text visual.
     */
    IMPORT_C void SetAlign(THuiAlignHorizontal aAlignHorizontal,
                           THuiAlignVertical aAlignVertical);


    /**
     * Sets the line spacing for multiline text visual when the text wraps around.
     *
     * @param aLineSpacing Line spacing between lines in multiline text visual.
     * @parem aUnits       Defines the units of the line spacing.
     *                     @see TLineSpacingUnits. 
     */
    IMPORT_C void SetLineSpacing(TInt aLineSpacing, TLineSpacingUnits aUnits = EPixels);


    /**
     * Sets the default text style of the text visual. This style describes the size
     * and color of the rendered text.
     *
     * @param aStyle           Text style.
     * @param aBackgroundType  Text background type. Affects the color of 
     *                         the text. Defaults to EHuiBackgroundTypeLight
     *                         (ie, black text will be used).
     */
    IMPORT_C void SetStyle(THuiPreconfiguredTextStyle aStyle, 
                           THuiBackgroundType aBackgroundType = EHuiBackgroundTypeLight);

    /**
     * Sets the default text style of the text visual. This style describes the style
     * the text is rendered with. The text style has to be created with the text style 
     * manager. If the given text style does not exist, the text style is reverted back
     * to default text style.
     * @see CHuiTextStyleManager
     *
     * @param aTextStyleId Text style ID provided by the text style manager.
     */
    IMPORT_C void SetTextStyle(TInt aTextStyleId);
    
    /**
     * Accesses the text style of the text visual.
     *
     * If a value has been set by SetFontId which is not in THuiTextStyle, then
     * this will return EHuiTextStyleNotAHuiStyle
     */    
    IMPORT_C THuiPreconfiguredTextStyle Style() const;

    /**
     * Accesses the text style id of the text visual, including non-predefined styles
     *
     * @return text style id
     */    
    IMPORT_C TInt TextStyle() const;

    /**
     * Sets the maximum number of lines displayed by the text visual.
     * The default is KMaxTInt.
     */
    IMPORT_C void SetMaxLineCount(TInt aMaxLineCount);
    
    /**
     * Determines the maximum number of line displayed by the text visual.
     */
    IMPORT_C TInt MaxLineCount() const;
    
    /**
     * Determines the visual's line wrapping mode.
     *
     * @return  Line wrapping mode.
     */
    IMPORT_C TLineWrap Wrapping() const;    
    
    /**
     * Sets the line wrapping mode.
     *
     * @param aWrap  Line wrapping mode.
     */
    IMPORT_C void SetWrapping(TLineWrap aWrap);
    
    /**
     * Determines the background type of the text visual. This was set with a 
     * call to SetStyle().
     */    
    IMPORT_C THuiBackgroundType BackgroundType() const;

    inline THuiTimedPoint& Offset()
        {
        return iOffset;
        }

    inline const THuiTimedPoint& Offset() const
        {
        return iOffset;
        }

    /**
     * Determines the size of the text in the visual.
     * Text must have been updated / or drawn to be able
     * to use these extents.
     *
     * @return Size of the text in pixels, or TSize(0,0) if 
     * text is undefined or the mesh hasn't been updated.
     * 
     * @see UpdateMeshL() 
     */
    IMPORT_C TSize TextExtents() const;

	/**
	 * Prepares text for drawing. Updates the text max line
	 * width according to the wrapping parameters, sets the
	 * text style and updates the text mesh.
	 * 
	 * @see UpdateMeshL()
	 */
	IMPORT_C virtual TBool PrepareDrawL();	

     /**
     * Sets the color of the text.
     *
     * @param aColor           Text color.
     */
    IMPORT_C void SetColor(const TRgb& aColor);

     /**
     * Sets the color of the text via skin id.
     *
     * @param aID           Skin-id of the color group to use
     * @param aIndex        Index within the color group
     */
     IMPORT_C void SetColor(const TAknsItemID& aID,const TInt aIndex);

    /**
     * Get the extents of a block of characters given the visual's
     * current font style.
     *
     * Gets the local pixel extents of a block of text within
     * this text visual.
     *
     * Note that these extents do not take into account line breaks,
     * so it will only return accurate results on single lines of text.
     *
     * @param aStart The starting character of the substring.
     * @param aEnd The end character of the substring.
     */
    IMPORT_C TRect SubstringExtents(TUint aStart, TUint aEnd) const;

    /**
     * @deprecated Use CHuiDropShadow
     *
     * Enables or disables the text shadow.
     * The text shadow is a special implementation
     * of shadow that looks good with text. If this value is
     * false, then the text will not have a shadow.
     * This method will also reset iShadow opacity to 0 or 1.
     * 
     * @param aDoEnable True enables shadows, false disables.
     */
    IMPORT_C void EnableShadow(TBool aDoEnable);    
    
    
    /**
     * Sets highlighting for a part of Text
     *
     * @param aStart Startng index of Highlighted text.
     * @param aEnd Ending index of Highlighted text.
     * @param aHighlightColor Background color of highlighted text.
     * @param aHightlightColor Font color of Highlighted text.
     */
    IMPORT_C void SetHighlightRange(TInt aStart, TInt aEnd, TRgb& aHighlightColor, TRgb& aHighlightTextColor);
 	  
    
    /** @endAPI */

    /**
     * Draw the text visual.
     */
    virtual void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;

    /**
     * Returns true if the text has changed and needs redrawing.
     */
    virtual TBool Changed() const;

    /**
     * Clear the changed flag of the visual, ie, the text has been redrawn.
     */
    virtual void ClearChanged();

    void NotifySkinChangedL();

    void GetClassName(TDes& aName) const
        {
        aName = _L("CHuiTextVisual");
        }
        
    void ExpandRectWithContent(TRect& aRect) const;

     /**
     * Updates the text mesh within this text visual. 
     * Updates the text font and  the text itself (possibly 
     * renders the text to a bitmap), if needed, and 
     * recalculates the text extents.
     */
    void UpdateMeshL() const;

    IMPORT_C void UpdateMeshL(const TDesC8& aBuffer);
    
private: 

private:

    /** Line wrapping mode. */
    TLineWrap iLineWrapping;

    /** Horizontal alignment. */
    THuiAlignHorizontal iAlignHorizontal;

    /** Vertical alignment. */
    THuiAlignVertical iAlignVertical;

    /** The text of drawn in the visual. */
    HBufC* iText; // Owned

    /** ETrue if the mesh has been updated. */
    mutable TBool iMeshUpdated;

    /** ETrue, if the extents have been updated. */
    mutable TBool iExtentsUpdated;

    /** ETrue, if the font and style has been updated. */
    mutable TBool iFontUpdated;

    /** Text mesh containing the text in a format suitable for 
        efficient drawing. */
    CHuiTextMesh* iTextMesh;

    /** Default text style. Affects both font and other features of the text */
    THuiPreconfiguredTextStyle iStyle;
        
    /** Text background type. Affects text style. */
    THuiBackgroundType iBackgroundType;

    /** Offset to the text position. */
    THuiTimedPoint iOffset;

    TRgb iFontColor;
    TAknsItemID iFontColorId;
    TInt iFontColorIndex;
    TBool iFontColorValid;

    /** Pictograph drawing interface */
    CAknPictographInterface* iPictographInterface; 

    /** Pictograph animator */
    CHuiPictographAnimator* iPictographAnimator;
    
    
    TInt iHighlightStart;
    TInt iHighlightEnd;
    TRgb iHighlightColor;
    TRgb iHighlightTextColor;

    
public:

    // @deprecated Use CHuiDropShadow
    THuiTimedValue iShadow;

    };

#endif  // __HUITEXTVISUAL_H__
