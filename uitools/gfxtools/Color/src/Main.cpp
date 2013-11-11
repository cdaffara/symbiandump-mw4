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


#include "Color.h"

int main()
	{
	Color::EquivalenceTest();

	Color::CreateBmConvPalette(Color::ENokiaBlue, "\\epoc32\\include\\NokiaBlue.pal");

	Color::CreatePaletteTable(Color::ENokiaBlue, "NokiaBlue");
	Color::CreatePaletteTable(Color::ENokiaGreen, "NokiaGreen");
	Color::CreatePaletteTable(Color::ENokiaPurple, "NokiaPurple");
    Color::CreatePaletteTable(Color::ENokiaGrey, "NokiaGrey");
	Color::CreatePaletteTable(Color::ESchemeRed, "SchemeRed");
	Color::CreatePaletteTable(Color::ESchemePink, "SchemePink");
	Color::CreatePaletteTable(Color::ESchemeOrange, "SchemeOrange");
    Color::CreatePaletteTable(Color::EPalette1, "Palette1");
    Color::CreatePaletteTable(Color::EPalette2, "Palette2");
    Color::CreatePaletteTable(Color::EPalette3, "Palette3");
    Color::CreatePaletteTable(Color::EPalette4, "Palette4");
    Color::CreatePaletteTable(Color::EPalette5, "Palette5");
    Color::CreatePaletteTable(Color::EPalette6, "Palette6");
    Color::CreatePaletteTable(Color::EPalette7, "Palette7");
    Color::CreatePaletteTable(Color::EPalette8, "Palette8");
    Color::CreatePaletteTable(Color::EPalette9, "Palette9");
    Color::CreatePaletteTable(Color::EPalette10, "Palette10");
    Color::CreatePaletteTable(Color::EPalette11, "Palette11");
    Color::CreatePaletteTable(Color::EPalette12, "Palette12");
    Color::CreatePaletteTable(Color::EPalette13, "Palette13");

	return 0;
	}
// End of File
