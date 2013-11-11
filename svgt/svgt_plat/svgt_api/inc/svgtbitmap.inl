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
 * Description:  SVGTbitmap header file
 *
*/

inline CSvgtBitmap::CSvgtBitmap( TInt8* aBitmapBuffer, TSize aBitmapSize,
                        TDisplayMode aDisplayMode, TInt aStride):iBitmapBuffer(aBitmapBuffer),
                                iBitmapSize(aBitmapSize),
                                iDisplayMode(aDisplayMode),
                                iStride(aStride)
    {
    }

inline TInt8* CSvgtBitmap::BitmapBuffer() const
    {
    return iBitmapBuffer;
    }
        
inline TSize CSvgtBitmap::SizeInPixels() const
    {
    return iBitmapSize;
    }
        
inline TDisplayMode CSvgtBitmap::DisplayMode() const
    {
    return iDisplayMode;
    }
    
inline TInt CSvgtBitmap::Stride() const
    {
    return iStride;
    }
