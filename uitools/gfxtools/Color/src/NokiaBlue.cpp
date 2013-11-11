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

const TRgb KNokiaBlue[30] =
	{
    0x00FFEEEE,
    0x00FFDBD6,
    0x00FFC2BD,
    0x00FFAA9C,
    0x00FF8E7B,
    0x00FF6D5A,
    0x00FF4D3A,
    0x00FF2D10,
    0x00EF1800,
    0x00C51400,
    0x00A51000,
    0x00840C00,
    0x00630C00,
    0x00420800,
    0x00290400,
    0x00080000,
    0x00FEFEFE,
    0x00FF513A,
    0x00FFAEA5,
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


TRgb TNokiaBlue::Color(int aIndex) const
	{
	// Color rotation for palette
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
		return TRgb(KNokiaBlue[aIndex - SchemeSpecificColorBase]);
		}
	};

// End of File
