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
#ifndef CDLTKPRIV_H
#define CDLTKPRIV_H

#include "CdlCompilerToolkit/CdlTkUtil.h"

namespace CdlCompilerToolkit {

const std::string KWhiteSpace(" \t\r\n");
const std::string KSpace(" ");
const std::string KEmptyString("");
const std::string KSectionBoundary("%%");
const std::string KCommentStart("//");
const std::string KCrlf("\r\n");
const std::string KCpp("0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
const std::string KCommonHeader(".common.h");

const std::string KType("aType");
const std::string KName("aName");
const std::string KEqualsSign("=");

const int KTypeSize = KType.size();

const std::string KPackageContentsApi("contents");

const std::string KDllInstHeader("dllinstances.hrh");

}	// end of namespace CdlCompilerToolkit

#endif
