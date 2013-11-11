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
*      Inline method definitions of class CFavouritesSrvDbIncremental.   
*      
*
*/


#ifndef FAVOURITES_SRV_DB_INCREMENTAL_INL
#define FAVOURITES_SRV_DB_INCREMENTAL_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesSrvDbIncremental::CFavouritesSrvDbIncremental
// ---------------------------------------------------------
//
CFavouritesSrvDbIncremental::CFavouritesSrvDbIncremental(): iStep( 0 )
    {
    }

// ---------------------------------------------------------
// CFavouritesSrvDbIncremental::~CFavouritesSrvDbIncremental
// ---------------------------------------------------------
//
CFavouritesSrvDbIncremental::~CFavouritesSrvDbIncremental()
    {
    iIncremental.Close();
    }

// ---------------------------------------------------------
// CFavouritesSrvDbIncremental::Step
// ---------------------------------------------------------
//
TInt CFavouritesSrvDbIncremental::Step()
    {
    return iStep;
    }

#endif

// End of File
