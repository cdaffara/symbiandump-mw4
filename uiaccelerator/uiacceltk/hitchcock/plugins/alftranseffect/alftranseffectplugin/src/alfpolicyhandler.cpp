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
* Description:   implementation of policy handler
*
*/



#include "alfpolicyhandler.h"
#include "alfpolicyrequesthandler.h"
#include <alflogger.h>

// ======== HELPER CLASS ========

NONSHARABLE_CLASS(TFullScreenBlock)
    {
public:
    TUid iFromUid;
    TUid iToUid;

    };

// ======== MEMBER FUNCTIONS ========

CPolicyHandler* CPolicyHandler::NewL()
    {
    __ALFFXLOGSTRING("CPolicyHandler::NewL >>");
    CPolicyHandler* self = new (ELeave) CPolicyHandler();
    CleanupStack::PushL(self);
    self->iClientRequestHandler = new (ELeave) CPolicyClientRequestHandler(self);
    CleanupStack::Pop(self);
    __ALFFXLOGSTRING("CPolicyHandler::NewL <<");
    return self;
    }

CPolicyHandler::~CPolicyHandler()
    {
    __ALFFXLOGSTRING("CPolicyHandler::~CPolicyHandler >>");
    delete iClientRequestHandler;

    iFullscreenPolicies.ResetAndDestroy();

    iControlPolicies.ResetAndDestroy();

    iFullScreenBlocks.ResetAndDestroy();

    iListBoxPolicies.ResetAndDestroy();
    __ALFFXLOGSTRING("CPolicyHandler::~CPolicyHandler <<");
    }

CPolicyHandler::CPolicyHandler()
    {
    }

