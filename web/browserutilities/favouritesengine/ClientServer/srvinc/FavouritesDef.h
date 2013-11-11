/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Internal constants / macros for Favourites Engine.
*      
*
*/


#ifndef FAVOURITES_DEF_H
#define FAVOURITES_DEF_H

//  INCLUDES

#include <e32std.h>

// MACROS


// CONSTANTS

/// Favourites Engine UID.
LOCAL_D const TUid KFavouritesUid = { 0x10008D38 };
/// Favourites Engine Server name.
_LIT( KFavouritesSrvName, "!FAVENGSRV" );
/// Favourites Engine Server executable.
_LIT( KFavouritesSrvExe, "FavouritesSrv" );
/// Exit delay in microseconds (10 sec).
LOCAL_D const TInt KFavouritesSrvExitDelay = 10000000;

#if defined(__MYSERVER_NO_PROCESSES__)
/// Favourites Engine Server stack size. 12k.
LOCAL_D const TInt KFavouritesSrvStackSize=0x3000;
/// Favourites Engine Server minimum heap size. 4k.
LOCAL_D const TInt KFavouritesSrvMinHeapSize=0x1000;
/// Favourites Engine Server maximum heap size. 4M.
LOCAL_D const TInt KFavouritesSrvMaxHeapSize=0x400000;
#endif

#endif

// End of File
