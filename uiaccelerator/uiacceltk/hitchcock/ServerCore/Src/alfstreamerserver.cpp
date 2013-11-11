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
* Description:   Server implementation fo share files in application private gage 
*                with controlled way
*
*/



#include <e32std.h>
#include <f32file.h>
#include <e32math.h>
#include "alfstreamerserver.h"
#include "alfstreamerconsts.h"
#include "alfstreamerbridge.h"
#include "alfwindowmanager.h"
#include "alfhierarchymodel.h"
#include "alflogger.h"
#include <ecom.h>
#include <alf/AlfTransEffectPlugin.h>
#include <alf/AlfTransEffectPlugin.hrh>


// Implements just Error() to avoid panic
NONSHARABLE_CLASS(CSimpleScheduler) : public CActiveScheduler
    {
    void Error( TInt ) const{} // From CActiveScheduler
    };

// ==================================
// Launcher implementation.
// ==================================   

// ---------------------------------------------------------------------------
// DoAlfStreamerServerThreadStartFunctionL
// Construct the server object
// ---------------------------------------------------------------------------
//   
LOCAL_C void DoAlfStreamerServerThreadStartFunctionL(CAlfStreamerBridge* aBridge)
    {
    CAlfStreamerServer::NewLC(aBridge);
    
    RThread().Rendezvous(KErrNone);
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy(); // server
    }

// ---------------------------------------------------------------------------
// Entry point into the new thread
// ---------------------------------------------------------------------------
//   
GLDEF_C TInt AlfStreamerServerThreadStartFunction(TAny* aBridge)
    {
    __UHEAP_MARK;
    RThread thread;

    TInt err = User::RenameThread(KAlfStreamerServerThreadName);
    if (err == KErrNone)
        {
        thread.SetPriority(EPriorityAbsoluteHigh);
        thread.Close();

        // Set up scheduler and cleanup stack for this thread
        CActiveScheduler* scheduler = new CSimpleScheduler;
        if (!scheduler)
            {
            return KErrNoMemory;
            }
        CActiveScheduler::Install(scheduler);
        CTrapCleanup* trapCleanup = CTrapCleanup::New();
        if (!trapCleanup)
            {
            return KErrNoMemory;
            }

        // Set initial trap harness, and construct server object
        TRAP(err,DoAlfStreamerServerThreadStartFunctionL((CAlfStreamerBridge*)aBridge));

        delete CActiveScheduler::Current();
        delete trapCleanup;
        }
    __UHEAP_MARKEND;
    return err;
    }