// ---------------------------------------------------------------------------
// Insert the new policy in the list of existing ones.  The list is kept
// sorted according to specificity, ie. how much is specified in the policy.
// ---------------------------------------------------------------------------
//
void CPolicyHandler::RegisterListBoxKmlL( const TUid& aUid,
                                          const TThreadId& aThreadId,
                                          const CAlfTransitionServerClient::TListBoxType aListBoxType,
                                          const TDesC& aResourceDir, 
                                          const TDesC& aBackgroundFilename, 
                                          const TDesC& aItemFilename,
                                          TInt aCachePriority, 
                                          TInt aWantedTime , TInt aMinTime )
    {
    __ALFFXLOGSTRING2("CPolicyHandler::RegisterListBoxKmlL - aUid 0x%x, aThreadId >>", aUid, aThreadId.Id());
    aCachePriority = 1;
    // For now, the cache priority is always 1 at registration time, and after
    // fetching the KML information from the policy, it gets set to 0. This is
    // used to trigger eviction from the KML cache. Newly registered KML:s have
    // priority over existing...
    CListBoxPolicy* policy = CListBoxPolicy::NewL( aUid, 
                                                   aThreadId, 
                                                   aListBoxType,
                                                   aResourceDir, 
                                                   aBackgroundFilename, 
                                                   aItemFilename,
                                                   aCachePriority,
                                                   aWantedTime,
                                                   aMinTime );
    CleanupStack::PushL( policy );
    TInt spec = policy->Specificity();  
    TInt count = iListBoxPolicies.Count();
    TInt uid = aUid.iUid;
    TInt i;
    
    // This keeps the list with ListBox KMLs sorted after specifity, 
    // High specificity is in the beginning of the list and low at the end.
    // This means that when getting a ListBox KML it will find the more specified
    // values first. A Uid counts as more specific than a TListBoxType.
    
    for ( i = 0; i < count; i++ )
        {
        CListBoxPolicy* curr = iListBoxPolicies[i];
        TInt currspec = curr->Specificity();
        
        if ( uid == curr->Uid().iUid && aListBoxType == curr->ListBoxType() )
            {
            // Duplicate policy, delete the old one.
            iListBoxPolicies.Remove( i );
            delete curr;
            break;
            }
        else if ( spec > currspec )
            {
            break;
            }
        }

    iListBoxPolicies.InsertL( policy, i );
    
    CleanupStack::Pop();
    __ALFFXLOGSTRING("CPolicyHandler::RegisterListBoxKmlL <<");
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterListBoxKml( const TUid& aUid,
        CAlfTransitionServerClient::TListBoxType aListBoxType )
    {
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterListBoxKml - Uid: 0x%x >>", aUid);
    for( TInt count = iListBoxPolicies.Count() - 1 ; count >= 0; count-- )
        {
        if( iListBoxPolicies[count]->Uid() == aUid &&
            iListBoxPolicies[count]->ListBoxType() == aListBoxType )
            {
            delete iListBoxPolicies[count];
            iListBoxPolicies.Remove( count );
            __ALFFXLOGSTRING("CPolicyHandler::UnregisterListBoxKml - return KErrNone<<");
            return KErrNone;
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::UnregisterListBoxKml - return KErrNotFound <<");
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterListBoxKml( const TThreadId& aThreadId )
    {
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterListBoxKml - aThreadId: %d >>", aThreadId.Id());
    TThreadId tid;
    TInt err = KErrNotFound;
    
    for( TInt count = iListBoxPolicies.Count() - 1 ; count >= 0; count-- )
        {
        tid = iListBoxPolicies[count]->ThreadId();
        if( tid.Id() == aThreadId.Id() )
            {
            delete iListBoxPolicies[count];
            iListBoxPolicies.Remove( count );
            err = KErrNone;
            }
        }
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterListBoxKml - return: %d <<", err);
    return err;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::GetListBoxKml( const TUid& aAppId,
                                    const CAlfTransitionServerClient::TListBoxType aListBoxType,
                                    TPtrC& aResourceDir,
                                    TPtrC& aBackgroundFilename, 
                                    TPtrC& aItemFilename, 
                                    TInt& aCachePriority, TInt& aWantedTime,
                                    TInt& aMinTime)
    {
    __ALFFXLOGSTRING3("CPolicyHandler::GetListBoxKml - aResourceDir: %S, aBackgroundFilename: %S, aItemFilename: %S >>", &aResourceDir, &aBackgroundFilename, &aItemFilename);
    TInt count = iListBoxPolicies.Count();
    TBool hit = EFalse;
    TInt i;
    aResourceDir.Set( TPtrC() );
    aBackgroundFilename.Set( TPtrC() );
    aItemFilename.Set( TPtrC() );
    
    for ( i = 0; i < count; i++ )
        {
        CListBoxPolicy* curr = iListBoxPolicies[i];
        TInt uid = curr->Uid().iUid;
        CAlfTransitionServerClient::TListBoxType listType = curr->ListBoxType();
        
        if ( ( !uid || uid == aAppId.iUid ) &&
             ( listType == CAlfTransitionServerClient::EListTypeAny ||
               listType == aListBoxType ) )
            {
            hit = ETrue;
            break;
            }
        }

    if ( !hit )
        {
        __ALFFXLOGSTRING("CPolicyHandler::GetListBoxKml - return KErrNotFound <<");
        return KErrNotFound;
        }

    aResourceDir.Set( iListBoxPolicies[i]->ResourceDir() );
    aBackgroundFilename.Set( iListBoxPolicies[i]->BackgroundFilename() );
    aItemFilename.Set( iListBoxPolicies[i]->ItemFilename() );
    aCachePriority = iListBoxPolicies[i]->CachePriority();
    aWantedTime = iListBoxPolicies[i]->WantedTime();
    aMinTime = iListBoxPolicies[i]->MinTime();
        
    // For now, this entry should have no priority over others
    iListBoxPolicies[i]->SetCachePriority( 0 );
    __ALFFXLOGSTRING("CPolicyHandler::GetListBoxKml - return KErrNone <<");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPolicyHandler::RegisterFullscreenKmlL(TUint aAction, const TUid& aUid, const TThreadId& aThreadId,
                                            const TDesC& aResourceDir, const TDesC& aFilename,
                                            TInt /*aCachePriority*/, TInt aWantedTime, TInt aMinTime)
    {
    __ALFFXLOGSTRING3("CPolicyHandler::RegisterFullscreenKmlL - aAction: %d aUid: 0x%x aThread.Id : %d", aAction, aUid, aThreadId.Id() );
    __ALFFXLOGSTRING2("CPolicyHandler::RegisterFullscreenKmlL - aResourceDir: %S, aFilename: %S", &aResourceDir, &aFilename );
    TInt cachePriority = 1; //aCachePriority; 
    // For now, the cache priority is always 1 at registration time, and after
    // fetching the KML information from the policy, it gets set to 0. This is
    // used to trigger eviction from the KML cache. Newly registered KML:s have
    // priority over existing...
    
    CFullscreenPolicy* policy = NULL;
    
    // Check if the action was already is registered and if so, delete it.
    TInt count = iFullscreenPolicies.Count()-1;
    for(; count >= 0; count--)
        {
        if( iFullscreenPolicies[count]->Action() == aAction && 
            iFullscreenPolicies[count]->Uid() == aUid)
            {
            policy = iFullscreenPolicies[count];
            iFullscreenPolicies.Remove(count);
            delete policy;
            policy = NULL;
            }
        } 

    // create and add the new action.
    policy = CFullscreenPolicy::NewL(aAction, aUid, aThreadId, aResourceDir, aFilename,
                                                        cachePriority, aWantedTime, aMinTime);
    CleanupStack::PushL(policy);
    iFullscreenPolicies.AppendL(policy);
    CleanupStack::Pop(policy);
    __ALFFXLOGSTRING("CPolicyHandler::RegisterFullscreenKmlL <<");
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterFullscreenKml(TUint aAction, const TUid& aUid)
    {
    __ALFFXLOGSTRING2("CPolicyHandler::UnregisterFullscreenKml - aAction: %d, aUid: 0x%x <<", aAction, aUid );
    TInt count = iFullscreenPolicies.Count() -1;
    for(; count >= 0; count--)
        {
        if(iFullscreenPolicies[count]->Action() == aAction && 
            iFullscreenPolicies[count]->Uid() == aUid)
            {
            CFullscreenPolicy* policy = iFullscreenPolicies[count];
            iFullscreenPolicies.Remove(count);
            delete policy;
            policy = NULL;
            __ALFFXLOGSTRING("CPolicyHandler::UnregisterFullscreenKml - return KErrNone <<");
            return KErrNone;
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::UnregisterFullscreenKml - return KErrNotFound <<");
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterFullscreenKml(const TThreadId& aThreadId)
    {
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterFullscreenKml - aThreadId: %d >>", aThreadId.Id());
    TInt count = iFullscreenPolicies.Count() -1;
    TInt err = KErrNotFound;
    TThreadId tid;
    
    for(; count >= 0; count--)
        {
        tid = iFullscreenPolicies[count]->ThreadId();
        if( tid.Id() == aThreadId.Id())
            {
            CFullscreenPolicy* policy = iFullscreenPolicies[count];
            iFullscreenPolicies.Remove(count);
            delete policy;
            policy = NULL;
            err = KErrNone;
            }
        }
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterFullscreenKml - return: %d <<", err);
    return err;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterAllKml(const TThreadId& aThreadId)
    {
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterAllKml - aThreadId: <<", aThreadId.Id());
    TInt err = KErrNotFound;
    TInt err1 = UnregisterFullscreenKml(aThreadId);
    TInt err2 = UnregisterControlKml(aThreadId);

    if(err1 == KErrNone || err2 == KErrNone)
        err = KErrNone;


    TInt err3 = UnregisterListBoxKml(aThreadId);
    if(err3 == KErrNone)
        err = KErrNone;
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterAllKml - return: %d <<", err);
    return err; 
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::GetFullscreenKml(  TUint aAction, const TUid& aToUid, const TUid& aFromUid,
                                        TPtrC& aResourceDir, TPtrC& aFilename,
                                        TInt& aCachePriority, TInt& aWantedTime, TInt& aMinTime)
    {
    __ALFFXLOGSTRING3("CPolicyHandler::GetFullscreenKml - aAction: %d, aToUid: %d, aFromUid :%d", aAction, aToUid, aFromUid );
    __ALFFXLOGSTRING2("CPolicyHandler::GetFullscreenKml - aResourceDir: %S, aFilename :%S", &aResourceDir, &aFilename );

    // Checking if the UID is blocked.
    TInt blockedUids = iFullScreenBlocks.Count();
    aResourceDir.Set( TPtrC() );
    aFilename.Set( TPtrC() );
    
    for( TInt i = 0 ;  i < blockedUids ; i++ )
        {
        if( iFullScreenBlocks[i]->iToUid == aToUid ||
            iFullScreenBlocks[i]->iFromUid == aFromUid)
            {
            __ALFFXLOGSTRING("CPolicyHandler::GetFullscreenKml - return KErrNotSupported <<");
            return KErrNotSupported;
            }
        }
        
    TInt count = iFullscreenPolicies.Count();
    // try to find it using both action and uid
    for(TInt i = 0; i < count; i++)
        {
        if(iFullscreenPolicies[i]->Action() == aAction && 
            iFullscreenPolicies[i]->Uid() == aToUid)
            {
            aResourceDir.Set( iFullscreenPolicies[i]->ResourceDir() );
            aFilename.Set( iFullscreenPolicies[i]->Filename() );
            aCachePriority = iFullscreenPolicies[i]->CachePriority();
            aWantedTime = iFullscreenPolicies[i]->WantedTime();
            aMinTime = iFullscreenPolicies[i]->MinTime();
            // Check if theres a block on this UID/Action pair.
            if( aFilename.Length() == 0 ) 
                {
                __ALFFXLOGSTRING("CPolicyHandler::GetFullscreenKml - return KErrNotSupported <<");
                return KErrNotSupported;
                }
                
            // For now, this entry should have no priority over others
            iFullscreenPolicies[i]->SetCachePriority(0);
            __ALFFXLOGSTRING("CPolicyHandler::GetFullscreenKml - return KErrNone <<");
            return KErrNone;
            }
        }
    // try to find the default (Uid == KNullUid)
    for(TInt i = 0; i < count; i++)
        {
        if(iFullscreenPolicies[i]->Action() == aAction && 
            iFullscreenPolicies[i]->Uid() == KNullUid)
            {
            aResourceDir.Set( iFullscreenPolicies[i]->ResourceDir() );
            aFilename.Set( iFullscreenPolicies[i]->Filename() );
            aCachePriority = iFullscreenPolicies[i]->CachePriority();
            aWantedTime = iFullscreenPolicies[i]->WantedTime();
            aMinTime = iFullscreenPolicies[i]->MinTime();
            
            // For now, this entry should have no priority over others
            iFullscreenPolicies[i]->SetCachePriority(0); 
            __ALFFXLOGSTRING("CPolicyHandler::GetFullscreenKml - return KErrNone <<");
            return KErrNone;
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::GetFullscreenKml - return KErrNotFound <<");
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::BlockFullScreenUid(const TUid& aUid, TBool aBlockFrom, TBool aBlockTo)
    {
    __ALFFXLOGSTRING3("CPolicyHandler::BlockFullScreenUid - aUid: 0x%x, aBlockFrom: %d, aBlockTo: %d >>", aUid, aBlockFrom, aBlockTo ); 
    TFullScreenBlock* block = NULL;
    
    TInt blockedUids = iFullScreenBlocks.Count();
    if( blockedUids > 0)
        {
        // Looking for an existing To block.
        if(!aBlockTo && aBlockFrom )
            {
            for(TInt i = 0; i < blockedUids; i++)
                {
                if(iFullScreenBlocks[i]->iFromUid == aUid)
                    {
                    __ALFFXLOGSTRING("CPolicyHandler::BlockFullScreenUid - return KErrNone <<");
                    return KErrNone;
                    }
                }
            }
        // Looking for an existing From block.
        else if(aBlockTo && !aBlockFrom)
            {
            for(TInt i = 0; i < blockedUids; i++)
                {
                if(iFullScreenBlocks[i]->iToUid  == aUid)
                    {
                    __ALFFXLOGSTRING("CPolicyHandler::BlockFullScreenUid - return KErrNone <<");
                    return KErrNone;
                    }
                }
            }
        // Trying to unblock stuff.
        else if(!aBlockTo && !aBlockFrom)
            {
            for(TInt i = blockedUids -1; i >= 0; i--)
                {
                if( iFullScreenBlocks[i]->iToUid  == aUid ||
                    iFullScreenBlocks[i]->iFromUid == aUid )
                    {
                    block = iFullScreenBlocks[i];
                    iFullScreenBlocks.Remove(i);
                    delete block;
                    block = NULL;
                    }
                }
            __ALFFXLOGSTRING("CPolicyHandler::BlockFullScreenUid - return KErrNone <<");
            return KErrNone;
            }
        // Looking for an existing To and From block with the same UID's.
        else
            {
            for(TInt i = 0; i < blockedUids; i++)
                {
                if( iFullScreenBlocks[i]->iToUid  == aUid &&
                    iFullScreenBlocks[i]->iFromUid == aUid )
                    {
                    __ALFFXLOGSTRING("CPolicyHandler::BlockFullScreenUid - return KErrNone <<");
                    return KErrNone;
                    }
                }
            }
        }

    // None found. Create!
    block = new TFullScreenBlock();
    if(NULL == block)
        {
        __ALFFXLOGSTRING("CPolicyHandler::BlockFullScreenUid - return KErrNoMemory <<");
        return KErrNoMemory;
        }

    if(aBlockTo)
        {
        block->iToUid = aUid;
        }
    if(aBlockFrom)
        {
        block->iFromUid = aUid;
        }

    TInt err = iFullScreenBlocks.Append(block);
    if(KErrNone != err)
        {
        delete block;
        block = NULL;
        }
    __ALFFXLOGSTRING1("CPolicyHandler::BlockFullScreenUid - return: %d <<", err);
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::RegisterControlAction(const TUid aUid, TUint aAction, const TDesC& aActionString)
    {
    __ALFFXLOGSTRING3("CPolicyHandler::RegisterControlAction - aUid: 0x%x, aAction: %d, aActionString: %S", aUid, aAction, &aActionString );
    TInt index = 0;
    CControlPolicy* pData = NULL;
    CActionPolicy* pPolicy = NULL;
    
    // TEST!!!!!!!!!!
    // First see if we have an empty entry that means a new KML has been registered
    // Adding just a KML does not remove the old one, the old one gets removed only
    // after we add an action to the new KML
    
    // The double action of first registering a KML file and then registering the action
    // should be replaced with single call where the control file and action are defined
    // in one call
    
    TInt count = iControlPolicies.Count();
    index = -1; // Setting index to negative value to use as check if match found.
    TInt i = 0;
    TInt j = 0;
    TInt k = 0;
    
    // We stop when we find a matching empty entry
    // But first old non-empty entries are deleted
    for( i = count - 1; i >= 0 && index == -1; i-- )
        {
        pData = iControlPolicies[i];
        if( pData->Uid() == aUid )
            {
            if ( pData->iControlPolicies.Count() == 0 )
                {
                index = i;
                // We have found an empty definition.
                // This means a new effect definition file has been added, and the new action
                // should be attached to it.
                // But first we have to remove the possible old effect file
                for ( j = count - 1; j >= 0; j-- )
                    {
                    pData = iControlPolicies[j];
                    if ( pData->Uid() == aUid )
                        {
                        for ( k = 0; pData && k < pData->iControlPolicies.Count(); k++ )
                            {
                            pPolicy = pData->iControlPolicies[k];
                            if ( pPolicy->iAction == aAction )
                                {
                                // Old definition found, remove the whole policy entry
                                iControlPolicies.Remove( j );
                                count--; // this value is referred later. It must be kept up to date.
                                delete pData;
                                pData = NULL;
                                }
                            }
                        }
                    }
                }
            }
        }
        
    if( index >= 0 )
        {
        index = -1; // paranoid...
        // find the empty entry again - if it exists
        // our indexes may have got messed up if we removed something
        // so we must recheck the index
        // We only accept entries with no policies, as we just checked
        // the existence of an entry with matching policy
        for( i = count - 1; i >= 0 && index == -1; i-- )
            {
            pData = iControlPolicies[i];
            if( pData->Uid() == aUid )
                {
                if ( pData->iControlPolicies.Count() == 0 )
                    {
                    index = i;
                    }
                }
            }
        }
// Now we have checked if we have an effect file definition that has no entries yet,
// and if we found one, we deleted old effect file definitions for this UID.
// Now we check if we just update an old action or add a new one to the existing entry      

    // First checking if we need to update a policy.
    // If we have an empty entry, we must use it, not add our action to a different entry
    // even if if has the same UID.
    // So we must not try to find an existing entry if we have already decided to use an
    // empty entry
    
    if ( index < 0 )
        {
        if(FindControlPolicy(aAction, aUid, pPolicy, index) == KErrNone)
            {
            // If we get here and pPolicy == NULL, something is really wrong outside of
            // our control.
            pPolicy->iAction = aAction;
            // Setting the action string is a leaving method.
            // Trap it here to avoid the rest of the system to become
            // leaving to.
            TRAPD(err, pPolicy->SetActionStringL(aActionString));
            if(KErrNone != err)
                {
                __ALFFXLOGSTRING1("CPolicyHandler::RegisterControlAction - return: %d <<", err);
                return err;
                }
            pPolicy->iPolicy = ESupported;
            iClientRequestHandler->RegisterControlAction( aUid, aAction);
            __ALFFXLOGSTRING("CPolicyHandler::RegisterControlAction - return KErrNone <<");
            return KErrNone;
            }
        }
        
    if( index < 0 )
        {
        __ALFFXLOGSTRING("CPolicyHandler::RegisterControlAction - return KErrNotFound <<");
        return KErrNotFound;
        }
        
    pData = iControlPolicies[index];
    pPolicy = new CActionPolicy();
    if(!pPolicy)
        {
        __ALFFXLOGSTRING("CPolicyHandler::RegisterControlAction - return KErrNoMemory <<");
        return KErrNoMemory;
        }
    
    // Assign some values
    pPolicy->iAction = aAction;
    pPolicy->iPolicy = ESupported;
    TRAPD(err, pPolicy->SetActionStringL(aActionString));
    if(KErrNone != err)
        {
        delete pPolicy;
        __ALFFXLOGSTRING1("CPolicyHandler::RegisterControlAction - return: %d <<", err);
        return err;
        }   

    // Then add it to the list.
    err = pData->iControlPolicies.Append(pPolicy);
    if(err != KErrNone)
        {
        delete pPolicy;
        pPolicy = NULL;
        __ALFFXLOGSTRING1("CPolicyHandler::RegisterControlAction - return: %d <<", err);
        return err;
        }
    iClientRequestHandler->RegisterControlAction(aUid, aAction);
    __ALFFXLOGSTRING1("CPolicyHandler::RegisterControlAction - return: %d <<", err);
    return err;
    }
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterControlAction(const TUid aUid, TUint aAction)
    {
    __ALFFXLOGSTRING2("CPolicyHandler::UnregisterControlAction - aUid %d, aAction: %d >>", aUid, aAction);
    CControlPolicy* pData = NULL;
    CActionPolicy* pPolicy = NULL;
        
    TInt count = iControlPolicies.Count();
    
    // Removing policy for action.
    for(TInt counter = 0; counter < count; counter++)
        {
        pData = iControlPolicies[counter];
        if(pData->Uid() == aUid)
            {
            TInt polActionCount = pData->iControlPolicies.Count();
            // Iterating the list from first. No problem since we only delete one item!
            for(TInt j = 0; j < polActionCount; j++)
                {
                pPolicy = pData->iControlPolicies[j];
                if(pPolicy->iAction == aAction)
                    {
                    // Call this before removing data permanently!
                    iClientRequestHandler->RemoveControlPolicy( aUid, aAction);
                    delete pPolicy;
                    pPolicy = NULL;
                    pData->iControlPolicies.Remove(j);
                    __ALFFXLOGSTRING("CPolicyHandler::UnregisterControlAction - return KErrNone<<");
                    return KErrNone;
                    }
                }
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::UnregisterControlAction - return KErrNotFound<<");
    return KErrNotFound;
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPolicyHandler::RegisterControlKmlL(const TUid &aUid, const TThreadId &aThreadId, 
                                         const TDesC& aResourceDir, const TDesC& aFilename,
                                         const TInt aWantedTime, const TInt aMinTime)
    {
    
    __ALFFXLOGSTRING2("CPolicyHandler::RegisterControlKmlL - aUid :%d aThread.Id :%d", aUid, aThreadId.Id() );
   __ALFFXLOGSTRING2("CPolicyHandler::RegisterControlKmlL - aResourceDir: %S, aFilename :%S", &aResourceDir, &aFilename );

    CControlPolicy* policy = NULL;
    TInt cachePriority = 1; //aCachePriority; 
    // For now, the cache priority is always 1 at registration time, and after
    // fetching the KML information from the policy, it gets set to 0. This is
    // used to trigger eviction from the KML cache. Newly registered KML:s have
    // priority over existing...

    // Check if the action was already is registered and if so, delete it.
    
// TEST!!!!!!!!!!!! Don't delete before action is added 
// Only delete if the entry has no policies, then we are replacing the xml file
// though we have added no actions yet
    TInt count = iControlPolicies.Count()-1;
    for(; count >= 0; count--)
        {
        if( iControlPolicies[count]->Uid() == aUid)
            {
            policy = iControlPolicies[count];
            if ( policy->iControlPolicies.Count() == 0 )
                {
                iControlPolicies.Remove(count);
                delete policy;
                policy = NULL;
                }
            }
        } 

    // Create the new action and add it.
    policy = CControlPolicy::NewL(aUid, aThreadId, aResourceDir, aFilename, cachePriority, aWantedTime, aMinTime);
    CleanupStack::PushL(policy);
    iControlPolicies.AppendL(policy);
    CleanupStack::Pop(policy);
    __ALFFXLOGSTRING("CPolicyHandler::RegisterControlKmlL <<");
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::RemoveControlKml(const TUid &aUid)
    {
    __ALFFXLOGSTRING1("CPolicyHandler::RemoveControlKml - aUid: %d ", aUid);
    
    TInt err = KErrNotFound;
    CControlPolicy* ptr = NULL;
    CActionPolicy* action = NULL;
    TInt policyCount = iControlPolicies.Count();
    // We may remove several KMLs if different actions have different effect files
    for( TInt i = policyCount - 1; i >= 0; i--)
        {
        ptr = iControlPolicies[i];
        if(ptr->Uid() == aUid)
            {
            // making sure that policies get removed from the running clients as well!
            TInt count = ptr->iControlPolicies.Count();
            if( count > 0)
                {
                for(TInt j = 0; j < count; j++)
                    {
                    action = ptr->iControlPolicies[j];
                    iClientRequestHandler->RemoveControlPolicy(aUid, action->iAction);
                    }
                }
            iControlPolicies.Remove(i);
            delete ptr;
            ptr = NULL;
            err = KErrNone;
            }
        }
    __ALFFXLOGSTRING1("CPolicyHandler::RemoveControlKml - return: %d <<", err);
    return err;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::UnregisterControlKml(const TThreadId &aThreadId)
    {
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterControlKml - aThreadId: %d ", aThreadId.Id() );
    CControlPolicy* ptr = NULL;
    CActionPolicy* action = NULL;
    TInt err = KErrNotFound;
    TThreadId tid;
    TInt policyCount = iControlPolicies.Count();
    
    for( TInt i = policyCount - 1; i >= 0; i--)
        {
        ptr = iControlPolicies[i];
        tid = ptr->ThreadId();
        
        if(tid.Id() == aThreadId.Id())
            {
            // making sure that policies get removed from the running clients as well!
            TInt count = ptr->iControlPolicies.Count();
            if(count > 0)
                {
                for(TInt j = 0; j < count; j++)
                    {
                    action = ptr->iControlPolicies[j];
                    iClientRequestHandler->RemoveControlPolicy(ptr->Uid(), action->iAction);
                    }
                }
            iControlPolicies.Remove(i);
            delete ptr;
            ptr = NULL;
            err = KErrNone;
            }
        }
    __ALFFXLOGSTRING1("CPolicyHandler::UnregisterControlKml - return: %d <<", err);
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::GetControlKml( TUid aUid, TUint aAction, TPtrC& aResourceDir, 
                    TPtrC& aFileName, TPtrC& aActionString, TInt& aCachePriority,
                    TInt& aWantedTime, TInt& aMinTime)
    {
    __ALFFXLOGSTRING2("CPolicyHandler::GetControlKml - aAction: %d aUid :0x%x", aAction, aUid );
    __ALFFXLOGSTRING3("CPolicyHandler::GetControlKml - aResourceDir: %S, aFilename :%S, aActionString: %S", &aResourceDir, &aFileName, &aActionString );
        
    CControlPolicy* ptr = NULL;
    CActionPolicy* pPolicy = NULL;
    aResourceDir.Set( TPtrC() );
    aFileName.Set( TPtrC() );
    aActionString.Set( TPtrC() );

    TInt itemCount = iControlPolicies.Count();
    for(TInt i = 0; i < itemCount; i++)
        {
        ptr = iControlPolicies[i];
        if( ptr->Uid() == aUid)
            {
            aResourceDir.Set( ptr->ResourceDir() );
            aFileName.Set( ptr->Filename() );
            aCachePriority = ptr->CachePriority();
            aWantedTime = ptr->WantedTime();
            aMinTime = ptr->MinTime();
            TInt actionCount = ptr->iControlPolicies.Count();
            for(TInt j = 0; j < actionCount; j++)
                {
                pPolicy = ptr->iControlPolicies[j];
                if(pPolicy->iAction == aAction)
                    {
                    aActionString.Set( pPolicy->ActionString() );
                    // For now, this entry should have no priority over others
                    ptr->SetCachePriority(0);
                    __ALFFXLOGSTRING("CPolicyHandler::GetControlKml - return KErrNone <<");
                    return KErrNone;
                    }
                }
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::GetControlKml - return KErrNotFound <<");
    return KErrNotFound;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::GetAllControlPolicies(RPointerArray<CClientControlPolicy>& aControlPolicies)
    {
    __ALFFXLOGSTRING("CPolicyHandler::GetAllControlPolicies >>");
    TInt err = KErrNone;
    
    TInt itemCount = iControlPolicies.Count();
    if( 0 >= itemCount)
        {
        __ALFFXLOGSTRING("CPolicyHandler::GetAllControlPolicies - return KErrNotFound <<");
        return KErrNotFound;
        }

    for(TInt i = 0; i < itemCount; i++)
        {
        CClientControlPolicy* ptr = new CClientControlPolicy();
        if ( ptr == NULL )
            {
            __ALFFXLOGSTRING("CPolicyHandler::GetAllControlPolicies - return KErrNoMemory <<");
            return KErrNoMemory;
            }
        ptr->iUid = iControlPolicies[i]->Uid();
        HBufC* fname = HBufC::New(iControlPolicies[i]->Filename().Length());
        if ( fname == NULL )
            {
            delete ptr;
            __ALFFXLOGSTRING("CPolicyHandler::GetAllControlPolicies - return KErrNoMemory <<");
            return KErrNoMemory;
            }
        fname->Des().Copy(iControlPolicies[i]->Filename());
        ptr->iFilename.Assign(fname);
        HBufC* resdir = HBufC::New(255);
        if ( resdir == NULL )
            {
            delete ptr;
            __ALFFXLOGSTRING("CPolicyHandler::GetAllControlPolicies - return KErrNoMemory <<");
            return KErrNoMemory;
            }
        ptr->iResourceDir.Assign(resdir);
        // In order to actually copy the pointers to the policy data, we
        // need to iterate through the entire list. 
        TInt count = iControlPolicies[i]->iControlPolicies.Count();
        if(count > 0)
            {
            for( TInt j = 0; j < count; j++)
                {
                err = ptr->iControlPolicies.Append(iControlPolicies[i]->iControlPolicies[j]);
                if(KErrNone != err)
                    {
                    delete ptr;
                    __ALFFXLOGSTRING1("CPolicyHandler::GetAllControlPolicies - return: %d <<", err);
                    return err;
                    }
                }
            err = aControlPolicies.Append(ptr); 
            if(KErrNone != err)
                {
                delete ptr;
                __ALFFXLOGSTRING1("CPolicyHandler::GetAllControlPolicies - return: %d <<", err);
                return err;
                }
            }
        else
            {
            delete ptr;
            ptr = NULL;
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::GetAllControlPolicies - return KErrNotFound<<");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::RequestPolicy(const RMessage2& aMessage)
    {
    return iClientRequestHandler->RequestPolicy(aMessage);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::GetPolicyL( TThreadId aClientId, TInt aPolicyCount, TPtr8& aOutBuf )
    {
    return iClientRequestHandler->GetPolicyL( aClientId, aPolicyCount, aOutBuf );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPolicyHandler::RemoveClient( TThreadId aClientId )
    {
    iClientRequestHandler->RemoveClient( aClientId );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
/*
void CPolicyHandler::RemoveDeadClients()
    {
    iClientRequestHandler->RemoveDeadClients();
    }
*/  

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
/*
void CPolicyHandler::DumpClientsL(RFileWriteStream& aFile)
    {
    iClientRequestHandler->DumpClientsL(aFile);
    }
*/  
    

//==============    PRIVATE METHODS =================/

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyHandler::FindControlPolicy(TUint aAction, TUid aUid, CActionPolicy*& aPolicy, TInt& aIndex)
    {
    __ALFFXLOGSTRING3("CPolicyHandler::FindControlPolicy - aAction: %d, aUid: 0x%x, aIndex: %d", aAction, aUid, aIndex );
    
    TInt count = iControlPolicies.Count();
    aIndex = -1;    // Setting index to low value to avoid accidental insertion.
    
    CControlPolicy* pData = NULL;
    for(TInt i = 0; i < count; i++)
        {
        pData = iControlPolicies[i];
        if(pData->Uid() == aUid)
            {
            aIndex = i;
            TInt j = pData->iControlPolicies.Count();
            CActionPolicy* pPolicy = NULL;
            for(TInt q = 0; q < j; q++)
                {
                pPolicy = pData->iControlPolicies[q];
                if(pPolicy->iAction == aAction)
                    {
                    aPolicy = pPolicy;
                    __ALFFXLOGSTRING("CPolicyHandler::FindControlPolicy - return KErrNone <<");
                    return KErrNone;
                    }
                }
            }
        }
    __ALFFXLOGSTRING("CPolicyHandler::FindControlPolicy - return KErrNotFound <<");
    return KErrNotFound;
    }


// --- EOF ---




