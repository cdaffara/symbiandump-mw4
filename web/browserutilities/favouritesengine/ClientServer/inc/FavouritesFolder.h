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
*      Declaration of class CFavouritesFolder.
*      
*
*/


#ifndef FAVOURITES_FOLDER_H
#define FAVOURITES_FOLDER_H

//  INCLUDES

#include <e32base.h>


// FORWARD DECLARATION


// CLASS DECLARATION

/**
* Base class for representing favourites array of folder names and uids.
* Instance of this class is used to look up uid for populating parent field
* of database file.
*/
class CFavouritesFolder
    {
    public:

        CFavouritesFolder( HBufC* aName, TUint aUid);

        inline HBufC*  Name() {return iName;}
        inline TInt    Uid()  {return iUid;}

    private:     // Data types
        HBufC*  iName;
        TUint   iUid;
    };

#endif
            
// End of File
