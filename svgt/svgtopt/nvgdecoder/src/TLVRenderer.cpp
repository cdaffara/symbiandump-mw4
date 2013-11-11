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
* Description:  NVG Decoder source file
*
*/

#include "TLVRenderer.h"
#include "MVGImageBinder.h"
#include "NVGUtil.h"
#include "NVGIconData.h"
#include "NVGTLVIcon.h"
#include "nvg.h"

#ifdef NVG_DEBUG
#include <time.h>
#endif

CTLVRenderer::CTLVRenderer(TInt aWidth, TInt aHeight)
        : iTargetWidth(aWidth),
          iTargetHeight(aHeight),
          iVGImageBinder(0),
          iPrepareToBindImage(0)
        
    {
    }

CTLVRenderer * CTLVRenderer::NewL(const TDesC8& aBuf, TInt aWidth, TInt aHeight)
    {
    CTLVRenderer* self  = CTLVRenderer::NewLC(aBuf,aWidth, aHeight);
    CleanupStack::Pop(self);
    return self;
    }
CTLVRenderer * CTLVRenderer::NewLC(const TDesC8& aBuf, TInt aWidth, TInt aHeight)
    {
    CTLVRenderer* self    = new (ELeave) CTLVRenderer(aWidth, aHeight);
    CleanupStack::PushL(self);
    self->ConstructL(aBuf);
    return self;
    }

void CTLVRenderer::ConstructL(const TDesC8& aBuf)
    {
    vgapi[EvgSeti]              = &CTLVRenderer::DvgSetiL;
    vgapi[EvgSetf]              = &CTLVRenderer::DvgSetfL;
    vgapi[EvgSetParameteri]     = &CTLVRenderer::DvgSetParameteriL;
    vgapi[EvgSetParameterf]     = &CTLVRenderer::DvgSetParameterfL;
    vgapi[EvgSetParameterfv]    = &CTLVRenderer::DvgSetParameterfvL;
    vgapi[EvgSetColor]          = &CTLVRenderer::DvgSetColorL;
    vgapi[EvgSetPaint]          = &CTLVRenderer::DvgSetPaintL;
    vgapi[EvgLoadMatrix]        = &CTLVRenderer::DvgLoadMatrixL;
    vgapi[EvgMultMatrix]        = &CTLVRenderer::DvgMultMatrixL;
    vgapi[EvgLoadIdentity]      = &CTLVRenderer::DvgLoadIdentityL;
    vgapi[EvgScale]             = &CTLVRenderer::DvgScaleL;
    vgapi[EvgTranslate]         = &CTLVRenderer::DvgTranslateL;
    vgapi[EvgAppendPathData]    = &CTLVRenderer::DvgAppendPathDataL;
    vgapi[EvgDrawPath]          = &CTLVRenderer::DvgDrawPathL;
    vgapi[EvgClearPath]         = &CTLVRenderer::DvgClearPathL;
    vgapi[EvguRect]             = &CTLVRenderer::DvguRectL;
    vgapi[EvguEllipse]          = &CTLVRenderer::DvguEllipseL;
    vgapi[EvguRoundRect]        = &CTLVRenderer::DvguRoundRectL;
    vgapi[EvguLine]             = &CTLVRenderer::DvguLineL;
    vgapi[EvgCreatePaint]       = &CTLVRenderer::DvgCreatePaintL;
    vgapi[EvgSetiv]             = &CTLVRenderer::DvgSetivL;
    vgapi[EvgClear]             = &CTLVRenderer::DvgClearL;
    vgapi[EvgSetfv]             = &CTLVRenderer::DvgSetfvL;
    vgapi[EvgRotate]            = &CTLVRenderer::DvgRotateL;
    vgapi[EvgCreatePath]        = &CTLVRenderer::DvgCreatePathL;
    vgapi[EvgCreateImage]       = &CTLVRenderer::DvgCreateImageL;
    vgapi[EvgGetPixels]         = &CTLVRenderer::DvgGetPixelsL;
    vgapi[EvgDrawImage]         = &CTLVRenderer::DvgDrawImageL;
    vgapi[EvgClearImage]        = &CTLVRenderer::DvgClearImageL;
    vgapi[EvgImageSubData]      = &CTLVRenderer::DvgImageSubDataL;
    vgapi[EvgDestroyImage]      = &CTLVRenderer::DvgDestroyImageL;
    vgapi[EvgDestroyPaint]      = &CTLVRenderer::DvgDestroyPaintL;
    vgapi[EvgDestroyPath]       = &CTLVRenderer::DvgDestroyPathL;
    vgapi[EvgPrepareToBindImage]= &CTLVRenderer::DvgPrepareToBindImageL;
    vgapi[EvgBindImage]         = &CTLVRenderer::DvgBindImageL;
    vgapi[EvgUnBindImage]       = &CTLVRenderer::DvgUnBindImageL;
    vgapi[EvgFlush]             = &CTLVRenderer::DvgFlushL;

    vgGetMatrix(iUserMatrix);

    iNVGIconData = CNVGIconData::NewL(aBuf);
    iDataLength  = aBuf.Length();

    iNVGIconData->BeginRead();
    iNVGIconData->ReadL(iNvgHeader, sizeof(iNvgHeader));
    
    INIT_HANDLECHECKER()
    }

CTLVRenderer::~CTLVRenderer()
    {
    if (iPathHandle != VG_INVALID_HANDLE)
        {
        VGDESTROYPATH(iPathHandle);
        }

    TInt handleListCount    = iTLVVGHandleList.Count();

    if (handleListCount > 0)
        {
        vgSetPaint(VG_INVALID_HANDLE, VG_FILL_PATH);
        vgSetPaint(VG_INVALID_HANDLE, VG_STROKE_PATH);
        }
    
    for (TInt i = 0; i < handleListCount; i++)
        {
        if (iTLVVGHandleList[i].iVGHandle)
            {
            switch (iTLVVGHandleList[i].iHandleType)
                {
                case TLVVGHandlePair::EVGPATH:
                    VGDESTROYPATH(iTLVVGHandleList[i].iVGHandle);
                    break;
                case TLVVGHandlePair::EVGPAINT:
                    VGDESTROYPAINT(iTLVVGHandleList[i].iVGHandle);
                    break;
                case TLVVGHandlePair::EVGIMAGE:
                    VGDESTROYIMAGE(iTLVVGHandleList[i].iVGHandle);
                    break;
                }
            }
        }

    iTLVVGHandleList.Close();
    if (iNVGIconData)
        {
        iNVGIconData->EndRead();    
        delete iNVGIconData;
        }
    ASSERT_HANDLE_COUNT()
    }

void CTLVRenderer::DvgSetiL()
    {
    TUint16 type;
    TUint16 value;
    type = iNVGIconData->ReadInt16L();
    value = iNVGIconData->ReadInt16L();

    //if ((VGParamType)LargVGParam_value != VG_SCISSORING)
        {
        vgSeti((VGParamType)type, value);
        }

    NVG_DEBUGSTMT(LogvgSeti((VGParamType)type, value));
    }

void CTLVRenderer::DvgSetfL()
    {
    TUint16 type   = iNVGIconData->ReadInt16L();
    VGfloat value     = iNVGIconData->ReadReal32L();

    vgSetf((VGParamType)type, value);

    NVG_DEBUGSTMT(LogvgSetf((VGParamType)type, value));
    }

void CTLVRenderer::DvgSetParameteriL()
    {
    TUint  handle;
    TUint16 type;
    TUint16 value;

    handle = iNVGIconData->ReadInt32L();
    type = iNVGIconData->ReadInt16L();
    value = iNVGIconData->ReadInt16L();

    vgSetParameteri(GetHandleL(handle), type, value);

    NVG_DEBUGSTMT(LogvgSetParameteri(GetHandleL(handle), type, value, handle));
    }

void CTLVRenderer::DvgSetParameterfL()
    {
    TUint handle = iNVGIconData->ReadInt32L();
    TUint16 type = iNVGIconData->ReadInt16L();
    TUint32 value = iNVGIconData->ReadReal32L();


    vgSetParameterf(GetHandleL(handle), type, value);

    NVG_DEBUGP4("vgSetParameterf(%d, %d, %d);\n", handle, type, value);
    }

void CTLVRenderer::DvgSetParameterfvL()
    {
    TUint handle = iNVGIconData->ReadInt32L();
    TUint16 type = iNVGIconData->ReadInt16L();
    float * value;
    TUint32 count = iNVGIconData->ReadInt32L();

    value = new (ELeave) float[count];
    CleanupStack::PushL(TCleanupItem(CleanupArray, value));
    
    for(int i=0; i<count; i++)
        {
        value[i] = iNVGIconData->ReadReal32L();
        }

    vgSetParameterfv(GetHandleL(handle), type, count, value);

    NVG_DEBUGSTMT(LogvgSetParameterfv(GetHandle(handle), type, count, value));
    CleanupStack::PopAndDestroy();
    }

