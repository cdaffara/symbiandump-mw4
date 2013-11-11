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

class TRgb
	{
public:
	TRgb();
	TRgb(long unsigned int val);
	TRgb(int aRed,int aGreen,int aBlue);
	TRgb &operator=(const TRgb& aColor);
	int operator==(const TRgb& aColor);
	int Difference(const TRgb& col) const;
public:
	unsigned char iRed;
	unsigned char iGreen;
	unsigned char iBlue;
	unsigned char iSpare;
	};

// End of File
