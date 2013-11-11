/*
* Copyright (c) 2000-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN OMA DM Adapter
*
*/



#include <e32base.h>
#include <ecom/implementationproxy.h>

#include "dmadadapter.h"
#include "dmadadapterimplconst.h"
#include "dmadengine.h"
#include "DmAdStore.h"
#include "dmadddf.h"
#include "dmadrtnodedataapic.h"
#include "vpnlogger.h"

#if defined(_DEBUG)
_LIT(KDmAdLogFolder,"vpn");
_LIT(KDmAdLogFile,"dmadipsecvpn.txt");
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
    CleanupStack::Pop();
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
        
    iCallBack = CDmAdCallBack::NewL(aDmCallback, KDmAdOwnAdUriForGetLuidAllocLFix);  
    iStore = CDmAdStore::NewL();
    iDdf = CDmAdDdf::NewL(iCallBack, iStore);
    iRtNodeDataApi = CDmAdRtNodeDataApi::NewL();    
    iEngine = CDmAdEngine::NewL(iCallBack, iStore, iDdf, iRtNodeDataApi);
    }

CDmAdAdapter::CDmAdAdapter(MSmlDmCallback* aDmCallback) : 
   CSmlDmAdapter(aDmCallback)
    {
    TRACE("CDmAdAdapter::CDmAdAdapter");
    }

CDmAdAdapter::~CDmAdAdapter()
    {
    DEBUG_LOG(_L("CDmAdAdapter::~CDmAdAdapter"));
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
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
    
    iEngine->ChildURIListL(aUri, aParentLuid, aPreviousUriSegmentList, aResultsRef, aStatusRef);
    }

void CDmAdAdapter::AddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef)
    {    
    TRACE("CDmAdAdapter::AddNodeObjectL");
    
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
    DEBUG_LOG1(_L8("aParentLuid = %S"), &aParentLuid);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
    
    iEngine->AddNodeObjectL(aUri, aParentLuid, aStatusRef);
    }

void CDmAdAdapter::UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::UpdateLeafObjectL");
    
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid = %S"), &aLuid);
    DEBUG_LOG(_L8("aObject:"));
    DEBUG_LOG_HEX(aObject);
    DEBUG_LOG1(_L8("aType = %S"), &aType);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
       
    iEngine->UpdateLeafObjectL(aUri, aLuid, aObject, aType, aStatusRef);
    }

void CDmAdAdapter::FetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::FetchLeafObjectL");
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
        
    iEngine->FetchLeafObjectL(aUri, aLuid, aType, aResultsRef, aStatusRef);
    }

void CDmAdAdapter::DeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::DeleteObjectL");
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
        
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
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
    
    iEngine->FetchLeafObjectSizeL(aUri, aLuid, aType, aResultsRef, aStatusRef);
    }
    
void CDmAdAdapter::ExecuteCommandL(const TDesC8& aUri,
                                   const TDesC8& aLuid,
                                   const TDesC8& aArgument,
                                   const TDesC8& aType,
                                   TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::ExecuteCommandL");
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
        
    iEngine->ExecuteCommandL(aUri, aLuid, aArgument, aType, aStatusRef);
    }
    
void CDmAdAdapter::ExecuteCommandL(const TDesC8& aUri,
                                   const TDesC8& aLuid,
                                   RWriteStream*& aStream,
                                   const TDesC8& aType,
                                   TInt aStatusRef)
    {
    TRACE("CDmAdAdapter::ExecuteCommandL");
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);
        
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
    

