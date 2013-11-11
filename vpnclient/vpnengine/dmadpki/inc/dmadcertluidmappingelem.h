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
* Description: Class definition of CDmAdCertLuidMappingElem.
*
*/



#ifndef __DMADCERTLUIDMAPPINGELEM_H__
#define __DMADCERTLUIDMAPPINGELEM_H__

#include <e32base.h>

class CDmAdCertLuidMappingElem : public CBase
    {
public:
    static CDmAdCertLuidMappingElem* NewL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber);
    static CDmAdCertLuidMappingElem* NewLC(const TDesC8& aIssuerName, const TDesC8& aSerialNumber);
    ~CDmAdCertLuidMappingElem();
    static void CleanupOperationDeleteCArrayPtr(TAny* aPtr);
    TPtrC8 Luid() const;
    TPtrC8 IssuerName() const;
    TPtrC8 SerialNumber() const;
    static HBufC8* BuildLuidL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber);
    
private:
    void ConstructL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber);
    CDmAdCertLuidMappingElem();
    
private:
    HBufC8*             iLuid;
    HBufC8*             iIssuerName;
    HBufC8*             iSerialNumber;
    };

#endif
