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

const TRgb KNokiaPurple[30] =
	{
    0x00FFEBFF,
    0x00FFD7FF,
    0x00F7BEFF,
    0x00F79EFF,
    0x00F77DFF,
    0x00EF59FF,
    0x00EF39FF,
    0x00E614FF,
    0x00D600EF,
    0x00B500C5,
    0x009400A5,
    0x00730084,
    0x005A0063,
    0x003A0042,
    0x00210029,
    0x00100019,
    0x00FEFEFE,
    0x00CE35CE,
    0x00FFCAFF,
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


TRgb TNokiaPurple::Color(int aIndex) const
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
		return TRgb(KNokiaPurple[aIndex - SchemeSpecificColorBase]);
		}
	};

// End of File
