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


#ifndef COLORIMP_H
#define COLORIMP_H

#include "Color.h"

const int KStandardGray[10] = {0xee, 0xdd, 0xbb, 0xaa, 0x88, 0x77, 0x55, 0x44, 0x22, 0x11};
const int KColorCubeMap[6] = {0xff, 0xcc, 0x99, 0x66, 0x33, 0x00};

class TNokiaBlue : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TNokiaGreen : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TNokiaPurple : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TNokiaGrey : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TSchemeRed : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TSchemePink : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TSchemeOrange : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette1 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette2 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette3 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette4 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette5 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette6 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette7 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette8 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette9 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette10 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette11 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette12 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

class TPalette13 : public TColorScheme
	{
public:
	TRgb Color(int aIndex) const;
	};

#endif
// End of File