void CTLVRenderer::DvgSetColorL()
    {

    TUint paintvalue = iNVGIconData->ReadInt32L();
    TUint32 rgba = iNVGIconData->ReadInt32L();

    vgSetColor(GetHandleL(paintvalue), rgba);

    NVG_DEBUGP6("vgSetColor(%d, ((%d << 24) | (%d << 16) | (%d << 8) | %d));\n", GetHandleL(paintvalue), (rgba & 0xFF000000) >> 24, (rgba & 0x00FF0000) >> 16,
            (rgba & 0x0000FF00) >> 8, (rgba & 0x000000FF));
    }

void CTLVRenderer::DvgSetPaintL()
    {
    TUint paintvalue = iNVGIconData->ReadInt32L();
    TUint8 value = iNVGIconData->ReadInt8L();

    vgSetPaint(GetHandleL(paintvalue), value);

    NVG_DEBUGSTMT(LogvgSetPaint(GetHandleL(paintvalue), value, paintvalue));
    }

TPoint CTLVRenderer::GetTranslatedPoint(VGfloat aTRMatrix[9], TPoint aPoint)
    {
    TPoint trPoint;

    trPoint.iX = aTRMatrix[0] * aPoint.iX + aTRMatrix[3] * aPoint.iY + aTRMatrix[6];
    trPoint.iY = aTRMatrix[1] * aPoint.iX + aTRMatrix[4] * aPoint.iY + aTRMatrix[7];

    return trPoint;
    }

VGfloat CTLVRenderer::MinVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4 )
    {
    VGfloat min = x1;

    if (min > x2)
        {
        min = x2;
        }
    if (min > x3)
        {
        min = x3;
        }
    if (min > x4)
        {
        min = x4;
        }

    return min;
    }

VGfloat CTLVRenderer::MaxVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4 )
    {
    VGfloat max = x1;

    if (max < x2)
        {
        max = x2;
        }
    if (max < x3)
        {
        max = x3;
        }
    if (max < x4)
        {
        max = x4;
        }

    return max;
    }

void CTLVRenderer::DvgSetivL()
    {
    TUint16 type = iNVGIconData->ReadInt16L();
    TUint16 count = iNVGIconData->ReadInt16L();
    VGint * value;

    value = new (ELeave) VGint[count];
    CleanupStack::PushL(TCleanupItem(CleanupArray, value));
    
    for(int i = 0; i < count; i++)
        {
        value[i] = iNVGIconData->ReadInt32L();
        }

    if (type == VG_SCISSOR_RECTS)
        {
        VGfloat * matrix = iUserMatrix;

        // calculate the rectangle with respect to the transformation applied
        for(int i = 0; i < count; i += 4)
            {
            TInt sx = i + 0;
            TInt sy = i + 1;
            TInt sw = i + 2;
            TInt sh = i + 3;

            TPoint leftBottom  = GetTranslatedPoint(matrix,
                                        TPoint(value[sx], value[sy]));
            TPoint leftTop     = GetTranslatedPoint(matrix,
                                        TPoint(value[sx], value[sy] + value[sh]));
            TPoint rightBottom = GetTranslatedPoint(matrix,
                                        TPoint(value[sx] + value[sw], value[sy]));
            TPoint rightTop    = GetTranslatedPoint(matrix,
                                        TPoint(value[sx] + value[sw], value[sy] + value[sh]));


            VGfloat minX = leftBottom.iX;
            VGfloat minY = leftBottom.iY;
            VGfloat maxX = leftBottom.iX;
            VGfloat maxY = leftBottom.iY;

            minX = MinVal4(leftBottom.iX, leftTop.iX, rightBottom.iX, rightTop.iX);
            minY = MinVal4(leftBottom.iY, leftTop.iY, rightBottom.iY, rightTop.iY);

            maxX = MaxVal4(leftBottom.iX, leftTop.iX, rightBottom.iX, rightTop.iX);
            maxY = MaxVal4(leftBottom.iY, leftTop.iY, rightBottom.iY, rightTop.iY);

            VGfloat newW = maxX - minX;
            VGfloat newH = maxY - minY;

            value[sx] = minX;
            value[sy] = minY;
            value[sw] = newW;
            value[sh] = newH;

            /*
            if (newW > iTargetWidth)
                {
                newW = iTargetWidth;
                }
            if (newH > iTargetHeight)
                {
                newH = iTargetHeight;
                }
             */
            }
        }

    vgSetiv((VGParamType)type, count, value);

    NVG_DEBUGSTMT(LogvgSetXv((VGParamType)type, count, 'i', value));
    CleanupStack::PopAndDestroy();
    }

void CTLVRenderer::DvgLoadMatrixL()
    {
    VGfloat matrix[9];
    for(int i=0;i<9;i++)
        {
        matrix[i] = iNVGIconData->ReadReal32L();
        }
    // keep the caller's matrix as base
    TInt matrixMode = vgGeti(VG_MATRIX_MODE);

    if (matrixMode == VG_MATRIX_PATH_USER_TO_SURFACE ||
        matrixMode == VG_MATRIX_IMAGE_USER_TO_SURFACE /* need to check?*/)
        {
        vgLoadMatrix(iUserMatrix);
        vgMultMatrix(matrix);
        }
    else
        {
        vgLoadMatrix(matrix);
        }

#ifdef NVG_DEBUG
    int matrixCount = clock() % 1000;
    NVG_DEBUGP2("VGfloat ldMatrix%d[] = {", matrixCount);
    
    NVG_DEBUGP4("    %f, %f, %f, ", matrix[0], matrix[1], matrix[2]);
    NVG_DEBUGP4("    %f, %f, %f, ", matrix[3], matrix[4], matrix[5]);
    NVG_DEBUGP4("    %f, %f, %f};\n", matrix[6], matrix[7], matrix[8]);

    NVG_DEBUGP2("vgLoadMatrix(ldMatrix%d);\n", matrixCount);
#endif
    }

void CTLVRenderer::DvgMultMatrixL()
    {
    VGfloat matrix[9];
    for (int i = 0; i < 9; i++)
        {
        matrix[i] = iNVGIconData->ReadReal32L();
        }

    vgMultMatrix(matrix);

#ifdef NVG_DEBUG
    int matrixCount = clock() % 1000;
    NVG_DEBUGP2("VGfloat mlMatrix%d[] = {", ++matrixCount);

    NVG_DEBUGP1("VGfloat matrix[] = {");
    NVG_DEBUGP4("    %f, %f, %f, ", matrix[0], matrix[1], matrix[2]);
    NVG_DEBUGP4("    %f, %f, %f, ", matrix[3], matrix[4], matrix[5]);
    NVG_DEBUGP4("    %f, %f, %f};\n", matrix[6], matrix[7], matrix[8]);

    NVG_DEBUGP2("vgMultMatrix(mlMatrix%d);\n", matrixCount);
#endif
    }

void CTLVRenderer::DvgLoadIdentityL()
    {
    if (vgGeti(VG_MATRIX_MODE) == (VGint)VG_MATRIX_IMAGE_USER_TO_SURFACE)
        {
        vgLoadIdentity();
        }
    NVG_DEBUGP1("vgLoadIdentity();\n");
    }

void CTLVRenderer::DvgScaleL()
    {
    VGfloat sx = iNVGIconData->ReadReal32L();
    VGfloat sy = iNVGIconData->ReadReal32L();
    vgScale(sx, sy);

    NVG_DEBUGP3("vgScale(%f, %f);\n", sx, sy);
    }

void CTLVRenderer::DvgTranslateL()
    {
    VGfloat tx = iNVGIconData->ReadReal32L();
    VGfloat ty = iNVGIconData->ReadReal32L();

    vgTranslate(tx, ty);

    NVG_DEBUGP3("vgTranslate(%f, %f);\n", tx, ty);
    }

