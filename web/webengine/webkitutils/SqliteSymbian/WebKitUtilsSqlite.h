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
* Description:  The API to WebKitUtils sqlite implementation
*
*/

#ifndef __WEBKITUTILSSQLITE_H__
#define __WEBKITUTILSSQLITE_H__

#include <e32std.h>

// Forward declarations
class RFs;

TInt sqlite3SymbianLastOsError(void);
IMPORT TInt sqlite3SymbianLibInit(void);
IMPORT void sqlite3SymbianLibFinalize(void);
RFs& sqlite3SymbianFs(void);


#endif  //__WEBKITUTILSSQLITE_H__
