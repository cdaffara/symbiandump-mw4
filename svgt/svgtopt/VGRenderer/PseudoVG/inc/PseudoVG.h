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
* Description:  PseudoVG header file
 *
*/

#ifndef PSEUDOVG_H_
#define PSEUDOVG_H_

enum OpenVGAPI
    {
    EvgClear = 0,
    EvgSeti,
    EvgSetf,
    EvgSetfv,
    EvgSetParameteri,
    EvgSetParameterf,
    EvgSetParameterfv,
    EvgSetColor,
    EvgSetPaint,
    EvgLoadMatrix,
    EvgMultMatrix,
    EvgLoadIdentity,
    EvgScale,
    EvgRotate,
    EvgTranslate,
    EvgAppendPathData,
    EvgDrawPath,
    EvgClearPath,
    EvguRect,
    EvguEllipse,
    EvguRoundRect,
    EvguLine,
    EvgCreatePaint,
    EvgCreatePath,
    EvgGetPixels,
    EvgDestroyImage,
    EvgDestroyPaint,
    EvgDestroyPath,
    EvgSetiv,
    EvgCreateImage,
    EvgDrawImage,
    EvgClearImage,
    EvgImageSubData,
    EvgPrepareToBindImage,
    EvgBindImage,
    EvgUnBindImage,
    EvgFlush,
    };

/*
enum OpenVGAPI
    {
    EvgClear = 0,
    EvgSeti,
    EvgSetf,
    EvgSetfv,
    EvgSetParameteri,
    EvgSetParameterf,
    EvgSetParameterfv,
    EvgSetColor,
    EvgSetPaint,
    EvgLoadMatrix,
    EvgMultMatrix,
    EvgLoadIdentity,
    EvgScale,
    EvgRotate,
    EvgTranslate,
    EvgAppendPathData,
    EvgDrawPath,
    EvguRect,
    EvguEllipse,
    EvguRoundRect,
    EvguLine,
    EvgCreatePaint,
    EvgCreatePath,
    EvgCreateImage,
    EvgGetPixels,
    EvgDrawImage,
    EvgDestroyImage,
    EvgDestroyPaint,
    EvgDestroyPath,
    EvgFlush,
    EvgSetiv
    };
*/

static const TUint     OpenVGAPICount = EvgFlush + 1;

static const TInt8     KNVGFileVersion      = 0x00;                 // file version is not really required for pseudo
static const TInt16    KNVGHeaderSize       = 0x34;
static const TInt16    KNVGReserved1        = 0x01;                 // 1 is for TLV type

static const TUid      KUidPseudo = { 968435518 }; // TODO?

#endif /*PSEUDOVG_H_*/