// ---------------------------------------------------------------------------
// LaunchServer
// ---------------------------------------------------------------------------
//   
TInt CAlfStreamerServer::LaunchServer(TThreadId& aThreadId, CAlfStreamerBridge* aBridge)
    {
    // First, check that ther server isn't already running.
    TFindServer findServer(ALFSTREAMER_SERVER_NAME);
    TFullName name;
    if (findServer.Next(name) == KErrNone)
        {
        return KErrAlreadyExists;
        }
    RThread serverThread;

    TInt err = serverThread.Create(
        KAlfStreamerServerThreadName,
        AlfStreamerServerThreadStartFunction,
        16384, // magic
        0, // uses caller thread's heap
        (TAny*)aBridge, 
        EOwnerThread);

    aThreadId = serverThread.Id();
    TRequestStatus status;
    serverThread.Rendezvous(status);
    serverThread.Resume();
    serverThread.Close();
    User::WaitForRequest(status);
    return err;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//   
void CAlfStreamerServer::NewLC(CAlfStreamerBridge* aBridge)
    {
    CAlfStreamerServer* pS = new (ELeave) CAlfStreamerServer(aBridge);
    CleanupStack::PushL(pS);
    pS->ConstructL();
    }

TInt DoRendezvous(TAny* aCallBack)
    {
    RThread().Rendezvous(KErrNone);
    CAsyncCallBack* cb = (CAsyncCallBack*) aCallBack;
    delete cb;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//   
void CAlfStreamerServer::ConstructL()
    {
    StartL(ALFSTREAMER_SERVER_NAME);
    
    iWindowHierarcy = CAlfHierarchyModel::NewL(*this);
    iWindowMgr = CAlfWindowManager::NewL(iWindowHierarcy);
    iThemesListener = CThemeRepositoryListener::NewL();
    iThemesListener->IssueRequest();
    iRendezvous = new (ELeave) CAsyncCallBack(EPriorityHigh*3);
    iRendezvous->Set(TCallBack(DoRendezvous, iRendezvous));
    iRendezvous->CallBack();
    }

// ---------------------------------------------------------------------------
// NewSessionL
// ---------------------------------------------------------------------------
//   
CSession2* CAlfStreamerServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& aMessage) const
    {
    RThread t;    
    aMessage.Client(t);
    CleanupClosePushL(t);
	CSession2* newSession = new(ELeave) CAlfStreamerServerSession(t.Id());
    CleanupStack::PopAndDestroy();
    iSessions++;
    
    return newSession;   
    }

CAlfStreamerServerSession* CAlfStreamerServer::WservSession(TInt aScreenNumber)
    {
    CAlfStreamerServerSession* windowServerSession = NULL;
    for(TInt i = 0; i < iWindowServerSessions.Count(); i++ )
        { 
        if(aScreenNumber == iWindowServerSessions[i]->ScreenNumber() )
            {
            windowServerSession = iWindowServerSessions[i];
            break;
            }
        }
    return windowServerSession;
    }


// ---------------------------------------------------------------------------
// HandleClientExit
// ---------------------------------------------------------------------------
//   
void CAlfStreamerServer::HandleClientExit(const CAlfStreamerServerSession* aClient)
    {
    iSessions--;
    if (WindowMgr())
        {
        WindowMgr()->HandleClientExit(aClient->ThreadId());
        }
        
    for (TInt i = iSignals.Count()-1; i >= 0 ; i--)
        {
        if (iSignals[i].iSession == aClient)
            {
            // Don't complete because client is already being deleted    
            iSignals.Remove(i);
            }         
        }
                
    if (!iSessions)
        {
        // CActiveScheduler::Stop(); // TODO: lets not die, if client dies.
        }
    }

// ---------------------------------------------------------------------------
// destructor of CAlfStreamerServer
// ---------------------------------------------------------------------------
//   
CAlfStreamerServer::~CAlfStreamerServer()
    {
    delete iThemesListener;
    delete iWindowMgr;
    delete iWindowHierarcy;
    iCompositionSessions.Close();
    iCompositionHostSessions.Close();
    iCompositionTokens.Close();
    iWindowServerSessions.Close();
    iAlfTargets.Close();
    iOptionalGRAM.Close();
    }

void CAlfStreamerServer::AppendCompositionSessionL(CAlfStreamerServerSession* aSession, TBool aHost)
    {
    if(!aHost)
        {
        TInt index = iCompositionSessions.Find(aSession);    
        if (index == KErrNotFound)
            {
            iCompositionSessions.AppendL(aSession);
            }
        }
    else
        {
        TInt index = iCompositionHostSessions.Find(aSession);    
        if (index == KErrNotFound)
            {
            iCompositionHostSessions.AppendL(aSession);
            }
        }
    }
void CAlfStreamerServer::RemoveCompositionSession(CAlfStreamerServerSession* aSession)    
    {
    TInt index = iCompositionSessions.Find(aSession);    
    if (index != KErrNotFound)
        {
        FreeCompositionSessionExtents(aSession);
        TPckgC<TInt> id(reinterpret_cast<TInt>(aSession));
        for(TInt u = 0; u < iWindowServerSessions.Count(); u++ )
            { 
            if( aSession->ScreenNumber() == iWindowServerSessions[u]->ScreenNumber())
                {
                TRAP_IGNORE(QueueRequestForSessionL(iWindowServerSessions[u], id, KAlfCompOpSessionClosed));
                }
            }
            
        iCompositionSessions.Remove(index);
        
        }
    TInt hostindex = iCompositionHostSessions.Find(aSession);    
    if (hostindex != KErrNotFound)
        {
        HBufC8* message = HBufC8::NewLC(8);
        TInt session = reinterpret_cast<TInt>(aSession);
        TPtr8 clientBuf = message->Des();
        clientBuf.Insert(0, TPtrC8((TUint8*)&session ,sizeof(TInt)));
        TInt newtarget = 0;
        clientBuf.Insert(1 * sizeof(TInt), TPtrC8((TUint8*)&newtarget ,sizeof(TInt)));
        for(TInt i = 0; i < iWindowServerSessions.Count(); i++ )
            { 
            if( aSession->ScreenNumber() == iWindowServerSessions[i]->ScreenNumber())
                {
                TRAP_IGNORE(QueueRequestForSessionL(iWindowServerSessions[i], *message, KAlfCompOpBindSourceToToken)); // bind to 0 -> remove binding
                }
            }
        CleanupStack::PopAndDestroy();
        iCompositionHostSessions.Remove(hostindex);
        }

    // clean possible tokens for this composition source
    TInt targetSession = reinterpret_cast<TInt>(aSession);
    for( TInt i = 0; i < iCompositionTokens.Count() ; i++)
        {
        if ( targetSession == iCompositionTokens[i].iTarget )
            {
            iCompositionTokens.Remove(i);
            i--;
            }
        };
    }

void CAlfStreamerServer::HandleCompositionRequestL(CAlfStreamerServerSession* aSession, TInt aOp, const RMessage2& aMessage)
    {
    TInt length = aMessage.GetDesLength(0);
    User::LeaveIfError(length);
    HBufC8* message = HBufC8::NewLC(length+4);
    TPtr8 clientBuf = message->Des();
    aMessage.ReadL(0,clientBuf);
 
    HandleCompositionRequestL(aSession,aOp, clientBuf);
        
    if( aOp == KAlfCompOpCreateSource )
        {
        AppendCompositionSessionL(aSession);
        TAlfCompParams* param = (TAlfCompParams*)clientBuf.Ptr();
        for (TInt i = iAlfTargets.Count()-1;i >= 0; i--)
            {
            if (iAlfTargets[i].iWindowHandle == param->iWindowHandle&& 
                iAlfTargets[i].iWindowGroup == param->iWindowGroup)
                {
                aSession->AlfToken() = iAlfTargets[i].iTarget;
                __ALFLOGSTRING("CAlfStreamerServer::HandleCompositionRequestL(): Found ALF token for composition session")
                }     
            }    
        aMessage.Complete(reinterpret_cast<TInt>(aSession));
        }
    else if ( aOp != KAlfCompOpSetZOrder )
        {
        aMessage.Complete(KErrNone);
        }

    CleanupStack::PopAndDestroy(); // unefficient..
    }

void CAlfStreamerServer::HandleCompositionRequestL(CAlfStreamerServerSession* aSession, TInt aOp, TPtr8& aClientBuf)
    {
    CAlfStreamerServerSession* wservsession = WservSession( aSession ? aSession->ScreenNumber() : 0 );
    if (!wservsession)
        {
        __ALFLOGSTRING("CAlfStreamerServer::HandleCompositionRequestL(): Window server not connected yet")
        User::Leave(KErrNotReady);
        }
        
    if( aSession )
        {
        if(aOp == KAlfCompOpSetExtent ) 
            {
            TInt* ptr = (TInt*)aClientBuf.Ptr();
            TInt screennumber = ptr[4];
            if(screennumber!=aSession->ScreenNumber())
                {
                aSession->ExtentSurfaceId().iInternal[0] = ptr[5];
                aSession->ExtentSurfaceId().iInternal[1] = ptr[6];
                aSession->ExtentSurfaceId().iInternal[2] = ptr[7];
                aSession->ExtentSurfaceId().iInternal[3] = ptr[8];
                wservsession = WservSession( screennumber);
                }
            }
        else if(aOp == KAlfCompOpSessionClosed)
            {
            FreeCompositionSessionExtents(aSession);
            }
        TInt targetId = reinterpret_cast<TInt>(aSession);
        aClientBuf.Insert(0, TPtrC8((TUint8*)&targetId,sizeof(TInt)));
        } 
    
    if( wservsession ) // don't send to any wservsession if composition source does not have a window anywhere and this is not SetExtent operation
        {
        QueueRequestForSessionL(wservsession, aClientBuf, aOp);
        }
    }

void CAlfStreamerServer::CreatePermissionTokenL(TInt aAlfToken, TInt aWindowHandle, TInt aWindowGroup)
    {
    __ALFLOGSTRING3("CAlfStreamerServer::CreatePermissionTokenL %d, %d, %d >>",aAlfToken,aWindowHandle,aWindowGroup)
    TAlfCompParams param = {aAlfToken, aAlfToken, aWindowHandle, aWindowGroup};
    if (iAlfTargets.Find(param) != KErrNotFound)
        {
        __ALFLOGSTRING("CAlfStreamerServer::CreatePermissionTokenL: Already added")
        return;
        }    
    TPckg<TAlfCompParams> ptr(param);
    HandleCompositionRequestL(0, KAlfCompOpCreateSource, ptr);
    User::LeaveIfError(iAlfTargets.Append(param));
    __ALFLOGSTRING("CAlfStreamerServer::CreatePermissionTokenL <<")
    }

void CAlfStreamerServer::ReleasePermissionTokenL(TInt aAlfToken)
    {
    __ALFLOGSTRING("CAlfStreamerServer::ReleasePermissionTokenL >>")
    RemoveTargetFromInactiveSurfaces(aAlfToken);
    for (TInt i = 0; i < iAlfTargets.Count(); i++)
        {
        if (iAlfTargets[i].iTarget == aAlfToken)
            {
            TPckg<TInt> ptr(aAlfToken);
            HandleCompositionRequestL(0, KAlfCompOpSessionClosed, ptr);        
            iAlfTargets.Remove(i);
            __ALFLOGSTRING("CAlfStreamerServer::ReleasePermissionTokenL: Found <<")
            return;
            }
        }
    __ALFLOGSTRING("CAlfStreamerServer::ReleasePermissionTokenL: Not Found <<")
    }

void CAlfStreamerServer::FormAQueueL(TInt aOp, TInt aSecureId, TInt aWindowGroup)
    {
    __ALFLOGSTRING3("CAlfStreamerServer::FormAQueueL( %d, %d, %d )",aOp, aSecureId, aWindowGroup)
    if (aOp == EAlfVolunteerForGoomTarget)
        {
        TAlfCompParams param = {aSecureId, aSecureId, aWindowGroup, aWindowGroup};
        if (iOptionalGRAM.Find(param) != KErrNotFound)
            {
            __ALFLOGSTRING("CAlfStreamerServer::FormAQueueL: Already added");
            return;
            }    
        User::LeaveIfError(iOptionalGRAM.Append(param));
        }
    else
        {
        for(TInt i = iOptionalGRAM.Count()-1; i >= 0; i-- )
            {
            if ( iOptionalGRAM[i].iWindowGroup == aWindowGroup )
                {
                iOptionalGRAM.Remove(i);
                }       
            }    
        }    
    }
void CAlfStreamerServer::QueueRequestSessionsL(TInt aAlfToken, const TPtrC8& aPtr, TInt aOp)
    {
    __ALFLOGSTRING("CAlfStreamerServer::QueueRequestSessionsL >>")
    for(TInt i = iCompositionSessions.Count()-1; i >= 0; i-- )
        {
        if (iCompositionSessions[i]->AlfToken() == aAlfToken)
            {
            if (iCompositionSessions[i]->IsBgAnimSession() && 
                (aOp == KAlfCompositionTargetHidden || aOp == KAlfCompositionTargetVisible ))
                { // omit
                continue;    
                }        
                    
            __ALFLOGSTRING("CAlfStreamerServer::QueueRequestSessionsL: Session found")
            iCompositionSessions[i]->QueueL(aPtr, aOp);
            }
        }
    __ALFLOGSTRING("CAlfStreamerServer::QueueRequestSessionsL <<")
    }


void CAlfStreamerServer::QueueRequestBGAnimSessionsL(const TPtrC8& aPtr, TInt aOp)
    {
    for(TInt i = iCompositionSessions.Count()-1; i >= 0; i-- )
        {
        if (iCompositionSessions[i]->IsBgAnimSession())
            {
            iCompositionSessions[i]->QueueL(aPtr, aOp);
            }
        }
    }

void CAlfStreamerServer::QueueRequestForSessionL(CAlfStreamerServerSession* aSession, const TPtrC8& aPtr, TInt aOp)
    {
    aSession->QueueL(aPtr, aOp);
    }

void CAlfStreamerServer::QueueRequestAllSessionsL(const TPtrC8& aPtr, TInt aOp, TBool aAlsoWServ)
    {
    if (aAlsoWServ)
        {
        for(TInt i = 0; i < iWindowServerSessions.Count(); i++ )
            { 
            iWindowServerSessions[i]->QueueL(aPtr, aOp);
            }    
        }
        
    for(TInt i = iCompositionSessions.Count()-1; i >= 0; i-- )
        { 
        iCompositionSessions[i]->QueueL(aPtr, aOp);
        }
    }

TInt CAlfStreamerServer::CreatePermissionToken(const RMessage2& aMessage, TInt aTarget, CAlfStreamerServerSession* aSession)
    {
    TInt length = aMessage.GetDesLength(0);
    User::LeaveIfError(length);
    HBufC8* message = HBufC8::NewLC(length);
    TPtr8 clientBuf = message->Des();
    aMessage.ReadL(0,clientBuf);

    TInt* ptr = (TInt*) clientBuf.Ptr();
    TInt newkey = Math::Random();
    
    TRAPD( err, iCompositionTokens.AppendL( TCompositionToken( newkey, ptr[0] /* customer specified key */,
                                                                ptr[1] /*flags*/, aTarget,
                                                                aSession->ScreenNumber() ) ) );
    if (err)
        {
        newkey = 0;
        }
    __ALFLOGSTRING3("CAlfStreamerServer::CreatePermissionToken - newkey %d target: %d, err: %d", newkey, aTarget, err )
            
    CleanupStack::PopAndDestroy(message);
    return newkey; 
    }

void CAlfStreamerServer::ValidateToken( CAlfStreamerServerSession* aSession, const RMessage2& aMessage)
    {
    TInt length = aMessage.GetDesLength(0);
    User::LeaveIfError(length);
    HBufC8* message = HBufC8::NewLC(length + 12);
    TPtr8 clientBuf = message->Des();
    aMessage.ReadL(0,clientBuf);
        
    TInt* ptr = (TInt*) clientBuf.Ptr();
    TInt tokenkey  = ptr[1];
    TInt secret = ptr[2];
    // only AlfStreamer server knows the tokens. Thus it will pass the existing target and attribs to the client
    TInt i = 0;
    while(  i < iCompositionTokens.Count() )
        {
        if ( tokenkey == iCompositionTokens[i].iKey && secret == iCompositionTokens[i].iSecretKey )
            {
            break;
            }
        i++;
        };
    
    if ( i < iCompositionTokens.Count())
        {
        CAlfStreamerServerSession* wservsession = WservSession( iCompositionTokens[i].iScreenNumber );
        if( wservsession )
            {
            aSession->SetScreenNumber( wservsession->ScreenNumber() );
//            CCompositionToken token(iCompositionTokens[i].iKey, iCompositionTokens[i].iSecretKey, iCompositionTokens[i].iFlags, iCompositionTokens[i].iTarget, iCompositionTokens[i].iScreenNumber);
            TInt session = reinterpret_cast<TInt>(aSession);
            clientBuf.Insert(0, TPtrC8((TUint8*)&session ,sizeof(TInt)));
  
            if( iCompositionTokens[i].iCombinedTarget )
                {
                clientBuf.Insert(1 * sizeof(TInt), TPtrC8((TUint8*)&iCompositionTokens[i].iKey ,sizeof(TInt)));
                clientBuf.Insert(2 * sizeof(TInt), TPtrC8((TUint8*)&iCompositionTokens[i].iFlags ,sizeof(TInt)));
                }
            else
                {
                clientBuf.Insert(1 * sizeof(TInt), TPtrC8((TUint8*)&iCompositionTokens[i].iTarget ,sizeof(TInt)));
                clientBuf.Insert(2 * sizeof(TInt), TPtrC8((TUint8*)&iCompositionTokens[i].iFlags ,sizeof(TInt)));
                }
            
            AppendCompositionSessionL(aSession, ETrue);
            // pass the updated buffer to client    
            QueueRequestForSessionL(wservsession, clientBuf, KAlfCompOpBindSourceToToken);
            CleanupStack::PopAndDestroy(); // unefficient..
            aMessage.Complete(KErrNone);
            }
        else
            {
            CleanupStack::PopAndDestroy(); // unefficient..
            aMessage.Complete(KErrNotFound);
            }
        }
    else
        {
        CleanupStack::PopAndDestroy(); // unefficient..
        aMessage.Complete(KErrNotFound); // client will get User::Leave after this.
        }
    }

void CAlfStreamerServer::HandleCompositionEventL(CAlfStreamerServerSession* aSession, TInt aOp, const RMessage2& aMessage)
    {
    if ( aOp == KAlfCompositionWServReady )
        {
        TInt index = iWindowServerSessions.Find(aSession);    
        if (index == KErrNotFound)
            {
            iWindowServerSessions.AppendL(aSession);
            }
        aMessage.Complete(KErrNone);
        return;
        }
    
    if ( aOp == KAlfCompositionLowOnGraphicsMemory ||  aOp == KAlfCompositionGoodOnGraphicsMemory 
		|| aOp == KAlfCompositionTargetHidden ||aOp == KAlfCompositionTargetVisible  || aOp == KAlfCompositionLayoutSwitchComplete)
        {
        aMessage.Complete(KErrNone);
        QueueRequestAllSessionsL(KNullDesC8(), aOp, ETrue);    
        return;
        }        
    
    TInt length = aMessage.GetDesLength(0);
    User::LeaveIfError(length);
    HBufC8* message = HBufC8::NewLC(length);
    TPtr8 clientBuf = message->Des();
    aMessage.ReadL(0,clientBuf);

    TInt* ptr = (TInt*) clientBuf.Ptr();
    CAlfStreamerServerSession* target =  reinterpret_cast<CAlfStreamerServerSession*>(*ptr);

    switch(aOp)
        {
        case KAlfCompositionFrameReady:
            {
            QueueRequestAllSessionsL(clientBuf.Right(4), aOp);    
            break;
            }
        case KAlfCompositionTargetCreated:
            {
            if (!target->MessagePtr().IsNull())
                {
                // create a combined target token
                if(ptr[1] > 0 )
                    {

                    TRAPD( err, iCompositionTokens.AppendL( 
                            TCompositionToken(
                            ptr[1], // new token
                            ptr[2], // secret key
                            0,
                            reinterpret_cast<TInt>(aSession),
                            aSession->ScreenNumber(),
                            ETrue // combined target
                           ) ) );
                    if (err)
                        {
                        ptr[1] = 0;
                        }
  
                    }
                
                target->MessagePtr().Complete(ptr[1]);    
                }
            break;
            }
        default:
            break;
        }
    CleanupStack::PopAndDestroy();
    aMessage.Complete(KErrNone);
    }    


void CAlfStreamerServer::FreeCompositionSessionExtents(CAlfStreamerServerSession* aSession)
    {
    if( !aSession->ExtentSurfaceId().IsNull())
        {
        for(TInt i = 0; i < iWindowServerSessions.Count(); i++)
            { 
            if( aSession->ScreenNumber() != iWindowServerSessions[i]->ScreenNumber() )
                {
                TInt session = reinterpret_cast<TInt>(aSession);
                TInt array[] = {session, 0,0,0,0, // TRect()
                                0, // screennumber does not matter
                                aSession->ExtentSurfaceId().iInternal[0],
                                aSession->ExtentSurfaceId().iInternal[1],
                                aSession->ExtentSurfaceId().iInternal[2],
                                aSession->ExtentSurfaceId().iInternal[3] };  
                TPtrC8 ptr((TUint8*)&array ,sizeof(array));
                QueueRequestForSessionL(iWindowServerSessions[i], ptr, KAlfCompOpSetExtent );
                }
            }
        aSession->ExtentSurfaceId().CreateNullId();
        }
    }

void CAlfStreamerServer::RemoveTargetFromInactiveSurfaces(TInt aTarget)
    {
    for (TInt i = 0; i<iInactiveSurfaces.Count(); i++)
        {
        if (aTarget == iInactiveSurfaces[i].iTarget)
            {
            iInactiveSurfaces.Remove(i);
            break;
            }    
        }  
    }
    
void CAlfStreamerServer::AddTargetFromInactiveSurfaces(TInt aTarget)
    {
    for (TInt i = 0; i<iInactiveSurfaces.Count(); i++)
        {
        if (aTarget == iInactiveSurfaces[i].iTarget)
            {
            return;
            }    
        }

    for (TInt i = 0; i<iAlfTargets.Count(); i++)
        {
        if (aTarget == iAlfTargets[i].iTarget)
            { // ret value ignored intentionally
            iInactiveSurfaces.Append(iAlfTargets[i]);
            break;
            }    
        }
    }
    
void CAlfStreamerServer::GetListOfWGsHavingInactiveSurfacesL(const RMessage2& aMessage, TInt aActiveAlso)
    {
    TBool optionalOnly(aActiveAlso==EAlfVolunteersForCommonGood);  // to lessen ambiquity..
        
    RArray<TAlfCompParams>* arrayPtr = 0;
    switch (aActiveAlso)
        {
        case EAlfInactiveWgs:
            arrayPtr = &iInactiveSurfaces;
            break;
        case EAlfAllWgsWithSurface:
            arrayPtr = &iAlfTargets;
            break;
        case EAlfVolunteersForCommonGood:
        default:
            arrayPtr = &iOptionalGRAM;      
            break;
        }
    
    RArray<TAlfCompParams>& arrayRef =  *arrayPtr; // :)
            
    TInt count = arrayRef.Count();    
    __ALFLOGSTRING1("CAlfStreamerServer::GetListOfWGsHavingInactiveSurfacesL >> count %d", count);
    if ( count == 0)
        {
        aMessage.Complete(KErrNotFound);
        return;
        }
                
    TInt maxLength = aMessage.GetDesMaxLength(0);
    TInt* arrayStart = new (ELeave) TInt [maxLength/4];
    TInt* array = arrayStart;
	
    TInt payloadSize = optionalOnly?8:4; // what a waste
    count = Min(maxLength/payloadSize-1, count);
    
    for ( TInt i = 0; i<count; i++ ) 
        {
        __ALFLOGSTRING1("CAlfStreamerServer::GetListOfWGsHavingInactiveSurfacesL adding %d", arrayRef[i].iWindowGroup);
        *array = arrayRef[i].iWindowGroup;
        array++;
        if (optionalOnly)
            {           
            *array = arrayRef[i].iSecureId;
            array++;
            }
        }
        
    *array = 0;
        
    TPtr8 ptr((TUint8*)arrayStart,maxLength,maxLength);
    aMessage.WriteL(0, ptr);
    delete[] arrayStart;
    aMessage.Complete(KErrNone);
   __ALFLOGSTRING("CAlfStreamerServer::GetListOfWGsHavingInactiveSurfacesL <<")
    }

void CAlfStreamerServer::AddSignal(CAlfStreamerServerSession* aSession, const RMessage2& aMessage)
    {
    TAlfSignal signal = { aSession, aMessage, aMessage.Int0(), aMessage.Int1()};
    if (iSignals.Append(signal))
        {
        aMessage.Complete(KErrNoMemory);
        }    
    }    
    
void CAlfStreamerServer::CompleteSignal(TInt aSignal, TInt aType)
    {
    for (TInt i = iSignals.Count()-1; i >= 0 ; i--)
        {
        if (iSignals[i].iHandle == aSignal && (iSignals[i].iFlags & aType))
            {
            iSignals[i].iMessage.Complete(iSignals[i].iHandle);
            iSignals.Remove(i);
            //break;
            }       
        }  
    }    

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//   
CAlfStreamerServerSession::CAlfStreamerServerSession(const TThreadId& aThreadId) : iScreenNumber(KErrNotFound), iThreadId(aThreadId)
    {
    }

// ---------------------------------------------------------------------------
// destructor of CAlfStreamerServerSession
// ---------------------------------------------------------------------------
//   
CAlfStreamerServerSession::~CAlfStreamerServerSession()
    {
    TInt i = 0;
    RImplInfoPtrArray pluginArray;
    REComSession::ListImplementationsL( KAlfGfxPluginInterfaceUId, pluginArray );    
    for ( i = iLoadedPlugins.Count() - 1; i >= 0; i-- )
        {
        TInt j = 0;    
        for ( j= 0; j < pluginArray.Count(); j++ )
            {
            TUid loaded = TUid::Uid(iLoadedPlugins[i]);
            TUid listed = pluginArray[j]->ImplementationUid();
            TPtrC8 listedopaque = pluginArray[j]->OpaqueData();
            if ( loaded == listed && ( (listedopaque.CompareF( KAlfDoNotUnloadPlugin )) != 0 ) )
                {
                ((CAlfStreamerServer*)(Server()))->WindowMgr()->DestroyPlugin(TUid::Uid(iLoadedPlugins[i]));
                }
            }
        }
    // Clear the data in plugin array
    for (TInt i = pluginArray.Count() - 1; i >= 0; i-- )
        {
        // destroy 
        delete pluginArray[i];
        pluginArray.Remove( i );
        }
    pluginArray.Close();
    iLoadedPlugins.Close();    
            
    CAlfStreamerServer* server = dynamic_cast<CAlfStreamerServer*>((CAlfStreamerServer*)Server()); // nice const cast     
    server->RemoveCompositionSession((CAlfStreamerServerSession*)this); // deja vu   
    server->HandleClientExit(this);
    }


// ---------------------------------------------------------------------------
// ServiceL
// ---------------------------------------------------------------------------
//   
void CAlfStreamerServerSession::ServiceL(const RMessage2& aMessage)
    {
   __ALFLOGSTRING1("CAlfStreamerServerSession::ServiceL %d", aMessage.Function())
   CAlfStreamerServer* server = (CAlfStreamerServer*)( Server() );
   
   TInt op = aMessage.Function();
   
   // handle composition control ops in different function
   if (op >= KAlfCompOpCreateSource)
       {
       HandleCompositionOpL(op, aMessage, server);
       return;
       }
         
   switch(op)
        {
        case EAlfBridgerBlindSend:
        case EAlfBridgerSendChunk:
        case EAlfBridgerRequestDataBlock:
        case EAlfBridgerAsyncronousData:
        case EDsNotifyNativeWindowData:
        case EAlfSynchronize:
            {
            server->WindowTree()->HandleMessageL( aMessage );
            return;
            }
        
        case EAlfDecodSLoadPlugin:
            {
            TInt index = iLoadedPlugins.Find(aMessage.Int0());
            TRAPD( err, server->WindowMgr()->LoadPluginL(TUid::Uid(aMessage.Int0())) );
            // was successfully loaded, add uid to session's array
            if ( err == KErrNone || err == KErrAlreadyExists )
                {
                if ( index == KErrNotFound )
                    {
                    iLoadedPlugins.AppendL(aMessage.Int0());
                    }
                }
            else    
                {
                User::Leave( err );
                }
            break;
            }
        case EAlfDecodSUnloadPlugin:
            {
            TInt index = iLoadedPlugins.Find(aMessage.Int0());
            if  (index != KErrNotFound )
                {
                RImplInfoPtrArray pluginArray;
                REComSession::ListImplementationsL( KAlfGfxPluginInterfaceUId, pluginArray );    
                TInt i = 0;    
                for ( i = 0; i < pluginArray.Count(); i++ )
                    {
                    TUid loaded = TUid::Uid(aMessage.Int0());
                    TUid listed = pluginArray[i]->ImplementationUid();
                    TPtrC8 listedopaque = pluginArray[i]->OpaqueData();
                    if ( loaded == listed && (listedopaque.CompareF( KAlfDoNotUnloadPlugin )) != 0 ) 
                        {
                        iLoadedPlugins.Remove(index);
                        server->WindowMgr()->DestroyPlugin(TUid::Uid(aMessage.Int0()));
                        }
                    }
                // Clear the data in plugin array, it is not going to be used any more.    
                for ( i = pluginArray.Count() - 1; i >= 0; i-- )
                    {
                    // destroy 
                    delete pluginArray[i];
                    pluginArray.Remove( i );
                    }
                pluginArray.Close();
                }        
            break;
            }        

        case EAlfDecodSSendSynch:
        case EAlfDecodSSendAsynch:
            {
            server->WindowMgr()->HandlePluginMsgL(aMessage); // will complete synch message immediately       
            return; // so message won't be completed
            }        

        case EAlfDecodSCancelAsynch:
            {
            server->WindowMgr()->CancelPluginMsg(TUid::Uid(aMessage.Int0()), aMessage.Int1());       
            break;        
            }        
        
        case EAlfDecodSPrepareFrame:
            {
            server->WindowMgr()->PrepareNewFrame(aMessage.Int0());
            break;
            } 
        case EAlfSetScreenRotation:
            {
            TInt rotation = aMessage.Int0();
            aMessage.Complete(KErrNone);    
            server->Bridge()->AddData( EAlfBridgeSetScreenRotation,rotation);
            break;
            } 
        case EAlfGetNativeWindowHandles:
            {
            TPckgC<volatile TAlfNativeWindowData> data(server->Bridge()->iAlfWindowData);
            aMessage.WriteL(0, data);
            break;    
            }

        case EAlfPostDataToCompositionClient:
            {
            TInt clientBufL = aMessage.GetDesLength(2);       
            TPtr8 ptr(0,0);
            HBufC8* buf = 0;
            if (clientBufL > 0)
                { 
                HBufC8::NewLC(aMessage.GetDesLength(2));
                ptr.Set((TUint8*)buf->Ptr(),clientBufL,clientBufL);
                }   
            TInt op = aMessage.Int0();
            TInt key = aMessage.Int1();
            
            aMessage.Complete(KErrNone);
            TRAP_IGNORE(server->QueueRequestSessionsL(key, ptr, op);)
            if (buf)
                {
                CleanupStack::PopAndDestroy();
                }
            break;                
            }
              
        case EAlfPostDataToCompositionTarget:
            {
            TInt clientBufL = aMessage.GetDesLength(2);      
            TInt int2 = aMessage.Int1();
            TPtr8 ptr(0,0);
            HBufC8* buf = 0;
            if (clientBufL > 0)
                { 
                HBufC8::NewLC(clientBufL);
                ptr.Set((TUint8*)buf->Ptr(),clientBufL,clientBufL);
                }
            else
                {
                ptr.Set((TUint8*)&int2, sizeof(TInt),sizeof(TInt));
                }    
            TInt op = aMessage.Int0();
            aMessage.Complete(KErrNone);
            TRAP_IGNORE(server->HandleCompositionRequestL(0, op, ptr);)
            if (buf)
                {
                CleanupStack::PopAndDestroy();
                }
            if (op == KAlfCompositionTargetVisible)
                {
                server->RemoveTargetFromInactiveSurfaces(int2);
                }
            else if (op == KAlfCompositionTargetHidden)
                {
                server->AddTargetFromInactiveSurfaces(int2);
                }                      
            break;                
            }
        case EAlfGetListOfWGsHavingInactiveSurfaces:
            {
            server->GetListOfWGsHavingInactiveSurfacesL(aMessage, aMessage.Int1());
            break;    
            }
        
        case EAlfQueueRequestBGSessions:
            {
            TInt clientBufL = aMessage.GetDesLength(1);       
            TPtr8 ptr(0,0);
            HBufC8* buf = 0;
            if (clientBufL > 0)
                { 
                HBufC8::NewLC(clientBufL);
                ptr.Set((TUint8*)buf->Ptr(),clientBufL,clientBufL);
                }   
            TInt op = aMessage.Int0();    
            aMessage.Complete(KErrNone);
            TRAP_IGNORE(server->QueueRequestBGAnimSessionsL(ptr, op);)
            if (buf)
                {
                CleanupStack::PopAndDestroy();
                }
            break;    
            }
        case EAlfGetNumberOfActiveEffects:
            {
            if (server->Bridge()) // though always there
                {    
                aMessage.Complete(server->Bridge()->iActiveEffectCount);
                }
            break;    
            }
        case EAlfRequestSignal:
            {
            server->AddSignal(this, aMessage);    
            return;
            }
        case EAlfCompleteSignal:
            {
            server->CompleteSignal(aMessage.Int0(), aMessage.Int1());    
            break;    
            }
        case EAlfSetDistractionWindow:
            {
            TInt windowGroupId = aMessage.Int0();
            TInt windowHandle = aMessage.Int1();
            TInt state = (TInt)aMessage.Int2();
            aMessage.Complete(KErrNone);    
            server->Bridge()->AddData( EAlfDSSetDistractionWindow, windowGroupId, windowHandle, (TAny*)state );
            break;
            }
        case EAlfVolunteerForGoomTarget:  
        case EAlfExcludeFromGoomTargets:
            { // all these volunteers, just get into a line..
            server->FormAQueueL(op, aMessage.Int0(), aMessage.Int1());    
            break;    
            }            
            
            
        default:
            {
            aMessage.Complete(KErrNotSupported);
            break;
            }        
        }
    if (!aMessage.IsNull())
        {
        aMessage.Complete(KErrNone);
        }
    __ALFLOGSTRING("CAlfStreamerServerSession::ServiceL exit")
    }

void CAlfStreamerServerSession::HandleCompositionOpL(TInt aOp, const RMessage2& aMessage, CAlfStreamerServer* aServer)
    {
    switch (aOp)
        {
        case KAlfCompOpRequestEvent:
            {
            iIsListeningCompositionEvents = ETrue;    
            if (!CompletedFromQueue(aMessage))
                {
                iMessagePtr = aMessage;
                }
            break;   
            }
            
    case KAlfCompOpCancelEventRequest:
            {
            if (!iMessagePtr.IsNull())
                {
                iMessagePtr.Complete(KErrCancel);
                }
            aMessage.Complete(KErrNone);
            break;   
            }    

    case KAlfCompositionWServReady:
    case KAlfCompositionFrameReady:
    case KAlfCompositionLowOnGraphicsMemory:
    case KAlfCompositionGoodOnGraphicsMemory:    
    case KAlfCompositionTargetHidden:
    case KAlfCompositionTargetVisible:
    case KAlfCompositionTargetCreated:
    case KAlfCompositionLayoutSwitchComplete:
        {
        aServer->HandleCompositionEventL(this, aOp, aMessage);
        break;
        }

    case KAlfCompOpCreateToken:
        {
        TInt token = aServer->CreatePermissionToken( aMessage, reinterpret_cast<TInt>(this), this );
        aMessage.Complete(token);
        break;
        }
    case KAlfCompOpBindSourceToToken:
        {
        aServer->ValidateToken(this, aMessage);        // called method will complete aMessage 
        break;
        }
    case KAlfCompOpSetZOrder:
        iMessagePtr = aMessage; 
    case KAlfCompOpCreateSource:
    case KAlfCompOpEnableAlpha:
    case KAlfCompOpSetOpacity:
    case KAlfCompOpSetRotation: 
    case KAlfCompOpSetExtent:
    case KAlfCompOpEnableKb:
    case KAlfCompOpSessionClosed:
        {
        aServer->HandleCompositionRequestL(this, aOp, aMessage);
        break;
        }
    case KAlfComOpSetBackgroundAnim:
        {
        TBool isBg(EFalse);
        TPckg<TBool> ptr(isBg);
        aMessage.Read(0,ptr);
        iIsBgAnimSession = isBg;
        aServer->HandleCompositionRequestL(this, aOp, aMessage);
        break;
        }

    case KAlfCompositionWServScreenNumber:
        {
        iScreenNumber  = aMessage.Int0();
        aMessage.Complete(KErrNone);
        break;
        }
    case KAlfCompositionSourceScreenNumber:
        {
        TInt length = aMessage.GetDesLength(0);
        User::LeaveIfError(length);
        HBufC8* message = HBufC8::NewLC(length);
        TPtr8 clientBuf = message->Des();
        aMessage.ReadL(0,clientBuf);

        TInt* ptr = (TInt*) clientBuf.Ptr();
        iScreenNumber  = ptr[0];
        
        CleanupStack::PopAndDestroy(message);
        aMessage.Complete(KErrNone);
        break;
        }
    default:        
        // add debug guards or remove
        __ALFLOGSTRING1("Oops, unknown composition command: %d ", aOp)
        User::Invariant();
        }
    }
    
TBool CAlfStreamerServerSession::CompletedFromQueue(const RMessage2& aMessage)
    {
    if (iQueue.Count())
        {
        TRAPD(err, aMessage.WriteL(0,*iQueue[0]/*->Des()*/));
        TInt cmd = iCmds[0];
        if (!err)
            {
            delete iQueue[0];
            iQueue.Remove(0);
            iCmds.Remove(0);
            }
        else
            {
            __ALFLOGSTRING1("CAlfStreamerServerSession::CompletedFromQueue err: %d",err)
            }
        if( !iMessagePtr.IsNull())
            {
            iMessagePtr.Complete(err?err:cmd);
            }
        else
            {
            aMessage.Complete(err?err:cmd);
            }
        return ETrue;
        }
    return EFalse;
    }
    
void CAlfStreamerServerSession::QueueL(const TPtrC8& aPtr, TInt aCmd)
    {
    if (!iIsListeningCompositionEvents)    
        {
        return;
        }
            
    if (!iMessagePtr.IsNull())
        {
        TRAPD(err, iMessagePtr.WriteL(0,aPtr));
        iMessagePtr.Complete(err?err:aCmd);
        }    
    else
        {
        HBufC8* ptr = aPtr.AllocLC();
        TInt err = iCmds.Append(aCmd);
        if (!err)
            {
            iQueue.AppendL(ptr);
            }
        User::LeaveIfError(err);    
        CleanupStack::Pop();    
        }        
    }

void CAlfStreamerServerSession::SetScreenNumber(TInt aScreennumber)
    {
    iScreenNumber = aScreennumber;
    }

TInt CAlfStreamerServerSession::ScreenNumber() const
    {
    return iScreenNumber;
    }

TSurfaceId& CAlfStreamerServerSession::ExtentSurfaceId()
    {
    return iExtentSurfaceId;
    }
// end of file
