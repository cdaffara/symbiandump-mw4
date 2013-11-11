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
*      Inline method definitions of class RFavouritesBuf.   
*      
*
*/


#ifndef FAVOURITES_BUF_INL
#define FAVOURITES_BUF_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesBuf::Lag()
// ---------------------------------------------------------
//
TInt RFavouritesBuf::Lag( TRead ) const
    {
    return Ptr( ERead ) - End( ERead );
    }

// ---------------------------------------------------------
// RFavouritesBuf::Lag()
// ---------------------------------------------------------
//
TInt RFavouritesBuf::Lag( TWrite ) const
    {
    return Ptr( EWrite ) - iBuf.iData;
    }

#endif

// End of File
