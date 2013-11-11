/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Extension Library source file
*
*/


#include "GfxRendererInfoP.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// TGfxRendererInfoP::TGfxRendererInfoP()
// ---------------------------------------------------------------------------
TGfxRendererInfoP::TGfxRendererInfoP()
    {
    }

// ---------------------------------------------------------------------------
// set device
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// void TGfxRendererInfoP::SetDevice( TUint8* /*aDstBuf*/,
// ---------------------------------------------------------------------------
void TGfxRendererInfoP::SetDevice( TUint8* /*aDstBuf*/,
                                   TInt32 aWidth,
                                   TInt32 aHeight,
                                   TInt32 /*aBytePerPixel*/ )
    {
    iWidth = aWidth;
    iHeight = aHeight;
    iClipMinX = 0;
    iClipMinY = 0;
    iClipMaxX = aWidth;
    iClipMaxY = aHeight;
    }


// ---------------------------------------------------------------------------
// set clip
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// void TGfxRendererInfoP::SetClip( const TGfxRectangle2D& aClip )
// ---------------------------------------------------------------------------
void TGfxRendererInfoP::SetClip( const TGfxRectangle2D& aClip )
    {
    iClipMinX = ( TInt32 ) aClip.iX;
    iClipMinY = ( TInt32 ) aClip.iY;
    iClipMaxX = ( TInt32 ) ( aClip.iX + aClip.iWidth );
    iClipMaxY = ( TInt32 ) ( aClip.iY + aClip.iHeight );
    }
