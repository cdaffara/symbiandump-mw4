/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class definition of CDmAdPKCS12Parms
*
*/




#ifndef __DMADPKCS12PARMS_H__
#define __DMADPKCS12PARMS_H__

#include <e32base.h>
#include <securitydefs.h>

#include "pkidefs.h"


class CDmAdPKCS12Parms : public CBase
    {
public:
    static CDmAdPKCS12Parms* NewL();
    static CDmAdPKCS12Parms* NewLC();
    ~CDmAdPKCS12Parms();
    
    TBool Deletable() const;
    void SetDeletable(TBool aDeletable);
    
    const RArray<TUid>& Applicability() const;
    void SetApplicabilityL(const RArray<TUid>& aApplicability);
    
    TPtrC8 Content() const;
    void SetContentL(const TDesC8& aContent);
    
    TPtrC8 Password() const;
    void SetPasswordL(const TDesC8& aPassword);
    
private:
    CDmAdPKCS12Parms();
    
private:
    TBool                       iDeletable;
    TBool                       iTrusted;
    RArray<TUid>                iApplicability;
    HBufC8*                     iContent; //CertificateDer;
    HBufC8*                     iPassword;
    };

#endif
