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
*      Implementation of class CRoapAppData.   
*      
*
*/


// INCLUDE FILES

#include "RoapAppData.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapAppData::NewL()
// ---------------------------------------------------------
//
CRoapAppData* CRoapAppData::NewL()
    {
    return new (ELeave) CRoapAppData();
    }
    
// ---------------------------------------------------------
// CRoapAppData::~CRoapAppData()
// ---------------------------------------------------------
//
CRoapAppData::~CRoapAppData()
    {
    iContentIdList.ResetAndDestroy();
    iContentIdList.Close();
    delete iRiAlias;
    }

// ---------------------------------------------------------
// CRoapAppData::CRoapAppData()
// ---------------------------------------------------------
//
CRoapAppData::CRoapAppData()
: iType( Roap::ERegistrationTrigger ),
  iContextStatus( Roap::EInvalidContext ),
  iDomainOperation( Roap::ENotDomainOperation )
    {
    }
