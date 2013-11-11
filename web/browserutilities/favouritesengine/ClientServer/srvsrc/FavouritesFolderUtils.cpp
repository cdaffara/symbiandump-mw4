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
*
*/



//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "FavouritesFolder.h"



// ----------------------------------------------------------------------------
// CFavouritesFolder class
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// CFavouritesFolder construtor
// ----------------------------------------------------------------------------
CFavouritesFolder::CFavouritesFolder( HBufC* aName, TUint aUid)
    {
    iName = aName;
    iUid = aUid;
    }
// End of File
