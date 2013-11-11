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
* Description:   Handles sending policy updates to connected clients.
*
*/


#ifndef __ALFPOLICYREQUESTHANDLER
#define __ALFPOLICYREQUESTHANDLER

//======= System includes =======
#include <e32cmn.h>
#include <e32base.h>
#include <s32file.h>

//======= User includes =======
#include "alfpolicy.h"
//#include "tfxserverclient.h"

//======= Forward declarations =======
class CPolicyHandler;
class CPolicyClientRequestHandler;

/**
 *  Helper class.
 *	This is basically a copy of the class that has already been defined in 
 *	policy.h. The difference between this class and the original one is the destructor
 *	This class does not call ResetAndDestroy on iControlPolicies since these are
 *  copies of the original ones.
 *	This class is used when retrieving the original policies that are already stored
 *	in the system.
 *	This class works as a container class and is friend with the classes that used it.
 *
 *  @since S60 3.2
 */ 
NONSHARABLE_CLASS(CClientControlPolicy) : public CBase
	{
public:
	CClientControlPolicy();
	~CClientControlPolicy();
private:
	friend class CPolicyClientRequestHandler; 
	friend class CPolicyHandler;
    friend class CPolicyEventReq;
	TUid iUid;
	RBuf16 iResourceDir;
	RBuf16 iFilename;
	RPointerArray<CActionPolicy> iControlPolicies;	// Never destroy this!
	};
	
/*
 * Helper class to handle transmission of policies when a client requests one
 * This is created the first time a client connects and requests policies. 
 * This class works as a container class and is friend with the classes that used it.
 * 
 * Since S60 V3.1
 */
NONSHARABLE_CLASS(CPolicyEventReq) : public CBase
	{
public:
    CPolicyEventReq();
    ~CPolicyEventReq();
    TUint TotalPolicyCount() const;
private:
	friend class CPolicyClientRequestHandler;
	TThreadId iClientId;
	RMessagePtr2 iMessage;
	RPointerArray<CClientControlPolicy> 	iOriginalControlPolicies;	// Original already stored policies
	RPointerArray<TControlPolicy> 			iAddedPolicies;				// Recently added policies
	RPointerArray<TControlPolicy> 			iRemovedPolicies;			// Recently removed policies.
	};

/**
 *	Handler class for Client policies.
 *	Keeps track of which clients have received which policies.
 *	@since S60 3.2
 */
