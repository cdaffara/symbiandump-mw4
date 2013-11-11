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

#include "TLVIconCreator.h"
#include "MVGImageBinder.h"
#include "NVGUtil.h"
#include "NVGIconData.h"
#include "NVGTLVIcon.h"
#include "nvg.h"

/*
 * Length of each encoded OpenVG API in bytes. -1 if not pre known.
 * The order here and in the enum TOpenVGAPI should be same.
 */ 
const TInt APICommandLength[] = 
    {
    16, //EvgClear
     4, //EvgSeti
     6, //EvgSetf
    -1, //EvgSetfv
     8, //EvgSetParameteri
     10, //EvgSetParameterf
    -1, //EvgSetParameterfv
     8, //EvgSetColor
     5, //EvgSetPaint
    36, //EvgLoadMatrix
    36, //EvgMultMatrix
     0, //EvgLoadIdentity
     8, //EvgScale
     4, //EvgRotate
     8, //EvgTranslate
    -1, //EvgAppendPathData
     6, //EvgDrawPath
     6, //EvgClearPath
    20, //EvguRect
    20, //EvguEllipse
    28, //EvguRoundRect
    20, //EvguLine
     4, //EvgCreatePaint
    29, //EvgCreatePath
     0, //EvgGetPixels
     4, //EvgDestroyImage
     0, //EvgDestroyPaint
     0, //EvgDestroyPath
    -1, //EvgSetiv
    17, //EvgCreateImage
     4, //EvgDrawImage
    20, //EvgClearImage
    -1, //EvgImageSubData
     0, //EvgPrepareToBindImage
     4, //EvgBindImage
     0, //EvgUnBindImage
     0  //EvgFlush
    };

CTLVIconCreator::CTLVIconCreator(TInt aWidth, TInt aHeight, CNVGTLVIcon * aNVGTLVIcon)
        : 
        iNVGDataPtr((TUint8 *)&aWidth, 1)
    {
    vgapi[EvgSeti]              = &CTLVIconCreator::DvgAddCommandL;        
    vgapi[EvgSetf]              = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgSetParameteri]     = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgSetParameterf]     = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgSetParameterfv]    = &CTLVIconCreator::DvgSetParameterfvL;
    vgapi[EvgSetColor]          = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgSetPaint]          = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgLoadMatrix]        = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgMultMatrix]        = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgLoadIdentity]      = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgScale]             = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgTranslate]         = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgAppendPathData]    = &CTLVIconCreator::DvgAppendPathDataL;
    vgapi[EvgDrawPath]          = &CTLVIconCreator::DvgDrawPathL;
    vgapi[EvgClearPath]         = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvguRect]             = &CTLVIconCreator::DvguRectL;
    vgapi[EvguEllipse]          = &CTLVIconCreator::DvguEllipseL;
    vgapi[EvguRoundRect]        = &CTLVIconCreator::DvguRoundRectL;
    vgapi[EvguLine]             = &CTLVIconCreator::DvguLineL;
    vgapi[EvgCreatePaint]       = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgSetiv]             = &CTLVIconCreator::DvgSetivL;                      
    vgapi[EvgClear]             = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgSetfv]             = &CTLVIconCreator::DvgSetfvL;
    vgapi[EvgRotate]            = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgCreatePath]        = &CTLVIconCreator::DvgCreatePathL;
    vgapi[EvgCreateImage]       = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgGetPixels]         = &CTLVIconCreator::DvgGetPixelsL;
    vgapi[EvgDrawImage]         = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgClearImage]        = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgImageSubData]      = &CTLVIconCreator::DvgImageSubDataL;
    vgapi[EvgDestroyImage]      = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgDestroyPaint]      = &CTLVIconCreator::DvgDestroyPaintL;
    vgapi[EvgDestroyPath]       = &CTLVIconCreator::DvgDestroyPathL;
    
    vgapi[EvgPrepareToBindImage]= &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgBindImage]         = &CTLVIconCreator::DvgAddCommandL;
    vgapi[EvgUnBindImage]       = &CTLVIconCreator::DvgAddCommandL;
        
    vgapi[EvgFlush]             = &CTLVIconCreator::DvgAddCommandL;
    
    iTargetWidth        = aWidth;
    iTargetHeight       = aHeight;
    iPrepareToBindImage = 0;
    iNVGTLVIcon         = aNVGTLVIcon;
    iLastVGPath         = 0;
    }

