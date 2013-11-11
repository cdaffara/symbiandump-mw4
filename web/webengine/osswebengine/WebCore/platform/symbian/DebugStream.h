/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#ifndef __DEBUGSTREAM_H__
#define __DEBUGSTREAM_H__

#include "FileStreamSymbian.h"

#define DEBUG_OUTPUT 0

namespace WebCore
{

#if DEBUG_OUTPUT
TextFileStreamOutput& debugStream();
extern const StreamEndOfLine& endl;

#define notImplemented() debugStream()<<"FIXME: UNIMPLEMENTED:"<<__FILE__<<" : "<<__LINE__<< "(" << __FUNCTION__<< ")" << endl;
#define sDebug debugStream() 

#else

#define notImplemented()
#define sDebug DummyStream()

#endif
}

#endif //!__DEBUGSTREAM_H__
