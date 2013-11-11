/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Structure for storing IPSec SA Specs
*
*/


// CLASS HEADER
#include "ipsecsalist.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsecSaSpecList::CIpsecSaSpecList() 
 : CArrayFixFlat<struct TIpsecSaSpec>(1)
     {     
     }