CTLVIconCreator * CTLVIconCreator::NewL(const TDesC8& aBuf, TInt aWidth, TInt aHeight, CNVGTLVIcon * aNVGTLVIcon)
    {
    CTLVIconCreator* self    = CTLVIconCreator::NewLC(aBuf,aWidth, aHeight, aNVGTLVIcon);
    CleanupStack::Pop(self);
    return self;
    }
    
CTLVIconCreator * CTLVIconCreator::NewLC(const TDesC8& aBuf, TInt aWidth, TInt aHeight, CNVGTLVIcon * aNVGTLVIcon)
    {
    CTLVIconCreator* self    = new (ELeave) CTLVIconCreator(aWidth, aHeight, aNVGTLVIcon);
    CleanupStack::PushL(self);
    self->ConstructL(aBuf);
    return self;
    }

void CTLVIconCreator::ConstructL(const TDesC8& aBuf)
    {
    vgGetMatrix(iUserMatrix);

    iNVGIconData = CNVGIconData::NewL(aBuf);
    iDataLength  = aBuf.Length();

    iCommandBuffer = (TUint8 *)aBuf.Ptr();
    iNVGIconData->BeginRead();
    iNVGIconData->ReadL(iNvgHeader, sizeof(iNvgHeader));
    iNVGTLVIcon->AddCommandL(iNvgHeader, sizeof(iNvgHeader));
    }

CTLVIconCreator::~CTLVIconCreator()
    {    
    delete iNVGIconData;    
    }

void CTLVIconCreator::DvgAddCommandL(TInt aLength, TInt aPos)
    {
    iNVGTLVIcon->AddCommandL(iCurrentCommand, iCommandBuffer + aPos, aLength);
    }

void CTLVIconCreator::DvgAddCommandL()
    {
    iNVGTLVIcon->AddCommandL(iCurrentCommand, iCommandBuffer + iNVGIconData->ReadPos(),
                            APICommandLength[iCurrentCommand]);
    iNVGIconData->SkipL(APICommandLength[iCurrentCommand]);
    }

void CTLVIconCreator::DvgSetParameterfvL()    
    {
    TInt length = 0;
    TInt cpos = iNVGIconData->ReadPos();

    TUint32 pvalue = iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt16L();
    TUint32 countt = iNVGIconData->ReadInt32L();
    length = length + sizeof(TUint32) + sizeof(TUint16) + sizeof(TUint32) + countt * sizeof(float);

    iNVGIconData->SkipL(countt * sizeof(TReal32));

    DvgAddCommandL(length, cpos);
    }

void CTLVIconCreator::DvgSetivL()    
    {
    TInt length   = 0;
    TInt cpos     = iNVGIconData->ReadPos();
    TUint16 type  = iNVGIconData->ReadInt16L();
    TUint16 count = iNVGIconData->ReadInt16L();

    length = sizeof(TInt16) + sizeof(TInt16);
    length += count * sizeof(TInt32);

    iNVGIconData->SkipL(count * sizeof(TInt32));

    DvgAddCommandL(length, cpos);
    }             


VGPath  CTLVIconCreator::CreatePathL()
    {
    const float KScale = 1.0f / 65536.0f;

    VGPath pathH = vgCreatePath( VG_PATH_FORMAT_STANDARD,
                                  VG_PATH_DATATYPE_S_32, KScale, 0.0f, 0, 0,
                                  VG_PATH_CAPABILITY_APPEND_TO );
    if (pathH == VG_INVALID_HANDLE)
        {
        User::LeaveIfError(CNvgEngine::OpenVGErrorToSymbianError(vgGetError()));
        }
    
    return pathH;
    }