void CTLVRenderer::DvgAppendPathDataL()
    {
    TUint16         numSegments;
    TUint16         coordinatecount;
    VGubyte *       pathSegments;
    VGubyte *       segmenthandle;
    TFloatFixPt *   pathData;
    TFloatFixPt *   coordhandle;

    iNVGIconData->ReadInt32L();
    numSegments = iNVGIconData->ReadInt16L();

    pathSegments = new (ELeave) VGubyte[numSegments];
    CleanupStack::PushL(TCleanupItem(CleanupArray, pathSegments));
    
    segmenthandle = pathSegments;
    for(int j=0;j<numSegments;j++, segmenthandle++)
        {
        *segmenthandle = iNVGIconData->ReadInt8L();
        }

    coordinatecount = iNVGIconData->ReadInt16L();
    pathData = new (ELeave) TFloatFixPt[coordinatecount];
    CleanupStack::PushL(TCleanupItem(CleanupTFloatFixArray, pathData));
    
    TInt lFloatFixVal = 0;
    coordhandle= pathData;
    for(int i=0;i<coordinatecount;i++, coordhandle++)
        {
        lFloatFixVal = iNVGIconData->ReadInt32L();
        coordhandle->copyfloatfix(lFloatFixVal);
        }

    vgAppendPathData(iPathHandle, numSegments, pathSegments, pathData);

    NVG_DEBUGP3("vgAppendPathData((VGPath)%d, (SENGMENTS)%d);\n", iPathHandle, numSegments);

    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    }

void CTLVRenderer::DvgDrawPathL()
    {
    TUint16 value;

    iNVGIconData->ReadInt32L();
    value = iNVGIconData->ReadInt16L();

    vgDrawPath(iPathHandle, value);
    vgClearPath(iPathHandle, VG_PATH_CAPABILITY_APPEND_TO);

    NVG_DEBUGSTMT(LogvgDrawPath(iPathHandle, value));
    }

void CTLVRenderer::DvgClearPathL()
    {
    TUint16 value;

    iNVGIconData->ReadInt32L();
    value = iNVGIconData->ReadInt16L();

    vgClearPath(iPathHandle, value);
    NVG_DEBUGP1("vgClearPath();\n");
    }

void CTLVRenderer::DvgCreatePaintL()
    {
    TUint  encodedHandle = iNVGIconData->ReadInt32L();

    VGPaint paintH = VGCREATEPAINT();
    if (paintH == VG_INVALID_HANDLE)
        {
        User::LeaveIfError(CNvgEngine::OpenVGErrorToSymbianError(vgGetError()));
        }

    TInt error = iTLVVGHandleList.Append(TLVVGHandlePair(paintH, encodedHandle, TLVVGHandlePair::EVGPAINT));
    if (error != KErrNone)
        {
        VGDESTROYPAINT(paintH);
        User::Leave(error);
        }

    NVG_DEBUGP2("%d = vgCreatePaint();\n", GetHandleL(encodedHandle));
    }

void CTLVRenderer::DvguRectL()
    {

    TReal32 x;
    TReal32 y;
    TReal32 width;
    TReal32 height;

    iNVGIconData->ReadInt32L();

    x   = iNVGIconData->ReadReal32L();
    y   = iNVGIconData->ReadReal32L();

    width   = iNVGIconData->ReadReal32L();
    height  = iNVGIconData->ReadReal32L();

    vguRect(iPathHandle, x, y, width, height);

    NVG_DEBUGP6("vguRect(%d, %f, %f, %f, %f);\n", iPathHandle, x, y, width, height);

    //##########workaround for graphics library which doesnt support vgu libraries:Start########
    /*
    const VGubyte pathSegments[] =
    {
        VG_MOVE_TO_ABS, VG_HLINE_TO_REL,
        VG_VLINE_TO_REL, VG_HLINE_TO_REL,
        VG_CLOSE_PATH
    };
    TFloatFixPt dataff[5];

    dataff[0]=x;
    dataff[1]= y;
    dataff[2]= width;
    dataff[3]= height;
    dataff[4]= -width;

	TInt32 numCommands = sizeof(pathSegments)/sizeof(pathSegments[0]);
    vgAppendPathData(iPathHandle, numCommands, pathSegments, dataff);
    */
    //##########workaround for graphics library which doesnt support vgu libraries:End########
    }

void CTLVRenderer::DvguEllipseL()
    {
    TReal32 cx;
    TReal32 cy;
    TReal32 width;
    TReal32 height;

    iNVGIconData->ReadInt32L();

    cx = iNVGIconData->ReadReal32L();
    cy = iNVGIconData->ReadReal32L();

    width  = iNVGIconData->ReadReal32L();
    height = iNVGIconData->ReadReal32L();

    vguEllipse(iPathHandle, cx, cy, width, height);

    NVG_DEBUGP6("vguEllipse(%d, %f, %f, %f, %f);\n",iPathHandle, cx, cy, width, height);

    //##########workaround for graphics library which doesnt support vgu libraries:Start########
    /*
    {
    const VGubyte pathSegments[] =
    {
        VG_MOVE_TO_ABS,
        VG_SCCWARC_TO_REL,
        VG_SCCWARC_TO_REL,
        VG_CLOSE_PATH
    };

    VGfloat widthDiv2    = width / 2;
    VGfloat heightDiv2   = height / 2;

    TFloatFixPt data[12];
    data[0]		= cx + widthDiv2;
    data[1]		= cy;
    data[2]		= widthDiv2;
    data[3]		= heightDiv2;
    data[4]		= 0;
    data[5]		= -width;
    data[6]		= 0;
    data[7]		= widthDiv2;
    data[8]		= heightDiv2;
    data[9]		= 0;
    data[10]	= width;
    data[11]	= 0;

    TInt32 numCommands = sizeof(pathSegments)/sizeof(pathSegments[0]);

    vgAppendPathData(iPathHandle, numCommands, pathSegments, data);
    }              										 */
    //##########workaround for graphics library which doesnt support vgu libraries:End########
    }

void CTLVRenderer::DvguRoundRectL()
    {
    TReal32 x;
    TReal32 y;
    TReal32 width;
    TReal32 height;
    TReal32 arcWidth;
    TReal32 arcHeight;

    iNVGIconData->ReadInt32L();

    x = iNVGIconData->ReadReal32L();
    y = iNVGIconData->ReadReal32L();

    width = iNVGIconData->ReadReal32L();
    height= iNVGIconData->ReadReal32L();

    arcWidth = iNVGIconData->ReadReal32L();
    arcHeight= iNVGIconData->ReadReal32L();

    //uncomment
    //vguRoundRect(iPathHandle, x, y, width, height, arcWidth, arcHeight);

    NVG_DEBUGP8("vguRoundRect(%d, %f, %f, %f, %f, %f, %f);\n", iPathHandle, x, y, width, height, arcWidth, arcHeight);

    //##########workaround for graphics library which doesnt support vgu libraries:Start########
    /* commented*/           {
    if(arcWidth < 0.0f)
            arcWidth = 0.0f;
    else if(arcWidth > width)
            arcWidth = width;

    if(arcHeight < 0.0f)
            arcHeight = 0.0f;
    else if(arcHeight > height)
            arcHeight = height;

    const VGubyte pathSegments[] =
    {
        VG_MOVE_TO_ABS, VG_HLINE_TO_REL,
        VG_SCCWARC_TO_REL, VG_VLINE_TO_REL,
        VG_SCCWARC_TO_REL, VG_HLINE_TO_REL,
        VG_SCCWARC_TO_REL, VG_VLINE_TO_REL,
        VG_SCCWARC_TO_REL, VG_CLOSE_PATH
    };
    
    VGfloat arcWidthDiv2    = arcWidth / 2;
    VGfloat arcHeightDiv2   = arcHeight / 2;
    
    TFloatFixPt data[26];
    data[0] 	= (x + arcWidthDiv2);
    data[1] 	= y;
    data[2] 	= width - arcWidth;
    data[3] 	= arcWidthDiv2;
    data[4] 	= arcHeightDiv2;
    data[5] 	= 0;
    data[6] 	= arcWidthDiv2;
    data[7] 	= arcHeightDiv2;
    data[8] 	= height - arcHeight;
    data[9]		= arcWidthDiv2;
    data[10] 	= arcHeightDiv2;
    data[11] 	= 0;
    data[12] 	= -arcWidthDiv2;
    data[13] 	= arcHeightDiv2;
    data[14] 	= -(width - arcWidth);
    data[15] 	= arcWidthDiv2;
    data[16] 	= arcHeightDiv2;
    data[17] 	= 0;
    data[18] 	= -arcWidthDiv2;
    data[19] 	= -arcHeightDiv2;
    data[20] 	= -(height - arcHeight);
    data[21] 	= arcWidthDiv2;
    data[22] 	= arcHeightDiv2;
    data[23] 	= 0;
    data[24] 	= arcWidthDiv2;
    data[25] 	= -arcHeightDiv2;
    
    TInt32 numCommands = sizeof(pathSegments)/sizeof(pathSegments[0]);
    
    vgAppendPathData(iPathHandle, numCommands, pathSegments, data);

                    }/* commented */
    //##########workaround for graphics library which doesnt support vgu libraries:End########
    }

