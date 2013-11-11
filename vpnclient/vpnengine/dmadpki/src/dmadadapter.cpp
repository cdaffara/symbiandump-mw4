/*
* Copyright (c) 2000-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI OMA DM Adapter
*
*/



#include <e32base.h>
#include <implementationproxy.h>

#include "dmadadapter.h"
#include "dmadadapterimplconst.h"
#include "dmadengine.h"
#include "DmAdStore.h"
#include "dmadddf.h"
#include "dmadrtnodedataapic.h"
#include "vpnlogger.h"

#if defined(_DEBUG)
_LIT(KDmAdLogFolder,"vpn");
_LIT(KDmAdLogFile,"dmadpki.log");
#endif

//===================================================================================================


#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KDmAdImplUid, CDmAdAdapter::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }


//===================================================================================================

CDmAdAdapter* CDmAdAdapter::NewL(MSmlDmCallback* aDmCallback)
    {       
    CDmAdAdapter* self = NewLC(aDmCallback);
    CleanupStack::Pop(self);
    return self;
    }

CDmAdAdapter* CDmAdAdapter::NewLC(MSmlDmCallback* aDmCallback)
    {
    INITIALIZE_DEBUG_LOG_L(KDmAdLogFolder, KDmAdLogFile);    
    DEBUG_LOG(_L("LOGGING INITIALIZED"));     
                                  
    CDmAdAdapter* self = new (ELeave) CDmAdAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->ConstructL(aDmCallback);
    return self;
    }
    
void CDmAdAdapter::ConstructL(MSmlDmCallback* aDmCallback)
    {
    TRACE("CDmAdAdapter::ConstructL");
    
    DEBUG_LOG(_L("ConstructL - CallBack (PKI)"));
    iCallBack = CDmAdCallBack::NewL(aDmCallback, KDmAdOwnAdUriForGetLuidAllocLFix);
    
    iStore = CDmAdStore::NewL(iCallBack);    
    iDdf = CDmAdDdf::NewL(iCallBack, iStore);
    
    DEBUG_LOG(_L("ConstructL - RtNodeDataApi"));
    iRtNodeDataApi = CDmAdRtNodeDataApi::NewL();
    DEBUG_LOG(_L("ConstructL - Engine"));
    iEngine = CDmAdEngine::NewL(iCallBack, iStore, iDdf, iRtNodeDataApi);
    
    }

CDmAdAdapter::CDmAdAdapter(MSmlDmCallback* aDmCallback) : 
   CSmlDmAdapter(aDmCallback)
    {
    }

CDmAdAdapter::~CDmAdAdapter()
    {
    DEBUG_LOG(_L("~CDmAdAdapter (PKI)"));
    
    delete iEngine;   
    delete iRtNodeDataApi;
    delete iDdf;
    delete iStore;    
    delete iCallBack;
    
    DEBUG_LOG(_L("FINALIZING LOGGING"));
    FINALIZE_DEBUG_LOG;
    }
    
//===================================================================================================

void CDmAdAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    TRACE("CDmAdAdapter::DDFVersionL");   
    iDdf->BuildDDFVersionL(aDDFVersion);
    }

void CDmAdAdapter::DDFStructureL(MSmlDmDDFObject& aDDF)
    {
    TRACE("CDmAdAdapter::DDFStructureL");
    iDdf->BuildDDFStructureL(aDDF);
    }


void CDmAdAdapter::ChildURIListL(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::ChildURIListL");
    iEngine->ChildURIListL(aUri, aParentLuid, aPreviousUriSegmentList, aResultsRef, aStatusRef);
    }

void CDmAdAdapter::AddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::AddNodeObjectL");
    
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
    DEBUG_LOG1(_L8("aParentLuid = %S"), &aParentLuid);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
    
    // PKCS#12 check here
    TInt status = CheckNodeStatus(aUri);

    if (status == KErrNone) 
        {
        iEngine->AddNodeObjectL(aUri, aParentLuid, aStatusRef);
        }
    else 
        {
        // Most probable reason for error code is that the
        // PKCS#12 node already exists in the LUID mapping
        DEBUG_LOG2(_L("Setting status ref %d to %d"), aStatusRef, status);
        iCallBack->SetStatusL(aStatusRef, status);
        }
    }

TInt CDmAdAdapter::CheckNodeStatus(const TDesC8& aUri) 
    {
    TRACE("CDmAdAdapter::CheckNodeStatusL");

    // Check whether the object is a PKCS#12 one
    TInt ret = aUri.Find(KDmAdOwnAdUriForPKCS12);

    // If ret is >= 0, pkcs12 uri was identified
    if (ret >= 0) 
        {
        // See whether the given PKCS#12 object
        // already exists
        TRAP(ret, CheckPKCS12ExistenceL(aUri));
        }
    else 
        {
        // Wasn't a PKCS#12 node, no further checks needed
        ret = KErrNone;
        }

    DEBUG_LOG1(_L(" Returning with value %d"), ret);

    return ret;
    }

