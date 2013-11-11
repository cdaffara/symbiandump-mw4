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
const char* gIntroText ="\
/*\n\
* ============================================================================\n\
*  Name     : %s.h\n\
*  Part of  : Palette\n\
*\n\
*  Description:\n\
*       Provides the K%sColor256Util definition.\n\
*\n\
*  Version:\n\
*\n\
*  Copyright (C) 2002 Nokia Corporation.\n\
*  This material, including documentation and any related\n\
*  computer programs, is protected by copyright controlled by\n\
*  Nokia Corporation. All rights are reserved. Copying,\n\
*  including reproducing, storing,  adapting or translating, any \n\
*  or all of this material requires the prior written consent of \n\
*  Nokia Corporation. This material also contains confidential \n\
*  information which may not be disclosed to others without the \n\
*  prior written consent of Nokia Corporation.\n\
*\n\
* ============================================================================\n\
*/\n\
\n\
#if !defined(__%s_H__)\n\
#define __%s_H__\n\
\n\
#include <gdi.h>\n\
\n\
\n\
const TColor256Util K%sColor256Util =\n\
	{\n\
	// color256array\n\
	{\n\
";


const char* gMiddleText ="\
	},\n\
	//color256inverse\n\
	{\n\
";


const char* gEndText ="\
	}\n\
	};\n\
\n\
#endif\n\
\n\
// End of file\n\
";
