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
* Description:   handles policies for transitions, wich are supported, what kml to use etc.
*
*/


#ifndef C_POLICYHANDLER_H
#define C_POLICYHANDLER_H

#include <e32base.h>
#include <s32file.h>


#include "alfpolicy.h"
#include "alfpolicyrequesthandler.h"
//#include "transitionserver.h"

//======= Forward declarations =======
class TFullScreenBlock;
class CClientControlPolicy;
class CFullscreenPolicy;
class CControlPolicy;
class CListBoxPolicy;

/**
 *  policy handler
 *
 *  handles policies for transitions, if it is supported or not, and if so, what 
 *  kml file to use for the effect. This class handles both Fullscreen and 
 *	control policies.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CPolicyHandler) : public CBase
    {  	
public: 
	friend class CPolicyClientRequestHandler;
	/**
     * Public part of two phase construction.
     * @since S60 3.2
     *
     * @return 
    */  
    static CPolicyHandler* NewL();

    /**
     * Destructor. 
     * @since S60 3.2
     *
    */  
    ~CPolicyHandler();

/**
	 * Register the KML files and resource directory to be used by a listbox.
	 * The KML files and resource directory will be used for listboxes of the specified type
     * in the process with the Uid aUid.
     *
     * KNullUid will be treated as the "default" Uid - listboxes in processes
	 * with no KML registered for their uid will use KML registered with KNullUid.
	 * EListTypeAny works the same way for listbox types.  More specific settings have 
     * priority over less specific ones, and the uid has higher priority than the list type,
     * which means that when assigning KML to a specific listbox, the search for KML is
     * done in the following order:
     *
     * 1. KML that has been registered with the uid of the listbox' app and with the
     *    listbox type that matches the specific listbox.
     * 2. KML that has been registered with the uid of the listbox' app, with type EListTypeAny.
     * 3. KML that has been registered with KNullUid and with the matching listbox type.
     * 4. KML that has been registered with KNullUid and EListTypeAny.
     *
	 * If a Uid is registered with aBackgroundFileName of length zero this Uid will be treated as
	 * having no effect, even if there is a "default" Uid. 
	 *
	 * @since S60 v3.1
	 * @param aUid the TUid to associate the kml files with, KNullUid as default
	 * @param aThreadId. The thread Id to associate the kml files with
     * @param aListBoxType the type of listbox to associate the KML files with,
     *        EListTypeAny to associate it with any type of list.
	 * @param aResourceDir the resource directory
	 * @param aBackgroundFileName the kml file to use for the background and highlight of the listbox, 
	 		  must be relative the resource directory.
	 * @param aListItemFilename the kml file to use for the listitems of the listbox, 
	 		  must be relative the resource directory.
	 * @return KErrNone if successful, otherwise another of the system-wide error codes.	
	 */
	void RegisterListBoxKmlL( const TUid& aUid, 
							  const TThreadId& aThreadId,
	                          const CAlfTransitionServerClient::TListBoxType aListBoxType,
	                          const TDesC& aResourceDir, 
							  const TDesC& aBackgroundFileName,
							  const TDesC& aListItemFilename,
							  TInt aCachePriority = -1,
							  TInt aWantedTime = -1, TInt aMinTime = -1 );
	/**
	 * Unregisters KML files previously registered for a listbox.
	 *
	 * @param aUid the Uid to unregisted KML files for.
	 * @param aListBoxType The type of listbox to unregister KML for.
	 * @return KErrNone if successful, otherwise another of the system-wide error codes.
	 */    
	TInt UnregisterListBoxKml( const TUid& aUid,
                               const CAlfTransitionServerClient::TListBoxType aListBoxType );
                               
    /**
	 * Unregisters KML files previously registered for a listbox from one Thread.
	 *
	 * @param aThreadId the thead id to unregisted KML files for.
	 * @return KErrNone if successful, otherwise another of the system-wide error codes.
	 */    
	TInt UnregisterListBoxKml( const TThreadId& aThreadId );


    /**
     * Get the KML files for listboxes in the app with the specified Uid.
     *
     * @param aAppId The AppUid of the application that the listbox belongs
     *        to.
     * @param aResourceDir
     * @param aBackgroundFilename Name of the KML file that defines the
     *        background behaviour.
     * @param aItemFilename Name of the KML file that defines the item
     *        behaviour.
     * @aCachePriority
     * @aWantedTime
     * @aMinTime
     */
    TInt GetListBoxKml(	const TUid& aAppId, 
                        const CAlfTransitionServerClient::TListBoxType aListBoxType,
                        TPtrC& aResourceDir,
                        TPtrC& aBackgroundFilename,
                        TPtrC& aItemFilename,
                        TInt& aCachePriority, TInt& aWantedTime, 
                        TInt& aMinTime );

  	/**
     * Registers a KML file for use with FullScreen transitions. 
     * If the filename stored here is NULL, the action is blocked
     * from any UID.
     * @since S60 3.2
     *
     * @param aAction. Action for which this KML file will be used.
     * @param aUid. UID for which this KML file will be used.
     * @param aThreadId. Thread ID for which this KML file will be used.
     * @param aResourceDir. Descriptor containing the full path
     *		  to the location of the KML file.
     * @param aFilename. Descriptor contrianing the name of the
     *		  KML file.
     * @param Proirity when it's stored in the cache.
     * @param wanted time between frames.
     * @param Minimum allowed time between frames.
     * @return KErrNone or any of the system error codes.
     */    
	void RegisterFullscreenKmlL( TUint aAction, const TUid& aUid, 
								 const TThreadId& aThreadId,
								 const TDesC& aResourceDir, const TDesC& aFilename,
								 TInt aCachePriority = -1, TInt aWantedTime = -1 , TInt aMinTime = -1);
  	/**
     * Unregisters a KML file for <action,uid> pair. Both action
     * and UID must match in order for the file to be unregistered.
     * @since S60 3.2
     *
     * @param aAction. Action to unregister.
     * @param aUid. UID to unregister.
     * @return KErrNone or any of the system error codes.
     */    
  	TInt UnregisterFullscreenKml(TUint aAction, const TUid& aUid);
  	
  	/**
     * Unregisters all of Fullscreen KML files from one thread Id
     * @since S60 3.2
     *
     * @param aThreadId. the thread Id to unregisted KML files for.
     * @return KErrNone or any of the system error codes.
     */   
     TInt UnregisterFullscreenKml(const TThreadId& aThreadId);
     
  	/**
     * Unregisters all of registered KML files for a thread ID.
     * @since S60 3.2
     *
     * @param aThreadId the thread id to unregisted KML files for.
     * @return KErrNone or any of the system wide error codes.
    */  
	TInt UnregisterAllKml(const TThreadId& aThreadId);
	
  	/**
     * Retrieves the path and name of a KML file that corresponds to the
     * supplied aAction and aToUid. This method will return 
     * KErrNotSupported if the <aAction, aToUid> pairs are not found,
     * the aToTuid or aFromUid are blocked. If the aToUid is the default
     * UID (KNullUid) and the default action has been blocked 
     * KerrNotSupported will also be returned.
     * @since S60 3.2
     *
     * @param aAction. The action for which to retireve file data.
     * @param aToUid. The ToUid for which to retrieve file data.
     * @param aFromUid. The FromUid for which to retrieve file data.
     * @param aResourceDir. Pointer reference where the resource
     *  	  directory will be stored.
     * @param aFileName. Pointere reference where the file name will
     * 		  be stored.
     * @return KerrNone or KErrNotSupported.
     */    
  	TInt GetFullscreenKml(	TUint aAction, const TUid& aToUid, 
  							const TUid& aFromUid, TPtrC& aResourceDir, TPtrC& aFilename,
  							TInt& aCachePriority, TInt& aWantedTime, TInt& aMinTime);

