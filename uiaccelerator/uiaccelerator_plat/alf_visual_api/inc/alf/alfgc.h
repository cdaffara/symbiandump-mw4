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
* Description:   Graphics context.
*
*/



#ifndef C_ALFGC_H
#define C_ALFGC_H

#include <e32base.h>
#include <gdi.h>

#include <alf/alftimedvalue.h>
#include <alf/alfrealline.h>

struct TAlfRealLine;


class CAlfTexture;
class CAlfTextStyle;


/** Horizontal alignment. */
enum TAlfAlignHorizontal
    {
    EAlfAlignHLeft,
    EAlfAlignHCenter,
    EAlfAlignHRight,

    /** Special alignment for localized text. This is converted to left or
        right alignment internally. */
    EAlfAlignHLocale,

    /** Special alignment for localized text. This is the mirrored
        alignment of the normal locale alignment. Converted to left or
        right alignment internally. */
    EAlfAlignHLocaleMirrored
    };


/** Vertical alignment. */
enum TAlfAlignVertical
    {
    EAlfAlignVTop,
    EAlfAlignVCenter,
    EAlfAlignVBottom
    };    
    
/** Polygon draw mode */
enum TAlfPolygonDrawMode
    {
    ENoFill,
    EFillNonZero,
    EFillEvenOdd
    };
    
/**
 * CAlfGc is a graphics context that is used for storing the state of the
 * drawing tools. 
 *
 */
NONSHARABLE_CLASS(CAlfGc) : public CBase
    {
public:

    /**
     * Stretch modes.
     */
    enum TStretchMode
        {
        EStretchNone = 0,   ///< Does not stretch
        EStretchHorizontal, ///< Fits to horizontal width (left-right)
        EStretchVertical,   ///< Fits to vertical height
        EStretchFull		///< Stretches the image to the full area of the visual, disregarding the original aspect ratio.

        };

public:
    
    static CAlfGc* NewL();
    ~CAlfGc();    
    void ActivateL();
    HBufC8* CommandBuffer();
    void Deactivate();

    void DrawImage(const CAlfTexture& aTexture, const TAlfRealRect& aDestinationRect);    
    void DrawText(const TDesC& aText, const TAlfRealRect& aDestinationRect);    
	void DrawEllipse(const TAlfRealRect& aDestinationRect);	
	void DrawLines(const RArray<TReal32> & aLinesPointCords);
	void DrawPoints(const RArray<TReal32> & aPointsCords); 
  	void DrawPolygon(const RArray<TReal32> & aPolygonPointCords);
  	void DrawRects(const RArray<TReal32> & aRectPointCords);
	
	// Setters
    void SetTextStyle(const CAlfTextStyle& aTextStyle);
	void SetPenColor(const TRgb& aColor);
	void SetPenWidth(const TReal32& aWidth);
	void SetOpacity(const TReal32& aOpacity);
	void SetPolygonDrawMode(TAlfPolygonDrawMode aPolygonDrawMode);
	void SetTextAlign(TAlfAlignHorizontal aAlignHorizontal, TAlfAlignVertical aAlignVertical);

    // Transformations
	void LoadIdentity();
	void Translate(const TReal32& aX, const TReal32& aY, const TReal32& aZ);
	void Scale(const TReal32& aX, const TReal32& aY, const TReal32& aZ);
	void Rotate(const TReal32& aAngle, const TReal32& aX, const TReal32& aY, const TReal32& aZ);
  
    
private:
    CAlfGc();
    void ConstructL();
    
    void AppendCommand( TInt aCommand, const TDesC8& aParams );
    void AppendCommandL( TInt aCommand, const TDesC8& aParams );
    
private:
    struct TAlfGcData;
    TAlfGcData* iData;
    };

#endif
