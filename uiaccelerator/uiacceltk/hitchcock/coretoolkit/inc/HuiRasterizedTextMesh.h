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
* Description:   Definition of HuiRasterizedTextMesh. CHuiRasterizedTextMesh 
*                stores a rasterized bitmap version of a text string.
*
*/



#ifndef __HUIRASTERIZEDTEXTMESH_H__
#define __HUIRASTERIZEDTEXTMESH_H__

#include <e32cmn.h>
#include <AknPictographDrawerInterface.h>

#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiRealPoint.h"
#include "uiacceltk/HuiImage.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiTexture.h"

class CFont;
class CFbsBitmap;
class CAknPictographInterface;

/**
 * CHuiRasterizedTextMesh is a concrete implementation of a CHuiTextMesh.
 * CHuiRasterizedTextMesh uses Symbian's BITGDI text drawing routines and Avkon
 * BiDi text utilities to draw text onto a bitmap. The bitmap is then copied
 * onto a CHuiTexture and drawn with CHuiGc. This makes CHuiRasterizedTextMesh
 * independent of the underlying rendering plugin. Multiple lines of text are
 * supported.
 */
NONSHARABLE_CLASS(CHuiRasterizedTextMesh) : public CHuiTextMesh, public MHuiTextureContentObserver
    {
public:

    /* Constructors and destructors. */

    /**
     * Constructs a new rasterized text mesh.
     */
    IMPORT_C static CHuiRasterizedTextMesh* NewL();

    /**
     * Constructs a new rasterized text mesh and leaves it on the cleanup stack.
     */
    static CHuiRasterizedTextMesh* NewLC();

    /**
     * Destructor.
     */
    virtual ~CHuiRasterizedTextMesh();


    /* Methods. */

    /**
     * Resets the contents of the text mesh.
     */
    virtual void Reset();

    /**
     * Draws the text mesh.
     *
     * @param aGc             Graphics context.
     * @param aShadowOpacity  Opacity of shadows.
     */
    virtual void Draw(CHuiGc& aGc, TReal32 aShadowOpacity = 1.0) const __SOFTFP;

    void BuildPictographsL();

    virtual void ExpandRectWithShadow(TRect& aRect) const;

protected:

    /* Constructors. */

    /**
     * Default constructor.
     */
    CHuiRasterizedTextMesh();

    /* Methods. */

    void ResetLines();
    void ResetPictographLines();

    TBool IsMaxLineCountReached() const;

    /**
     * Updates the mesh and the extents. Both text and font must be set
     * when this is called.
     * 
     * @param aRasterize Set this to ETrue if text is to be rendered. Set to
     *                   EFalse if you only need to calculate text extents.
     */
    virtual void BuildL(TBool aRasterize);

    /**
     * Draws the lines of the text mesh.
     */
    void DrawLines(CHuiGc& aGc, const THuiRealPoint& aOffset,
                   THuiAlignHorizontal aLineAlignment, TReal32 aShadowOpacity) const;


    /**
     * Draws the pictographlines of the text mesh.
     */
    void DrawPictographLines(CHuiGc& aGc, const THuiRealPoint& aOffset,
                   THuiAlignHorizontal aLineAlignment) const;


    /* Implementation of MHuiTextureContentObserver. */

    void TextureContentUploaded(CHuiTexture& aTexture);

    void TextureContentReleased(CHuiTexture& aTexture);

    void RestoreTextureContentL(CHuiTexture& aTexture);


private:

    /** Each line of rasterized text is stored in an SLine. */
    struct SRasterizedLine
        {
        SRasterizedLine()
            {
            iTexture = NULL;
            iGap = 0;
            }
        /** Texture that holds the rasterized version of a text line. The
            size of the texture determines the extents of the line. */
        CHuiTexture* iTexture;
        
        /** Extra gap to the next line in pixels. Not applied to the last
            line. */
        TInt iGap;
        };

protected:

    /**
     * Rasterizes a single line of text using the defined text style.
     * The result is a new line texture.
     *
     * @param aTextLine  The line of text to rasterize.
     * @param aLine		 SRasterizedLine entry that will
     *                   store the rasterized line content.
     *
     * @return  <code>ETrue</code>, if the maximum number of lines has
     *          not been reached (rasterization was successful);
     *          otherwise <code>EFalse</code>. BuildL() is stopped
     *          when this method returns EFalse.
     */
    TBool RasterizeLineL(const TDesC& aTextLine, SRasterizedLine & aLine);

    /**
     * Rasterizes a single line of pictographs using the specified font.
     * The result is a new line texture.
     *
     * @param aTextLine  The line of text to rasterize.
     * @param aFont      Font to be used in rasterization.
     * @param aLine		 SRasterizedLine entry that will
     *                   store the rasterized line content.
     *
     * @return  <code>ETrue</code>, if the maximum number of lines has
     *          not been reached (rasterization was successful);
     *          otherwise <code>EFalse</code>. BuildL() is stopped
     *          when this method returns EFalse.
     */
    TBool RasterizePictographLineL(const TDesC& aTextLine, CFont* aFont, SRasterizedLine & aLine);

    /** 
     * Sets the pictograph interface, ownership is NOT transferred.
     *
     * @param aExtents  Width and height of the text mesh.
     */
    virtual void InitPictographsL(CAknPictographInterface* aInterface);

    /**
     * Internal utility for using pre-rasterized mesh
     */
    virtual void UpdateMeshL(const TDesC8& aBuffer);
    
private:
    
    void DoBuildL(TInt aRasterizeFlags);    

private:

    /** Rasterized lines. 
     * @see SRasterizedLine.
     */
    RArray<SRasterizedLine> iLines;
    
    /** Rasterized line pictographs.
     * @see SRasterizedLine.
     */
    RArray<SRasterizedLine> iPictographLines;

    /** Bitmap onto which Symbian BITGDI draws pictographs. */
    CFbsBitmap* iPictographBitmap;

    TBool iUsingPreRasterizedMesh;
    };


#endif // __HUIRASTERIZEDTEXTMESH_H__

