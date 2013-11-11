/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class definition of CDmAdDdf.
*
*/



#ifndef __DMADDDF_H__
#define __DMADDDF_H__

#include <smldmadapter.h>
#include "dmadddfapi.h"
#include "dmadcallback.h"
#include "DmAdStore.h"
#include "version.h"

//------------------------------------------------------------------------------------------------
// DDF version
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdDdfVersion, "1.0"); 

//------------------------------------------------------------------------------------------------
// ./NokiaIPSecVPN Type property
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdNokiaIpSecVpnRootTypeProperty, "com.nokia.devman/1.0/ipsecvpn"); 

//------------------------------------------------------------------------------------------------
// ./NokiaIPSecVPN/General/ClientVersion - see version.h
//------------------------------------------------------------------------------------------------
//_LIT8(KDmAdClientVersion, "0.0");

//------------------------------------------------------------------------------------------------
// URIs
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdOwnAdUriForGetLuidAllocLFix, "NokiaIPSecVPN/Policy");  // URI not object group level?

    //------- root -----------------
_LIT8(KDmAdNodeNokiaIpSecVpn, "NokiaIPSecVPN");

    //------- common -----------------
_LIT8(KDmAdNodeRt, "");
_LIT8(KDmAdNodeGeneral, "General");

_LIT8(KDmAdLeafClientVersion, "ClientVersion");
_LIT8(KDmAdLeafEventLog, "EventLog");
_LIT8(KDmAdLeafName, "Name");
_LIT8(KDmAdLeafId, "ID");
_LIT8(KDmAdLeafContent, "Content");

    //------- AP -----------------
_LIT8(KDmAdNodeAp, "AP");
_LIT8(KDmAdNodePolicyRef, "PolicyRef");
_LIT8(KDmAdLeafConRef, "ConRef");
_LIT8(KDmAdLeafThisRef, "ThisRef");
_LIT8(KDmAdLeafUri, "URI");

    //------- Policy -----------------
_LIT8(KDmAdNodePolicy, "Policy");
_LIT8(KDmAdLeafVersion, "Version");
_LIT8(KDmAdLeafDescription, "Description");
_LIT8(KDmAdLeafIssuer, "Issuer");
_LIT8(KDmAdLeafContact, "Contact");

//------------------------------------------------------------------------------------------------
// URI descriptions
//------------------------------------------------------------------------------------------------
    //------- root -----------------
_LIT8(KDmAdDescNodeNokiaIpSecVpn, "NokiaIPSecVPN");

    //------- common -----------------
_LIT8(KDmAdDescNodeRt, "Placeholder for one set of settings");
_LIT8(KDmAdDescNodeGeneral, "General");

_LIT8(KDmAdDescLeafClientVersion, "ClientVersion");
_LIT8(KDmAdDescLeafEventLog, "EventLog");
_LIT8(KDmAdDescLeafName, "Name");
_LIT8(KDmAdDescLeafId, "ID");
_LIT8(KDmAdDescLeafContent, "Content");

    //------- AP -----------------
_LIT8(KDmAdDescNodeAp, "AP");
_LIT8(KDmAdDescNodePolicyRef, "PolicyRef");
_LIT8(KDmAdDescLeafConRef, "ConRef");
_LIT8(KDmAdDescLeafThisRef, "ThisRef");
_LIT8(KDmAdDescLeafUri, "URI");

    //------- Policy -----------------
_LIT8(KDmAdDescNodePolicy, "Policy");
_LIT8(KDmAdDescLeafVersion, "Version");
_LIT8(KDmAdDescLeafDescription, "Description");
_LIT8(KDmAdDescLeafIssuer, "Issuer");
_LIT8(KDmAdDescLeafContact, "Contact");

//------------------------------------------------------------------------------------------------
// ChildURIListL constants
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdListOfNokiaIpSecVpnChildren, "General/AP/Policy");

_LIT8(KDmAdListOfPolicyXChildren, "Name/ID/Version/Description/Issuer/Contact/Content");
_LIT8(KDmAdListOfApXChildren, "Name/ThisRef/PolicyRef/ConRef");

_LIT8(KDmAdListOfPolicyRefChildren, "ID/URI");

_LIT8(KDmAdListOfIpSecVpnGeneralChildren, "ClientVersion/EventLog");



//------------------------------------------------------------------------------------------------
// FetchLink URIs
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdFetchLinkAp, "AP");
_LIT8(KDmAdFetchLinkSnap, "BearerManagementSNAP");
_LIT8(KDmAdFetchLinkPolicy, "NokiaIPSecVPN/Policy");

//------------------------------------------------------------------------------------------------
// TDFFormat - EBool
//------------------------------------------------------------------------------------------------
_LIT8(KDmAdDfFormatBoolTrue, "True" );
_LIT8(KDmAdDfFormatBoolFalse, "False" );

//------------------------------------------------------------------------------------------------
// CDmAdDdf 
//------------------------------------------------------------------------------------------------
class CDmAdDdf : public CBase, public MDmAdDdfApi
    {
public:
    static CDmAdDdf* NewL(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore);
    static CDmAdDdf* NewLC(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore);
    ~CDmAdDdf();

    void BuildDDFVersionL(CBufBase& aDDFVersion);
    void BuildDDFStructureL(MSmlDmDDFObject& aDDF);
    
    void NotRtNodeAddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef);
    void NotRtNodeUpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef);
    TInt NotRtNodeFetchLeafObjectLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, const TDesC8& /*aType*/, CBufBase*& aObject); 
    void NotRtNodeDeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef);
    
    TBool IsNodeRtNodeL(const TDesC8& aUri);
    TBool IsLeafUnderRtNodeL(const TDesC8& aUri);
    TPtrC8 RtNodeUriForLeafL(const TDesC8& aLeafUri);
    TBool IsTopLevelRtNode(const TDesC8& aUri);
    HBufC8* ParentRtNodeUriForRtNodeLC(const TDesC8& aUri);
    void BuildChildUriListLC(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, CBufBase*& aCurrentList);
    
private:
    CDmAdDdf(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore);
    void ConstructL(void);

private:
    MDmAdCallBack*          iCallBack;
    CDmAdStore*             iStore;
    };

#endif