void CTLVRenderer::DvguLineL()
    {
    TReal32 x0;
    TReal32 y0;
    TReal32 x1;
    TReal32 y1;

    iNVGIconData->ReadInt32L();

    x0 = iNVGIconData->ReadReal32L();
    y0 = iNVGIconData->ReadReal32L();

    x1 = iNVGIconData->ReadReal32L();
    y1 = iNVGIconData->ReadReal32L();

    vguLine(iPathHandle, x0, y0, x1, y1);

    NVG_DEBUGP6("vguLine(%d, %f, %f, %f, %f);\n", iPathHandle, x0, y0, x1, y1);

    //##########workaround for graphics library which doesnt support vgu libraries:Start########
    /*              {
    const VGubyte pathSegments[] =
    {
        VG_MOVE_TO_ABS, VG_LINE_TO_ABS
    };

    TFloatFixPt data[4];
    data[0] = x0;
    data[1] = y0;
    data[2] = x1;
    data[3] = y1;

	TInt32 numCommands = sizeof(pathSegments)/sizeof(pathSegments[0]);

    vgAppendPathData(iPathHandle, numCommands, pathSegments, data);
                    }*/
    //##########workaround for graphics library which doesnt support vgu libraries:End########

    }

void CTLVRenderer::DvgClearL()
    {

    TReal32 width;
    TReal32 height;

    iNVGIconData->ReadReal32L();
    iNVGIconData->ReadReal32L();
    width  = iNVGIconData->ReadReal32L();
    height = iNVGIconData->ReadReal32L();

    if (!iScaled)
        {
        VGfloat scaleX = ((VGfloat)iTargetWidth)  / width;
        VGfloat scaleY = ((VGfloat)iTargetHeight) / height;
        vgLoadMatrix(iUserMatrix);
        vgTranslate(((VGfloat)iTargetWidth) / 2, ((VGfloat)iTargetHeight) / 2);
        vgScale(scaleX, scaleY);
        vgTranslate(- width / 2, - height / 2);

        vgGetMatrix(iUserMatrix);
        iScaled = ETrue;
        }
#if 0
    // create a path and fill with color.
    VGint color = vgGeti(VG_CLEAR_COLOR);
    NVG_DEBUGP2("\nclear color %d", color);
    vgClearPath(iPathHandle, VG_PATH_CAPABILITY_APPEND_TO);

    const VGubyte pathSegments[] =
    {
            VG_MOVE_TO_ABS,
            VG_LINE_TO_ABS,
            VG_LINE_TO_ABS,
            VG_LINE_TO_ABS,
            VG_CLOSE_PATH
    };
    VGfloat* coords = NULL;
    coords = new  VGfloat[8];
    coords[0] = x;
    coords[1] = y;
    coords[2] = x + width;
    coords[3] = y;
    coords[4] = x + width;
    coords[5] = y + height;
    coords[6] = x;
    coords[7] = y + height;

    VGint orgPaint = vgGetPaint(VG_FILL_PATH);
    VGint paint = VGCREATEPAINT();
    vgSetPaint(paint, VG_FILL_PATH);
    vgSetColor(paint, color);
    vgAppendPathData(iPathHandle, 5, pathSegments, coords);

    vgDrawPath(iPathHandle, VG_FILL_PATH);
    vgClearPath(iPathHandle, VG_PATH_CAPABILITY_APPEND_TO);
    vgSetPaint(orgPaint, VG_FILL_PATH);
    VGDESTROYPAINT(paint);
#endif

    NVG_DEBUGP1("vgClear();\n");
    }

void CTLVRenderer::DvgSetfvL()
    {
    TInt innerpos = 1;
    TUint16 type;
    TUint16 count;
    VGfloat * value;

    type  = iNVGIconData->ReadInt16L();
    count = iNVGIconData->ReadInt16L();

    value = new (ELeave) VGfloat[count];
    CleanupStack::PushL(TCleanupItem(CleanupArray, value));
    
    for(int i=0; i<count; i++)
        {
        value[i] = iNVGIconData->ReadReal32L();
        }
    
    innerpos = innerpos + 2*sizeof(TUint16) + count*sizeof(VGfloat);

    vgSetfv((VGParamType)type, count, value);

    NVG_DEBUGSTMT(LogvgSetXv((VGParamType)type, count, 'f', (VGint *)value));
    
    CleanupStack::PopAndDestroy();
    }

void CTLVRenderer::DvgRotateL()
    {
    TReal32 angle;

    angle = iNVGIconData->ReadReal32L();

    vgRotate(angle);

    NVG_DEBUGP2("vgRotate(%f);\n", angle);
    }

void CTLVRenderer::DvgCreatePathL()
    {
    TUint32 pathFormat;
    TUint8  datatype;
    TReal32 scale;
    TReal32 bias;
    TUint32 segmentCapacityHint;
    TUint32 coordCapacityHint;
    TUint32 capabilities;

    pathFormat = iNVGIconData->ReadInt32L();
    datatype = iNVGIconData->ReadInt8L();

    scale = iNVGIconData->ReadReal32L();
    bias  = iNVGIconData->ReadReal32L();

    segmentCapacityHint = iNVGIconData->ReadInt32L();
    coordCapacityHint = iNVGIconData->ReadInt32L();
    capabilities  = iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt32L();

    iPathHandle = VGCREATEPATH(pathFormat, (VGPathDatatype)datatype, scale, bias, segmentCapacityHint, coordCapacityHint,
            (VGbitfield)capabilities);
    
    if (iPathHandle == VG_INVALID_HANDLE)
        {
        User::LeaveIfError(CNvgEngine::OpenVGErrorToSymbianError(vgGetError()));
        }
    
    NVG_DEBUGP9("%d = vgCreatePath(%d, (VGPathDatatype)%d, %f, %f, %d, %d, (VGPathCapabilities)%d);\n",
                iPathHandle, pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities);
    }

void CTLVRenderer::DvgCreateImageL()
    {
    TInt32 format   = (VGImageFormat)iNVGIconData->ReadInt32L();
    TInt width      = (VGImageFormat)iNVGIconData->ReadInt32L();
    TInt height     = (VGImageFormat)iNVGIconData->ReadInt32L();
    TUint8 aq       = (VGImageFormat)iNVGIconData->ReadInt8L();

    TInt    encodedHandle = iNVGIconData->ReadInt32L();
    VGImage image = 0;

    if (iPrepareToBindImage)
        {
        TPoint leftBottom   = GetTranslatedPoint(iUserMatrix, TPoint(0, 0));
        TPoint rightBottom  = GetTranslatedPoint(iUserMatrix, TPoint(width, 0));
        TPoint leftTop      = GetTranslatedPoint(iUserMatrix, TPoint(0, height));
        TPoint rightTop     = GetTranslatedPoint(iUserMatrix, TPoint(width, height));

        VGfloat maxX = leftBottom.iX;
        VGfloat maxY = leftBottom.iY;

        maxX = MaxVal4(leftBottom.iX, leftTop.iX, rightBottom.iX, rightTop.iX);
        maxY = MaxVal4(leftBottom.iY, leftTop.iY, rightBottom.iY, rightTop.iY);

        width  = maxX - 0;
        height = maxY - 0;

        iPrepareToBindImage = 0;
        }

    image = VGCREATEIMAGE((VGImageFormat)format,
                               width, height, aq);

    if (image == VG_INVALID_HANDLE)
        {
        User::LeaveIfError(CNvgEngine::OpenVGErrorToSymbianError(vgGetError()));
        }
    
    TInt error = iTLVVGHandleList.Append(TLVVGHandlePair(image, encodedHandle, TLVVGHandlePair::EVGIMAGE, aq));
    if (error != KErrNone)
        {
        VGDESTROYIMAGE(image);
        User::Leave(error);
        }
    
    NVG_DEBUGP6("%d = vgCreateImage(%d, %d, %d, %d);\n", GetHandleL(encodedHandle), format, width, height, (TInt32)aq);
    }

void CTLVRenderer::DvgClearImageL()
    {
    TInt encodedImageHandle = iNVGIconData->ReadInt32L();
    VGImage imageHandle     = GetHandleL(encodedImageHandle);

    TInt x      = iNVGIconData->ReadInt32L();
    TInt y      = iNVGIconData->ReadInt32L();
    TInt width  = iNVGIconData->ReadInt32L();
    TInt height = iNVGIconData->ReadInt32L();
    vgClearImage(imageHandle, x, y, width, height);

    NVG_DEBUGP6("vgClearImage(%d, %d, %d, %d, %d);\n", imageHandle, x, y, width, height);
    }

