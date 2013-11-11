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


// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)
#include "Extract.h"
#include "LayoutCompilerErr.h"
#include "HtmlParse.h"
#include "LayoutParse.h"
#include "LayoutWriter.h"
#include <fstream>
#include <iostream>

typedef LayoutProcessArgsErr<LayoutExtract> LayoutExtractArgsErr;


int LayoutExtract::Extract(const vector<string>& args)
	{
	ParseArgs(args);

	THtmlParseLayout html;
	ifstream in(iDocName.c_str());
	if (!in.is_open())
		throw NotFoundErr(iDocName);
	cout << "reading html " << iDocName << endl;
	html.Parse(in);
	in.close();

	TLayout* layout = &html;
	TLayParseLayout lay;
	if (iMergeName.size())
		{
		ifstream merge(iMergeName.c_str());
		if (!merge.is_open())
			throw NotFoundErr(iDocName);
		cout << "reading layout " << iMergeName << endl;
		lay.Parse(merge);
		merge.close();

		cout << "merging " << iDocName << " into " << iMergeName << endl;
		lay.Merge(TLayout::KMergeModeMerge, html);
		layout = &lay;
		}

	TLayoutWriter writer(*layout, iLayoutName);
	writer.Write("");

	return 0;
	}

void LayoutExtract::ParseArgs(const vector<string>& args)
	{
	if (args.size() < 4)
		throw LayoutExtractArgsErr();

	iDocName = args[2];

	unsigned int layoutIndex = 3;
	if (args[3][0] == '-')
		{
		if (args[3].size() < 3 || args[3][1] != 'o')
			throw LayoutExtractArgsErr();

		iMergeName = args[3].substr(2);
		layoutIndex++;
		}

	if (args.size() <= layoutIndex)
		throw LayoutExtractArgsErr();

	iLayoutName = args[layoutIndex];
	}

void LayoutExtract::ShowHelp(ostream& stream)
	{
	stream << endl;
	stream << "LayoutCompiler extract <docName> [-o<oldLayout>] <layoutName>" << endl;
	stream << "  <docName> is the name of the HTML Layout specification" << endl;
	stream << "  <oldLayout> is an optional layout file which the extracted layout will be" << endl;
	stream << "  merged with" << endl;
	stream << "  <layoutName> is the name of the resulting layout file" << endl;
	stream << "  You can use the same name for <oldLayout> and <layoutName>" << endl;
	}

// End of File