private:
  	/**
     * Retrieves all stored policies for CCoeControls. The policies
     * will be copied (by pointer) and stored in the supplied arrray.
     * This method is only used by CPolicyClientRequestHandler.
     *
     * @param aControlPolicies. Array where the policies will be stored.
     * @return KerrNone or any of the system error codes.
     */    
  	TInt GetAllControlPolicies(RPointerArray<CClientControlPolicy>& aControlPolicies);

public:
  	/**
     * Passes the call on to the AlfPolicyRequestHandler
     * @since S60 3.2
     *
     * @param aMessage. Reference to the message sent by the client.
     * @return KErrNone or any of the system error messages.
     */    
  	TInt RequestPolicy(const RMessage2& aMessage);

  	/**
     * Passes the call on to the AlfPolicyRequestHandler
     *
     * @param aClientId id of the client that wants the policies
     * @param aPolicyCount number of policies the caller can handle
     * @param aOutBuf the buffer that will be filled with the policies
     * @return KErrNone or any of the system error messages.
     */    
  	TInt GetPolicyL( TThreadId aClientId, TInt aPolicyCount, TPtr8& aOutBuf );

  	/**
     * Passes the call on to the AlfPolicyRequestHandler
     * @since S60 3.2
     *
     * @param aClientId id of the client whose request are cancelled
     */    
  	void RemoveClient( TThreadId aClientId );

	/**
     * Removes all requests that belong to terminated clients.
     * Passes the call on to the PolicyRequtHandler
     *
     * @since S60 3.2
     */
//	void RemoveDeadClients();
	
	/**
	 * Dumps a list of threadids of all connected clients waiting for 
	 * policy updates
	 *
	 * Used for testing
     * @since S60 3.2
	 *
	 * @param aFile an open write stream to dump clients to.
	 */
