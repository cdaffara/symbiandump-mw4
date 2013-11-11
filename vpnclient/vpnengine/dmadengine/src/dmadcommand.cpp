/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CDmAdCommand.
*
*/


#include "dmadcommand.h"
#include "vpnlogger.h"


CDmAdCommand* CDmAdCommand::NewL(const TDesC8& aUri, const TInt aStatusRef, TBool aLeaf)
    {
    TRACE("CDmAdCommand::NewL");
    
    CDmAdCommand *self = NewLC(aUri, aStatusRef, aLeaf);
    CleanupStack::Pop(); // self
    return self;
    }

CDmAdCommand* CDmAdCommand::NewLC(const TDesC8& aUri, const TInt aStatusRef, TBool aLeaf)
    {
    CDmAdCommand *self = new (ELeave) CDmAdCommand(aStatusRef, aLeaf);
    CleanupStack::PushL(self);
    self->ConstructL(aUri);
    return self;
    }

void CDmAdCommand::ConstructL(const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdCommand::ConstructL");
    }
    
CDmAdCommand::CDmAdCommand(const TInt aStatusRef, TBool aLeaf) :
    iStatusRef(aStatusRef), iStatus(KErrNone), iLeaf(aLeaf)
    {
    TRACE("CDmAdCommand::CDmAdCommand");
    }
    
CDmAdCommand::~CDmAdCommand()
    {
    TRACE("CDmAdCommand::~CDmAdCommand");
    }
    
void CDmAdCommand::CleanupOperationDeleteCArrayPtr(TAny* aPtr)
    {
    if (aPtr == NULL)
        {
        return;
        }
    CArrayPtr<CDmAdCommand>* array = REINTERPRET_CAST(CArrayPtr<CDmAdCommand>*,aPtr);
    array->ResetAndDestroy();
    delete array;
    }

