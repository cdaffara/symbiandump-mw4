/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main functionality for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMMONITOR_H
#define GOOMMONITOR_H

#include <e32std.h>

/**
 * Creates thread for Graphics OOM (Out Of Memory) watchers.
 * Called by the server in which the GOOM Monitor runs.
 *
 * @lib goommonitor.lib
 */
IMPORT_C void CreateGOOMWatcherThreadL();

#endif // GOOMMONITOR_H
