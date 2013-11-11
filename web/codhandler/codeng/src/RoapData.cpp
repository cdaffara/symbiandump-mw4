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
*      Implementation of class CRoapData.   
*      
*
*/


// INCLUDE FILES

#include "RoapData.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapData::NewL()
// ---------------------------------------------------------
//
CRoapData* CRoapData::NewL()
    {
    return new (ELeave) CRoapData();
    }
    
// ---------------------------------------------------------
// CRoapData::~CRoapData()
// ---------------------------------------------------------
//
CRoapData::~CRoapData()
    {
    iContentIdList.ResetAndDestroy();
    iContentIdList.Close();
    delete iRiAlias;
    }

// ---------------------------------------------------------
// CRoapData::Reset()
// ---------------------------------------------------------
//
void CRoapData::Reset()
    {
    iType = Roap::ERegistrationTrigger;
    iContextStatus = Roap::EInvalidContext;
    iDomainOperation = Roap::ENotDomainOperation;
    iContentIdList.ResetAndDestroy();
    delete iRiAlias;
    iRiAlias = NULL;
    }

// ---------------------------------------------------------
// CRoapData::CRoapData()
// ---------------------------------------------------------
//
CRoapData::CRoapData()
: iType( Roap::ERegistrationTrigger ),
  iContextStatus( Roap::EInvalidContext ),
  iDomainOperation( Roap::ENotDomainOperation )
    {
    }
