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
* Description: Implementation of CDmAdCertParms.
*
*/


#include "dmadcertreqparms.h"
#include "dmadstoreprivkey.h"

CDmAdCertReqParms* CDmAdCertReqParms::NewL()
    {
    CDmAdCertReqParms* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CDmAdCertReqParms* CDmAdCertReqParms::NewLC()
    {
    CDmAdCertReqParms* self = new (ELeave) CDmAdCertReqParms();
    CleanupStack::PushL(self);
    return self;
    }

CDmAdCertReqParms::CDmAdCertReqParms()
    {
    }
    
CDmAdCertReqParms::~CDmAdCertReqParms()
    {
    delete iSubjectName;
    delete iRfc822Name;
    delete iContent;
    delete iKeyIdentifierByUri; // key luid
    }

TPtrC8 CDmAdCertReqParms::SubjectName() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iSubjectName != 0)
        {
        ret.Set(*iSubjectName);
        }
    return ret;
    }
 
void CDmAdCertReqParms::SetSubjectNameL(const TDesC8& aSubjectName)
    {
    delete iSubjectName;
    iSubjectName = 0;
    if (aSubjectName.Length() > 0)
        {
        iSubjectName = aSubjectName.AllocL();
        }
    }

TPtrC8 CDmAdCertReqParms::Rfc822Name() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iRfc822Name != 0)
        {
        ret.Set(*iRfc822Name);
        }
    return ret;
    }
 
void CDmAdCertReqParms::SetRfc822NameL(const TDesC8& aRfc822Name)
    {
    delete iRfc822Name;
    iRfc822Name = 0;
    if (aRfc822Name.Length() > 0)
        {
        iRfc822Name = aRfc822Name.AllocL();
        }
    }

TInt CDmAdCertReqParms::KeyLength() const
    {
    return iKeyLength;
    }
    
void CDmAdCertReqParms::SetKeyLength(TInt aKeyLength)
    {
    iKeyLength = aKeyLength;
    }
 
TPtrC8 CDmAdCertReqParms::Content() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iContent != 0)
        {
        ret.Set(*iContent);
        }
    return ret;
    }
 
void CDmAdCertReqParms::SetContentL(const TDesC8& aContent)
    {
    delete iContent;
    iContent = 0;
    if (aContent.Length() > 0)
        {
        iContent = aContent.AllocL();
        }
    }

TPtrC8 CDmAdCertReqParms::KeyIdentifierByUri() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iKeyIdentifierByUri != 0)
        {
        ret.Set(*iKeyIdentifierByUri);
        }
    return ret;
    }
 
void CDmAdCertReqParms::SetKeyIdentifierByUriL(const TDesC8& aKeyIdentifierByUri)
    {
    delete iKeyIdentifierByUri;
    iKeyIdentifierByUri = 0;
    if (aKeyIdentifierByUri.Length() > 0)
        {
        iKeyIdentifierByUri = aKeyIdentifierByUri.AllocL();
        }
    }
