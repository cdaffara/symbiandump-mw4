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


#ifndef COLOR_H
#define COLOR_H

#include "RGB.H"

class TColorScheme
	{
public:
	enum 
		{
		StandardGrayBase = 216,
		SchemeSpecificColorBase = 226
		};

public:
	virtual TRgb Color(int aIndex) const = 0;
	};


class Color
	{
public:
	enum TSchemeId
		{
		ENokiaBlue,
		ENokiaGreen,
		ENokiaPurple,
        ENokiaGrey,
        ESchemeRed,
        ESchemePink,
        ESchemeOrange,
        EPalette1,
        EPalette2,
        EPalette3,
        EPalette4,
        EPalette5,
        EPalette6,
        EPalette7,
        EPalette8,
        EPalette9,
        EPalette10,
        EPalette11,
        EPalette12,
        EPalette13
		};

public:
	static TColorScheme* CreateColorSchemeL(TSchemeId aScheme);
	static void CreateBmConvPalette(TSchemeId aScheme, char* aFilename);
	static void CreatePaletteTable(TSchemeId aScheme, char* aPaletteName);
	static void EquivalenceTest();
private:
	typedef int TEquivalenceList[256];
	static void MakeEqivalenceList(TSchemeId aScheme, TEquivalenceList& aList);
	};


#endif
// End of File
