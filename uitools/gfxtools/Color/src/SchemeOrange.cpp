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

const TRgb KSchemeOrange[30] =
	{
    0x00EECCFF,     // 226
    0x0099BBFF,
    0x0088AAFF,
    0x0077AAFF,
    0x0055AAFF,
    0x0044AAFF,
    0x0033AAFF,
    0x0011AAFF,
    0x0000AAFF,
    0x002299FF,
    0x001166FF,
    0x000066EE,
    0x001166EE,
    0x001166DD,
    0x001166CC,
    0x001166BB,     // 241
    0x00FEFEFE,     // undefined
    0x00001100,
    0x0011BBFF,  
    0x00FEFEFE,     // undefined
    0x00FEFEFE,     // undefined
    0x00FEFEFE,     // undefined
    0x00FEFEFE,     // undefined
    0x0000EEFF,     // coding yellow
    0x000000DD,     // coding red
    0x00FF7700,     // coding blue
    0x0000EE00,     // coding green
    0x00339900,     // nokia brand color green 253
    0x00CC3300,     // nokia brand color blue 254
    0x00DD00DD      // mask pink
	};


TRgb TSchemeOrange::Color(int aIndex) const
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
		return TRgb(KSchemeOrange[aIndex - SchemeSpecificColorBase]);
		}
    };

// End of File