void CTLVRenderer::DvgDrawImageL()
    {
    TInt encodedImageHandle = iNVGIconData->ReadInt32L();
    VGImage imageHandle     = GetHandleL(encodedImageHandle);

    vgDrawImage(imageHandle);

    NVG_DEBUGP2("vgDrawImage(%d);\n", imageHandle);
    }

void CTLVRenderer::DvgImageSubDataL()
    {
    TInt encodedImageHandle = iNVGIconData->ReadInt32L();
    VGImage imageHandle     = GetHandleL(encodedImageHandle);
    TInt dataLength         = 0;

    TInt dataStride = iNVGIconData->ReadInt32L();
    TInt dataFormat = iNVGIconData->ReadInt32L();
    TInt x          = iNVGIconData->ReadInt32L();
    TInt y          = iNVGIconData->ReadInt32L();
    TInt width      = iNVGIconData->ReadInt32L();
    TInt height     = iNVGIconData->ReadInt32L();

    dataLength      = iNVGIconData->ReadInt32L();
    if (dataLength)
        {
        TUint8 * data   = new (ELeave) TUint8[dataLength];
        CleanupStack::PushL(TCleanupItem(CleanupArray, data));

        iNVGIconData->ReadL(data, dataLength);
        TUint8 * dataPtr;
        if (dataStride < 0)
            {
            dataPtr = data + ( height - 1 ) * (-dataStride);
            }
        else
            {
            dataPtr = data;
            }

        vgImageSubData(imageHandle, dataPtr, dataStride, (VGImageFormat)dataFormat, x, y, width, height);

        NVG_DEBUGSTMT(LogvgImageSubData(imageHandle, dataPtr, dataLength, dataStride, (VGImageFormat)dataFormat, x, y, width, height));
        CleanupStack::PopAndDestroy();
        }
    else
        {
        User::Leave(KErrCorrupt);
        }
    }

void CTLVRenderer::DvgGetPixelsL()
    {
    //not used in SVGEngine
    }

void CTLVRenderer::DvgDestroyImageL()
    {
    VGImage imageHandle = RemoveHandleL(iNVGIconData->ReadInt32L());
    VGDESTROYIMAGE(imageHandle);
    NVG_DEBUGP2("vgDestroyImage(%d);\n", imageHandle);
    }

void CTLVRenderer::DvgDestroyPaintL()
    {
    TInt paint = iNVGIconData->ReadInt32L();
    VGDESTROYPAINT(RemoveHandleL(paint));
    NVG_DEBUGP2("vgDestroyPaint(%d);\n", GetHandleL(paint));
    }

void CTLVRenderer::DvgDestroyPathL()
    {
    // only one path handle is maintained, which is a member variable
    NVG_DEBUGP2("vgDestroyPath(%d);\n", iPathHandle);
    }

void CTLVRenderer::DvgFlushL()
    {
    //no need to implement?
    NVG_DEBUGP1("vgFlush();\n");
    }

TInt CTLVRenderer::ExecuteL(TInt aIndex)
    {
    if (0 <= aIndex && aIndex < EvgFlush + 1)
        {
        (this->*(vgapi[aIndex]))();
        }
    else
        {
        return KErrCorrupt;
        }
    
    return KErrNone;
    }

TInt CTLVRenderer::ExecuteL()
    {
    TInt error = KErrNone;

    NVG_DEBUGP1("TLV rendering starts");
    
    while (!iNVGIconData->EOF())
        {
        error = ExecuteL(iNVGIconData->ReadInt8L());
        if (error != KErrNone)
            {
            break;
            }
        }

    NVG_DEBUGP1("TLV rendering ends");
    
    return error;
    }

void CTLVRenderer::GetHandlePair(int aIndex, TLVVGHandlePair & aPair)
    {
    TInt handleListCount    = iTLVVGHandleList.Count();

    for (TInt i = 0; i < handleListCount; i++)
        {
        if (iTLVVGHandleList[i].iTLVHandle == aIndex)
            {
            aPair = iTLVVGHandleList[i];
            break;
            }
        }
    }

VGHandle CTLVRenderer::RemoveHandleL(int aIndex)
    {
    TInt handleListCount    = iTLVVGHandleList.Count();
    VGHandle handle         = 0;

    for (TInt i = 0; i < handleListCount; i++)
        {
        if (iTLVVGHandleList[i].iTLVHandle == aIndex)
            {
            handle = iTLVVGHandleList[i].iVGHandle;
            iTLVVGHandleList.Remove(i);
            break;
            }
        }
    
    if (!handle)
        {
        User::Leave(KErrCorrupt);
        }
    return handle;
    }

VGHandle CTLVRenderer::GetHandleL(int aIndex)
    {
    VGHandle handle = GetHandle(aIndex);

    if (!handle)
        {
        User::Leave(KErrCorrupt);
        }
    return handle;
    }

VGHandle CTLVRenderer::GetHandle(int aIndex)
    {
    TInt handleListCount    = iTLVVGHandleList.Count();
    VGHandle handle         = 0;

    for (TInt i = 0; i < handleListCount; i++)
        {
        if (iTLVVGHandleList[i].iTLVHandle == aIndex)
            {
            handle = iTLVVGHandleList[i].iVGHandle;
            break;
            }
        }
    return handle;
    }

void CTLVRenderer::SetHandle(int aIndex, VGHandle aHandle)
    {
    TInt handleListCount    = iTLVVGHandleList.Count();

    for (TInt i = 0; i < handleListCount; i++)
        {
        if (iTLVVGHandleList[i].iTLVHandle == aIndex)
            {
            iTLVVGHandleList[i].iVGHandle = aHandle;
            break;
            }
        }
    }

void CTLVRenderer::DvgPrepareToBindImageL()
    {
    iPrepareToBindImage = 1;
    NVG_DEBUGP1("vgPrepareToBindImage();\n");
    }

void CTLVRenderer::DvgBindImageL()
    {
    iPrepareToBindImage = 0;

    TInt imageHandle = iNVGIconData->ReadInt32L();

    if (iVGImageBinder)
        {
        iVGImageBinder->BindClientBuffer((VGHandle)GetHandleL(imageHandle));
        }
    NVG_DEBUGP2("vgBindImage(%d);\n", imageHandle);
    }

void CTLVRenderer::DvgUnBindImageL()
    {
    if (iVGImageBinder)
        {
        iVGImageBinder->UnBindClientBuffer();
        }
    NVG_DEBUGP1("vgUnBindImage();\n");
    }

