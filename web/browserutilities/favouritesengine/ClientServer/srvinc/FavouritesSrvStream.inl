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
*      Inline method definitions of class HFavouritesSrvStream.   
*      
*
*/


#ifndef FAVOURITES_SRV_STREAM_INL
#define FAVOURITES_SRV_STREAM_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// HFavouritesSrvStream::HFavouritesSrvStream()
// ---------------------------------------------------------
//
HFavouritesSrvStream::HFavouritesSrvStream( MStreamBuf& aHost )
: iHost( aHost )
    {
    }

// ---------------------------------------------------------
// HFavouritesSrvStream::~HFavouritesSrvStream()
// ---------------------------------------------------------
//
HFavouritesSrvStream::~HFavouritesSrvStream()
    {
    iHost.Release();
    }

// ---------------------------------------------------------
// HFavouritesSrvStream::Host()
// ---------------------------------------------------------
//
MStreamBuf& HFavouritesSrvStream::Host()
    {
    return iHost;
    }

#endif

// End of File