//	void DumpClientsL(RFileWriteStream& aFile);
  	
  	/**
     * Blocks/Unblocks a UID for fullscreen effects. If both aBlockTo
     * and aBlockFrom are false, the UID is unblocked. This unblocking 
     * feature will unblock all blocks containing the supplied UID. 
     * @since S60 3.2
     *
     * @param aUid. The UID to block.
     * @param aBlockTo. True if the ToUid is to be blocked.
     * @param aBlockFrom. True if the FromUid is to be blocked.
     * @return KErrNone or any of hte system error codes.
     */    
  	TInt BlockFullScreenUid(const TUid& aUid, TBool aBlockFrom, 
  							TBool aBlockTo);

  	/**
     * Registers a KML file for use with a CCoeControl. Although a file
     * has been registered, the policy for this UID is still unsupported
     * since no actions have been registered.
     * @since S60 3.2
     *
     * @param aUid. The UID for which the KML file contains actions.
     * @param aThreadId, the Thread ID to associate the kml files with.
     * @param aResourceDir. Descriptor containing the full path to
     * 		  the location of the KML file.
     * @param aFilename. Descriptor containing the filename of the 
     * 		  KML file.
     */    
  	void RegisterControlKmlL(const TUid &aUid, 
  							 const TThreadId& aThreadId,
  							 const TDesC& aResourceDir, 
  							 const TDesC& aFilename,
  							 TInt aWantedTime = -1,
  							 TInt aMinTime = -1);
  	
  	/**
     * Unregisters a KML file for the supplied UID. This method will 
     * also unregister all actions for this UID.
     * @since S60 3.2
     *
     * @param aUid. The UID to unregister.
     * @return KerrNone if successfully unregistered, otherwise
     *		   KErrNotFound.
     */    
  	TInt RemoveControlKml(const TUid &aUid);
  	
  	/**
     * Unregisters KML files for the supplied Thread ID. This method will 
     * also unregister all actions from them.
     * @since S60 3.2
     *
     * @param aThreadId. The thead ID to unregister for.
     * @return KerrNone if successfully unregistered, otherwise
     *		   KErrNotFound.
     */    
  	TInt UnregisterControlKml(const TThreadId& aThreadId);
  	
  	
  	/**
     * Retrieves the resource directory, filename and action for a 
     * CCoeControl animation based on the UID and Action supplied.
     * If the action was not found the supplied pointer will be set
     * to NULL.
     * @since S60 3.2
     *
     * @param aUid. UID of the CCoeControl.
     * @param aAction. Action to be performed.
     * @param aResourceDir. Pointer reference that will contain the
     *		  full path to where the KML file is located. Not owning.
     * @param aFileName. Pointer reference that will contain the
     *		  filename. Not owning.
     * @param aActionString. Pointer reference that will contain the
     *		  string-name of the action to be performed.
     * @return KerrNone if data was found otherwise KErrNotFound.
     */    
  	TInt GetControlKml(	TUid aUid, TUint aAction, TPtrC& aResourceDir, 
  						TPtrC& aFileName, TPtrC& aActionString, TInt& aCachePriority,
  						TInt& aWantedTime, TInt& aMinTime);
  	
   	/**
     * Registers an actionstring  for a CCoeControl with a <UID, Action>
     * pair. This method will fail if no KML file has been registered for  
     * the UID.
     * @since S60 3.2
     *
     * @param aUid. UID for which to register the action string.
     * @param aAction. Action for which to register the actionstring.
     * @param aActionString. Descriptor containing the action string
     *		  to register.
     * @return KErrNone if successfull, otherwise any of the system
     *		   error codes.
     */    
  	TInt RegisterControlAction( const TUid aUid, TUint aAction, 
  								const TDesC& aActionString);
  	
  	/**
     * Unregisters an CCoeControl transition action for a UID.
     * @since S60 3.2
     *
     * @param aUid. UID for which to unregister the action.
     * @param aAction. Action to unregister.
     * @return KErrNone if action is unregistered, otherwise KerrNotFound.
     */    
  	TInt UnregisterControlAction( const TUid aUid, TUint aAction );

private:
    CPolicyHandler();

  	/**
     * Looks through the stored policy list for a specific policy.
     * @since S60 3.2
     *
     * @param aAction. Action too look for.
     * @param aUidT. Uid to look for
     * @param aPolicy. Reference to a CActionPolicy where the policy
     *		  will be stored for future reference.
     * @param aIndex. Reference where the index of the policy item 
     *		  will be stored for future reference.
     * @return KErrNone if found. Otherwise KErrNotFound.
     */    
    TInt FindControlPolicy(	TUint aAction, TUid aUidT, 
    						CActionPolicy*& aPolicy, TInt& aIndex);
    
private:	// data
    RPointerArray<CFullscreenPolicy> 	iFullscreenPolicies;
    
    RPointerArray<CControlPolicy> 		iControlPolicies;
    
    CPolicyClientRequestHandler* 		iClientRequestHandler;
    
    RPointerArray<TFullScreenBlock> 	iFullScreenBlocks;

    RPointerArray<CListBoxPolicy> 		iListBoxPolicies;

    };


#endif
