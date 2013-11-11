/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*  Note that we do not use OpenVG's line/arc rendering directly, since the
*  curves we are dealing with may have variable widths, which is not supported
*  by OpenVG yet. Also, only one opacity factor per path is supported.
*
*/

#include "HuiVg10CurvePath.h"
#include "HuiVg10Gc.h"
#include "uiacceltk/HuiRealPoint.h"
#include "uiacceltk/HuiUtil.h"

CHuiVg10CurvePath::CHuiVg10CurvePath()
        : CHuiCurvePath()
    {
    }
    
    
void CHuiVg10CurvePath::ConstructL()
    {
    iPathCommands = 0;
    iLastOpacityFactor = -1.0;
    iMaxTupleCount = 0;
    iUsedTupleCount = 0;
    iPathCoords = 0;

    iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 
                         1.0f, 0.0f, 32, 32, VG_PATH_CAPABILITY_APPEND_TO);
    HUI_VG_INVARIANT();
    }
    
  
CHuiVg10CurvePath::~CHuiVg10CurvePath()
    {
    Reset();
    vgDestroyPath(iPath);
    iPath = 0;
    HUI_VG_INVARIANT();
    }
  
    
void CHuiVg10CurvePath::Reset()
    {
    CHuiCurvePath::Reset();
    ResetPath();
    }

void CHuiVg10CurvePath::ResetPath()
    {
    ResetTuples();
    vgClearPath(iPath, VG_PATH_CAPABILITY_APPEND_TO);
    HUI_VG_INVARIANT();
    }
    
void CHuiVg10CurvePath::ResetTuples()
    {
    delete[] iPathCoords;
    delete[] iPathCommands;
    iPathCoords = 0;
    iPathCommands = 0;
    iMaxTupleCount = 0;
    iUsedTupleCount = 0;
    }

TInt CHuiVg10CurvePath::SegmentTuples(const TSegment& aSegment) const
    {
    if(aSegment.iType == ESegmentTypeLine)
        {
        return 2;
        }
    else if(aSegment.iType == ESegmentTypeArc)
        {
        const TReal32 KAnglePerSegment = 5;
        TInt tuples = Abs(aSegment.iAngleDelta) / KAnglePerSegment + 1;
        if(tuples < 2)
            {
            tuples = 2;
            }
        return tuples;
        }
    ASSERT(0);
    return 0;
    }
        
void CHuiVg10CurvePath::Update(TReal32 aStartPos, TReal32 aEndPos, 
                               TReal32 aAlphaFactor,
                               MHuiMappingFunction* aAlphaFunction,
                                 MHuiMappingFunction* aWidthFunction) __SOFTFP
    {
    const TReal32 KEpsilon = .001;
    if(!GetVisual() && !NeedUpdate() && Abs(iLastOpacityFactor - aAlphaFactor) < KEpsilon)
        {
        // No need to update every time the path is drawn.
        return;
        }

    iLastOpacityFactor = aAlphaFactor;        
    
    // Allocate enough memory for the path coordinates and commands. This is 
    // the maximum number of path nodes that can be. If aStartPos and aEndPos are 
    // somewhere in the middle, a smaller amount of data is generated.
    TInt tuples = 0;                    
    TInt i = 0;    
    for(i = 0; i < SegmentCount(); ++i)
        {
        tuples += SegmentTuples(Segment(i)) - 1;
        }
        
    // One extra tuple for the end.        
    tuples++;
    
    TRAPD(err, SetTupleCountL(tuples));
    if(err != KErrNone)
        {
        ResetPath();
        return;
        }
   
    // Generate commands for the entire path, starting from aStartPos
    // and stopping at aEndPos.
    TReal32 pos = 0;
    iUsedTupleCount = 0;
    
    for(i = 0; i < SegmentCount(); ++i)
        {
        const TSegment& segment = Segment(i);
        TReal32 segmentEnd = pos + segment.iLength;
        
        if(aStartPos > segmentEnd)
            {
            // The start is past the end of the segment.
            continue;
            }            
        if(aEndPos < pos)
            {
            // We're past the end!
            break;
            }
            
        TReal32 start = pos;
        TReal32 end = segmentEnd;
        TBool isFinal = EFalse;        
        
        if(start < aStartPos)
            {
            start = aStartPos;
            }
        if(end >= aEndPos || i == SegmentCount() - 1)
            {
            end = aEndPos;
            isFinal = ETrue;
            }
        else
            {
            isFinal = EFalse;
            }                   
            
        // Normalize.
        start -= pos;
        end -= pos;
            
        MakeSegmentTuples(segment, start, end, isFinal, aAlphaFactor, 
                          aAlphaFunction, aWidthFunction);
        
        pos += segment.iLength;
        }
    ASSERT(iUsedTupleCount <= iMaxTupleCount);
    ASSERT(iUsedTupleCount > 0);

    // First traverse the curve from start to end 
    vgAppendPathData(iPath, iUsedTupleCount, iPathCommands, iPathCoords);
    HUI_VG_INVARIANT();
   
    // And then back again on the other side
    TInt tailIndex = 2 * iMaxTupleCount - iUsedTupleCount;
    vgAppendPathData(iPath, iUsedTupleCount, &iPathCommands[tailIndex], &iPathCoords[2 * tailIndex]);
    HUI_VG_INVARIANT();

    // We do not need the tuple arrays anymore so we can free them
    ResetTuples();
    
    CHuiCurvePath::Update(aStartPos, aEndPos, aAlphaFactor, 
                          aAlphaFunction, aWidthFunction);
    }

