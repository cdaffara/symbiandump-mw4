/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Abstracts the Sqlite calls for Symbian 
*
*/


#ifndef __OS_SYMBIAN_H__
#define __OS_SYMBIAN_H__

// INCLUDES
#include <e32std.h>
#include "os.h"
#include "sqliteInt.h"

// CONSTANTS

// MACROS

// DATA TYPES
#ifdef __COUNT_FILE_IO__
/**
TDbFileIOCounters structure holds counters used for collecting information about the file I/O statistics:
file I/O operations count and the time spent in their execution.
The structure can be used only when __COUNT_FILE_IO__ macro is defined.
@internalComponent
*/
struct TDbFileIOCounters
	{
	TInt	iWriteOpCnt;		
	TUint	iWriteOpTime;
	TInt	iReadOpCnt;		
	TUint	iReadOpTime;
	TInt	iSyncOpCnt;
	TUint	iSyncOpTime;
	TInt	iOpenOpCnt;
	TUint	iOpenOpTime;
	TInt	iSeekOpCnt;
	TUint	iSeekOpTime;
	TInt	iDeleteOpCnt;
	TUint	iDeleteOpTime;
	TInt	iCloseOpCnt;
	TUint	iCloseOpTime;
	TInt	iExistOpCnt;
	TUint	iExistOpTime;
	TInt	iLockOpCnt;
	TUint	iLockOpTime;
	TInt	iUnlockOpCnt;
	TUint	iUnlockOpTime;
	TInt	iTruncateOpCnt;
	TUint	iTruncateOpTime;
	TInt	iFileSizeOpCnt;
	TUint	iFileSizeOpTime;
	TInt	iIsDirWOpCnt;
	TUint	iIsDirWOpTime;
	TInt	iTempFileNameOpCnt;
	TUint	iTempFileNameOpTime;
	};

void sqlite3SymbianZeroDbFileIOCounters();
void sqlite3SymbianGetDbFileIOCounters(TDbFileIOCounters& aDbFileIOCounters);
#endif//__COUNT_FILE_IO__


// FORWARD DECLARATIONS
class RFs;

// FUNCTION PROTOTYPES
#ifdef __MEM_TRACE__

void sqlite3SymbianPrintMemCounters(void);
void sqlite3SymbianResetMemCounters(void);

#else//__MEM_TRACE__

#define sqlite3SymbianPrintMemCounters() void(0)
#define sqlite3SymbianResetMemCounters() void(0)

#endif//__MEM_TRACE__

// LOCAL FUNCTIONS
IMPORT_C TInt sqlite3SymbianLibInit(void);
IMPORT_C void sqlite3SymbianLibFinalize(void);
IMPORT_C TInt sqlite3SymbianLastOsError(void);
IMPORT_C RFs& sqlite3SymbianFs(void);

#endif//__OS_SYMBIAN_H__
