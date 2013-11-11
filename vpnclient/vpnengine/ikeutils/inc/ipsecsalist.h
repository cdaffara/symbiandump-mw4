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


#ifndef C_IPSECSALIST_H
#define C_IPSECSALIST_H

#include <e32base.h>
#include <ipsecpolapi.h>

class CIpsecSaSpecList : public CArrayFixFlat<struct TIpsecSaSpec>
    {
public:
    IMPORT_C CIpsecSaSpecList();          
    };

#endif // C_IPSECSALIST_H
