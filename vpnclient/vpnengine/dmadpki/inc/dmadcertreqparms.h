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
* Description: Class definition of CDmAdCertReqParms.
*
*/



#ifndef __DMADCERTREQPARMS_H__
#define __DMADCERTREQPARMS_H__

#include <e32base.h>

class CDmAdCertReqParms : public CBase
    {
public:
    static CDmAdCertReqParms* NewL();
    static CDmAdCertReqParms* NewLC();
    ~CDmAdCertReqParms();
    
    TPtrC8 SubjectName() const;
    void SetSubjectNameL(const TDesC8& aSubjectName);
    TPtrC8 Rfc822Name() const;
    void SetRfc822NameL(const TDesC8& aRfc822Name);
    TInt KeyLength() const;
    void SetKeyLength(TInt aKeyLength);
    TPtrC8 Content() const;
    void SetContentL(const TDesC8& aContent);
    TPtrC KeyUri() const;
    void SetKeyUriL(const TDesC& aContent);
    
    TPtrC8 KeyIdentifierByUri() const;
    void SetKeyIdentifierByUriL(const TDesC8& aKeyIdentifierByUri);
    
private:
    CDmAdCertReqParms();
    
private:
    HBufC8*             iSubjectName;
    HBufC8*             iRfc822Name;
    TInt                iKeyLength;
    HBufC8*             iContent;
    
    HBufC8*             iKeyIdentifierByUri; // key luid
    };

#endif
