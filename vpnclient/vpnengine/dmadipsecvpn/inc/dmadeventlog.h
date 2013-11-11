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
* Description: Class definition of TDmAdEventLog.
*
*/



#ifndef __DMADEVENTLOG_H__
#define __DMADEVENTLOG_H__

#include <e32base.h>
#include <f32file.h>
#include <barsc.h>

#include "eventviewer2.h"

//------------------------------------------------------------------------------------------------
// TDmAdEventLog
//------------------------------------------------------------------------------------------------
class TDmAdEventLog
    {
public:
    static void DeleteLogL(void);
    static void EventLogL(CBufBase& aEventLog);
private:
    static void FormatTimeL(RResourceFile& aResourceFile, TDes& aDateTimeText, TTime aTime);
    static HBufC* CategoryTextLC(RResourceFile& aResourceFile, TLogCategory2 aCategory);
    static void OpenResourceFileL(RFs& aFs, RResourceFile& aResourceFile, const TDesC& aFilename);
    static HBufC* ReadResourceLC(RResourceFile& aResourceFile, TInt aMsgId);
    };

#endif