void CHuiVg10CurvePath::SetTupleCountL(TInt aTupleCount)
    {
    if(iMaxTupleCount != aTupleCount)
        {
        ResetPath();
        
        // Allocate new arrays.
        iMaxTupleCount = aTupleCount;
        
        if(iMaxTupleCount > 0)
            {
            iPathCoords = new (ELeave) VGfloat[2 * iMaxTupleCount * 2];
            iPathCommands = new (ELeave) VGubyte[iMaxTupleCount * 2];

            // Since every path command is the same, we can do this at 
            // allocation time
            for (TInt i = 0; i < iMaxTupleCount * 2; i++)
                {
                iPathCommands[i] = VG_LINE_TO;
                }
            }
        }
    }

void CHuiVg10CurvePath::MakeSegmentTuples(const TSegment& aSegment,
                                          TReal32 aStart, TReal32 aEnd,
                                          TBool aIsFinal,
                                          TReal32 aAlphaFactor,
                                          MHuiMappingFunction* aAlphaFunction,
                                          MHuiMappingFunction* aWidthFunction)
    {
    // aStart and aEnd are in range [0,length].
    THuiRealPoint point;
    THuiRealPoint normal;
    TReal32 normStart = (aStart + aSegment.iTotalPos) / Length();
    TReal32 normEnd = (aEnd + aSegment.iTotalPos) / Length(); 
    
    if(aSegment.iType == ESegmentTypeLine)
        {
        EvaluateSegment(aStart, aSegment, point, &normal);
        TReal32 width = 1;
        if(aWidthFunction)
            {
            width = aWidthFunction->MapValue(normStart, 0);
            }
        TReal32 alpha = 1;
        if(aAlphaFunction)
            {
            alpha = aAlphaFunction->MapValue(normStart, 0);
            }
        AddTuple(point, normal, width, alpha * aAlphaFactor);
        
        if(aIsFinal)
            {
            // Does not reach the end, so we must also make an end tuple.
            EvaluateSegment(aEnd, aSegment, point, &normal);
            if(aWidthFunction)
                {
                width = aWidthFunction->MapValue(normEnd, 0);
                }
            else
                {
                width = 1;
                }
            if(aAlphaFunction)
                {
                alpha = aAlphaFunction->MapValue(normEnd, 0);
                }
            else
                {                
                alpha = 1;
                }
            AddTuple(point, normal, width, alpha * aAlphaFactor);
            }
        }
        
    if(aSegment.iType == ESegmentTypeArc)
        {
        // Make a set of tuples.
        TInt count = SegmentTuples(aSegment);
        TInt endCount = count;
        if(aIsFinal)
            {
            endCount = count - 1;
            }
        else
            {
            count--;
            endCount--;
            }
        
        for(TInt i = 0; i < count; ++i)
            {
            TReal32 off = i / TReal32(endCount);
            EvaluateSegment(aStart + (aEnd - aStart) * off, aSegment, point, 
                            &normal);
            TReal32 width = 1;
            if(aWidthFunction)
                {
                width = aWidthFunction->MapValue(normStart + 
                    (normEnd - normStart) * off, 0);
                }
            TReal32 alpha = 1;
            if(aAlphaFunction)
                {
                alpha = aAlphaFunction->MapValue(normStart + 
                    (normEnd - normStart) * off, 0);
                }
            AddTuple(point, normal, width, alpha * aAlphaFactor);
            }
        }
    }

