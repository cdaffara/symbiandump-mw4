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

const TRgb KPalette10[30] =
	{
    0x00FFEFF7,
    0x00FFD7EF,
    0x00FFBEDE,
    0x00FF9ECE,
    0x00FF7DBD,
    0x00FF59AD,
    0x00FF399C,
    0x00FF148C,
    0x00EF0073,
    0x00C50063,
    0x00A50052,
    0x00840042,
    0x00630031,
    0x00420021,
    0x00290010,
    0x00100008,
    0x00FEFEFE,
    0x00FF3D9C,
    0x00FF9ACE,
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

TRgb TPalette10::Color(int aIndex) const
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
		return TRgb(KPalette10[aIndex - SchemeSpecificColorBase]);
		}
	};

// End of File
