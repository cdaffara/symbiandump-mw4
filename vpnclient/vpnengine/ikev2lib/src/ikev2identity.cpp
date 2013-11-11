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
* Description:
*
*/


#include "ikev2identity.h"

static const TUint16 KHeaderLength = 4;

CIkeV2Identity* CIkeV2Identity::NewL(TUint8 aIdType, const TDesC8& aIdentity)
    {
    CIkeV2Identity* self = new (ELeave) CIkeV2Identity;
    CleanupStack::PushL(self);
    self->ConstructL(aIdType, aIdentity);
    CleanupStack::Pop(self);
    
    return self;
    }


void CIkeV2Identity::ConstructL(TUint8 aIdType, const TDesC8& aIdentity)
    {
    _LIT8(KReserverField, "\0\0\0");
    
    iIdPayloadData = HBufC8::NewL(KHeaderLength + aIdentity.Length());
    TPtr8 idPayloadDataPtr = iIdPayloadData->Des(); 
    
    idPayloadDataPtr.Append(&aIdType, sizeof(aIdType));
    idPayloadDataPtr.Append(KReserverField);
    idPayloadDataPtr.Append(aIdentity);
    }


CIkeV2Identity::~CIkeV2Identity()
    {
    delete iIdPayloadData;
    }


TUint8 CIkeV2Identity::IdType() const
    {
    return (*iIdPayloadData)[0];
    }


TPtrC8 CIkeV2Identity::Identity() const
    {
    return iIdPayloadData->Mid(KHeaderLength);
    }

TPtrC8 CIkeV2Identity::PayloadData() const
    {
    return *iIdPayloadData;
    }
