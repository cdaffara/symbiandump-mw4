/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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


// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

// disable "decorated name length exceeded, name was truncated" warning	
#pragma warning (disable:4503)

#include "ZoomLevelNames.h"
#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"

#include <CdlCompilerToolkit/CdlTkUtil.h>

#include <AknDef.hrh>

#include <fstream>
#include <iostream>

using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<CZoomLevelNames> ZoomLevelNamesArgsErr;

//
// CZoomLevelNames
//

void CZoomLevelNames::ProcessOptions(vector<string>& aArgs)
	{
	bool zoomFileOk = false;
	ifstream in;
	string zoomFile;

	for(vector<string>::iterator pArg = aArgs.begin(); pArg != aArgs.end(); ++pArg)
		{
		string& arg = *pArg;
		if (arg.size() >= 2 && arg.substr(0,2) == "-z")
			{
			zoomFile = arg.substr(2);
			try
				{
				CdlTkUtil::OpenInput(in, zoomFile);
				zoomFileOk = true;
				}
			catch (const CdlTkFileOpenErr&)
				{
				}
			aArgs.erase(pArg);
			break;
			}
		}
	if(zoomFileOk)
		{
		string line;
		while (!in.eof())
			{
			getline(in, line);
			string::size_type comma = line.find(",");
			if(comma == string::npos)
				{
				cerr << "Zoom file incorrect format." << endl;
				throw GeneralErr("Zoom file incorrect format");
				}
			int num = CdlTkUtil::ParseInt(line.substr(0, comma));
			insert(make_pair(num, line.substr(comma+1)));
			}
		in.close();
		}
	else
		{
		// assume that normal zoom is the only one
		insert(make_pair(EAknUiZoomNormal, KDefaultZoomInstanceName));
		}
	}
