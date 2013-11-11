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
* Description: Class definition of CDmAdCertParms.
*
*/



#ifndef __DMADCERTPARMS_H__
#define __DMADCERTPARMS_H__

#include <e32base.h>
#include <securitydefs.h>

#include "pkidefs.h"


class CDmAdCertParms : public CBase
    {
public:
    static CDmAdCertParms* NewL();
    static CDmAdCertParms* NewLC();
    ~CDmAdCertParms();
    
    TPKICertificateOwnerType Type() const;
    void SetType(TPKICertificateOwnerType aType);
            
    TBool Deletable() const;
    void SetDeletable(TBool aDeletable);
    
    TBool Trusted() const;
    void SetTrusted(TBool aTrusted);
    
    const RArray<TUid>& Applicability() const;
    void SetApplicabilityL(const RArray<TUid>& aApplicability);
    
    TPtrC8 Content() const;
    void SetContentL(const TDesC8& aContent);
    
    TPtrC8 KeyId() const;
    void SetKeyId(const TDesC8& aKeyLuid);
        
private:
    CDmAdCertParms();
    
private:
    TPKICertificateOwnerType iType;   
    TBool                    iDeletable;
    TBool                    iTrusted;
    RArray<TUid>             iApplicability;
    HBufC8*                  iContent; //CertificateDer;

    //----- only for AttachCertificateL --------
    TKeyIdentifier      iKeyId;
    };

#endif
