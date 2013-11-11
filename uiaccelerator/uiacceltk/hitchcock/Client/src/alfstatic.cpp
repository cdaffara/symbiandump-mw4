/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Static class.
*
*/



#include "alf/alfstatic.h"

/**
 * Thread local storage space. Writable static data is not supported in
 * Symbian, so static data is stored in this struct.
 */

struct TTlsData
    {
    /** Primary environment. */
    CAlfEnv* iPrimaryEnv;
    };

EXPORT_C CAlfStatic* CAlfStatic::NewLC(CAlfEnv* aPrimaryEnv)
    {
    CAlfStatic* self = new (ELeave) CAlfStatic();
    CleanupStack::PushL(self);
    self->ConstructL(aPrimaryEnv);
    return self;        
    }

EXPORT_C CAlfStatic* CAlfStatic::NewL(CAlfEnv* aPrimaryEnv)
    {
    CAlfStatic* self = NewLC(aPrimaryEnv);
    CleanupStack::Pop(self);
    return self;        
    }

CAlfStatic::~CAlfStatic()
    {
    Dll::FreeTls();
    delete iData;        
    }

EXPORT_C CAlfEnv& CAlfStatic::Env()
    {
    return *Data()->iPrimaryEnv;
    }


CAlfStatic::CAlfStatic()
    {
    
    }

void CAlfStatic::ConstructL(CAlfEnv* aPrimaryEnv)
    {
    iData = new (ELeave) TTlsData();
    Mem::FillZ(iData, sizeof(TTlsData));    
    
    iData->iPrimaryEnv = aPrimaryEnv;
    
    Dll::SetTls(iData);    
    }

EXPORT_C struct TTlsData* CAlfStatic::Data()
    {
    return static_cast<TTlsData*>(Dll::Tls());        
    }

