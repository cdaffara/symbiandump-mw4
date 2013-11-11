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
* Description:   Text visual
*
*/



#ifndef C_ALFTEXTVISUAL_H
#define C_ALFTEXTVISUAL_H

class TAknsItemID;
class TRgb;

#include <alf/alfvisual.h>
#include <alf/alftextstylemanager.h>
#include <alf/alfgc.h>
#include <AknFontSpecification.h> 
#include <AknFontIdOffsets.hrh>

/** Background type suggestions.
 * 
 * These enums suggest which sort of background is under the visual,
 * in order for it to be rendered in an appropriate colour.
 * A dark background means that the text will be rendered in white, and a
 * light background means that the text will be black.*/
enum TAlfBackgroundType
    {
    /** background beneath the font is light. */
    EAlfBackgroundTypeLight,
    
    /** background beneath the font is dark. */
    EAlfBackgroundTypeDark
    };

/**
 *  Text visual for text drawing.
 * Text visual shows one or more lines of text.
 *
 * @code
 * CAlfTextVisual* textVisual = CAlfTextVisual::AddNewL( control );
 * textVisual->SetTextL( _L( "Hello" ) ); * 
 * @endcode
 * 
 * @lib alfclient.lib
 * @since S60 v3.2
 */
class CAlfTextVisual : public CAlfVisual
    {

public:

    /* Types. */
    
    enum TLineWrap
        {
        /** Lines must be broken manually with line break characters. 
            If the visual does not use clipping, long text lines will 
            go outside the visual. */
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


    /**
     * Constructor, which gives ownership to the control.
     *
     * @param aOwnerControl The control
     * @param aParentLayout If given, the parent layout.
     * @return New instance. Ownership NOT transreffed (owned by control)
     */
    IMPORT_C static CAlfTextVisual* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfTextVisual();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfTextVisual();

    /**
     * Set the text shown in the visual. The text may contain formatting
     * codes to change the size, color, and alignment of the text.
     *
     * @param aText  Descriptor containing the text.
     */
    IMPORT_C void SetTextL(const TDesC& aText);
    
    /**
     * Returns the text of the visual.
     *
     * @return The text.
     */
    IMPORT_C const TDesC& Text() const;
    
    /**
     * Sets the default text style of the text visual. This style describbes the size
     * and colour of the rendered text. Parameters passed here can be overridden by
     * call to SetFontSpec().
     * @see SetFontSpec()
     *
     * @param aStyle           Text style.
     * @param aBackgroundType  Text background type. Affects the color of 
     *                         the text. Defaults to EAlfBackgroundTypeLight
     *                         (ie, black text will be used).
     */
    IMPORT_C void SetStyle(
        TAlfPreconfiguredTextStyle aStyle, 
        TAlfBackgroundType aBackgroundType = EAlfBackgroundTypeLight);
        

    /**
     * Sets the default text style of the text visual. This style describes the style
     * the text is rendered with. The text style has to be created with the text style 
     * manager. If the given text style does not exist, the text style is reverted back
     * to default text style.
     * @see CAlfTextStyleManager
     *
     * @param aTextStyleId Text style ID provided by the text style manager.
     */
    IMPORT_C void SetTextStyle(TInt aTextStyleId);
            
    /**
     * Sets the alignment of the text visual.
     *
     * @param aAlignHorizontal Horizontal alignment.
     * @param aAlignVertical Vertical alignment.
     */
    IMPORT_C void SetAlign(TAlfAlignHorizontal aAlignHorizontal,
                           TAlfAlignVertical aAlignVertical);
                           
    /**
     * Sets the line spacing for multiline text visual when the text wraps around.
     *
     * @param aLineSpacing Line spacing between lines in multiline text visual.
     * @parem aUnits       Defines the units of the line spacing.
     *                     @see TLineSpacingUnits. 
     */
    IMPORT_C void SetLineSpacing(TInt aLineSpacing, TLineSpacingUnits aUnits = EPixels);

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
     * Determines the size of the text in the visual.
     * Text must have been updated / or drawn to be able
     * to use these extents.
     *
     * @return Size of the text in pixels, or TSize(0,0) if 
     * text is undefined or the mesh hasn't been updated.
     * 
     * @see UpdateMeshL() 
     */
    IMPORT_C TRect SubstringExtents(TUint aStart, TUint aEnd) const;

    /**
     * Accesses the text style of the text visual.
     *
     * @deprecated
     *
     * @return the text style, but only valid if the text style corresponds to a preconfigured 
     *      text style
     */    
    IMPORT_C TAlfPreconfiguredTextStyle Style();

    /**
     * Accesses the text style id of the text visual.
     *
     * @note This API should be used in preference to @c Style
     *
     * @return the text style id, this can be used to access the actual text style, 
     *      see @c CAlfTextStyleManager::TextStyle
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
    IMPORT_C TAlfBackgroundType BackgroundType();

    /**
     * @deprecated Use CAlfDropShadow
     *
     * Returns shadow opacity.
     *
     * @return Shadow opacity
     */
    IMPORT_C TAlfTimedValue ShadowOpacity();

    /**
     * @deprecated Use CAlfDropShadow
     *
     * Sets shadow opacity.
     * Changes the opacity of the text soft shadow.
     * Currently SetShadowStrength is only implemented in the OpenGL renderer,
     * and will have no effect in BitGDI. 
     *
     * @param aShadowOpacity Shadow opacity
     * @see EnableShadow     
     */
    IMPORT_C void SetShadowOpacity(const TAlfTimedValue& aShadowOpacity);
       
     /**
     * Sets the color of the text.
     *
     * @param aColor           Text color.
     */
    IMPORT_C void SetColor(TRgb aColor);

     /**
     * Sets the color of the text via skin id.
     *
     * @param aID           Skin-id of the color group to use
     * @param aIndex        Index within the color group
     */
     IMPORT_C void SetColor(const TAknsItemID& aId,const TInt aIndex);
    
    /**
     * Sets the color of the text via skin id.
     *
     * @param aTextColorTable    Skin-id name of the color group to use
     * @param aColorIndex        Index within the color group
     */    
    IMPORT_C void SetColor(const TDesC& aTextColorTable,const TDesC& aColorIndex);
    
    /**
     * @deprecated Use CAlfDropShadow
     * 
     * Enables or disables the text shadow.
     * The rasterized text shadow is a special implementation
     * of shadow that looks good with text. If this value is
     * false, then the text will not have a rasterized shadow.
     * 
     * @param aDoEnable True enables rasterized shadows, false disables.
     * @see SetShadowOpacity
     * @see ShadowOpacity     
     */     
     IMPORT_C void EnableShadow(TBool aDoEnable);     
 
    /**
     * Sets offset for the text.
     *
     * @param aOffset text offset.
     */    
     IMPORT_C void SetOffset(const TAlfTimedPoint& aOffset);
     
    /**
     * Gets offset for the text.
     *
     * @return text offset.
     */    
     IMPORT_C TAlfTimedPoint Offset() const;
     
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);
    
    /**
     * Sets highlighting for a part of Text
     * Start and end index of text to be highlighted, are ordinal indices. 
     * In case of multiple language, bidirectional text (e.g. English + Arabic), 
     * mixed text selection for highlight, might give strange results.
     * e.g.TextVisual text :  "E n g l i s h c i b a r A"  ( Arabic script is 
     *      read/written from right to left. Assume 'c i b a r A' is Arabic script )
     *      and highlight range : 5-9.
     *      Expected result : "i s h" and "r A" should get highlighted 
     *      Actual Result:  "i s h c i"
     * Hence, it is recommended to use different text visuals in case of selection of 
     * bidirectional text.
     *                                 
     * @param aStart Startng index of Highlighted text.
     * @param aEnd Ending index of Highlighted text.
     * @param aHighlightColor Background color of highlighted text.
     * @param aHightlightColor Font color of Highlighted text.
     */
    IMPORT_C void SetHighlightRange(TInt aStart, TInt aEnd, TRgb& aHighlightColor, TRgb& aHighlightTextColor);

    /** Internal utils */
    void PrepareForUpdateMesh();
    void UpdateMesh(TBool aSynchronousUpdate = EFalse);     	 
    void ReleaseMesh();
    
protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
 
private:

    struct TTextVisualPrivateData;
    TTextVisualPrivateData* iTextVisualData;
    };


#endif // C_ALFTEXTVISUAL_H
