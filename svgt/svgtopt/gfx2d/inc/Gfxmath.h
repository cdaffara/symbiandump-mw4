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
* Description:      Math function header
 *
*/

#ifndef GFXMATH_H
#define GFXMATH_H




#include "e32def.h"

/*define this to make all math to use floating point (otherwise uses 16:16 fixed point)*/

//#define SVG_FLOAT_BUILD


typedef signed   __int64 svgInt64;
typedef unsigned __int64 svgUint64;

float svgFixedToFloat(int x);
int svgScalarFromFloat(float f);

int svgScalarMul(int r1, int r2);
int svgScalarDiv(int x, int y);


#endif /*GFXMATH_H*/