#ifdef NVG_DEBUG
void CTLVRenderer::LogvgSetf(VGParamType type, VGfloat value)
    {
    RDebug::Print(_L("vgSetf("));
    switch(type)
        {
        case VG_STROKE_LINE_WIDTH:
            {
            RDebug::Print(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;
        case VG_STROKE_MITER_LIMIT:
            {
            RDebug::Print(_L("VG_STROKE_MITER_LIMIT"));
            }
            break;
        case VG_STROKE_DASH_PHASE:
            {
            RDebug::Print(_L("VG_STROKE_DASH_PHASE"));
            }
            break;

        case VG_MATRIX_MODE:
            {
            RDebug::Print(_L("VG_MATRIX_MODE"));
            }
            break;
        case VG_FILL_RULE:
            {
            RDebug::Print(_L("VG_FILL_RULE"));
            }
            break;
        case VG_IMAGE_QUALITY:
            {
            RDebug::Print(_L("VG_IMAGE_QUALITY"));
            }
            break;
        case VG_IMAGE_MODE:
            {
            RDebug::Print(_L("VG_IMAGE_MODE"));
            }
            break;
        case VG_RENDERING_QUALITY:
            {
            RDebug::Print(_L("VG_RENDERING_QUALITY"));
            }
            break;
        case VG_BLEND_MODE:
            {
            RDebug::Print(_L("VG_BLEND_MODE"));
            }
            break;
        case VG_MASKING:
            {
            RDebug::Print(_L("VG_MASKING"));
            }
            break;
        case VG_SCISSORING:
            {
            RDebug::Print(_L("VG_SCISSORING"));
            }
            break;
        case VG_PIXEL_LAYOUT:
            {
            RDebug::Print(_L("VG_PIXEL_LAYOUT"));
            }
            break;
        case VG_FILTER_FORMAT_LINEAR:
            {
            RDebug::Print(_L("VG_FILTER_FORMAT_LINEAR"));
            }
            break;
        case VG_FILTER_FORMAT_PREMULTIPLIED:
            {
            RDebug::Print(_L("VG_FILTER_FORMAT_PREMULTIPLIED"));
            }
            break;
        case VG_FILTER_CHANNEL_MASK:
            {
            RDebug::Print(_L("VG_FILTER_CHANNEL_MASK"));
            }
            break;
        case VG_STROKE_CAP_STYLE:
            {
            RDebug::Print(_L("VG_STROKE_CAP_STYLE"));
            }
            break;
        case VG_STROKE_JOIN_STYLE:
            {
            RDebug::Print(_L("VG_STROKE_JOIN_STYLE"));
            }
            break;
        case VG_STROKE_DASH_PHASE_RESET:
            {
            RDebug::Print(_L("VG_STROKE_DASH_PHASE_RESET"));
            }
            break;
            /* Implementation limits (read-only) */
        case VG_SCREEN_LAYOUT:
            {
            RDebug::Print(_L("VG_SCREEN_LAYOUT"));
            }
            break;
        case VG_MAX_SCISSOR_RECTS:
            {
            RDebug::Print(_L("VG_MAX_SCISSOR_RECTS"));
            }
            break;
        case VG_MAX_DASH_COUNT:
            {
            RDebug::Print(_L("VG_MAX_DASH_COUNT"));
            }
            break;
        case VG_MAX_KERNEL_SIZE:
            {
            RDebug::Print(_L("VG_MAX_KERNEL_SIZE"));
            }
            break;
        case VG_MAX_SEPARABLE_KERNEL_SIZE:
            {
            RDebug::Print(_L("VG_MAX_SEPARABLE_KERNEL_SIZE"));
            }
            break;
        case VG_MAX_COLOR_RAMP_STOPS:
            {
            RDebug::Print(_L("VG_MAX_COLOR_RAMP_STOPS"));
            }
            break;
        case VG_MAX_IMAGE_WIDTH:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_WIDTH"));
            }
            break;
        case VG_MAX_IMAGE_HEIGHT:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_HEIGHT"));
            }
            break;
        case VG_MAX_IMAGE_PIXELS:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_PIXELS"));
            }
            break;
        case VG_MAX_IMAGE_BYTES:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_BYTES"));
            }
            break;
        case VG_MAX_FLOAT:
            {
            RDebug::Print(_L("VG_MAX_FLOAT"));
            }
            break;
        case VG_MAX_GAUSSIAN_STD_DEVIATION:
            {
            RDebug::Print(_L("VG_MAX_GAUSSIAN_STD_DEVIATION"));
            }
            break;

        default:
            {
            RDebug::Print(_L("%d"), type);
            }
            break;
        };

    RDebug::Print(_L(", "));
    RDebug::Print(_L("%f"), value);
    RDebug::Print(_L(");\n"));
    return;
    }

void CTLVRenderer::LogvgSeti (VGParamType type, VGint value)
    {
    RDebug::Print(_L("vgSeti("));

    switch(type)
        {
        case VG_STROKE_LINE_WIDTH:
            {
            RDebug::Print(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;
        case VG_STROKE_MITER_LIMIT:
            {
            RDebug::Print(_L("VG_STROKE_MITER_LIMIT"));
            }
            break;
        case VG_STROKE_DASH_PHASE:
            {
            RDebug::Print(_L("VG_STROKE_DASH_PHASE"));
            }
            break;

        case VG_MATRIX_MODE:
            {
            RDebug::Print(_L("VG_MATRIX_MODE"));
            }
            break;
        case VG_FILL_RULE:
            {
            RDebug::Print(_L("VG_FILL_RULE"));
            }
            break;
        case VG_IMAGE_QUALITY:
            {
            RDebug::Print(_L("VG_IMAGE_QUALITY"));
            }
            break;
        case VG_IMAGE_MODE:
            {
            RDebug::Print(_L("VG_IMAGE_MODE"));
            }
            break;
        case VG_RENDERING_QUALITY:
            {
            RDebug::Print(_L("VG_RENDERING_QUALITY"));
            }
            break;
        case VG_BLEND_MODE:
            {
            RDebug::Print(_L("VG_BLEND_MODE"));
            }
            break;
        case VG_MASKING:
            {
            RDebug::Print(_L("VG_MASKING"));
            }
            break;
        case VG_SCISSORING:
            {
            RDebug::Print(_L("VG_SCISSORING"));
            }
            break;
        case VG_PIXEL_LAYOUT:
            {
            RDebug::Print(_L("VG_PIXEL_LAYOUT"));
            }
            break;
        case VG_FILTER_FORMAT_LINEAR:
            {
            RDebug::Print(_L("VG_FILTER_FORMAT_LINEAR"));
            }
            break;
        case VG_FILTER_FORMAT_PREMULTIPLIED:
            {
            RDebug::Print(_L("VG_FILTER_FORMAT_PREMULTIPLIED"));
            }
            break;
        case VG_FILTER_CHANNEL_MASK:
            {
            RDebug::Print(_L("VG_FILTER_CHANNEL_MASK"));
            }
            break;
        case VG_STROKE_CAP_STYLE:
            {
            RDebug::Print(_L("VG_STROKE_CAP_STYLE"));
            }
            break;
        case VG_STROKE_JOIN_STYLE:
            {
            RDebug::Print(_L("VG_STROKE_JOIN_STYLE"));
            }
            break;
        case VG_STROKE_DASH_PHASE_RESET:
            {
            RDebug::Print(_L("VG_STROKE_DASH_PHASE_RESET"));
            }
            break;
            /* Implementation limits (read-only) */
        case VG_SCREEN_LAYOUT:
            {
            RDebug::Print(_L("VG_SCREEN_LAYOUT"));
            }
            break;
        case VG_MAX_SCISSOR_RECTS:
            {
            RDebug::Print(_L("VG_MAX_SCISSOR_RECTS"));
            }
            break;
        case VG_MAX_DASH_COUNT:
            {
            RDebug::Print(_L("VG_MAX_DASH_COUNT"));
            }
            break;
        case VG_MAX_KERNEL_SIZE:
            {
            RDebug::Print(_L("VG_MAX_KERNEL_SIZE"));
            }
            break;
        case VG_MAX_SEPARABLE_KERNEL_SIZE:
            {
            RDebug::Print(_L("VG_MAX_SEPARABLE_KERNEL_SIZE"));
            }
            break;
        case VG_MAX_COLOR_RAMP_STOPS:
            {
            RDebug::Print(_L("VG_MAX_COLOR_RAMP_STOPS"));
            }
            break;
        case VG_MAX_IMAGE_WIDTH:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_WIDTH"));
            }
            break;
        case VG_MAX_IMAGE_HEIGHT:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_HEIGHT"));
            }
            break;
        case VG_MAX_IMAGE_PIXELS:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_PIXELS"));
            }
            break;
        case VG_MAX_IMAGE_BYTES:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_BYTES"));
            }
            break;
        case VG_MAX_FLOAT:
            {
            RDebug::Print(_L("VG_MAX_FLOAT"));
            }
            break;
        case VG_MAX_GAUSSIAN_STD_DEVIATION:
            {
            RDebug::Print(_L("VG_MAX_GAUSSIAN_STD_DEVIATION"));
            }
            break;

        default:
            {
            RDebug::Print(_L("%d"), type);
            }
            break;
        };

    RDebug::Print(_L(", "));
    switch(value)
        {
        case VG_RENDERING_QUALITY_NONANTIALIASED:
            {
            RDebug::Print(_L("VG_RENDERING_QUALITY_NONANTIALIASED"));
            }
            break;
        case VG_RENDERING_QUALITY_FASTER:
            {
            RDebug::Print(_L("VG_RENDERING_QUALITY_FASTER"));
            }
            break;
        case VG_RENDERING_QUALITY_BETTER:
            {
            RDebug::Print(_L("VG_RENDERING_QUALITY_BETTER"));
            }
            break;
        case VG_MATRIX_PATH_USER_TO_SURFACE:
            {
            RDebug::Print(_L("VG_MATRIX_PATH_USER_TO_SURFACE"));
            }
            break;
        case VG_MATRIX_IMAGE_USER_TO_SURFACE:
            {
            RDebug::Print(_L("VG_MATRIX_IMAGE_USER_TO_SURFACE"));
            }
            break;
        case VG_MATRIX_FILL_PAINT_TO_USER :
            {
            RDebug::Print(_L("VG_MATRIX_FILL_PAINT_TO_USER"));
            }
            break;
        case VG_MATRIX_STROKE_PAINT_TO_USER:
            {
            RDebug::Print(_L("VG_MATRIX_STROKE_PAINT_TO_USER"));
            }
            break;
        case VG_CAP_BUTT:
            {
            RDebug::Print(_L("VG_CAP_BUTT"));
            }
            break;
        case VG_CAP_ROUND:
            {
            RDebug::Print(_L("VG_CAP_ROUND"));
            }
            break;
        case VG_CAP_SQUARE:
            {
            RDebug::Print(_L("VG_CAP_SQUARE"));
            }
            break;
        case VG_BLEND_SRC:
            {
            RDebug::Print(_L("VG_BLEND_SRC"));
            }
            break;
        case VG_BLEND_SRC_OVER:
            {
            RDebug::Print(_L("VG_BLEND_SRC_OVER"));
            }
            break;
        case VG_BLEND_DST_OVER:
            {
            RDebug::Print(_L("VG_BLEND_DST_OVER"));
            }
            break;
        case VG_BLEND_SRC_IN:
            {
            RDebug::Print(_L("VG_BLEND_SRC_IN"));
            }
            break;
        case VG_BLEND_DST_IN:
            {
            RDebug::Print(_L("VG_BLEND_DST_IN"));
            }
            break;
        case VG_BLEND_MULTIPLY:
            {
            RDebug::Print(_L("VG_BLEND_MULTIPLY"));
            }
            break;
        case VG_BLEND_SCREEN:
            {
            RDebug::Print(_L("VG_BLEND_SCREEN"));
            }
            break;
        case VG_BLEND_DARKEN:
            {
            RDebug::Print(_L("VG_BLEND_DARKEN"));
            }
            break;
        case VG_BLEND_LIGHTEN:
            {
            RDebug::Print(_L("VG_BLEND_LIGHTEN"));
            }
            break;
        case VG_BLEND_ADDITIVE:
            {
            RDebug::Print(_L("VG_BLEND_ADDITIVE"));
            }
            break;
        case VG_IMAGE_QUALITY_NONANTIALIASED:
            {
            RDebug::Print(_L("1"));
            }
            break;
        case VG_IMAGE_QUALITY_FASTER:
            {
            RDebug::Print(_L("VG_IMAGE_QUALITY_FASTER"));
            }
            break;
        case VG_IMAGE_QUALITY_BETTER:
            {
            RDebug::Print(_L("VG_IMAGE_QUALITY_BETTER"));
            }
            break;
        case VG_FALSE:
            {
            RDebug::Print(_L("VG_FALSE"));
            }
            break;
        case VG_RED:
            {
            RDebug::Print(_L("VG_RED"));
            }
            break;

        case VG_DRAW_IMAGE_NORMAL:
            {
            RDebug::Print(_L("VG_DRAW_IMAGE_NORMAL"));
            }
            break;
        case VG_DRAW_IMAGE_MULTIPLY:
            {
            RDebug::Print(_L("VG_DRAW_IMAGE_MULTIPLY"));
            }
            break;
        case VG_DRAW_IMAGE_STENCIL:
            {
            RDebug::Print(_L("VG_DRAW_IMAGE_STENCIL"));
            }
            break;
        case VG_JOIN_MITER:
            {
            RDebug::Print(_L("VG_JOIN_MITER"));
            }
            break;
        case VG_JOIN_ROUND:
            {
            RDebug::Print(_L("VG_JOIN_ROUND"));
            }
            break;
        case VG_JOIN_BEVEL:
            {
            RDebug::Print(_L("VG_JOIN_BEVEL"));
            }
        case VG_EVEN_ODD:
            RDebug::Print(_L("VG_EVEN_ODD"));
            break;
        case VG_NON_ZERO:
            RDebug::Print(_L("VG_NON_ZERO"));
            break;
        default:
            {
            RDebug::Print(_L("%d"), value);
            }
            break;
        };

    RDebug::Print(_L(");\n"));
    return;
    }

