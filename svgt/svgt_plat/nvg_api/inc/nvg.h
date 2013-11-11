/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef NVG_H
#define NVG_H

#include <e32base.h>
#include <VG/openvg.h>
#include <VG/vgu.h>

#include <fbs.h>

class TDereferencer;
class MNVGIcon;
class CNvgFitToViewBoxImpl;
class MVGImageBinder;
class TNVGEngineInternal;

enum TNvgAlignStatusType
    {
    ENvgPreserveAspectRatio_None,
    ENvgPreserveAspectRatio_XminYmin,
    ENvgPreserveAspectRatio_XmidYmin,
    ENvgPreserveAspectRatio_XmaxYmin,
    ENvgPreserveAspectRatio_XminYmid,
    ENvgPreserveAspectRatio_XmidYmid,
    ENvgPreserveAspectRatio_XmaxYmid,
    ENvgPreserveAspectRatio_XminYmax,
    ENvgPreserveAspectRatio_XmidYmax,
    ENvgPreserveAspectRatio_XmaxYmax
    };

enum TNvgMeetOrSliceType
    {
    ENvgMeet,
    ENvgSlice
    };

enum TNvgBitEncoding
    {
    EEightBitEncoding = 1,
    ESixteenBitEncoding ,
    EThirtyTwoBitEncoding,
    EFloatEncoding
    };

class CNvgEngine :  public CBase
    {
public :    
    /**
     * @brief  constructor
     * @version
     * @param   None
     * @return  None
     */
    IMPORT_C static CNvgEngine* NewL();
    
    /**
     * @berief Destructor
     */
    IMPORT_C ~CNvgEngine();
    
    /**
     * @brief  Set the angle for rotation of the NVG graphic
     * @version
     * @param    aAngle counter-clockwise rotation by a given angle (expressed in degrees)
     *              aX, aY  point around which the rotation must take place
     * @return  None
     */
    IMPORT_C void Rotate(TReal32 aAngle, TReal32 aX, TReal32 aY) __SOFTFP;
	
	/**
     * @brief  Gets the viewbox width and height from the NVG bytedata
     * @version
     * @param   aBuf NVG byte data of the file
     * @return  None
     */
    IMPORT_C TSize ContentDimensions(const TDesC8& aBuf);
	
	/**
     * @brief  Render NVG onto the bitmap
     * @version
     * @param    aBuf  byte data of NVG file
     *           aSize Size in pixels of the target
     *           aDstBitmap Target bitmap
     *           aMask Target mask
     * @return  None
     */
    IMPORT_C TInt DrawNvg(const TDesC8& aBuf, const TSize& aSize, CFbsBitmap* aDstBitmap, CFbsBitmap* aMask );
   
    /**
     * @brief   Creates NVGIcon, openVG objects are cached
     * @param   aBuf  byte data of NVG file
     *          aSize Size in pixels of the target
     * @return  NVG icon object on success, 0 on failure
     */
    IMPORT_C MNVGIcon * CreateNVGIcon(const TDesC8& aBuf, const TSize& aSize);
    
	/**
     * @brief  Set the preserve aspect ratio for the NVG file
     * @version
     * @param  
     * @return  None
     */
    IMPORT_C void SetPreserveAspectRatio( TNvgAlignStatusType aPreserveAspectSetting, TNvgMeetOrSliceType aSmilFitSetting );

	/**
     * @brief  Reset the OpenVG handles in NVG Engine
     * @version
     * @param  
     * @return  None
     */
    IMPORT_C void ResetNvgState();
    
	/**
     * @brief  Sets the BackGround Colour for the target
     * @version
     * @param  aRGBA8888Color colour in 32 bit RGBA format
     * @return  None
     */
    IMPORT_C void SetBackgroundColor( TUint32 aRGBA8888Color );
    
    /**
     * @fn      SetVGImageBinder
     * @brief   Sets the callback for binding image to surface, nvgengine doesn't owns the implementation
     * @param   aImageBinder    an implementation for MVGImageBinder, ownership is with the caller
     */
    void SetVGImageBinder(MVGImageBinder * aImageBinder)
        {
        iVGImageBinder = aImageBinder;
        }
    
    /**
     * @fn      OpenVGErrorToSymbianError
     * @brief   Converts OpenVg errors to symbian error type
     */
    static TInt OpenVGErrorToSymbianError( TInt aError );

private :
    
    /**
     * Checks whether the given matrix is identity
     */
    TBool IsIdentity(VGfloat array[]);
    
    /**
     * Draws the nvg icon
     */
    void DoDrawNVGL(const TDesC8& aBuf, const TSize& aSize, CFbsBitmap* aDstBitmap, CFbsBitmap* aMask);
    
    /**
     * Draws the nvg-tlv icon
     */
    TInt DrawTLVL(const TDesC8& aBuf, const TSize& aTargetSize, CFbsBitmap* aDstBitmap, CFbsBitmap* aMask);
    
    /**
     * Draws the nvg-cs icon
     */
    TInt DrawCommandSectionL(TDereferencer * aIconData, const TSize& aTargetSize, CFbsBitmap* aDstBitmap, CFbsBitmap* aMask);
    
	/**
     * Constructor
     */
    CNvgEngine();
    
    /**
     * Constructor
     */
    void ConstructL();
	
	/**
     * SetPaint gradient or solid in OpenVG
     */
    TInt SetFillPaintL(TDereferencer * aIconData);
	
	/**
     * SetColor in OpenVG
     */
    TInt SetColorRampL(TDereferencer * aIconData);
	
	/**
     * Call vgDrawPath in OpenVG
     */
    void DrawPathL(TDereferencer * aIconData);
    
    /**
     * call SetTranslate in OpenVG
     */
    void SetTransformL(TDereferencer * aIconData, TUint32 &transVal,const VGfloat* aCurrentMatrix);
    
    /**
     * Set stroke color in Openvg
     */
    void SetStrokePaintL(TDereferencer * aIconData);

    /**
     * Sets the stroke Width
     */
    void SetStrokeWidth(const TUint8* aBuf);
    
    /**
     * Sets the miter limit
     */
     void SetStrokeMiterLimit(const TUint8* aBuf);
     

	/**
     * Generate Mask for modes not supported by custom binding API
     */
    void GenerateMask(CFbsBitmap* aMask);
   
   
    /**
     * To clear the background of the target
     */
    void ClearBackground();
       
    /**
     * Stores the callres matrix mode and matricies
     */
    void UpdateClientMatrices();
    
    /**
     * Restores the callres matrix mode and matricies
     */
    void RestoreClientMatrices();
    
    /**
     * Point transforms according to the given matrix
     */
    TPoint GetTranslatedPoint(VGfloat aTRMatrix[9], TPoint aPoint);

    /**
     * Gives the minimum of the given arguments
     */
    VGfloat MinVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4 );

    /**
     * Gives the maximum of the given arguments
     */
    VGfloat MaxVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4 );
    
    /**
     * Initializes the OpenVg handles
     */
    TInt InitializeGC();
    
    /**
     * Create the path handle
     */
    TInt CreatePathHandle(TInt16 aPathDataType, TReal32 aScale, TReal32 aBias);

    /**
     * Applies the rotation
     */
    void ApplyRotation();

    /**
     * Applies the scissoring
     */
    void ApplyScissoring(VGfloat aMatrix[], const TSize& aTargetSize);
    
    /**
     * Applies viewbox to viewport transformation
     */
    void ApplyViewboxToViewPortTransformationL(const TSize& aTargetSize, TReal32 aViewboxX, TReal32 aViewboxY,
                                            TReal32 aViewboxW, TReal32 aViewboxH);

    /**
     * Executes the nvg-cs commands
     */
    void ExecuteNVGCSCommandLoopL(TUint16 aCommandCount, TDereferencer * aIconData, TDereferencer * aOffsetVector,
                                              TDereferencer * aCommandSection, TInt aNVGVersion);

