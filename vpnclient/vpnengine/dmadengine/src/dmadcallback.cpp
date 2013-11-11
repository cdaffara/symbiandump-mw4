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
* Description:   Call back implementation
*
*/


#include <utf.h>

#include "dmadcallbackc.h"
#include "dmadutil.h"
#include "vpnlogger.h"

#if defined(_DEBUG)
_LIT(KDmAdLogFolder,"vpn");
_LIT(KDmAdLogFile,"dmadengine.log");
#endif


DMAD_EXPORT_C CDmAdCallBack* CDmAdCallBack::NewL(MSmlDmCallback* aCallBack, const TDesC8& aSomeOwnUri)
    {
    
    //Is first method called from the dmadengine
    //This why the logging is initiated here.
    INITIALIZE_DEBUG_LOG_L(KDmAdLogFolder, KDmAdLogFile);
    DEBUG_LOG1(_L8("LOGGING INITIALIZED for %S"), &aSomeOwnUri);   
    
    CDmAdCallBack* self = NewLC(aCallBack, aSomeOwnUri);
    CleanupStack::Pop(); // self
    return self;
    }

DMAD_EXPORT_C CDmAdCallBack* CDmAdCallBack::NewLC(MSmlDmCallback* aCallBack, const TDesC8& aSomeOwnUri)
    {
    CDmAdCallBack* self = new (ELeave) CDmAdCallBack(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL(aSomeOwnUri);
    return self;
    }

void CDmAdCallBack::ConstructL(const TDesC8& aSomeOwnUri)
    {
    TRACE("CDmAdCallBack::ConstructL");
    iSomeOwnUri = aSomeOwnUri.AllocL();
    }
    
CDmAdCallBack::CDmAdCallBack(MSmlDmCallback* aCallBack) : iCallBack(aCallBack)
    {
    }
    
DMAD_EXPORT_C CDmAdCallBack::~CDmAdCallBack()
    {    
    
    DEBUG_LOG1(_L8("FINALIZING LOGGING %S"), iSomeOwnUri);            
    FINALIZE_DEBUG_LOG;
    
    delete iSomeOwnUri;
    }
    
DMAD_EXPORT_C void CDmAdCallBack::SetResultsL(TInt aResultsRef, CBufBase& aObject, const TDesC8& aType)
    {
    TRACE("CDmAdCallBack::SetResultsL");
    
    DEBUG_LOG1(_L("rref = %d"), aResultsRef);
    DEBUG_LOG1(_L("lth  = %d"), aObject.Ptr(0).Length());
    
    DEBUG_LOG_HEX(aObject.Ptr(0));
        
    iCallBack->SetResultsL(aResultsRef, aObject, aType);    
    }

DMAD_EXPORT_C void CDmAdCallBack::SetStatusL(TInt aStatusRef, TInt aError)
    {
    TRACE("CDmAdCallBack::SetStatusL");

    DEBUG_LOG1(_L("rref   = %d"), aStatusRef);
    DEBUG_LOG1(_L("status = %d"), aError);
    
    iCallBack->SetStatusL(aStatusRef, TDmAdUtil::MapStatusCode(aError));
    }
    
DMAD_EXPORT_C void CDmAdCallBack::SetMappingL(const TDesC8& aUri, const TDesC8& aLuid)
    {
    TRACE("CDmAdCallBack::SetMappingL");
    
    DEBUG_LOG1(_L8("aUri %S"), &aUri);  
    DEBUG_LOG(_L8("aLuid:"));              
    DEBUG_LOG_HEX(aLuid);      
            
    iCallBack->SetMappingL(aUri, aLuid);
    }

DMAD_EXPORT_C void CDmAdCallBack::FetchLinkL(const TDesC8& aUri, CBufBase& aData, MSmlDmAdapter::TError& aStatus)
    {
    TRACE("CDmAdCallBack::FetchLinkL");

    aData.Reset();        
    iCallBack->FetchLinkL(aUri, aData, aStatus);

    DEBUG_LOG1(_L("FetchLinkL status %d"), aStatus);
    DEBUG_LOG1(_L("Data Length %d"), aData.Ptr(0).Length());
    DEBUG_LOG1(_L8("aUri = %S"), &aUri);    
    DEBUG_LOG_HEX(aData.Ptr(0));
    
    //Some adapters are a bit sloppy and returns a malformed URI, which
    //ends with / character. This causes problems in our code.
    //So if the last character is /, we strip it off.
    
    TUint dataSize = aData.Size();
    if (dataSize > 0)
        {
        TUint lastPosition = dataSize - 1;
        TUint8 lastCharacter;
        aData.Read(lastPosition, &lastCharacter, 1);
        
        if (lastCharacter == '/')
            {
            DEBUG_LOG(_L("WARNING: last character is /. Stripping it off..."));
            aData.ResizeL(lastPosition);
            }
        
        }            
}

DMAD_EXPORT_C HBufC8* CDmAdCallBack::GetLuidAllocL(const TDesC8& aUri)
    {
    TRACE("CDmAdCallBack::GetLuidAllocL");
        
    HBufC8* luid = iCallBack->GetLuidAllocL(aUri);
    
    DEBUG_LOG1(_L8("aUri %S"), &aUri);       
    DEBUG_LOG(_L("luid:"));
    DEBUG_LOG_HEX(*luid);

#if 1 // restore adapter index (fixes agent)
    
    HBufC8* tempLuid = iCallBack->GetLuidAllocL(*iSomeOwnUri);
    DEBUG_LOG1(_L8("iCallBack->GetLuidAllocL(%S) called"), iSomeOwnUri);
    delete tempLuid;
#endif       

    return luid;
    }

DMAD_EXPORT_C HBufC8* CDmAdCallBack::FindChildUriL(const TDesC8& aParentUri, const TDesC8& aChildLuid)
    {
    TRACE("CDmAdCallBack::FindChildUriL");
    
    DEBUG_LOG1(_L8("aParentUri = %S"), &aParentUri);
    DEBUG_LOG(_L8("aChildLuid:"));
    DEBUG_LOG_HEX(aChildLuid);
        
    CBufBase* childUriList = CBufFlat::NewL(16);
    CleanupStack::PushL(childUriList);
    MSmlDmAdapter::TError status;
    FetchLinkL(aParentUri, *childUriList, status);
    if (status == MSmlDmAdapter::ENotFound)
        {
        DEBUG_LOG(_L("Node not found"));        
        CleanupStack::PopAndDestroy(); // childUriList
        return 0;
        }
    else if (status != MSmlDmAdapter::EOk)
        {
        DEBUG_LOG1(_L("Operation failed with %d"), status);        
        User::Leave(KErrGeneral);
        }

    CArrayFix<TPtrC8>* uriSegList;
    TDmAdUtil::ParseUriLC(childUriList->Ptr(0), uriSegList);

    TBool found = EFalse;
    HBufC8* uri = 0;
    for (TInt i=0; i<uriSegList->Count(); ++i)
        {
        uri = TDmAdUtil::BuildUriLC(aParentUri, uriSegList->At(i));
        HBufC8* luid = GetLuidAllocL(*uri);
        if (luid->Compare(aChildLuid) == 0)
            {
            found = ETrue;
            delete luid;
            break;
            }
        CleanupStack::PopAndDestroy(); // uri
        delete luid;
        }

    if (found)
        {
        CleanupStack::Pop(); // uri
        }
    
    CleanupStack::PopAndDestroy(2); //uriSegList, childUriList
    
    if (found)
        {
        DEBUG_LOG1(_L8("Found uri %S"), uri);                
        return uri;
        }
    else
        {
        DEBUG_LOG(_L("Uri not found"));        
        return 0;
        }
    }
