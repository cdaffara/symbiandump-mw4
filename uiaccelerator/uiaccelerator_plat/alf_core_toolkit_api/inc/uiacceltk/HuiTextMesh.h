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
* Description:   Definition of CHuiTextMesh. CHuiTextMesh stores a cached 
*                version of a text string.
*
*/



#ifndef __HUITEXTMESH_H__
#define __HUITEXTMESH_H__


#include <e32base.h>
#include <AknFontSpecification.h> 

#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiFont.h>
#include <uiacceltk/huitextstyle.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiEnv.h>


/* Forward declarations. */
class CHuiGc;
class CHuiTexture;
class CAknPictographInterface;

/**
 * CHuiTextMesh the abstract base class for text meshes. Text meshes store one or more 
 * lines of text in a format suitable for rendering using a low-level graphics API.
 * The rendering plugin is responsible for choosing the appropriate concrete text
 * mesh implementation when a new text mesh needs to be created.
 */
NONSHARABLE_CLASS(CHuiTextMesh) : public CBase,
                                  public MHuiDisplayVisibleAreaObserver, 
                                  public MHuiDisplayDeletionObserver,
                                  public MHuiActionObserver
    {
public:    

    /* Types. */
    
    /** Line wrapping and truncation modes. */
    enum TLineMode
        {
        /** Lines are truncated if they don't fit into the maximum line width.
            Line breaks must be explicitly defined with line break characters. */
        ELineModeTruncate,
        
        /** Automatic line wrapping. Text is wrapped onto multiple lines if it doesn't
            fit into the maximum line width. If there are more lines than 
            allowed by the maximum line count, the extra lines are truncated. 
            Line break characters will also cause line breaks. */
        ELineModeWrap
        };


    /* Constructors and destructors. */

    /** 
     * Destructor.
     */
    virtual ~CHuiTextMesh();           

    
    /* Methods. */

    /**
     * Resets the contents of the text mesh.
     */
    virtual void Reset();
    
    
    /**
    * Deletes textures for all rasterized lines.
    * Derived class should supply an implementation.
    */
    virtual void ResetLines() {}
    
    
    /**
    * Deletes textures for all rasterized pictograph lines.
    * Derived class should supply an implementation.
    */
    virtual void ResetPictographLines() {}


    void SetLineMode(TLineMode aLineMode);
    
    TLineMode LineMode() const;
    
    /**
     * Sets the maximum line width, in pixels. Does not rebuild the 
     * mesh.
     *
     * @param aMaxLineWidth  Maximum line width.
     *
     * @return  ETrue, if the new width is different than the previous.
     *          The mesh should be rebuilt in that case.
     */
    TBool SetMaxLineWidth(TInt aMaxLineWidth);
    
    /**
     * Returns the maximum line width.
     */    
    TInt MaxLineWidth() const;
    
    /**
     * Sets the maximum line count. Does not rebuild the mesh.
     *
     * @param aMaxLineCount  Maximum line count.
     *
     * @return  ETrue, if the new count is different than the previous.
     *          The mesh should be rebuilt in that case.
     */
    TBool SetMaxLineCount(TInt aMaxLineCount);

    /**
     * Returns the maximum line count.
     */    
    TInt MaxLineCount() const;

    /**
     * Selects the default font used by the text mesh.
     *
     * @param aFont  Font texture.
     */
    void SetFontL(const THuiFont& aFont, TBool aRefreshTextures);

    /**
     * Prepares the text mesh with a string of text.
     *
     * @param aText       One or more strings of text.
     * @param aRasterize  EFalse when the extents need to be recalculated,
     *                    but the text doesn't need to be re-rendered.
     *
     * @todo  Support formatting escape sequences.
     */
    void SetTextL(const TDesC& aText, TBool aRasterize);
    
    /**
     * Returns the text string of the mesh.
     *
     * @return  Pointer to descriptor, or <code>NULL</code>.
     */
    const TDesC* Text() const;
    
    /**
     * Sets the distance of the sharp shadow. The sharp shadow is a 
     * black version of the text drawn with an offset.
     */
    void EnableRasterizedShadow(const TBool aIsEnabled);

    /**
     * Determines if the rasterized shadow is enabled.
     */
    TBool RasterizedShadow() const;

    /**
     * Returns the extents of the text.
     */
    TSize Extents() const;    
    
    /**
     * Draws the text mesh. 
     *
     * @param aGc             Graphics context.
     * @param aShadowTexture  Texture to use for rendering shadows.    
     * @param aShadowOpacity  Opacity of shadows.
     */     
    virtual void Draw(CHuiGc& aGc, TReal32 aShadowOpacity = 0.f) const __SOFTFP = 0;

    /**
     * Sets the text style of the text mesh. This style describes the style
     * the text is rendered with. The text style has to be created with the text style 
     * manager. If the given text style does not exist, the text style is reverted back
     * to default text style.
     * @see CHuiTextStyleManager::CreateTextStyleL
     *
     * @param aTextStyleId Text style ID provided by the text style manager.
     */
    void SetTextStyle(TInt aTextStyleId);

    /**
     * Retrieves the text style of the text mesh. This style describes the style
     * the text is rendered with. 
     * @see CHuiTextStyleManager::SetTextStyle
     *
     * @param Text style ID provided by the text style manager.
     */
    TInt TextStyle() const;
    
    /** 
     * Initializes the pictograph support.
     *
     * @param aInterface Interface that can be used for drawing pictographs.
     */
    virtual void InitPictographsL(CAknPictographInterface* aInterface);

    /**
     * Updates the mesh pictographs.
     */
    virtual void BuildPictographsL();

    /**
     * Sets the line spacing for multiline text visual when the text wraps around.
     *
     * @param aLineSpacing Line spacing between lines in multiline text visual.
     */
    void SetLineSpacing(TInt aLineSpacingInPixels);
    
    
    /** 
    * Calculate scale factors from LCD resolution to TvOut resolution 
    */
    void CalculateTvOutScales();
    
    /** 
     * Sets the realted visual
     *
     * @param aVisual which owns this text mesh
     */
    void SetRelatedVisual(CHuiVisual* aVisual);
    
    /* From MHuiDisplayVisibleAreaObserver */
    /**
    * Display size observer. Is notified when the display is resized.
    *
    * @param aDisplay  Display that sends the notification.
    */
    void NotifyDisplayVisibleAreaChanged(CHuiDisplay& aDisplay);
    
    /* From MHuiDisplayDeletionObserver */
    /**
    * Display deletion observer. Is notified when the display is about to be
    * destroyed.
    *
    * @param aDisplay  Display that sends the notification.
    */
    void NotifyDisplayDeletion(CHuiDisplay& aDisplay);
    
    /* From MHuiActionObserver */
    /**
     * Called by the scheduler when an action command is executed.
     *
     * @param aActionCommand  The command that is being executed.
     *                        TextVisual is interested in 
     *                        KHuiActionNewTVOutDisplay command.
     */
    void HandleActionL(const THuiActionCommand& aActionCommand);


protected:

    /* Constructors. */
    
    /**    
     * Default constructor.
     */
    CHuiTextMesh();
   
    /**    
     * 2nd phase constructor.
     */
    void ConstructL();

    /* Methods. */
    
    /** 
     * Changes the extents of the text mesh.
     *
     * @param aExtents  Width and height of the text mesh.
     */
    void SetExtents(const TSize& aExtents);
    
    /**
     * Updates the mesh and the extents. Both text and font must be set 
     * when this is called.
     *
     * @param aRasterize Set this to ETrue if text is to be rendered. Set to
     *                   EFalse if you only need to calculate text extents.
     */
    virtual void BuildL(TBool aRasterize) = 0;

    /**
     * Releases the font associated with this text mesh'es text style.
     */
    void ReleaseFont();

public: 

    /**
     * Expands the given content rect with shadow.
     * 
     * @param aRect Content rect to be modified.
     */    
    virtual void ExpandRectWithShadow(TRect& aRect) const;    

    /** Internal utility for prerasterized mesh */
    virtual void UpdateMeshL(const TDesC8& aBuffer);

    
protected:

    /** Current text style ID */
    TInt iTextStyleId;
    
    /** Interface for drawing pictographs, NOT owned */
    CAknPictographInterface* iPictographInterface;
    
    /** The line spacing for multiline text visual when the text wraps around. */
    TInt iLineSpacing;
    
    /** Scale factor from LCD resolution to TvOut resolution */
    TReal32 iTextMeshScale;
    
    // Related text visual
    CHuiVisual* iVisual;

private:

    /** Line wrapping and truncation mode. */
    TLineMode iLineMode;
    
    /** Maximum line width. */
    TInt iMaxLineWidth;
    
    /** TvOut scaled maximum line width. Calculated using scale factors 
    *   from LCD resolution to TvOut resolution */
    TInt iScaledMaxLineWidth;
    
    /** Maximum line count. */
    TInt iMaxLineCount;

    /** Current text string. */
    HBufC* iString;

    /** Extents of the text string using the current font. */
    TSize iExtents;

    /** TvOut scaled extents of the text string using the current font. */
    TSize iScaledExtents;

    /** Are rasterized shadows being used. */
    TBool iRasterizedShadow;
    };


#endif // __HUITEXTMESH_H__
