/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/
// These are the standard data type translations supplied by the CDL compiler toolkit

struct SStdTranslation
	{
	const char* iType;
	const char* iInit;
	const char* iRef;
	};

// these are the standard translations. First gets matched last.
const SStdTranslation gStdTranslations[] =
	{
		{
		"aType", 
		"aType const aName = ?value", 
		"&aName"
		},
		{
		"TDesC",
		"_LIT(aName,\"\")",
		"LIT_AS_DESC_PTR(aName)"
		},
		{
		"TCdlArray<aType>",
		"CDL_ARRAY_START(aType, aName)\n\t{\n?array_contents\n\t}\nCDL_ARRAY_END(aType, aName)",
		"&aName"
		},
		{
		"TInt64", 
		"SCdlInt64 const aName = { ?low_word, ?high_word }", 
		"(const TInt64*)&aName"
		},
		{
		"TVersion", 
		"SCdlVersion const aName = { ?major, ?minor, ?build }", 
		"(const TVersion*)&aName"
		},
		{
		"TDateTime", 
		"CDL_DATETIME(aName, ?year, ?month, ?day, ?hour, ?minute, ?second, ?microSecond)", 
		"(const TDateTime*)&aName"
		},
		{
		"TPoint", 
		"SCdlPoint const aName = { ?x, ?y }", 
		"(const TPoint*)&aName"
		},
		{
		"TSize", 
		"SCdlSize const aName = { ?width, ?height }", 
		"(const TSize*)&aName"
		},
		{
		"TRect", 
		"SCdlRect const aName = { { ?left, ?top }, { ?right+1, ?bottom+1 } }", 
		"(const TRect*)&aName"
		},
		{
		"TRgb", 
		"CDL_COLOR(aName, ?red, ?green, ?blue)", 
		"(const TRgb*)&aName"
		},
	};