NONSHARABLE_CLASS(CPolicyClientRequestHandler) : public CBase
	{
public:
	CPolicyClientRequestHandler(CPolicyHandler* aParent);
	~CPolicyClientRequestHandler();
	/**
	 * Adds or update a policy for all connected clients.
	 *
	 * @since S60 3.2
	 *
	 * @param aAction The action ID of the policy that is to be updated.
	 * @param aUid the UID of the policy that is to be updated.
	 * @param The policy that is to be set on the policy that 
	 *		  corresponds to the previous parameters.
	 * @return 
	 */    
	void RegisterControlAction(const TUid aUid, TUint aAction);
	
	/**
     * Removes a Control policy. This will remove a policy either from
     * the retrieved original policies, the recently added policies. If
     * none of the above apply, it creates a policy that will be sent to
     * the client letting the client know that the specified policy has
     * been removed from the system. For a policy to be removed both aAction
     * and aUid must be set in a policy item.
     *
     * @since S60 3.2
     *
     * @param aAction Action contained in the policy to be removed.
     * @param aUid UID contained in the policy that is to be removed.
     */    	
     TInt RemoveControlPolicy(const TUid aUid, TUint aAction );
     
	/**
     * Handles policyrequest fro mthe client. If there are no polices
     * waiting for this client, the message will be stored away and completed
     * when a new policy is added. This policy may either be a new policy, updated
     * policy or remove policy request.
     *
     * @since S60 3.2
     *
     * @param aMessage. Reference to the original message.
     * @return KErrNone or any of the system error code.
     */    	
	TInt RequestPolicy(const RMessage2& aMessage);
	
	/**
     * Handles GetPolicy request from a client. The message that is received
     * from the client must conatain a TTServerControlPolicy at position 0. 
     * The TTServerControlPolicy structure is filled with data from a waiting policy.
     * Policies are sent in a LIFO way.
     *
     * @since S60 3.2
     *
     * @param aClientId id of the client that wants the policies
     * @param aPolicyCount number of policies the caller can handle
     * @param aOutBuf the buffer that will be filled with the policies
     * @return KErrNone or any of the system error messages.
     */    	
	TInt GetPolicyL( TThreadId aClientId, TInt aPolicyCount, TPtr8& aOutBuf );

	/**
     * Removes all data that is used for a client.
     *
     * @param aClientId client id
     */    	
	void RemoveClient( TThreadId aClientId );
	
	/**
     * Removes all requests that belong to terminated clients.
     *
     * @since S60 3.2
     */
	void RemoveDeadClients();
	
	/**
	 * Dumps a list of threadids of all connected clients waiting for 
	 * policy updates
	 *
	 * Used for testing
     * @since S60 3.2
	 *
	 * @param aFile an open write stream to dump clients to.
	 */
	void DumpClientsL(RFileWriteStream& aFile);
	
	/**
	 * Return total number of policies
	 */
    TUint TotalPolicyCount() const;

private:

	/**
     * Inserts a client into the client list. This method also retrieves
     * all policies that already have been stored in the system. If no 
     * policies exist, the message is stored for later copletion when a new
     * policye gets inserted.
     * 
     * @since S60 3.2
     * 
     * @param aMessage. Reference to the original message.
     * @return KErrNone or one of the system error codes.
     *
     */    	
     TInt InsertClient(const RMessage2& aMessage);

	/**
     * Searches the iAddedPolicies array for a UID corresponding to the inserted UID. If not found,
     * it creates a new CControlPolicy entry and adds a TActionPolicy. If the UID is found, it calls
     * AddControlPolicy.
     *
     * @since S60 3.2
     *
     * @param aPtr RPointerArray containing a list of CControlPolicies.
     * @param aUid UID to add.
     * @param aAction. Action to add.
     * @param aPolicy. Policy to set.
     * @return KErrNone if CControlPolicy contaning UID was found or created. Otherwise KErrNoMemory or
     *		   one of the system error messages.
     */    	
	TInt AddControlPolicyAction( RPointerArray<TControlPolicy>& aAddedPolicies, 
								 RPointerArray<TControlPolicy>& aRemovedPolicies,
								 const TUid aUid, TUint aAction);
	
	/**
     * Searches the retrieved list of policies for a policy corresponding to the UID and action.
     *
     * @since S60 3.2
     *
     * @param aPtr. Pointer array containing a list of policies.
     * @param aUid. UID to search for.
     * @param aAction. Action to search for.
     * @return KErrNone if found otherwise KErrNotFound.
     */    	
	TInt CheckStoredControlPolicy(RPointerArray<CClientControlPolicy>& aPtr, const TUid aUid, TUint aAction);
	
	/**
     * Searches for a CControlPolicy object containing the UID inserted. If found, it calls RemovePolicyInfo.
     * If the CControlPolicy object doesn't contain any policy actions after the call, it will be removed.
     *
     * @since S60 3.2
     * 
     * @param aPolUid. Array containing pointers to CControlPolicy objects.
     * @param aAction. Action of the policy to remove.
     * @param aUid. UID of the policy to remove.
     * @return KErrNone if policy is removed, KErrNotFound if policy is not found or one of
     *		   the system error codes.
     */    	
	TInt RemovePolicyUid(RPointerArray<TControlPolicy>& aPolUid, TUint aAction, const TUid aUid);

	/**
     * Searches for a CClientControlPolicy object containing the UID inserted. If found, it calls 
     * RemovePolicyInfo. If the CClientControlPolicy object doesn't contain any policy actions after 
     * the call, it will be removed.
     *
     * @since S60 3.2
     * 
     * @param aPolUid. Array containing pointers to CClientControlPolicy objects.
     * @param aAction. Action of the policy to remove.
     * @param aUid. UID of the policy to remove.
     * @return KErrNone if policy is removed, KErrNotFound if policy is not found or one of
     *		   the system error codes.
     */    	
	TInt RemovePolicyUid(RPointerArray<CClientControlPolicy>& aPolUid, TUint aAction, const TUid aUid);

	/**
     *
     * @since S60 3.2
     * 
     * @param aPolInfo. Reference to a pointer array containing a list of policy actions.
     * @param aAction. Action to delete.
     * @param aDelete. If ETrue, really delete the pointer otherwise, only remove the pointer from 
     *        the pointerarray with Remove()
     * @return KErrNone 
     */    	
	TInt RemovePolicyInfo(RPointerArray<CActionPolicy>& aPolInfo, TUint aAction, TBool aDelete = EFalse);

private: //data	
	// Array containing identifiers of connected clients.
	RPointerArray<CPolicyEventReq> 	iControlPolicyClients;	

// Having everything in one array did not work very well, must keep the original implementation
// for the time being
// We have only one client (singleton, so we store everything in one place.
// It should be simpler to have only one set or arrays to keep up to date
//	RPointerArray<CClientControlPolicy> 	iOriginalControlPolicies;	// Original already stored policies
//	RPointerArray<TControlPolicy> 			iAddedPolicies;				// Recently added policies
//	RPointerArray<TControlPolicy> 			iRemovedPolicies;			// Recently removed policies.
// I assume that there can be only one pending message (because we are called by a singleton).
// If there can be more, we must make this an array.	
// We leave this to CPolicyEventReq for the time being...
//	RMessagePtr2 iMessage;

	// Pointer to main class for GetAllPolicies. Not owned!
	CPolicyHandler* iParent;
	
	};
	
	

#endif // __ALFPOLICYREQUESTHANDLER