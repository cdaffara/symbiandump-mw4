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
* Description: 
*       
*
*/


#include "ColorImp.h"

const TRgb KPalette1[30] =
	{
    0x00EFEFFF,
    0x00D6D7FF,
    0x00BDBEFF,
    0x009C9EFF,
    0x007B7DFF,
    0x005A59FF,
    0x003A39FF,
    0x001014FF,
    0x000000EF,
    0x000000C5,
    0x000000A5,
    0x00000084,
    0x00000063,
    0x00000042,
    0x00000029,
    0x00000010,
    0x00FEFEFE,
    0x003135FF,
    0x00CECAFF,
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

TRgb TPalette1::Color(int aIndex) const
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
		return TRgb(KPalette1[aIndex - SchemeSpecificColorBase]);
		}
	};

// End of File
