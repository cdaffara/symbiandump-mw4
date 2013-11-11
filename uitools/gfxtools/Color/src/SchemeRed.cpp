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

const TRgb KSchemeRed[30] =
	{
    0x00CCDDFF,
    0x00BBCCFF,
    0x00AABBFF,
    0x0099AAFF,
    0x008899FF,
    0x007788FF,
    0x006677FF,
    0x005566EE,
    0x004455EE,
    0x003344EE,
    0x002233EE,
    0x000000EE,
    0x000000BB,
    0x000000AA,
    0x00000088,
    0x00000077,     // 241
    0x00FEFEFE,     // undefined
    0x005566FF,  
    0x00BBCCCC,  
    0x00FEFEFE,     // undefined
    0x00FEFEFE,     // undefined
    0x00FEFEFE,     // undefined
    0x00FEFEFE,     // undefined
	0x0000EEFF,		// coding yellow
	0x000000DD,		// coding red
	0x00FF7700,		// coding blue
	0x0000EE00,		// coding green
    0x00339900,     // nokia brand color green 253
    0x00CC3300,     // nokia brand color blue 254
	0x00DD00DD		// mask pink
	};


TRgb TSchemeRed::Color(int aIndex) const
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
		return TRgb(KSchemeRed[aIndex - SchemeSpecificColorBase]);
		}
    };

// End of File
