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
* Description:  NVG Decoder header file
 *
*/


#ifndef NVGCSICON_H_
#define NVGCSICON_H_

#include <e32base.h>
#include <openvg.h>

#include "NVGIcon.h"

class CNVGIconData;
class CNvgEngine;
class COpenVGHandleStore;

class CNVGCSIcon : public CBase, public MNVGIcon
    {
private:
    enum TNVGCSIconCommands
        {
        EPath,
        EPathData,
        EPaint,
        EColorRamp,
        ETransform,
        EStrokeWidth,
        EStrokeMiterLimit,
        EStrokeLineJoinCap,
        EStrokePaint,
        EStrokeColorRamp
        };

    CNVGCSIcon();
    void ConstructL(const TDesC8& aBuf);
    
public:
    static CNVGCSIcon * NewL(const TDesC8& aBuf);
    static CNVGCSIcon * NewLC(const TDesC8& aBuf);
    virtual ~CNVGCSIcon();
    
    virtual TInt Draw(const TSize aSize, CNvgEngine * aNVGEngine);
    
    TInt SetViewBox(TReal32 x, TReal32 y, TReal32 w, TReal32 h) __SOFTFP;

    TInt SetPreserveAspectRatio(TInt aPreserveAspectSetting, 
            TInt aSmilFitSetting);

    TInt Rotate(TReal32 aAngle, TReal32 aX, TReal32 aY) __SOFTFP;
    
    /**
     * Adds new path drawing command. 
     * @param aPath vgPath to be appended
     */
    void AddDrawPathCommandL(VGPath aPath, VGbitfield aPaintMode);
    
    /**
     * Adds path data. This is required if vgCreatePath fails. 
     */        
    void AddPathDataL(VGint numSegments, const VGubyte * pathSegments, const void * pathData);
    
    /**
     * Adds linear gradient drawing command. Stores the gradient data and matrix
     * to be used with VG_MATRIX_FILL_PAINT_TO_USER mode.
     * 
     * @param aCount            Gradient data parameter count
     * @param aGradientData     Linear gradient data to be given to vgSetParameterfv
     * @param aGradientMatrix   Linear gradient matrix used for FILL_PAINT_TO_USER matrix mode
     */
    void AddLinearGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint);

    /**
     * Adds radial gradient drawing command. Stores the gradient data and matrix
     * to be used with VG_MATRIX_FILL_PAINT_TO_USER mode.
     * 
     * @param aCount            Gradient data parameter count
     * @param aGradientData     Linear gradient data to be given to vgSetParameterfv
     * @param aGradientMatrix   Linear gradient matrix used for FILL_PAINT_TO_USER matrix mode
     */
    void AddRadialGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint);
    
    /**
     * Adds a setColor command to be used with paints.
     * 
     * @param aRgba RGBA color in hex format to be set with vgSetColor
     */
    void AddSetColorCommandL(VGuint aRgba);

    /**
     * Adds color ramp to the icon data structure.
     * 
     * @param aStopCount    Count of the color ramps, has to be multiple of 5
     * @param aColorRamps   Color ramp data
     */
    void AddColorRampCommandL(VGPaint aPaint);
    
    /**
     * Adds a transformation command which is multiplied to the current matrix
     * 
     * @param aTransformMatrix  Transformation matrix used for multiplication
     * @param aFlag 1 multiplies  aTransformMatrix with current matrix, 0 loads original matrix
     */
    void AddSetTransformCommandL(const VGfloat* aTransformMatrix, TInt aFlag);
    
    /**
     * Adds set stroke-width command
     * 
     * @param aStrokeWidth stroke width
     */
    void AddSetStrokeWidthCommandL(VGfloat aStrokeWidth);
    
    /**
     * Adds set stroke-miterlimit command
     * 
     * @param aMiterLimit miter length     
     */
    void AddSetStrokeMiterLimitCommandL(VGfloat aMiterLimit);
    
    /**
     * Adds set line join cap command
     * 
     * @param aCapStyle   end cap style
     * @param aJoinStyle  line join style
     */
    void AddStrokeLineJoinCapCommandL(VGint aCapStyle, VGint aJoinStyle);

    /**
     * Adds linear gradient drawing command. Stores the gradient data and matrix
     * 
     * @param aCount            Gradient data parameter count
     * @param aGradientData     Linear gradient data to be given to vgSetParameterfv
     * @param aGradientMatrix   Linear gradient matrix used for STROKE_PAINT_TO_USER matrix mode
     */
    void AddStrokeLinearGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint);

    /**
     * Adds radial gradient drawing command. Stores the gradient data and matrix
     * to be used with VG_MATRIX_STROKE_PAINT_TO_USER mode.
     * 
     * @param aCount            Gradient data parameter count
     * @param aGradientData     Linear gradient data to be given to vgSetParameterfv
     * @param aGradientMatrix   Linear gradient matrix used for VG_MATRIX_STROKE_PAINT_TO_USER matrix mode
     */
    void AddStrokeRadialGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint);
    
    /**
     * Adds a setColor command to be used with stroke.
     * 
     * @param aRgba RGBA color in hex format
     */
    void AddStrokeSetColorCommandL(VGuint aRgba);
    
    /**
     * Adds color ramp to the icon data structure.
     * 
     * @param aStopCount    Count of the color ramps, has to be multiple of 5
     * @param aColorRamps   Color ramp data
     */
    void AddStrokeColorRampCommandL(VGPaint aPaint);
    
private:

    TInt DoDrawL(const TSize aSize);

    void AddLinearGradientCommandDataL(VGPaint aPaint, VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix);
    void AddRadialGradientCommandDataL(VGPaint aPaint, VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix);
    void AddSetColorCommandDataL(VGuint aRgba);

    void DrawPaintL(VGPaint aPaint, VGMatrixMode aMatrixMode, TUint & aLastPaintType, TUint & aLastPaintColor, VGPaintMode aPaintMode);
    void DrawColorRampL(VGPaint aPaint);
    void SetViewBoxToViewTransformationL(const TSize aSize);
    void SetRotation();

    void UpdateClientMatrices();
    void RestoreClientMatrices();
    
private:
    TReal32             iViewBoxX;
    TReal32             iViewBoxY;
    TReal32             iViewBoxW;
    TReal32             iViewBoxH;
    
    TReal32             iRotationAngle;
    TReal32             iRotationX;
    TReal32             iRotationY;
    
    TInt                iPreserveAspectSetting;
    TInt                iSmilFitSetting;

    CNVGIconData *      iNVGIconData;

    VGPath              iPath;          // when create path fails this handle will be used
    VGPaint             iFillPaint;     // currently just one object for fill paint
    VGPaint             iStrokePaint;   // currently just one object for stroke paint

    // caller's parameters
    VGPaint             iUserStrokePaint;
    VGPaint             iUserFillPaint;

    VGint               iMatrixMode;
    TReal32             iImageMatrix[9];
    TReal32             iPathMatrix[9];
    TReal32             iFillPaintMatrix[9];
    TReal32             iStrokePaintMatrix[9];
    CNvgEngine *        iNVGEngine;
    COpenVGHandleStore* iOpenVGHandles;
    
    TUint       iLastFillPaintType;
    TUint       iLastStrokePaintType;
    TUint       iLastFillPaintColor;
    TUint       iLastStrkePaintColor;
    
    TUint       iResetFillPaint;
    VGPaint     iResetStrokePaint;
    };

#endif
