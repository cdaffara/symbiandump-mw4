/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Sub session base.
*
*/



#include "alf/alfappserver.h"
#include "alf/alfappsrvsessionbase.h"
#include "alfsrvsubsessionbase.h"
#include "alflogger.h"

NONSHARABLE_CLASS(CAlfSrvSubSessionBase::TPrivateData)
    {
    public: 
        MAlfExtension* iObject; // owned, use Release() to destroy
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvSubSessionBase::CAlfSrvSubSessionBase( CAlfAppSrvSessionBase& aSession )
    : iSession( aSession )
    {
    }

CAlfSrvSubSessionBase* CAlfSrvSubSessionBase::NewLC( const RMessage2& aMessage, 
                                                    TInt aImplementationUid,  
                                                    TInt aImplementationId,
                                                    CAlfAppSrvSessionBase& aSession) 
    {
    CAlfSrvSubSessionBase* me = new (ELeave) CAlfSrvSubSessionBase(aSession);
    CleanupClosePushL(*me);
    me->ConstructL(aMessage, aImplementationUid, aImplementationId);
    return me;
    }

void CAlfSrvSubSessionBase::ConstructL(const RMessage2& aMessage, 
                                       TInt aImplementationUid,  
                                       TInt aImplementationId )
    {    
    ConstructL();
    
    // leaves if no implementation found   
    iData->iObject = iSession.AlfServer()->CreateExtensionL(
                           aImplementationUid, aImplementationId,aMessage);

    iFactoryUid = aImplementationUid;
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAlfSrvSubSessionBase::ConstructL()
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iObject = 0;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvSubSessionBase::~CAlfSrvSubSessionBase()
    {
    if (iData && iData->iObject)
        {
        // todo: notify server about implementation removal
        iData->iObject->Release();
        }
        
    delete iData;
    }
  
// ---------------------------------------------------------------------------
// Process message
// ---------------------------------------------------------------------------
//
void CAlfSrvSubSessionBase::ProcessMessageL(const RMessage2& aMessage)
    {
    // Call the virtual function.
    DoProcessMessageL( aMessage );
    }
  
// ---------------------------------------------------------------------------
// Processes the message.
// ---------------------------------------------------------------------------
//  
void CAlfSrvSubSessionBase::DoProcessMessageL(const RMessage2& aMessage)
    {
    if (iData && iData->iObject)
        {
        // session owns the buffers and everything related to msg handling     
        CAlfClientMessage* msg = iSession.CreateNewMessageHolderL(aMessage);
         
        TRAPD(err, iData->iObject->HandleCmdL(msg->iDecodedOp, 
                                              *msg->iInBuf, msg->ReplyBufAsTDes8()));

        iSession.CompleteCmd(0, err);  
        return;
        }

    User::Leave(KErrNotSupported);
    }
  
// ---------------------------------------------------------------------------
// Returns the session
// ---------------------------------------------------------------------------
//  
CAlfAppSrvSessionBase& CAlfSrvSubSessionBase::Session()
    {
    return iSession;
    }


MHuiVisualOwner* CAlfSrvSubSessionBase::AsVisualOwner()
    {
    if (iData && iData->iObject)
        {
        return static_cast<MHuiVisualOwner*>(iData->iObject->GetInterface(EHuiInterfaceVisualOwner));
        }

    return 0;
    }

CHuiControl* CAlfSrvSubSessionBase::AsHuiControl()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CHuiControl*>(iData->iObject->GetInterface(EHuiObjectTypeControl));
        }

    return 0;
    }

CHuiVisual* CAlfSrvSubSessionBase::AsHuiVisual()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CHuiVisual*>(iData->iObject->GetInterface(EHuiObjectTypeVisual));
        }

    return 0;
    }

CHuiLayout* CAlfSrvSubSessionBase::AsHuiLayout()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CHuiLayout*>(iData->iObject->GetInterface(EHuiObjectTypeLayout));
        }

    return 0;
    }

CHuiBrush* CAlfSrvSubSessionBase::AsHuiBrush()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CHuiBrush*>(iData->iObject->GetInterface(EHuiObjectTypeBrush));
        }

    return 0;
    }

MHuiMappingFunction* CAlfSrvSubSessionBase::AsMappingFunction()
    {
    if (iData && iData->iObject)
        {
        return static_cast<MHuiMappingFunction*>(iData->iObject->GetInterface(EHuiInterfaceMappingFunction));
        }

    return 0;
    }

CHuiControlGroup* CAlfSrvSubSessionBase::AsHuiControlCroup()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CHuiControlGroup*>(iData->iObject->GetInterface(EHuiObjectTypeControlGroup));
        }

    return 0;    
    }
    
CHuiDisplay* CAlfSrvSubSessionBase::AsHuiDisplay()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CHuiDisplay*>(iData->iObject->GetInterface(EHuiObjectTypeDisplay));
        }

    return 0;    
    }

CAlfBrushHandler* CAlfSrvSubSessionBase::AsBrushHandler()
    {
    if (iData && iData->iObject)
        {
        return static_cast<CAlfBrushHandler*>(iData->iObject->GetInterface(EAlfBrushHandler));
        }

    return 0;    
    }

MAlfExtension* CAlfSrvSubSessionBase::AsCommandHandler()
    {
    if (iData)
        {
        return iData->iObject;
        }

    return 0;    
    }

// Todo: Currently we don't support trulu asynch operations as 
// MAlfExtension does not have interfaces to utilize for this purpose
//
// It should be evaluated whether there are real use cases for asynchronous command handling
void CAlfSrvSubSessionBase::CancelAsynchRequests()
    {
    }
    