void CTLVRenderer::LogvgSetXv(VGParamType type, VGint count, TInt8 vtype, VGint * data)
    {
    int vgsetvDataCount = clock() % 1000;
    RDebug::Print(_L("VGfloat * vgsetvData%d = new VGfloat[%d];\n"), ++vgsetvDataCount, count);

    for (int i=0; i<count; ++i)
        {
        RDebug::Print(_L("*(vgsetvData%d+i) = "), vgsetvDataCount);
        if (vtype == 'f')
            {
            TRealFormat rf;
            RDebug::Print(_L("%f"), (TReal)*(((TReal*)data)+i), rf);
            }
        else if (vtype == 'i')
            {
            RDebug::Print(_L("%d"), (TInt)*(data+i));
            }

        RDebug::Print(_L(";\n"));
        }

    RDebug::Print(_L("vgSet%cv("), vtype);

    switch(type)
        {
        case VG_STROKE_DASH_PATTERN:
            RDebug::Print(_L("VG_STROKE_DASH_PATTERN"));
            break;
        case VG_SCISSOR_RECTS:
            RDebug::Print(_L("VG_SCISSOR_RECTS"));
            break;
        case VG_CLEAR_COLOR:
            RDebug::Print(_L("VG_CLEAR_COLOR"));
            break;
        case VG_STROKE_LINE_WIDTH:
            {
            RDebug::Print(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;
        case VG_STROKE_MITER_LIMIT:
            {
            RDebug::Print(_L("VG_STROKE_MITER_LIMIT"));
            }
            break;
        case VG_STROKE_DASH_PHASE:
            {
            RDebug::Print(_L("VG_STROKE_DASH_PHASE"));
            }
            break;

        case VG_MATRIX_MODE:
            {
            RDebug::Print(_L("VG_MATRIX_MODE"));
            }
            break;
        case VG_FILL_RULE:
            {
            RDebug::Print(_L("VG_FILL_RULE"));
            }
            break;
        case VG_IMAGE_QUALITY:
            {
            RDebug::Print(_L("VG_IMAGE_QUALITY"));
            }
            break;
        case VG_IMAGE_MODE:
            {
            RDebug::Print(_L("VG_IMAGE_MODE"));
            }
            break;
        case VG_RENDERING_QUALITY:
            {
            RDebug::Print(_L("VG_RENDERING_QUALITY"));
            }
            break;
        case VG_BLEND_MODE:
            {
            RDebug::Print(_L("VG_BLEND_MODE"));
            }
            break;
        case VG_MASKING:
            {
            RDebug::Print(_L("VG_MASKING"));
            }
            break;
        case VG_SCISSORING:
            {
            RDebug::Print(_L("VG_SCISSORING"));
            }
            break;
        case VG_PIXEL_LAYOUT:
            {
            RDebug::Print(_L("VG_PIXEL_LAYOUT"));
            }
            break;
        case VG_FILTER_FORMAT_LINEAR:
            {
            RDebug::Print(_L("VG_FILTER_FORMAT_LINEAR"));
            }
            break;
        case VG_FILTER_FORMAT_PREMULTIPLIED:
            {
            RDebug::Print(_L("VG_FILTER_FORMAT_PREMULTIPLIED"));
            }
            break;
        case VG_FILTER_CHANNEL_MASK:
            {
            RDebug::Print(_L("VG_FILTER_CHANNEL_MASK"));
            }
            break;
        case VG_STROKE_CAP_STYLE:
            {
            RDebug::Print(_L("VG_STROKE_CAP_STYLE"));
            }
            break;
        case VG_STROKE_JOIN_STYLE:
            {
            RDebug::Print(_L("VG_STROKE_JOIN_STYLE"));
            }
            break;
        case VG_STROKE_DASH_PHASE_RESET:
            {
            RDebug::Print(_L("VG_STROKE_DASH_PHASE_RESET"));
            }
            break;
            /* Implementation limits (read-only) */
        case VG_SCREEN_LAYOUT:
            {
            RDebug::Print(_L("VG_SCREEN_LAYOUT"));
            }
            break;
        case VG_MAX_SCISSOR_RECTS:
            {
            RDebug::Print(_L("VG_MAX_SCISSOR_RECTS"));
            }
            break;
        case VG_MAX_DASH_COUNT:
            {
            RDebug::Print(_L("VG_MAX_DASH_COUNT"));
            }
            break;
        case VG_MAX_KERNEL_SIZE:
            {
            RDebug::Print(_L("VG_MAX_KERNEL_SIZE"));
            }
            break;
        case VG_MAX_SEPARABLE_KERNEL_SIZE:
            {
            RDebug::Print(_L("VG_MAX_SEPARABLE_KERNEL_SIZE"));
            }
            break;
        case VG_MAX_COLOR_RAMP_STOPS:
            {
            RDebug::Print(_L("VG_MAX_COLOR_RAMP_STOPS"));
            }
            break;
        case VG_MAX_IMAGE_WIDTH:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_WIDTH"));
            }
            break;
        case VG_MAX_IMAGE_HEIGHT:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_HEIGHT"));
            }
            break;
        case VG_MAX_IMAGE_PIXELS:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_PIXELS"));
            }
            break;
        case VG_MAX_IMAGE_BYTES:
            {
            RDebug::Print(_L("VG_MAX_IMAGE_BYTES"));
            }
            break;
        case VG_MAX_FLOAT:
            {
            RDebug::Print(_L("VG_MAX_FLOAT"));
            }
            break;
        case VG_MAX_GAUSSIAN_STD_DEVIATION:
            {
            RDebug::Print(_L("VG_MAX_GAUSSIAN_STD_DEVIATION"));
            }
            break;

        default:
            {
            RDebug::Print(_L("%d"), type);
            }
            break;
        };

    RDebug::Print(_L(", %d, vgsetvData%d);\n"), count, vgsetvDataCount);
    RDebug::Print(_L("delete [] vgsetvData%d;\n"), vgsetvDataCount);
    return;
    }

