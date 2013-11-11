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
*      Implementation of inline methods of class CFavouritesSrvDb.
*
*
*/


#ifndef FAVOURITES_SRV_DB_INL
#define FAVOURITES_SRV_DB_INL

// ---------------------------------------------------------
// CFavouritesSrvDb::InTransaction
// ---------------------------------------------------------
//
inline TBool CFavouritesSrvDb::InTransaction()
    {
    return iInTransaction;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::Database
// ---------------------------------------------------------
//
inline RDbDatabase& CFavouritesSrvDb::Database()
    {
    return iDatabase;
    }

#endif
            
// End of File
