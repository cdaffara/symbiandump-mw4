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

const TRgb KSchemePink[30] =
	{
    0x00EFEFEF,     // 226
    0x00E6E7E6,
    0x00DEDBDE,
    0x00D6D2D6,
    0x00BDC2BD,
    0x00B5B2B5,
    0x00949294,
    0x00848284,
    0x00737173,
    0x005A595A,
    0x004A4D4A,
    0x00636163,     // NOTE! Conflicts with battery, signal and scroll indicator colors
    0x003A3D3A,
    0x00292D29,
    0x00211C21,
    0x00101010,     // 241
    0x00FEFEFE,     // undefined
    0x00949694,     
    0x00C5C6C5,     
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


TRgb TSchemePink::Color(int aIndex) const
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
		return TRgb(KSchemePink[aIndex - SchemeSpecificColorBase]);
		}
    };

// End of File
