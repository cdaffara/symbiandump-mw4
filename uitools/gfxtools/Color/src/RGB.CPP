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


// 
//
// Copyright (c) 1997-2000 Symbian Ltd.  All rights reserved.
//

#pragma warning (disable:4514)
#include "RGB.H"
#include <math.h>
#include <stdlib.h>

TRgb::TRgb()
	: iRed(255),iGreen(255),iBlue(255),iSpare(0)
	{}

TRgb::TRgb(long unsigned int val)
	: iRed((unsigned char)(val&0xff)),iGreen((unsigned char)((val>>8)&0xff)),iBlue((unsigned char)((val>>16)&0xff)),iSpare(0)
	{}

TRgb::TRgb(int r,int g,int b)
	: iRed((unsigned char)r),iGreen((unsigned char)g),iBlue((unsigned char)b),iSpare(0)
	{}

TRgb &TRgb::operator=(const TRgb &col)
	{
	iRed=col.iRed;
	iGreen=col.iGreen;
	iBlue=col.iBlue;
	return(*this);
	}

int TRgb::operator==(const TRgb &col)
	{
	return(iRed==col.iRed && iGreen==col.iGreen && iBlue==col.iBlue);
	}

int TRgb::Difference(const TRgb& col) const
	{
	return abs(iRed-col.iRed) + abs(iGreen-col.iGreen) + abs(iBlue-col.iBlue);
	}

// End of File
