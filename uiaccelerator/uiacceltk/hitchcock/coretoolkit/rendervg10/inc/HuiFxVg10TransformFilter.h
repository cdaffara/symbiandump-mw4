/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef HUIFXVG10TRANSFORMFILTER_H_
#define HUIFXVG10TRANSFORMFILTER_H_

#include <math.h>
#include "HuiFxVg10FilterBase.h"

class CHuiFxVg10TransformFilter: public CHuiFxVg10FilterBase
    {
public: // from CHuiFxFilter
    virtual TBool Draw(CHuiFxEngine& aEngine, CHuiGc& aGc,
                       CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
                       const TRect& aTargetRect, const TRect& aSourceRect, TBool aHasSurface);
    static CHuiFxVg10TransformFilter* NewL();
    void DrawEffect(CHuiFxEngine& /*aEngine*/, VGImage /*aTargetImage*/, VGImage /*aSourceImage*/, TInt /*aWidth*/, TInt /*aHeight*/, TBool /*aHasSurface*/) {}
    void CalculateMargin(TMargins &aMargin) const;  
    CHuiFxVg10TransformFilter *CloneL() const;
protected:
    void ConstructL();
    
    void identity()
        {
        iMatrix [0] = 1.f; iMatrix [1] = 0.f; iMatrix [2] = 0.f; iMatrix [3] = 0.f;
        iMatrix [4] = 0.f; iMatrix [5] = 1.f; iMatrix [6] = 0.f; iMatrix [7] = 0.f;
        iMatrix [8] = 0.f; iMatrix [9] = 0.f; iMatrix[10] = 1.f; iMatrix[11] = 0.f;
        iMatrix[12] = 0.f; iMatrix[13] = 0.f; iMatrix[14] = 0.f; iMatrix[15] = 1.f;
        }
    
    void translate(TReal32 tx, TReal32 ty, TReal32 tz)
        {
        const TReal32 tm[16] =
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            tx,  ty,  tz,  1.f
        };
        
        multiply(tm); 
        }
    
    void scale (const TReal32 sx, const TReal32 sy, const TReal32 sz)
        {
        const TReal32 sm[16] =
        {
            sx, 0.f, 0.f, 0.f,
            0.f,  sy, 0.f, 0.f,
            0.f, 0.f,  sz, 0.f,
            0.f, 0.f, 0.f, 1.f
        };

        multiply(sm);
        }
    
    void rotate (const TReal32 angle, const TReal32 x, const TReal32 y, const TReal32 z)
        {
        static const TReal32 deg_to_rad = 2.f * (TReal32)M_PI / 360.f;
        // normalised vector components
        TReal32 x_n;
        TReal32 y_n;
        TReal32 z_n;
        TReal32 angle_rad = angle * deg_to_rad;

        // normalise if needed
        const TReal32 length = (TReal32)sqrt(double(x * x) + 
                      double(y * y) + 
                      double(z * z));
        if(fabs(length - 1.0f) > EPSILON) {
            // in fp calculations, division by zero -> (+/-)inf
            // can't really help if it's the case.
            const TReal32 inv_length = 1.f / length;
            x_n = x * inv_length;
            y_n = y * inv_length;
            z_n = z * inv_length;
        } else {
            x_n = x;
            y_n = y;
            z_n = z;
        }

        const TReal32 c = cos(angle_rad);
        const TReal32 s = sin(angle_rad);
        const TReal32 c_1 = 1.f - c;

        const TReal32 rm[16] =
        {
                x_n * x_n * c_1 + c,       y_n * x_n * c_1 + z_n * s, x_n * z_n * c_1 - y_n * s, 0.f,
                x_n * y_n * c_1 - z_n * s, y_n * y_n * c_1 + c,       y_n * z_n * c_1 + x_n * s, 0.f,
                x_n * z_n * c_1 + y_n * s, y_n * z_n * c_1 - x_n * s, z_n * z_n * c_1 + c,   0.f,
                0.f,               0.f,               0.f,           1.f
        };

        multiply(rm);
        }
 
    void frustum (const TReal32 l, const TReal32 r, const TReal32 b, const TReal32 t, const TReal32 n, const TReal32 f)
        {
        const TReal32 rl = 1.f / (r - l);
        const TReal32 tb = 1.f / (t - b);
        const TReal32 fn = 1.f / (f - n);

        const TReal32 A = (r + l) * rl;
        const TReal32 B = (t + b) * tb;
        const TReal32 C = -(f + n) * fn; 
        const TReal32 D = -2.f * f * n * fn;

        const TReal32 fm[16] =
        {
            2.f * n * rl,   0.f,        0.f,    0.f,
            0.f,        2.f * n * tb,   0.f,    0.f,
            A,      B,      C,  -1.f,
            0.f,        0.f,        D,  0.f
        };
        
        multiply(fm);
        }

    void multiply (const TReal32* b)
        {
        TReal32 dst[16];
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                TReal32 sum = 0.f;
                for (int k = 0; k < 4; k++)
                    sum += iMatrix[i + 4 * k] * b[k + 4 * j];

                dst[i + 4 * j] = sum;
            }
        }

        for (int i = 0; i < 16; i++)
            iMatrix[i] = dst[i];
        }
    
    void shear (const TReal32 aX, const TReal32 aY, const TReal32 aZ)
        {
        TReal32 shearMatrix[16] =
            {
            1.0f,   aX,   aX, 0.0f,
              aY, 1.0f,   aY, 0.0f,
              aZ,   aZ, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            };
        
        multiply(shearMatrix);

        }

private:
    TReal32             iTranslationX;
    TReal32             iTranslationY;
    TReal32             iTranslationZ;
    
    TReal32             iScaleX;
    TReal32             iScaleY;
    TReal32             iScaleZ;
    TReal32             iScaleOriginX;
    TReal32             iScaleOriginY;
    TReal32             iScaleOriginZ;
    
    TReal32             iRotationOriginX;
    TReal32             iRotationOriginY;
    TReal32             iRotationOriginZ;
    TReal32             iRotationAngle;
    TReal32             iRotationAxisX;
    TReal32             iRotationAxisY;
    TReal32             iRotationAxisZ;
    
    TReal32             iSkewAngleX;
    TReal32             iSkewAngleY;
    TReal32             iSkewAngleZ;
    TReal32             iSkewOriginX;
    TReal32             iSkewOriginY;
    TReal32             iSkewOriginZ;
    
    TReal32             iMatrix[16];
    };

#endif
