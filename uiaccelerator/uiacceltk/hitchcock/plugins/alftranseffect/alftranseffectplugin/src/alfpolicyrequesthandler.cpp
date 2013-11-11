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
* Description:   Contains a class that is responsible for sending policy updates 
*                to connected clients
*
*/



#include <e32std.h>
#include <s32mem.h>

//======== USER INCLUDES ========
#include "alfpolicyrequesthandler.h"
#include "alfpolicyhandler.h"
//#include "tfxserverdef.h"
#include <alflogger.h>


// ======== HELPER CLASS FUNCTIONS ========

CPolicyEventReq::CPolicyEventReq()
    {
    }

// ---------------------------------------------------------------------------
// Destructor for CPolicyEventReq
// ---------------------------------------------------------------------------
//
CPolicyEventReq::~CPolicyEventReq()
	{
	iOriginalControlPolicies.ResetAndDestroy();
	iAddedPolicies.ResetAndDestroy();
	iRemovedPolicies.ResetAndDestroy();

	if(!iMessage.IsNull() && iMessage.Handle() != 0)	// We should release a a stored message. 
		{
        __ALFFXLOGSTRING("CPolicyEventReq:: Message complete, Disconnected");
		iMessage.Complete( KErrDisconnected );
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TUint CPolicyEventReq::TotalPolicyCount() const
    {
    TUint count = 0;
    for ( TInt i = iOriginalControlPolicies.Count() - 1; i >= 0; --i )
        {
        count += iOriginalControlPolicies[i]->iControlPolicies.Count();
        }
    count += iAddedPolicies.Count();
    count += iRemovedPolicies.Count();
    return count;
    }

// ---------------------------------------------------------------------------
// Constructor for CClientControlPolicy
// ---------------------------------------------------------------------------
//
CClientControlPolicy::CClientControlPolicy()
	{
	}

	
// ---------------------------------------------------------------------------
// Destructor for CClientControlPolicy
// ---------------------------------------------------------------------------
//
CClientControlPolicy::~CClientControlPolicy()
	{
	
	for( TInt i =  iControlPolicies.Count() - 1; i >= 0; i--)
		{
		iControlPolicies.Remove(i);
		}
		
	iControlPolicies.Close();
	iResourceDir.Close();
	iFilename.Close();
	}

	
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor. 
// ---------------------------------------------------------------------------
//
CPolicyClientRequestHandler::CPolicyClientRequestHandler(CPolicyHandler* aParent) :
		 iParent(aParent)
	{
	}
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPolicyClientRequestHandler::~CPolicyClientRequestHandler()
	{
    RemoveDeadClients();
	iControlPolicyClients.ResetAndDestroy();
	}
	
	
// ---------------------------------------------------------------------------
// Iterates over all connected clients and appends a policy to each if necessary.
// ---------------------------------------------------------------------------
//
void CPolicyClientRequestHandler::RegisterControlAction(const TUid aUid, TUint aAction)
	{
	CPolicyEventReq* pPolReq = NULL;
	
	// Look for existing clients. If none found, all is OK.
	TInt contrCount = iControlPolicyClients.Count();
	// We have some connected clients. Update the new policies for all of these.
	for(TInt i = 0; i < contrCount; i++)
		{
		pPolReq = iControlPolicyClients[i];
		// Checking if we already have stored the policy. Since we only have a copy of the pointer
		// to the original object we need not update it. It has already been done in the policyhandler.
		if(KErrNone == CheckStoredControlPolicy(pPolReq->iOriginalControlPolicies, aUid, aAction))
			{
			return;
			}
		// If it's not in the original list, see if we have it somewhere else. If so, complete
		// the stored away message if possible.
		if(KErrNone == AddControlPolicyAction(pPolReq->iAddedPolicies, pPolReq->iRemovedPolicies, aUid, aAction))
			{
			if(!pPolReq->iMessage.IsNull())
				{
                __ALFFXLOGSTRING("CPolicyEventReq:: Message complete, Register control action");
				pPolReq->iMessage.Complete(KErrNone);
				pPolReq->iMessage = RMessagePtr2();
				}
			}
		}
	}
	

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::RemoveControlPolicy(const TUid aUid, TUint aAction )
	{
	
	TInt clientCount = iControlPolicyClients.Count();
	TInt err = KErrNone;
	// Lint will complain because the tPol pointer is not deleted here.
	// The pointer is stored into a pointerarray and deleted when the array is destroyed.
	// This is the reason why this warning/error is disabled.
	/*lint -e429  -e830 */

	for(TInt i = 0; i < clientCount; i++)
		{
		if( KErrNotFound == RemovePolicyUid(iControlPolicyClients[i]->iOriginalControlPolicies, 
											aAction, aUid) 	&& 
			KErrNotFound == RemovePolicyUid(iControlPolicyClients[i]->iAddedPolicies, aAction, aUid))
			{
			TControlPolicy* tPol = new TControlPolicy;
			if(NULL == tPol)
				{
				return KErrNoMemory;
				}
			tPol->iAction = aAction;
			tPol->iId = aUid;
			tPol->iPolicy = ENotSupported;
			
			err = iControlPolicyClients[i]->iRemovedPolicies.Append(tPol); //Takes ownership
			if( KErrNone != err)
				{
				delete tPol;
				tPol = NULL;
				return err;
				}
				
			if(!iControlPolicyClients[i]->iMessage.IsNull())
				{
                __ALFFXLOGSTRING("CPolicyEventReq:: Message complete, Remove control policy");
				iControlPolicyClients[i]->iMessage.Complete(KErrNone);
				iControlPolicyClients[i]->iMessage = RMessagePtr2();
				}
			}
		} 
	/*lint +e429  +e830 */
	return KErrNone;
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::RequestPolicy( const RMessage2& aMessage )
	{
	TInt err = KErrNone;
	CPolicyEventReq* req = NULL;
	
	RThread client;
	err = aMessage.Client( client );
	if( err != KErrNone )
		{
		client.Close();
		return err;
		}
		
	TThreadId currClient = client.Id();
	client.Close();
	
	TInt clientCount = iControlPolicyClients.Count();
	for( TInt i = 0; i < clientCount; i++ )
		{
		req = iControlPolicyClients[i];
		if(req->iClientId == currClient)
			{
			if(	req->iAddedPolicies.Count() == 0 && req->iOriginalControlPolicies.Count() == 0
				&& req->iRemovedPolicies.Count() == 0)
				{
				// Check if it's OK to add this request to the list.
				// This is done by just storing the message. 
				if( req->iMessage.IsNull() )
					{
					req->iMessage = aMessage;
					return KErrNone;
					}
				// If message is not null, something is wrong.
				return KErrGeneral;
				}
			// If we have a stored message, complete it. Should not happen!
			if( !req->iMessage.IsNull() )
				{
                __ALFFXLOGSTRING("CPolicyEventReq:: Message complete, stored message, should not happen");
				req->iMessage.Complete( KErrNone );
	    		// NULLify the message pointer after it has been completed.
	    		// If not done, we can't check if the message is valid or not.
				req->iMessage = RMessagePtr2();
				return KErrNone;
				}
            // Otherwise complete this message.
            __ALFFXLOGSTRING("CPolicyEventReq:: Message complete, return policy count");
            aMessage.Complete( req->TotalPolicyCount() );
			return KErrNone;
			}
		}
	// Client not found. Insert it!
	err = InsertClient( aMessage );
	if( err != KErrNotFound )	
		{
		// If failed to insert the client, complete the message so it may try again.
		// Also, if there are control policies, we report the number of them
		// If error is KErrNotFound, we have stored the message, and it will be completed
		// later (as soon as there are policies)
        __ALFFXLOGSTRING1("CPolicyEventReq:: insert client returned %d", err);
		aMessage.Complete( err );
		}
	return KErrNone;
	}
	
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::GetPolicyL( TThreadId aClientId, TInt aPolicyCount, TPtr8& aOutBuf )
    {
    CPolicyEventReq* 		req 	= NULL;
    TControlPolicy* 		tAddPol	= NULL;
    CActionPolicy*			tPol 	= NULL;
    CClientControlPolicy*	tOUid 	= NULL;	
    TControlPolicy* 		tRemPol = NULL;
        
    TThreadId id = aClientId;
    TInt count = 0;
    RDesWriteStream outstream( aOutBuf );
    CleanupClosePushL( outstream );
    
    for( TInt i = iControlPolicyClients.Count() - 1; i >= 0; --i )
        {
        req = iControlPolicyClients[i];
        
        if ( req->iClientId == id )
            {
            int added = 0;

            count = req->TotalPolicyCount();
            if ( count > aPolicyCount )
                {
                count = aPolicyCount;
                }
            outstream.WriteUint32L( count );

            TInt addedPolicyCount       = req->iAddedPolicies.Count();
            TInt origPolicyCount        = req->iOriginalControlPolicies.Count();
            TInt removedPoliciesCount   = req->iRemovedPolicies.Count();

            for ( TInt q = origPolicyCount - 1; q >= 0 && added < count; --q )
                {
                tOUid = req->iOriginalControlPolicies[q];
                TInt origPolicyInfoCount = tOUid->iControlPolicies.Count();
                for ( TInt qi = origPolicyInfoCount - 1; qi >= 0 && added < count; --qi )
                    {
                    tPol = tOUid->iControlPolicies[qi];
                    outstream.WriteInt32L( tOUid->iUid.iUid );
                    outstream.WriteInt32L( tPol->iAction );
                    outstream.WriteUint8L( tPol->iPolicy );
                    added++;

                    // Here we only remove the policy from the local list of original policies.
                    // we don't delete it since it's a copy of the pointer to the
                    // original object and we don't want to remove it permanently!
                    tOUid->iControlPolicies.Remove( qi );
                    }
                if ( tOUid->iControlPolicies.Count() == 0 )
                    {
                    // It's OK to delete tOUid. It doesn't destroy anything except itself!
                    req->iOriginalControlPolicies.Remove(q);
                    delete tOUid;
                    }
                }

            for ( TInt z = addedPolicyCount - 1; z >= 0 && added < count; --z )
                {
                tAddPol = req->iAddedPolicies[z];
                outstream.WriteInt32L( tAddPol->iId.iUid );
                outstream.WriteInt32L( tAddPol->iAction );
                outstream.WriteUint8L( tAddPol->iPolicy );
                  added++;

                // Here we delete the actual object. This is possible since
                // it has been created specifically for this client.
                delete tAddPol;
                req->iAddedPolicies.Remove( z );
                }
            for(TInt s = removedPoliciesCount - 1; s >= 0 && added < count; s--)
                {
                tRemPol = req->iRemovedPolicies[s];
                outstream.WriteInt32L( tRemPol->iId.iUid );
                outstream.WriteInt32L( tRemPol->iAction );
                outstream.WriteUint8L( tRemPol->iPolicy );
                   added++;
                // Here we delete the actual object. This is possible since
                // it has been created specifically for this client.
                delete tRemPol;
                req->iRemovedPolicies.Remove(s);
                }
            outstream.CommitL();
            CleanupStack::PopAndDestroy(); // outstream
            
            return KErrNone;
            }
        }
        
    outstream.WriteUint32L( 0 );
    outstream.CommitL();
    CleanupStack::PopAndDestroy(); // outstream
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPolicyClientRequestHandler::RemoveClient( TThreadId aClientId )
	{
	for( TInt i = iControlPolicyClients.Count() - 1; i >= 0; i--)
		{
		if( iControlPolicyClients[i]->iClientId == aClientId )
			{
			if(!iControlPolicyClients[i]->iMessage.IsNull())
			    {
                __ALFFXLOGSTRING("CPolicyEventReq:: Message complete, remove client");
    			iControlPolicyClients[i]->iMessage.Complete( KErrCancel );
                iControlPolicyClients[i]->iMessage = RMessage2(); //Null message so destructor wont try to complete.
			    }
   			delete iControlPolicyClients[i];
			iControlPolicyClients.Remove(i);
			}
		}
	}

// ---------------------------------------------------------------------------
// Removes all requests belonging to a terminated client.
// ---------------------------------------------------------------------------
//
void CPolicyClientRequestHandler::RemoveDeadClients()
	{
	// Lint seems to have some trouble identifying the TThreadId class.
	// Due to this, Lint will report several errors here that are incorrect.
	// These errors are disabled.
	/*lint -e10 -e1013 -e1055 -e746 -e747 */
	RThread client;
	for( TInt i = iControlPolicyClients.Count() - 1; i >= 0; i--)
		{
		TInt err = client.Open(iControlPolicyClients[i]->iClientId.Id());
		if(err == KErrNone)
			{
			if(client.ExitType() != EExitPending)
				{
				//Client is dead, remove from list.
				iControlPolicyClients[i]->iMessage = RMessage2(); //Null message so destructor wont try to complete.
				delete iControlPolicyClients[i];
				iControlPolicyClients.Remove(i);
				}
		    client.Close();
			}
		else
			{
			//Could not open client, client handle was invalid.
			//So clean it up from list too.
			iControlPolicyClients[i]->iMessage = RMessage2(); //Null message so destructor wont try to complete.
			delete iControlPolicyClients[i];
			iControlPolicyClients.Remove(i);
			}
		}
	/*lint +e10 +e1013 +e1055 +e746 +e747 */
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPolicyClientRequestHandler::DumpClientsL(RFileWriteStream& aFile)
	{
	// Lint seems to have some trouble identifying the TThreadId class.
	// Due to this, Lint will report several errors here that are incorrect.
	// These errors are disabled.
	/*lint -e10 -e1013 -e1055 -e746 -e747 */
	for(TInt i = iControlPolicyClients.Count() - 1; i >= 0; i--)
		{
		TPckgC<TThreadId> clientid(iControlPolicyClients[i]->iClientId.Id());
		aFile.WriteL(clientid);
		}
	/*lint +e10 +e1013 +e1055 +e746 +e747 */
	}

	
// =================== PRIVATE METHODS ==============

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::InsertClient(const RMessage2& aMessage)
	{
	
	TInt err = KErrNone;
	
	CPolicyEventReq* req = new CPolicyEventReq();
	if (!req)
		{
		return KErrNoMemory;
		}

	// Getting the client ID and storing it.	
	RThread client;
	err = aMessage.Client(client);
	if( err != KErrNone )
		{
		delete req;
		return err;
		}
	req->iClientId = client.Id();
	client.Close();
	
    err = iControlPolicyClients.Append(req);
    if( err == KErrNone )
    	{
		// Requesting all policies from the policy handler.
        err = iParent->GetAllControlPolicies( req->iOriginalControlPolicies );
		if( err == KErrNotFound )
			{
			// If no polices found, store message for later.
			req->iMessage = aMessage;
			return err;
			} 
        err = req->TotalPolicyCount();
    	}
    else
        {
        delete req;
        }
    return err;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::AddControlPolicyAction(	RPointerArray<TControlPolicy>& aAddedPolicies,
															RPointerArray<TControlPolicy>& aRemovedPolicies,
															const TUid aUid, TUint aAction)
	{
	
	
	// Lint will complain because the tPol pointer is not deleted here.
	// The pointer is stored into a pointerarray and deleted when the array is destroyed.
	// This is the reason why this warning/error is disabled.
	/*lint -e429 -e830 */
		
	TControlPolicy* tPol = NULL;
	TInt err = KErrNone;
	
	// Checking if we have any newly added policies.
	TInt polUidCount = aAddedPolicies.Count();
	for(TInt i = 0; i < polUidCount; i++)
		{
		tPol = aAddedPolicies[i];
		if(tPol->iId == aUid && tPol->iAction == aAction)
			{
			return KErrNone;
			}
		}
	// Checking if this policy has been set to be removed.
	polUidCount = aRemovedPolicies.Count();
	for(TInt i = 0; i < polUidCount; i++)
		{
		tPol = aRemovedPolicies[i];
		if(tPol->iId == aUid && tPol->iAction == aAction)
			{
			aRemovedPolicies.Remove(i);
			delete tPol;
			tPol = NULL;
			return KErrNone;
			}
		}
	
	// No policy found. Adding a new one.
	// For some reason Lint doesn't like this if() clause. Perhaps Lint doesn't
	// understand that new can return NULL. Disabling the "if always evaluates to False"
	// error message.
	/*lint -e774 -e831 */
	tPol = NULL;
	tPol = new TControlPolicy;
	if(NULL == tPol)
		{
		return KErrNoMemory;
		}
	/*lint +e774 +e831 */

	tPol->iAction = aAction;
	tPol->iId = aUid;
	tPol->iPolicy = ESupported;

	err = aAddedPolicies.Append(tPol);
	if(KErrNone == err)
		{
		return err;
		}

	// Clenup on failure
	delete tPol;
	tPol = NULL;
	return err;			

	/*lint +e429 +e830 */	
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::CheckStoredControlPolicy(	RPointerArray<CClientControlPolicy>& aPtr, 
															TUid aUid, TUint aAction)
	{
	
	TInt polUidCount = aPtr.Count();
	for(TInt i = 0; i < polUidCount; i++)
		{
		if(aPtr[i]->iUid == aUid)
			{
			TInt actionCount = aPtr[i]->iControlPolicies.Count();
			for(TInt j = 0; j < actionCount; j++)
				{
				if(aPtr[i]->iControlPolicies[j]->iAction == aAction)
					{
					return KErrNone;
					}
				}
			}
		}
	return KErrNotFound;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
// 

TInt CPolicyClientRequestHandler::RemovePolicyUid(RPointerArray<TControlPolicy>& aAddedPolicies, TUint aAction, const TUid aUid)
	{
	TControlPolicy* tUid = NULL;

	TInt policyCount = aAddedPolicies.Count();
	for(TInt i = 0; i < policyCount; i++)
		{
		tUid = aAddedPolicies[i];
		if(tUid->iId == aUid && tUid->iAction == aAction)
			{
			delete tUid;
			tUid = NULL;
			aAddedPolicies.Remove(i);
			return KErrNone;
			}
		} 
	return KErrNotFound;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::RemovePolicyUid(RPointerArray<CClientControlPolicy>& aPolUid, TUint aAction, const TUid aUid)
	{
	CClientControlPolicy* tUid = NULL;
	
	TInt policyCount = aPolUid.Count();
	for(TInt i = 0; i < policyCount; i++)
		{
		tUid = aPolUid[i];
		if(tUid->iUid == aUid)
			{
			TInt err = RemovePolicyInfo(tUid->iControlPolicies, aAction, EFalse);
			if ( err == KErrNone)
				{
				if(tUid->iControlPolicies.Count() == 0)
					{
					delete tUid;
					tUid = NULL;
					aPolUid.Remove(i);
					}
				return err;
				}
			}
		}
	return KErrNotFound;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CPolicyClientRequestHandler::RemovePolicyInfo(RPointerArray<CActionPolicy>& aPolInfo, TUint aAction, TBool aDelete)
	{
	CActionPolicy* tPol = NULL;
	TInt polInfoCount = aPolInfo.Count();
	
	for(TInt i = 0; i < polInfoCount; i++)
		{
		tPol = aPolInfo[i];
		if(tPol->iAction == aAction)
			{
			if(aDelete)
				{
				delete tPol;
				tPol = NULL;
				}
			aPolInfo.Remove(i);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}



// --- EOF ---


