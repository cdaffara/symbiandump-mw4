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
* Description: Device Management Adapter Engine.
*
*/


#include <utf.h>

#include "dmadengine.h"
#include "dmadutil.h"
#include "vpnlogger.h"


DMAD_EXPORT_C void CDmAdEngine::ChildURIListL(const TDesC& /*aUri*/, 
                                              const TDesC& /*aParentLuid*/, 
                                              const CArrayFix<TSmlDmMappingInfo>& /*aPreviousUriSegmentList*/, 
                                              const TInt /*aResultsRef*/, 
                                              const TInt /*aStatusRef*/)
    {
    TRACE("CDmAdEngine::ChildURIListL");
    DEBUG_LOG(_L("Empty method"));
    }

DMAD_EXPORT_C void CDmAdEngine::AddNodeObjectL(const TDesC& /*aUri*/, 
                                               const TDesC& /*aParentLuid*/, 
                                               const TInt /*aStatusRef*/)
    {
    TRACE("CDmAdEngine::AddNodeObjectL");
    DEBUG_LOG(_L("Empty method"));
    }

DMAD_EXPORT_C void CDmAdEngine::AddLeafObjectL(const TDesC& /*aUri*/, 
                                               const TDesC& /*aParentLuid*/, 
                                               const TDesC8& /*aObject*/, 
                                               const TDesC& /*aType*/, 
                                               const TInt /*aStatusRef*/)
    {
    TRACE("CDmAdEngine::AddLeafObjectL");
    DEBUG_LOG(_L("Empty method"));
    }

DMAD_EXPORT_C void CDmAdEngine::UpdateLeafObjectL(const TDesC& /*aUri*/, 
                                                  const TDesC& /*aLuid*/, 
                                                  const TDesC8& /*aObject*/, 
                                                  const TDesC& /*aType*/, 
                                                  const TInt /*aStatusRef*/)
    {
    TRACE("CDmAdEngine::UpdateLeafObjectL");
    DEBUG_LOG(_L("Empty method"));
    }

DMAD_EXPORT_C void CDmAdEngine::FetchLeafObjectL(const TDesC& /*aUri*/, 
                                                 const TDesC& /*aLuid*/, 
                                                 const TDesC& /*aType*/, 
                                                 const TInt /*aResultsRef*/, 
                                                 const TInt /*aStatusRef*/)
    {
    TRACE("CDmAdEngine::FetchLeafObjectL");
    DEBUG_LOG(_L("Empty method"));
    }

DMAD_EXPORT_C void CDmAdEngine::DeleteObjectL(const TDesC& /*aUri*/, 
                                              const TDesC& /*aLuid*/, 
                                              const TInt /*aStatusRef*/)
    {
    TRACE("CDmAdEngine::DeleteObjectL");
    DEBUG_LOG(_L("Empty method"));
    }

DMAD_EXPORT_C void CDmAdEngine::EndMessageL()
    {
    TRACE("CDmAdEngine::EndMessageL");
    DEBUG_LOG(_L("Empty method"));
    }
    
//===================================================================================================

DMAD_EXPORT_C void CDmAdEngine::UpdateLeafObjectL(const TDesC8& /*aUri*/,
                                                  const TDesC8& /*aLuid*/,
                                                  RWriteStream*& /*aStream*/,
                                                  const TDesC8& /*aType*/,
                                                  TInt aStatusRef)
    {
    TRACE("CDmAdEngine::UpdateLeafObjectL");        
    DEBUG_LOG(_L("Unsupported stream method called"));
        
    iCallBack->SetStatusL(aStatusRef, KErrGeneral);
    }
    
DMAD_EXPORT_C void CDmAdEngine::FetchLeafObjectSizeL(const TDesC8& aUri,
                                                     const TDesC8& aLuid,
                                                     const TDesC8& aType,
                                                     TInt aResultsRef,
                                                     TInt aStatusRef)
    {
    TRACE("CDmAdEngine::FetchLeafObjectSizeL");
        
    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid %S"), &aLuid);
    DEBUG_LOG1(_L8("aType %S"), &aType);
    DEBUG_LOG2(_L("aResultsRef = %d, aStatusRef = %d"), aResultsRef, aStatusRef);
    
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TInt status = KErrNone;
    CBufBase* object = 0;
    TRAPD(err, status = DoFetchLeafObjectL(uri, aLuid, aType, object));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        return;
        }
    if (status != KErrNone)
        {
        delete object;
        iCallBack->SetStatusL(aStatusRef, status);
        return;
        }
    
    CleanupStack::PushL(object);
    TInt objSizeInBytes = object->Size();
    HBufC8* sizeBuf = TDmAdUtil::IntToDes8LC(objSizeInBytes);
    object->Reset();
    object->InsertL(0, *sizeBuf);
        
    iCallBack->SetResultsL(aResultsRef, *object, aType);
    iCallBack->SetStatusL(aStatusRef, status);
    CleanupStack::PopAndDestroy(2); //sizeBuf, object
    }
    
