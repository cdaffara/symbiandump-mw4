/*
* Copyright (c) 2007 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class, which handles storing and accessing certificate requests.
*
*/




#ifndef C_CERTIFICATEREQUESTSTORE_H
#define C_CERTIFICATEREQUESTSTORE_H

#include <e32base.h>
#include <f32file.h>

#include "pkidefs.h"

/**
 *  Class, which handles storing and accessing certificate requests.
 *
 *  @since S60 v3.2
 */
class CCertificateRequestStore : public CBase
    {

public:

    static CCertificateRequestStore* NewL();
    ~CCertificateRequestStore();

    TInt CertReqCountL();
    CArrayFixFlat<TCertificateRequestListEntry>* GetCertReqListLC();
    
    HBufC*  SaveCertRequestLC(const TDesC8& aCertReqData);
    HBufC8* ReadCertRequestLC(const TDesC& aCertReqObjectName);
    void DeleteCertRequestL(const TDesC& aCertReqObjectName);
    
private:

    void ConstructL();

    TFileName* GetNewCertReqFileNameLC();

    RFs iFileSession;
    TFileName iPrivatePath;
    };


#endif // C_CERTIFICATEREQUESTSTORE_H
