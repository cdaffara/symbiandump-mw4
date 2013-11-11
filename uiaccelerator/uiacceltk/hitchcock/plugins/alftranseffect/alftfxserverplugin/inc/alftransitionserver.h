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
* Description:   This is the API used by Theme server to control the
*                appearence of transitions.
*
*/



#ifndef R_TRANSITIONSERVER_H
#define R_TRANSITIONSERVER_H

#include <e32base.h>
#include <gfxtranseffect/gfxtransdatatype.h>
#include <alfdecoderserverclient.h>

class CWaitingTimer;
class TParse;

/**
 *  transition control interface.
 *
 *  This class is used to control transition appearence and behaviour
 *  in the KML transition server.
 *
 *  @since S60 3.2
 */
class CAlfTransitionServerClient : public CBase
	{
	//the Shutdown() should only be called from within the CTransitionServerController.
	//This makes sure that is true.
	friend class CAlfTransitionServerController;
public:
    /**
     * The different list types that can be configured with different KML. 
     */
    enum TListBoxType
        {
        /**
         * The default alternative: Use this one to register KML for all
         * kinds of list.
         */
        EListTypeAny,
        
        /**
         * Main pane lists and 1-column grids.
         */
        EListTypeMainPane,
        
        /**
         * Options menu and lists in popups.
         */
        EListTypeMenuPaneOrPopup,
        
        /**
         * Lists in settings pages.
         */
        EListTypeSettingPage,
        
        /**
         * Grids with more than one column.
         */
        EListTypeGrid
        };

	/**
     * default constructor
     *
     * @since S60 3.2
     */
	CAlfTransitionServerClient();
	
	/**
     * default destructor
     *
     * @since S60 3.2
     */
	~CAlfTransitionServerClient();
	
	/**
     * connects to the transition server.
     *
     * @since S60 3.2
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */
	TInt Connect();
	
	/**
     * disconnects from the transition server
     *
     * @since S60 3.2
     */
	void Disconnect();
	
	/**
     * Registers the KML file and resource directory to be used by a fullscreen effect.
     * The KML file and resource directory will be used for fullscreen effects with 
     * the corresponding aActionID and aUid. KNullUid will be treated as the "default" 
     * Uid - fullscreen effects that supply a not registered Uid will use the KML file 
     * associated with <aActionId, KNullUid>
     * Any aActionID that has not got a KML file associated to it, will be treated as
     * "unsupported".
     * If a <aActionId, aUid> pair is registered with aFilename a length zero the pair will be 
     * treated as "unsupported" even if there is a default filename for the aActionId
     *
     * @since S60 3.2
     * @param aActionID the fullscreen effect ID to associate this kml file with.
     * @param aUid the TUid to associate the kml with, KNullUid as default
     * @param aResourceDir the resource directory
     * @param aFilename the kml file to use, must be relative the resource directory.
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */
	TInt RegisterFullscreenKml(TUint aActionID, const TUid& aUid, const TDesC& aResourceDir, 
										const TDesC& aFilename);
	
	/**
     * tells transition server to remove the KML for a specific fullscreen effect, 
     * from now treating it as "unsupported"
     *
     * @since S60 3.2
     * @param aActionId the fullscreen effect ID to remove KML from.
     * @param aUid the Uid.
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */
	TInt UnregisterFullscreenKml(TUint aActionID, const TUid& aUid);
	
	/**
     * Blocks all fullscreen transitions with a certain Uid independently of the actionId
     * Transitions can be blocked both when doing a fullscreen effect from the Uid and when 
     * doing a fullscreen effect to the Uid.
     * The blocking of a Uid can be removed by setting aBlockFrom and aBlockTo to EFalse.
     *	
     * @since S60 3.2
     * @param aUid the TUid that should be blocked 
     * @param aBlockFrom ETrue if transitions should be blocked when doing a transition from the Uid.
     * @param aBlockTo ETrue if transitions should be blocked when doing a transition to the Uid.
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */    
	TInt BlockFullScreenUid(const TUid& aUid, TBool aBlockFrom, TBool aBlockTo);
	
	/**
     * Registers the KML file and resource directory to be used for a control transition.
     * The KML file and resource directory will be used for control transitions with 
     * the corresponding aUid.
     * Any aUid that has not got a KML file associated to it, will be treated as
     * "unsupported"
     * If you register a KML file on a previously registered uid, the old KML will
     * be automaticly unregistered, and its actions removed.
     *
     * @since S60 3.2
     * @param aUid the TUid to associate the kml with
     * @param aResourceDir the resource directory
     * @param aFilename the kml file to use, must be relative the resource directory.
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */    
	TInt RegisterControlKml(const TUid &aUid, const TDesC& aResourceDir, const TDesC& aFilename);

	/**
     * Registers the KML file and resource directory to be used for a control transition.
     * The KML file and resource directory will be used for control transitions with 
     * the corresponding aUid.
     * Any aUid that has not got a KML file associated to it, will be treated as
     * "unsupported"
     * If you register a KML file on a previously registered uid, the old KML will
     * be automaticly unregistered, and its actions removed.
     *
     * @since S60 3.2
     * @param aUid the TUid to associate the kml with
     * @param aResourceDir the resource directory
     * @param aFilename the kml file to use, must be relative the resource directory.
     * @param aWantedTime Specifies the wanted time between frames for this control effect.
     			If below zero the default wanted time will be used.
     * @param aMinTime Specifies the minimum allowed time between frames for this control effect.
     			If below zero the default minimum time will be used.
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */    
	TInt RegisterControlKml(const TUid &aUid, const TDesC& aResourceDir, const TDesC& aFilename,
									 TInt aWantedTime, TInt aMinTime);

  	/**
     * Unregisters all of KML files and resource directory for the current thread.
     *
     * @since S60 3.2
     * 
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */ 
  	TInt UnregisterAllKml();
  	
  	/**
     * Unregisters the KML file and resource directory for a control transition.
     * The Uid will not have any KML file associated with it.
     * This also unregisters all control actions that have been registered for this
     * Uid. 
     *
     * @since S60 3.2
     * @param aUid the Uid to unregister
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */ 
  	TInt UnregisterControlKml(const TUid &aUid);
  
  	/**
     * Registers an action string to be used with a <aUid, aActionID> pair for a control 
     * transition. 
     * When a control transition is started the TfxServer will use the KML file
     * registered with the Uid. The TfxServer will then use the aUid and aActionId to find
     * the corresponding aActionString. The TfxServer will then run the action in the KML file
     * with the name aActionString.
     *
     * The registration will fail if there isn't a KML file registered with the Uid.
     *
     * @since S60 3.2
     * @param aUid the Uid that will be sent through the GfxTransEffect API
     * @param aActionID the actionID that will be sent through the GfxTransEffect API
     * @param aActionString the name of the action in the KML file
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */    
	TInt RegisterControlAction(const TUid& aUid, TUint aActionID, const TDesC& aActionString);

  	/**
     * Unregisters a aActionString from a <aUid, aActionID> pair. This means that there will not be
     * any transitions for the specified <aUid, aActionId> pair. 
     *
     * @since S60 3.2
     * @param aUid the Uid
     * @param aActionId the ActionId
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */    
	TInt UnregisterControlAction(const TUid& aUid, TUint aActionID);

	/**
     * Sets wanted time between frames.
     * @since S60 3.2
     *
     * @param aTime. Time in milliseconds (0-99).
     * @return KErrNone or any of the system error codes.
    */  
	TInt SetWantedTime(TInt aTime);

	/**
     * Sets minimum allowed time between frames.
     * @since S60 3.2
     *
     * @param aTime. Time in milliseconds (0-99).
     * @return KErrNone or any of the system error codes.
    */  
	TInt SetMinTime(TInt aTime);

	/**
     * Registers the KML file and resource directory to be used by a fullscreen effect.
     * The KML file and resource directory will be used for fullscreen effects with 
     * the corresponding aActionID and aUid. KNullUid will be treated as the "default" 
     * Uid - fullscreen effects that supply a not registered Uid will use the KML file 
     * associated with <aActionId, KNullUid>
     * Any aActionID that has not got a KML file associated to it, will be treated as
     * "unsupported".
     * If a <aActionId, aUid> pair is registered with aFilename of length zero the pair will be 
     * treated as "unsupported" even if there is a default filename for the aActionId
     *
     * @since S60 3.2
     * @param aActionID the fullscreen effect ID to associate this kml file with.
     * @param aUid the TUid to associate the kml with, KNullUid as default
     * @param aResourceDir the resource directory
     * @param aFilename the kml file to use, must be relative the resource directory.
     * @param aCachePriority The priority of the resources for this fullscreen effect in the cache.
     			(higher numbers have higher priority). If below zero the default priority will be used.
     * @param aWantedTime Specifies the wanted time between frames for this fullscreen effect.
     			If below zero the default wanted time will be used.
     * @param aMinTime Specifies the minimum allowed time between frames for this fullscreen effect.
     			If below zero the default minimum time will be used.
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */
	TInt RegisterFullscreenKml(TUint aActionID, const TUid& aUid, const TDesC& aResourceDir, 
										const TDesC& aFilename, TInt aCachePriority, TInt aWantedTime,
										TInt aMinTime);

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
     * @param aListBoxType the type of listbox to associate the KML files with,
     *        EListTypeAny to associate it with any type of list.
	 * @param aResourceDir the resource directory
	 * @param aBackgroundFileName the kml file to use for the background and highlight of the listbox, 
	 		  must be relative the resource directory.
	 * @param aListItemFilename the kml file to use for the listitems of the listbox, 
	 		  must be relative the resource directory.
	 * @return KErrNone if successful, otherwise another of the system-wide error codes.
	 */
	TInt RegisterListBoxKml(const TUid& aUid, const TListBoxType aListBoxType, 
                                     const TDesC& aResourceDir, 
									 const TDesC& aBackgroundFilename, 
									 const TDesC& aListItemFilename);

	/**
	 * Unregisters KML files previously registered for a listbox.
	 *
	 * @since S60 S60 v3.1
	 * @param aUid the Uid to unregisted KML files for.
     * @param aListBoxType The listbox type to unregister KML files for.
	 * @return KErrNone if successful, otherwise another of the system-wide error codes.
	 */    
	TInt UnregisterListBoxKml(const TUid& aUid, const TListBoxType aListBoxType);

    /**
     * Set the wanted time per frame for listbox effects.
     *
     * @param aWantedTime The wanted time per frame, in milliseconds.
     * 
     * @return KErrNone on success, or one of the system-wide error codes.
     */
    TInt SetListBoxFrameTime( const TInt aWantedTime );
    
    /**
     * Set the minimum time per frame for listbox effects.
     *
     * @param aMinFrameTime The minimum time per frame, in milliseconds.
     *
     * @return KErrNone on success, or ome of the system-wide error codes.
     */    
    TInt SetListBoxMinFrameTime( const TInt aMinFrameTime );

    /**
     * Set the wanted time per frame for listbox effects.
     *
     * @param aWantedTime The wanted time per frame, in milliseconds.
     * 
     * @return KErrNone on success, or one of the system-wide error codes.
     */
    TInt SetControlFrameTime( const TInt aWantedTime );
    
    /**
     * Set the minimum time per frame for listbox effects.
     *
     * @param aMinFrameTime The minimum time per frame, in milliseconds.
     *
     * @return KErrNone on success, or ome of the system-wide error codes.
     */    
    TInt SetControlMinFrameTime( const TInt aMinFrameTime );

	/**
     * Checks if the system is connected.
     * @since S60 3.2
     *
     * @return ETrue if connected, otherwise EFalse.
    */  
    TBool IsConnected();
    
    /**
     * Switch Tfx Server working on low memory mode from good memory mode.
     * 
     * @since S60 3.2
     * 
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */ 
  	TInt FreeRam();
  	
  	/**
     * Switch Tfx Server working on good memory mode from low memory mode.
     *
     * @since S60 3.2
     * 
     * @return KErrNone if successful, otherwise another of the system-wide error codes.
     */ 
  	TInt MemoryGood();
  	
  	
private:
	/**
	 * Verifies that server really is alive.
	 * @since S60 3.2
	 *
	 * @return KErrNone if alive, otherwise any of the system error codes.
	 */
	TInt VerifyConnection();

	/**
     * shuts down the server. Not to be called by other then the transitionservercontroller
     * @since S60 3.2
     *
     * @return KErrNone or any of the system error codes.
    */  
    TInt Shutdown(TThreadId& aServerId);

    /**
     * @see RegisterListBoxKml
     */
	TInt RegisterListBoxKmlL(const TUid& aUid, const TListBoxType aListBoxType, 
                             const TDesC& aResourceDir, 
                             const TDesC& aBackgroundFilename, 
                             const TDesC& aListItemFilename);
	
	/**
	 * Only needed to allow finding effect files on different drives
	 */
	TInt FindEffectFile( const TDesC& aResourceDir, const TDesC& aFilename ); 

                             
private: // data
    
   	RAlfTfxClient iTfxServer;
	TUid iPluginImplementation;
	TBool iConnected;
	TBool iHasPlugin;
	// We keep these as members to avoid allocating and deallocating memory all the time
	HBufC8* iTransferBuffer;
	HBufC8* iReturnBuffer;
	// RFs, filename and parse are needed only to allow overriding the drive definition
	// from the manifest file
    RFs iFs; // file system
    TParse iParse; // for parsing filename locations
	RFile iFile;
	};
	
	
#endif // R_TRANSITIONSERVER_H