void CHuiVg10CurvePath::AddTuple(const THuiRealPoint& aPoint,
                                 const THuiRealPoint& aNormal,
                                 TReal32 aWidth, TReal32 /*aAlpha*/)
    {
    ASSERT(iUsedTupleCount < iMaxTupleCount);

    // Make a pair of vertices. Normal is to the left.
    //    
    // The first half of the path goes from the beginning of the curve 
    // to the end, while the second half traverses the same route 
    // backwards:
    //
    //     0                     iUsedTupleCount
    //     >--->--->---> ... >---v
    //     |                     |
    //     ^---<---<---< ... <---<
    //     iMaxTupleCount -      iMaxtupleCount
    //     iUsedTupleCount
    //
    // The coordinate array looks like this:
    //
    //     +---+---+---+---+---+---+---+---+    N: tuple N left vertex
    //     | 0 | 1 | 2 | X | X | 2'| 1'| 0'|   N': tuple N right vertex
    //     +---+---+---+---+---+---+---+---+    X: unused
    //
    VGfloat* vertex1 = &iPathCoords[2 * iUsedTupleCount];
    VGfloat* vertex2 = &iPathCoords[2 * (2 * iMaxTupleCount - iUsedTupleCount - 1)];
    const CHuiVisual* visual = GetVisual();

    if (visual)
        {
        THuiRealPoint point1(aPoint.iX + aNormal.iX * aWidth/2,aPoint.iY + aNormal.iY * aWidth/2);
        THuiRealPoint point2(aPoint.iX - aNormal.iX * aWidth/2,aPoint.iY - aNormal.iY * aWidth/2);
        point1 = visual->LocalPointInPixels(point1);
        point2 = visual->LocalPointInPixels(point2);  
        vertex1[0] = point1.iX;
        vertex1[1] = point1.iY;
        vertex2[0] = point2.iX;
        vertex2[1] = point2.iY;
        }
    else
        {
        vertex1[0] = aPoint.iX + aNormal.iX * aWidth/2;
        vertex1[1] = aPoint.iY + aNormal.iY * aWidth/2;
        vertex2[0] = aPoint.iX - aNormal.iX * aWidth/2;
        vertex2[1] = aPoint.iY - aNormal.iY * aWidth/2;     
        }

    iUsedTupleCount++;
    }

void CHuiVg10CurvePath::Draw(const TPoint& aOrigin, CHuiGc* aGc) const
    {
    CHuiVg10Gc* gc = static_cast<CHuiVg10Gc*>(aGc);
    ASSERT(gc);
    gc->UpdateColor();
    
    VGPaint paint = vgGetPaint(VG_FILL_PATH);
    VGfloat color[4], modifiedColor[4];
    
    if (paint != VG_INVALID_HANDLE)
        {
        vgGetParameterfv(paint, VG_PAINT_COLOR, 4, color);
        modifiedColor[0] = color[0];
        modifiedColor[1] = color[1];
        modifiedColor[2] = color[2];
        modifiedColor[3] = iLastOpacityFactor;
        vgSetParameterfv(paint, VG_PAINT_COLOR, 4, modifiedColor);
        HUI_VG_INVARIANT();
        }

    gc->UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgTranslate(aOrigin.iX, aOrigin.iY);
    vgDrawPath(iPath, VG_FILL_PATH);
    vgTranslate(-aOrigin.iX, -aOrigin.iY);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

    HUI_VG_INVARIANT();

    if (paint != VG_INVALID_HANDLE)
        {
        vgSetParameterfv(paint, VG_PAINT_COLOR, 4, color);
        HUI_VG_INVARIANT();
        }
    }