void CDmAdAdapter::CheckPKCS12ExistenceL(const TDesC8& aUri)
    {
    TRACE("CDmAdAdapter::CheckPKCS12Existence");
    DEBUG_LOG1(_L8(" Testing with URI: '%S'"), &aUri);

    HBufC8* foundUri(NULL);

    // Check whether the "LUID" (actually, just the full URI) 
    // already exists...
    DEBUG_LOG(_L(" Comparing LUID/URI to see whether PKCS#12 node exists"));
    foundUri = iCallBack->GetLuidAllocL(aUri);
    CleanupStack::PushL(foundUri);

    if (foundUri->Length() > 0) 
        {
        DEBUG_LOG1(_L8(" Found URI: '%S'"), foundUri);
        DEBUG_LOG(_L(" PKCS#12 node already exists, no need to add, leaving"));
        User::Leave(KErrAlreadyExists);
        }
    else 
        {
        DEBUG_LOG(_L8(" Found URI: (empty)"));
        DEBUG_LOG(_L(" PKCS#12 node is a new one, proceeding with add"));
        }

    CleanupStack::PopAndDestroy(foundUri);
    }


void CDmAdAdapter::UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, 
                                     const TDesC8& aObject, const TDesC8& aType, 
                                     TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::UpdateLeafObjectL");
    
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid = %S"), &aLuid);
    DEBUG_LOG(_L8("aObject:"));
    DEBUG_LOG_HEX(aObject);
    DEBUG_LOG1(_L8("aType = %S"), &aType);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);

    TInt err = CheckNodeStatus(aUri);

    if (err == KErrNone) 
        {
        iEngine->UpdateLeafObjectL(aUri, aLuid, aObject, aType, aStatusRef);
        }
    else 
        {
        // If this PKCS#12 object already existed, do nothing
        iCallBack->SetStatusL(aStatusRef, err);
        }
    }

void CDmAdAdapter::FetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::FetchLeafObjectL");
    iEngine->FetchLeafObjectL(aUri, aLuid, aType, aResultsRef, aStatusRef);
    }

void CDmAdAdapter::DeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::DeleteObjectL");
    iEngine->DeleteObjectL(aUri, aLuid, aStatusRef);
    }

void CDmAdAdapter::CompleteOutstandingCmdsL()
    {
    TRACE("CDmAdAdapter::CompleteOutstandingCmdsL");
    iEngine->CompleteOutstandingCmdsL();
    }
    
void CDmAdAdapter::UpdateLeafObjectL(const TDesC8& aUri,
                                     const TDesC8& aLuid,
                                     RWriteStream*& aStream,
                                     const TDesC8& aType,
                                     TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::UpdateLeafObjectL");
    iEngine->UpdateLeafObjectL(aUri, aLuid, aStream, aType, aStatusRef);
    }
    
void CDmAdAdapter::FetchLeafObjectSizeL(const TDesC8& aUri,
                                        const TDesC8& aLuid,
                                        const TDesC8& aType,
                                        TInt aResultsRef,
                                        TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::FetchLeafObjectSizeL");
    iEngine->FetchLeafObjectSizeL(aUri, aLuid, aType, aResultsRef, aStatusRef);
    }
    
void CDmAdAdapter::ExecuteCommandL(const TDesC8& aUri,
                                   const TDesC8& aLuid,
                                   const TDesC8& aArgument,
                                   const TDesC8& aType,
                                   TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::ExecuteCommandL");
    iEngine->ExecuteCommandL(aUri, aLuid, aArgument, aType, aStatusRef);
    }
    
void CDmAdAdapter::ExecuteCommandL(const TDesC8& aUri,
                                   const TDesC8& aLuid,
                                   RWriteStream*& aStream,
                                   const TDesC8& aType,
                                   TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::ExecuteCommandL");
    iEngine->ExecuteCommandL(aUri, aLuid, aStream, aType, aStatusRef);
    }
    
void CDmAdAdapter::CopyCommandL(const TDesC8& aTargetUri,
                                const TDesC8& aTargetLuid,
                                const TDesC8& aSourceUri,
                                const TDesC8& aSourceLuid,
                                const TDesC8& aType,
                                TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::CopyCommandL");
    iEngine->CopyCommandL(aTargetUri, aTargetLuid, aSourceUri, aSourceLuid, aType, aStatusRef);
    }
    
void CDmAdAdapter::StartAtomicL()
    {
    TRACE("CDmAdAdapter::StartAtomicL");
    iEngine->StartAtomicL();
    }
    
void CDmAdAdapter::CommitAtomicL()
    {
    TRACE("CDmAdAdapter::CommitAtomicL");
    iEngine->CommitAtomicL();
    }
    
void CDmAdAdapter::RollbackAtomicL()
    {
    TRACE("CDmAdAdapter::RollbackAtomicL");
    iEngine->RollbackAtomicL();
    }
    
TBool CDmAdAdapter::StreamingSupport(TInt& aItemSize)
    {
    TRACE("CDmAdAdapter::StreamingSupport");
    return iEngine->StreamingSupport(aItemSize);
    }
    
void CDmAdAdapter::StreamCommittedL()
    {
    TRACE("CDmAdAdapter::StreamCommittedL");
    iEngine->StreamCommittedL();
    }
    
