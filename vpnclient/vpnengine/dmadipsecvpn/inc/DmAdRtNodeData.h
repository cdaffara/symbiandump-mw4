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
* Description: Class definition of CDmAdRtNodeData.
*
*/



#ifndef __DMADRTNODEDATA_H__
#define __DMADRTNODEDATA_H__


#include <e32base.h>

#include "dmadcallback.h"
#include "dmadstorevpnap.h"
#include "vpnapi.h"

class CX509Certificate;

typedef enum
    {
    EDmAdUriTypeVpnAp,
    EDmAdUriTypePolicy,
    } TDmAdUriType;
    
//------------------------------------------------------------------------------------------------
// CDmAdVpnApData
//------------------------------------------------------------------------------------------------
class CDmAdVpnApData : public CBase
    {
public:
    static CDmAdVpnApData* NewL(MDmAdCallBack* aCallBack);
    ~CDmAdVpnApData();
        
    
    void SetNameL(const TDesC8& aName);
    HBufC8* NameLC(void);
    
    void SetPolicyRefIdL(const TDesC& aPolicyRefId);
    HBufC* PolicyRefIdLC(void);
    
    void SetConRefL(const TDesC8& aConRef);
    HBufC8* ConRefLC(void);
    
    HBufC8* ThisRefLC(const TDesC8& aLuid);
    HBufC8* PolicyRefUriLC(void);
    
    void RestoreL(const TVpnApParms& aVpnApParms);
    void ToStoreL(TVpnApParms& aVpnApParms);

private:    
    CDmAdVpnApData(MDmAdCallBack* aCallBack);
    
private:
    MDmAdCallBack*      iCallBack;
            
    HBufC*              iName;
    HBufC*             iPolicyRefId;
    HBufC8*             iConRef;
    };

//------------------------------------------------------------------------------------------------
// CDmAdPolicyData
//------------------------------------------------------------------------------------------------
class CDmAdPolicyData : public CBase
    {
public:
    static CDmAdPolicyData* NewL(void);
    ~CDmAdPolicyData();
    
    void SetNameL(const TDesC8& aName);
    HBufC8* NameLC(void);
    void SetIdL(const TDesC8& aId);
    HBufC8* IdLC(void);
    void SetVersionL(const TDesC8& aVersion);
    HBufC8* VersionLC(void);
    void SetDescriptionL(const TDesC8& aDescription);
    HBufC8* DescriptionLC(void);
    void SetIssuerL(const TDesC8& aIssuer);
    HBufC8* IssuerLC(void);
    void SetContactL(const TDesC8& aContact);
    HBufC8* ContactLC(void);
    void SetContentL(const TDesC8& aContent);
    TPtrC8 Content(void);
    void DetailsRestoreL(const TVpnPolicyDetails& aDetails);
    void DetailsToStoreL(TVpnPolicyDetails& aDetails);
    void ContentRestoreL(const TDesC8& aContent);
    TPtrC8 ContentToStore(void);

private:    
    CDmAdPolicyData();
    
private:
    HBufC*              iName;
    HBufC*              iId;
    HBufC*              iVersion;
    HBufC*              iDescription;
    HBufC*              iIssuer;
    HBufC*              iContact;
    HBufC8*             iContent;
    };

//------------------------------------------------------------------------------------------------
// CDmAdRtNodeData
//------------------------------------------------------------------------------------------------
class CDmAdRtNodeData : public CBase
    {
public:
    static CDmAdRtNodeData* NewL(const TDesC8& aUri, MDmAdCallBack* aCallBack);
    ~CDmAdRtNodeData();
    TDmAdUriType UriType(void) const;
    static TDmAdUriType UriTypeL(const TDesC8& aUri);
    void UpdateLeafDataL(const TDesC8& aUri, const TDesC8& aObject);
    void FetchLeafObjectLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);

    inline CDmAdVpnApData* VpnApData(void) const;
    inline CDmAdPolicyData* PolicyData(void) const;
    
private:
    void ConstructL(const TDesC8& aUri, MDmAdCallBack* aCallBack);
    CDmAdRtNodeData();
    void UpdateLeafDataVpnApL(const TDesC8& aUri, const TDesC8& aObject);
    void UpdateLeafDataPolicyL(const TDesC8& aUri, const TDesC8& aObject);
    void FetchLeafObjectVpnApLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
    void FetchLeafObjectPolicyLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
    
private:
    TDmAdUriType    iUriType;
    union
        {
        CDmAdVpnApData*     iVpnAp;
        CDmAdPolicyData*    iPolicy;
        } u;
    };

#include "DmAdRtNodeData.inl"

#endif