DMAD_EXPORT_C void CDmAdEngine::ExecuteCommandL(const TDesC8& aUri,
                                                const TDesC8& aLuid,
                                                const TDesC8& aArgument,
                                                const TDesC8& aType,
                                                TInt aStatusRef)
    {
    TRACE("CDmAdEngine::ExecuteCommandL");
    
    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid %S"), &aLuid);
    
    DEBUG_LOG(_L("Argument:"));
    DEBUG_LOG_HEX(aArgument);
    
    DEBUG_LOG1(_L8("aType %S"), &aType);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
        
    if (!iDdfApi->IsLeafUnderRtNodeL(aUri))
        {
        iDdfApi->NotRtNodeUpdateLeafObjectL(aUri, aLuid, aArgument, aType, aStatusRef);
        }
    else
        {
        iCallBack->SetStatusL(aStatusRef, KErrNotFound);
        }
    }
    
DMAD_EXPORT_C void CDmAdEngine::ExecuteCommandL(const TDesC8& /*aUri*/,
                                                const TDesC8& /*aLuid*/,
                                                RWriteStream*& /*aStream*/,
                                                const TDesC8& /*aType*/,
                                                TInt aStatusRef)
    {
    TRACE("CDmAdEngine::ExecuteCommandL");    
    DEBUG_LOG(_L("Unsupported stream method called"));

    iCallBack->SetStatusL(aStatusRef, KErrGeneral);
    }
    
DMAD_EXPORT_C void CDmAdEngine::CopyCommandL(const TDesC8& aTargetUri,
                                             const TDesC8& aTargetLuid,
                                             const TDesC8& aSourceUri,
                                             const TDesC8& aSourceLuid,
                                             const TDesC8& aType,
                                             TInt aStatusRef)
    {
    TRACE("CDmAdEngine::CopyCommandL");
    
    DEBUG_LOG1(_L8("aTargetUri = %S"), &aTargetUri);
    DEBUG_LOG1(_L8("aTargetLuid = %S"), &aTargetLuid);
    DEBUG_LOG1(_L8("aSourceUri = %S"), &aSourceUri);
    DEBUG_LOG1(_L8("aSourceLuid = %S"), &aSourceLuid);
    DEBUG_LOG1(_L8("aType = %S"), &aType);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
    

    TInt err = KErrNone;
    TPtrC8 sourceUri(TDmAdUtil::RemoveDotSlash(aSourceUri));
    TInt status = KErrNone;
    CBufBase* object = 0;
    TRAP(err, status = DoFetchLeafObjectL(sourceUri, aSourceLuid, aType, object));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        return;
        }
    if (status != KErrNone)
        {
        delete object;
        iCallBack->SetStatusL(aStatusRef, status);
        return;
        }
    
    CleanupStack::PushL(object);

    DEBUG_LOG(_L("object:"));
    DEBUG_LOG_HEX(object->Ptr(0));
    
    TPtrC8 targetUri(TDmAdUtil::RemoveDotSlash(aTargetUri));
    TRAP(err, DoUpdateLeafObjectL(targetUri, aTargetLuid, object->Ptr(0), aType, aStatusRef));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        }

    CleanupStack::PopAndDestroy(); // object
    }
    
DMAD_EXPORT_C void CDmAdEngine::StartAtomicL()
    {
    TRACE("CDmAdEngine::StartAtomicL");
    DEBUG_LOG(_L("Empty method"));
    }
    
DMAD_EXPORT_C void CDmAdEngine::CommitAtomicL()
    {
    TRACE("CDmAdEngine::CommitAtomicL");
    DEBUG_LOG(_L("Empty method"));
    }
    
DMAD_EXPORT_C void CDmAdEngine::RollbackAtomicL()
    {
    TRACE("CDmAdEngine::RollbackAtomicL");
    DEBUG_LOG(_L("Empty method"));
    }
    
DMAD_EXPORT_C TBool CDmAdEngine::StreamingSupport(TInt& /*aItemSize*/)
    {
    TRACE("CDmAdEngine::StreamingSupport");      
    return EFalse;
    }
    
DMAD_EXPORT_C void CDmAdEngine::StreamCommittedL()
    {
    TRACE("CDmAdEngine::StreamCommittedL");
    DEBUG_LOG(_L("Empty method"));
    }
    
