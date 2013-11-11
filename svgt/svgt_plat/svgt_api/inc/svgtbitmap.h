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

#ifndef SVGTBITMAP_H_
#define SVGTBITMAP_H_

#include <e32base.h>
#include <displaymode.h>

class CSvgtBitmap : public CBase
    {
public: // constructors 
    inline CSvgtBitmap( TInt8* aBitmapBuffer, TSize aBitmapSize,
                            TDisplayMode aDisplayMode, TInt aStride);

public: //class methods
    /*
     * returns target image buffer
     */
    inline TInt8* BitmapBuffer() const;
    
    /*
     * returns target image size in pixels
     */
    inline TSize SizeInPixels() const;
    
    /*
     * returns target image display mode
     */
    inline TDisplayMode DisplayMode() const;
    
    /*
     * returns target image bytes per scanline
     */
    inline TInt Stride() const;
    
private:    
    TInt8* iBitmapBuffer;
    TSize iBitmapSize;
    TDisplayMode iDisplayMode;
    TInt iStride;
    };

#include <svgtbitmap.inl>

#endif /* SVGTBITMAP_H_ */
