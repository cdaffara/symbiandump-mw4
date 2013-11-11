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
* Description: 
*       
*
*/


#include "ColorImp.h"

const TRgb KPalette11[30] =
	{
    0x00FFEFFF,
    0x00FFD7F7,
    0x00FFBEEF,
    0x00FF9EE6,
    0x00FF7DDE,
    0x00FF59D6,
    0x00FF39CE,
    0x00FF14C5,
    0x00EF00B5,
    0x00C50094,
    0x00A5007B,
    0x00840063,
    0x0063004A,
    0x00420031,
    0x00290021,
    0x00100010,
    0x00FEFEFE,
    0x00FF5DD6,
    0x00FFC2EF,
    0x00FEFEFE, // undefined
    0x00FEFEFE, // undefined
    0x00FEFEFE, // undefined
    0x00FEFEFE, // undefined
	0x0000EEFF,		// coding yellow
	0x000000DD,		// coding red
	0x00FF7700,		// coding blue
	0x0000EE00,		// coding green
    0x00339900,  // nokia brand color green 253
    0x00CC3300,  // nokia brand color blue 254
	0x00DD00DD		// mask pink
	};

TRgb TPalette11::Color(int aIndex) const
	{
	// color rotation for palette
	if (aIndex == 0)
		return TRgb(0x00000000);
	else if (aIndex == 0xff)
		return TRgb(0x00ffffff);
	else if (aIndex == 215)
		return TRgb(0x00dd00dd);
	else if (aIndex < StandardGrayBase)
		{
		return TRgb(KColorCubeMap[aIndex/36], KColorCubeMap[(aIndex/6)%6], KColorCubeMap[aIndex%6]);
		}
	else if (aIndex < SchemeSpecificColorBase)
		{
		int gray = aIndex - StandardGrayBase;
		return TRgb(KStandardGray[gray], KStandardGray[gray], KStandardGray[gray]);
		}
	else
		{
		return TRgb(KPalette11[aIndex - SchemeSpecificColorBase]);
		}
	};

// End of File
