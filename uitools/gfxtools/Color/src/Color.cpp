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


#pragma warning (disable:4710)
#include "ColorImp.h"
#include <fstream.h>
#include <stdio.h>
#include <iostream.h>

extern const char* gIntroText;
extern const char* gMiddleText;
extern const char* gEndText;

TColorScheme* Color::CreateColorSchemeL(TSchemeId aScheme)
	{
	switch (aScheme)
		{
		case ENokiaBlue:
			return new TNokiaBlue;
			break;
		case ENokiaGreen:
			return new TNokiaGreen;
			break;
		case ENokiaPurple:
			return new TNokiaPurple;
			break;
        case ENokiaGrey:
			return new TNokiaGrey;
			break;
		case ESchemeRed:
			return new TSchemeRed;
			break;
		case ESchemePink:
			return new TSchemePink;
			break;
		case ESchemeOrange:
			return new TSchemeOrange;
			break;
        case EPalette1:
			return new TPalette1;
			break;
        case EPalette2:
			return new TPalette2;
			break;
        case EPalette3:
			return new TPalette3;
			break;
        case EPalette4:
			return new TPalette4;
			break;
        case EPalette5:
			return new TPalette5;
			break;
        case EPalette6:
			return new TPalette6;
			break;
        case EPalette7:
			return new TPalette7;
			break;
        case EPalette8:
			return new TPalette8;
			break;
        case EPalette9:
			return new TPalette9;
			break;
        case EPalette10:
			return new TPalette10;
			break;
        case EPalette11:
			return new TPalette11;
			break;
        case EPalette12:
			return new TPalette12;
			break;
        case EPalette13:
			return new TPalette13;
			break;
		}
	return 0;
	}

void Color::CreateBmConvPalette(TSchemeId aScheme, char* aFilename)
	{
	cout << "Creating " << aFilename << endl;

	TColorScheme* scheme = CreateColorSchemeL(aScheme);

	ofstream out(aFilename);
	if (out.is_open())
		{
		for (int i=0; i<256; i++)
			{
			char str[32];
			sprintf(str, "0x%08X\n", scheme->Color(i));
			out << str;
			}
		}
	out.close();

	delete scheme;
	}

void Color::CreatePaletteTable(TSchemeId aScheme, char* aPaletteName)
	{
	char buf[1024];
	sprintf(buf, "\\s60\\palette\\inc\\%s.h", aPaletteName);
	cout << "Creating " << buf << endl;

	TColorScheme* scheme = CreateColorSchemeL(aScheme);

	ofstream out(buf);
	if (out.is_open())
		{
		sprintf(buf, gIntroText, aPaletteName, aPaletteName, aPaletteName, aPaletteName, aPaletteName);
		out << buf;

		int i;
		for (i=0; i<256; i++)
			{
			char str[32];
			sprintf(str, "\t0x%08X", scheme->Color(i));
			out << str;
			if (i == 255)
				out << endl;
			else
				out << "," << endl;
			}

		out << gMiddleText;

		for (i=0; i<4096; i++)
			{
			TRgb color = TRgb((i & 0x00f) * 17,((i & 0x0f0) * 17)>>4,((i & 0xf00) * 17)>>8);

			int nearest = 0;
			int distance = scheme->Color(0).Difference(color);

			for (int paletteIndex = 1; paletteIndex < 256; paletteIndex++)
				{
				TRgb paletteColor = scheme->Color(paletteIndex);

				if (paletteColor == color)
					{
					nearest = paletteIndex;
					break;
					}

				int paletteDistance = paletteColor.Difference(color);
				if (paletteDistance < distance)
					{
					nearest = paletteIndex;
					distance = paletteDistance;
					}
				}

			if (i%16 == 0)
				out << "\t";
			else
				out << " ";

			char str[32];
			sprintf(str, "0x%02X", nearest);
			out << str;

			if (i<4095)
				out << ",";
			if (i%16 == 15)
				out << endl;
			}

		out << gEndText;
		}
	out.close();

	delete scheme;
	}

void Color::EquivalenceTest()
	{
	const int KNumSchemes = 6;

	TEquivalenceList list[KNumSchemes];
	for (int s=0; s<KNumSchemes; s++)
		MakeEqivalenceList((Color::TSchemeId)s, list[s]);

	for (int i=0; i<256; i++)
		{
		for (int s=0; s<KNumSchemes; s++)
			{
			if (list[0][i] != list[s][i])
				{
				cout << "Palette equivalence failure at " << s << "[" << i << "]";
				cout << " " << list[0][i] << "!=" << list[s][i];
				cout << endl;
				}
			}
		}
	}

void Color::MakeEqivalenceList(TSchemeId aScheme, TEquivalenceList& aList)
	{
	TColorScheme* scheme = CreateColorSchemeL(aScheme);

	for (int i=0; i<256; i++)
		{
		for (int j=0; j<256; j++)
			{
			if (scheme->Color(i) == scheme->Color(j))
				{
				aList[i] = j;
				break;
				}
			}
		}

	delete scheme;
	}

// End of File