void CTLVIconCreator::DvgAppendPathDataL()    
    {
    TUint16         numSegments;
    TUint16         coordinatecount;
    VGubyte *       pathSegments;
    TFloatFixPt *   pathData;
    TFloatFixPt *   coordhandle;    

    iNVGIconData->ReadInt32L();
    numSegments = iNVGIconData->ReadInt16L();
    
    pathSegments = new (ELeave) VGubyte[numSegments];
    CleanupStack::PushL(TCleanupItem(CleanupArray, pathSegments));
    
    for(int j=0; j < numSegments; j++)
        {	
        pathSegments[j] = iNVGIconData->ReadInt8L();
        }
    
    coordinatecount = iNVGIconData->ReadInt16L();
    pathData = new (ELeave) TFloatFixPt[coordinatecount];
    CleanupStack::PushL(TCleanupItem(CleanupArray, pathData));
    
    TInt lFloatFixVal = 0;
    
    coordhandle = pathData;
    for(int i = 0; i < coordinatecount; i++, coordhandle++)					
        {
        lFloatFixVal = iNVGIconData->ReadInt32L();
        coordhandle->copyfloatfix(lFloatFixVal);
        }
    
    iLastVGPath = CreatePathL();
    vgAppendPathData(iLastVGPath, numSegments, pathSegments, pathData);  

    CleanupStack::PopAndDestroy(2);
    }

void CTLVIconCreator::DvgDrawPathL()    
    {
    iNVGIconData->ReadInt32L();
    TUint16 value = iNVGIconData->ReadInt16L();
    
    iNVGTLVIcon->AddDrawPathCommandL(iLastVGPath, (VGPaintMode)value);    
    }

void CTLVIconCreator::DvguRectL()
    {
    iNVGIconData->ReadInt32L();
    
    TReal32 x   = iNVGIconData->ReadReal32L();
    TReal32 y   = iNVGIconData->ReadReal32L();
    
    TReal32 width   = iNVGIconData->ReadReal32L();
    TReal32 height  = iNVGIconData->ReadReal32L();
    
    iLastVGPath = CreatePathL();    
    vguRect(iLastVGPath, x, y, width, height);
    }

void CTLVIconCreator::DvguEllipseL()
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
    
    iLastVGPath = CreatePathL();    
    vguEllipse(iLastVGPath, cx, cy, width, height);
    }

void CTLVIconCreator::DvguRoundRectL()
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

    iLastVGPath = CreatePathL();    
    vguRoundRect(iLastVGPath, x, y, width, height, arcWidth, arcHeight);
    }

void CTLVIconCreator::DvguLineL()
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
    
    iLastVGPath = CreatePathL();
    vguLine(iLastVGPath, x0, y0, x1, y1);
    }
    
void CTLVIconCreator::DvgSetfvL()
    {
    TUint16 count;
    TInt length = 0;
    TInt cpos   = iNVGIconData->ReadPos();
    
    iNVGIconData->ReadInt16L();
    count = iNVGIconData->ReadInt16L();
    
    iNVGIconData->SkipL(sizeof(TReal32) * count);
    
    length = length + 2 * sizeof(TUint16) + count * sizeof(VGfloat);
    
    DvgAddCommandL(length, cpos);
    }

void CTLVIconCreator::DvgCreatePathL()
    {
    iNVGIconData->SkipL(APICommandLength[EvgCreatePath]);
    }

void CTLVIconCreator::DvgImageSubDataL()
    {
    TInt cpos   = iNVGIconData->ReadPos();
    
    TInt encodedImageHandle = iNVGIconData->ReadInt32L();
    TInt dataLength         = 0;
    
    iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt32L();
    iNVGIconData->ReadInt32L();        
    dataLength      = iNVGIconData->ReadInt32L();
    iNVGIconData->SkipL(dataLength);
    
    DvgAddCommandL(iNVGIconData->ReadPos() - cpos, cpos);
    }
    
void CTLVIconCreator::DvgGetPixelsL()
    {
    }

void CTLVIconCreator::DvgDestroyPaintL()
    {
    }

void CTLVIconCreator::DvgDestroyPathL()
    {
    }

void CTLVIconCreator::DvgFlushL()
    {
    }

TInt CTLVIconCreator::ExecuteL(TInt aIndex)
    {
    iCurrentCommand = aIndex;
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

TInt CTLVIconCreator::ExecuteL()
    {
    TInt error = KErrNone;
        
    while (!iNVGIconData->EOF())
        {
        error = ExecuteL(iNVGIconData->ReadInt8L());
        if (error != KErrNone)
            {
            break;
            }
        }
    
    return error;
    }
