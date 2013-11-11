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
* Description: Implementation of CDmAdRtNode.
*
*/


#include "DmAdRtNode.h"
#include "dmadcallback.h"
#include "dmadcommand.h"
#include "dmadutil.h"
#include "vpnlogger.h"

DMAD_EXPORT_C CDmAdRtNode* CDmAdRtNode::NewL(const TDesC8& aUri, const TDesC8& aLuid, MDmAdCallBack* aCallBack, MDmAdRtNodeDataApi* aRtNodeDataApi)
    {
    TRACE("CDmAdRtNode::NewL");
    
    CDmAdRtNode *self = new (ELeave) CDmAdRtNode(aRtNodeDataApi);
    CleanupStack::PushL(self);
    self->ConstructL(aUri, aLuid, aCallBack);
    CleanupStack::Pop();        // self
    return self;
    }

void CDmAdRtNode::ConstructL(const TDesC8& aUri, const TDesC8& aLuid, MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdRtNode::ConstructL");
    
    DEBUG_LOG1(_L8("aUri: %S"), &aUri);
    DEBUG_LOG(_L("aLuid:"));
    DEBUG_LOG_HEX(aLuid);
    
    iCommands = new (ELeave) CArrayPtrFlat<CDmAdCommand>(4);
    iUri = aUri.AllocL();
    iLuid = aLuid.AllocL();
    iData = iRtNodeDataApi->CreateDmAdRtNodeDataL(aUri, aCallBack);
    }
    
CDmAdRtNode::CDmAdRtNode(MDmAdRtNodeDataApi* aRtNodeDataApi)
    : iRtNodeDataApi(aRtNodeDataApi), iSomeLeafAddedToRtNode(EFalse), iJustFetched(ETrue)
    {
    TRACE("CDmAdRtNode::CDmAdRtNode");
    }

DMAD_EXPORT_C CDmAdRtNode::~CDmAdRtNode()
    {
    TRACE("CDmAdRtNode::~CDmAdRtNode");
    
    CDmAdCommand::CleanupOperationDeleteCArrayPtr(iCommands);
    delete iUri;
    delete iLuid;
    iRtNodeDataApi->DeleteDmAdRtNodeData(iData);
    }
    
DMAD_EXPORT_C TPtrC8 CDmAdRtNode::Uri(void) const
    {
    TRACE("CDmAdRtNode::Uri");
    
    __ASSERT_ALWAYS(iUri != NULL, User::Invariant());
    
    TPtrC8 ret(*iUri);
    return ret;
    }

DMAD_EXPORT_C TPtrC8 CDmAdRtNode::Luid(void) const
    {
    TRACE("CDmAdRtNode::Luid");
    
    __ASSERT_ALWAYS(iLuid != NULL, User::Invariant());
    
    TPtrC8 ret(*iLuid);
    return ret;
    }

DMAD_EXPORT_C void CDmAdRtNode::SetLuidL(const TDesC8& aLuid)
    {
    TRACE("CDmAdRtNode::SetLuidL");
    
    DEBUG_LOG1(_L8("Node uri: %S"), iUri);
    DEBUG_LOG(_L("aLuid:"));
    DEBUG_LOG_HEX(aLuid);    
    
    __ASSERT_ALWAYS(aLuid.Length() > 0, User::Invariant());
    
        
    delete iLuid;
    iLuid = NULL;
    
    iLuid = aLuid.AllocL();
    }
    
    
void CDmAdRtNode::CleanupOperationDeleteCArrayPtr(TAny* aPtr)
    {    
    __ASSERT_ALWAYS(aPtr != NULL, User::Invariant());
    
    CArrayPtr<CDmAdRtNode>* array = REINTERPRET_CAST(CArrayPtr<CDmAdRtNode>*,aPtr);
    array->ResetAndDestroy();
    delete array;
    }


void CDmAdRtNode::AppendCommandL(CDmAdCommand* aCommand)
    {
    TRACE("CDmAdRtNode::AppendCommandL");
    
    iCommands->AppendL(aCommand);
    iJustFetched = EFalse;
    }

void CDmAdRtNode::AddLeafObjectL(const TDesC8& aUri, const TDesC8& aObject, TInt aStatusRef)
    {
    TRACE("CDmAdRtNode::AddLeafObjectL");
    
    CDmAdCommand* command = CDmAdCommand::NewLC(aUri, aStatusRef, ETrue);

    iRtNodeDataApi->UpdateLeafDataL(iData, aUri, aObject);
    iSomeLeafAddedToRtNode = ETrue;
    
    AppendCommandL(command);
    CleanupStack::Pop(); //command
    }

void CDmAdRtNode::UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aObject, TInt aStatusRef)
    {
    TRACE("CDmAdRtNode::UpdateLeafObjectL");
    
    CDmAdCommand* command = CDmAdCommand::NewLC(aUri, aStatusRef, ETrue);
    
    iRtNodeDataApi->UpdateLeafDataL(iData, aUri, aObject);
    
    AppendCommandL(command);
    CleanupStack::Pop(); //command
    }
    
TBool CDmAdRtNode::IsSomeChild(const TDesC8& aSomeParentUri) const
    {
    TRACE("CDmAdRtNode::IsSomeChild");
    
    TPtrC8 someParentUri(TDmAdUtil::RemoveDotSlash(aSomeParentUri));
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(*iUri));
    if (uri.Find(someParentUri) == 0)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
TBool CDmAdRtNode::AreUriTypesSame(const TDesC8& aUri)
    {
    TRACE("CDmAdRtNode::AreUriTypesSameL");
    
    return iRtNodeDataApi->AreUriTypesSame(iData, aUri);
    }
    