private :
        
    CFbsBitmap*         iDstBimtap;             //! Target Bitmap onto which the drawing will be rendered
    TSize               iCurrentBufferSize;     //! Width and height of the target bitmap
	
	VGPath     iPath;          //! The path elements are appended to this path	
	VGPaint    iPaintFill;     //! Paint handle for fill
	VGPaint    iPaintStroke;   //! Paint handle for stroke
	
    VGPaint    iGradPaintFill;
    VGPaint    iGradPaintStroke;        
    VGPaint    iUserStrokePaint;
    TUint32    iUserStrokePaintColor;

	TInt   iFillAlpha;       //! For fill alpha value	
	TInt   iStrokeAlpha;     //! Set alpha value for stroke
			
	VGboolean idoFill;       //! Flags for fill
	VGboolean idoStroke;     //! Flags for stroke
	
	VGfloat * iColorRamps;                     //! This member variable is not used just kept for BC
	CNvgFitToViewBoxImpl * iFitToViewBoxImpl;  //! This member variable is not used just kept for BC
	
	VGfloat iRotateAngle;   //! Rotation angle in degrees	
	TBool iRotateApplied;   //! Is rotation angle specified
	
	/*
	 * caller's matrix mode and matricies, must be restored after operation
	 */
    VGint               iMatrixMode;
    TReal32             iImageMatrix[9];
    TReal32             iPathMatrix[9];
    TReal32             iFillPaintMatrix[9];
    TReal32             iStrokePaintMatrix[9];
    
    TInt16              iLastPathDataType;
		
	TNvgAlignStatusType iPreserveAspectSetting;    //! Aspect Ratio setting
    TNvgMeetOrSliceType iSmilFitSetting;           //! SMIL Fit setting
    
    TUint32     iBackgroundColor;
    
	TReal32 iCentreX;      //! Rotate center x-coordinate
	TReal32 iCentreY;      //! Rotate center y-coordinate
	
    MVGImageBinder *    iVGImageBinder;     //! image binder

    /**
     * This class is added to maintain BC.
     * If you want to add new member variable you can add in this class, without breaking BC.
     * It is better to add members which are depend on macro's in this class.
     */
    TNVGEngineInternal  *iInternal;         
    
#ifdef    OPENVG_OBJECT_CACHING
    VGPath CreatePath();
#endif
};

#endif
