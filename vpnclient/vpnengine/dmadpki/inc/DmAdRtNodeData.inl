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
* Description:   Inlined node data methods
*
*/



#ifndef __DMADRTNODEDATA_INL__
#define __DMADRTNODEDATA_INL__

inline CDmAdCertData* CDmAdRtNodeData::CertData(void) const
    {
    return u.iCert;
    }

inline CDmAdCertReqData* CDmAdRtNodeData::CertReqData(void) const
    {
    return u.iCertReq;
    }

inline CDmAdPrivKeyData* CDmAdRtNodeData::PrivKeyData(void) const
    {
    return u.iPrivKey;
    }

inline CDmAdPKCS12Data* CDmAdRtNodeData::PKCS12Data(void) const
    {
    return u.iPKCS12;
    }

#endif