void CTLVRenderer::LogvgSetParameteri(VGHandle handle, VGint paramType, VGint value, TInt /*Lpvalue*/)
    {

    RDebug::Print(_L("vgSetParameteri(%d, "), handle);

    switch(paramType)
        {
        case VG_PAINT_TYPE:
            {
            RDebug::Print(_L("VG_PAINT_TYPE"));
            }
            break;
        case VG_PAINT_COLOR:
            {
            RDebug::Print(_L("VG_PAINT_COLOR"));
            }
            break;
        case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
            {
            RDebug::Print(_L("VG_PAINT_COLOR_RAMP_SPREAD_MODE"));
            }
            break;
        case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
            {
            RDebug::Print(_L("VG_PAINT_COLOR_RAMP_PREMULTIPLIED"));
            }
            break;
        case VG_PAINT_COLOR_RAMP_STOPS:
            {
            RDebug::Print(_L("VG_PAINT_COLOR_RAMP_STOPS"));
            }
            break;

            /* Linear gradient paint parameters */
        case VG_PAINT_LINEAR_GRADIENT:
            {
            RDebug::Print(_L("VG_PAINT_LINEAR_GRADIENT"));
            }
            break;
            /* Radial gradient paint parameters */
        case VG_PAINT_RADIAL_GRADIENT:
            {
            RDebug::Print(_L("VG_PAINT_RADIAL_GRADIENT"));
            }
            break;
            /* Pattern paint parameters */
        case VG_PAINT_PATTERN_TILING_MODE:
            {
            RDebug::Print(_L("VG_PAINT_PATTERN_TILING_MODE"));
            }
            break;
        default:
            {
            RDebug::Print(_L("%d"), paramType);
            }
            break;
        };

    RDebug::Print(_L(", "));

    switch(value)
        {
        case VG_PAINT_TYPE_COLOR:
            {
            RDebug::Print(_L("VG_PAINT_TYPE_COLOR"));
            }
            break;
        case VG_PAINT_TYPE_LINEAR_GRADIENT:
            {
            RDebug::Print(_L("VG_PAINT_TYPE_LINEAR_GRADIENT"));
            }
            break;
        case VG_PAINT_TYPE_RADIAL_GRADIENT:
            {
            RDebug::Print(_L("VG_PAINT_TYPE_RADIAL_GRADIENT"));
            }
            break;
        case VG_PAINT_TYPE_PATTERN:
            {
            RDebug::Print(_L("VG_PAINT_TYPE_PATTERN"));
            }
            break;
        default:
            {
            RDebug::Print(_L("%d"), value);
            }
            break;
        };
    RDebug::Print(_L(");\n"));
    return;
    }

void CTLVRenderer::LogvgSetPaint(VGPaint paint, VGbitfield paintModes, TInt /*Lpvalue*/)
    {
    RDebug::Print(_L("vgSetPaint(%d, "), paint);

    switch(paintModes)
        {
        case VG_STROKE_PATH:
            {
            RDebug::Print(_L("VG_STROKE_PATH"));
            }
            break;
        case VG_FILL_PATH:
            {
            RDebug::Print(_L("VG_FILL_PATH"));
            }
            break;
        default:
            {
            RDebug::Print(_L("%d"), paintModes);
            }
            break;
        };
    RDebug::Print(_L(");\n"));
    return;
    }

void CTLVRenderer::LogvgDrawPath(VGuint path, VGbitfield paintModes)
    {
    RDebug::Print(_L("vgDrawPath(%d, "), path);

    switch(paintModes)
        {
        case VG_STROKE_PATH:
            {
            RDebug::Print(_L("VG_STROKE_PATH"));
            }
            break;
        case VG_FILL_PATH:
            {
            RDebug::Print(_L("VG_FILL_PATH"));
            }
            break;
        case VG_FILL_PATH |  VG_STROKE_PATH:
            {
            RDebug::Print(_L("VG_FILL_PATH |  VG_STROKE_PATH"));
            }
            break;
        default:
            {
            RDebug::Print(_L("%d"), paintModes);
            }
            break;
        };
    RDebug::Print(_L(");\n"));
    return;
    }

void CTLVRenderer::LogvgSetParameterfv(VGPaint handle, VGint paramtype, TInt count, VGfloat * data)
    {
    int vgSetParameterfvDataCount = clock() % 1000;
    RDebug::Print(_L("VGfloat * vgSetParameterfvData%d = new VGfloat[%d];\n"), ++vgSetParameterfvDataCount, count);

    for (int i=0; i<count; ++i)
        {
        RDebug::Print(_L("*(vgSetParameterfvData%d+i) = %f;\n"), vgSetParameterfvDataCount, data[i]);
        }

    RDebug::Print(_L("vgSetParameterfv(%d, "), handle);

    switch(paramtype)
        {
        case VG_PAINT_TYPE:
            {
            RDebug::Print(_L("VG_PAINT_TYPE"));
            }
            break;
        case VG_PAINT_COLOR:
            {
            RDebug::Print(_L("VG_PAINT_COLOR"));
            }
            break;
        case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
            {
            RDebug::Print(_L("VG_PAINT_COLOR_RAMP_SPREAD_MODE"));
            }
            break;
        case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
            {
            RDebug::Print(_L("VG_PAINT_COLOR_RAMP_PREMULTIPLIED"));
            }
            break;
        case VG_PAINT_COLOR_RAMP_STOPS:
            {
            RDebug::Print(_L("VG_PAINT_COLOR_RAMP_STOPS"));
            }
            break;
        case VG_PAINT_LINEAR_GRADIENT:
            {
            RDebug::Print(_L("VG_PAINT_LINEAR_GRADIENT"));
            }
            break;
        case VG_PAINT_RADIAL_GRADIENT:
            {
            RDebug::Print(_L("VG_PAINT_RADIAL_GRADIENT"));
            }
            break;
        case VG_PAINT_PATTERN_TILING_MODE:
            {
            RDebug::Print(_L("VG_PAINT_PATTERN_TILING_MODE"));
            }
            break;
        default:
            {
            RDebug::Print(_L("%d"), paramtype);
            }
            break;
        };
    RDebug::Print(_L(", %d, vgSetParameterfvData%d);\n"), count, vgSetParameterfvDataCount);
    RDebug::Print(_L("delete [] vgSetParameterfvData%d;\n"), vgSetParameterfvDataCount);
    return;
    }

void CTLVRenderer::LogvgImageSubData(VGImage imageHandle, const void * dataPtr, TInt dataLength, VGint dataStride,
            VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height)
    {
    int imageDataCount = clock() % 1000;

    RDebug::Print(_L("VGbyte imageData%d[] =  {"), ++imageDataCount);
    
    for (TInt i = 0; i < dataLength; ++i)
        {
        if (i == dataLength-1)
            {
            int data = (int)*(((VGbyte *)(dataPtr))+i);
            RDebug::Print(_L("%d};\n"), data);
            }
        else
            {
            int data = (int)*(((VGbyte *)(dataPtr))+i);
            RDebug::Print(_L("%d, "), data);
            }

        if (i % 10 == 0)
            {
            RDebug::Print(_L("\n\t"));
            }
        }
    
    RDebug::Print(_L("vgImageSubData(%d, imageData%d, %d, (VGImageFormat)%d, %d, %d, %d, %d);\n"),
                imageHandle, imageDataCount, dataStride, dataFormat, x, y, width, height);
    }

#endif

