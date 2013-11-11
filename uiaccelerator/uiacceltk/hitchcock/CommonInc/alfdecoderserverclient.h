/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Interface for granting a permission and for using files from 
*                apps' private cage. No capabilities required 
*
*/



#ifndef __ALFBRIDGERCLIENT_H__
#define __ALFBRIDGERCLIENT_H__

#include <e32std.h>
#include <e32def.h>
#include <e32base.h>

NONSHARABLE_CLASS(AlfServerStarter)
	{
	public:
	static IMPORT_C void StartL(TRequestStatus& aStatus, TBool aCreateProcess = ETrue);
	};


/**
* internal client for decoder server
*/    
NONSHARABLE_CLASS(RAlfBridgerClient): public RSessionBase
    {
public:
    /**
	* Connects to server. Assumes that server is up and running
	*/
    IMPORT_C TInt Connect();
    
    IMPORT_C TInt SendSynch(TInt aOp, const TDesC8& aBuffer);
    
    IMPORT_C TInt SendSynch(TInt aOp, const TIpcArgs& aIPCArgs );

    IMPORT_C void SendAsynchronous(TInt aOp, const TIpcArgs& aIPCArgs,TRequestStatus& aStatus );

    IMPORT_C TInt SendBlind(TInt aOp, const TIpcArgs& aIPCArgs );
    
     /**
     * Asks list of window groups that have graphics surfaces even
     * they are not supposed to be seen on screen
     * @param aArray array to be populated
     * @return error code.
     */    
    IMPORT_C TInt GetListOfInactiveWindowGroupsWSurfaces(RArray<TInt>* aWindowGroups);
    
     /**
     * Asks list of window groups that have graphics surfaces attached
     * @param aArray array to be populated
     * @return error code.
     */    
    IMPORT_C TInt GetListOfWindowGroupsWSurfaces(RArray<TInt>* aWindowGroups);

     /**
     * Asks the number of active effects
     * @return error code or number of active effects (>= 0).
     */    
     IMPORT_C TInt EffectsCount();

     /**
     * Asks list of window groups that potentially use graohics memory even no direct EGL nor surface access on main display
     * @param aArray array to be populated
     * @return error code.
     */   
    IMPORT_C void RAlfBridgerClient::GetOptionalGraphicsMemUsers(RArray<TInt>* aOptionalCandidates);

     
private:
    
    TInt GetListOfWindowGroups(RArray<TInt>* aWindowGroups, TInt aType);
    TInt iSpare1;
    TInt iSpare2;
    };


NONSHARABLE_CLASS(RAlfTfxClient): public RAlfBridgerClient
    {
public:
    /**
	* Connects to server. Assumes that server is up and running
	*/
    IMPORT_C TInt Open();

    /**
    * Loads effects plugin with given implementation uid. 
    * If the plugin was already loaded, returns KErrAlreadyExists, otherwise system wide error codes.	
    */
    IMPORT_C TInt LoadTfxPlugin(const TUid& aPluginUid);

    /**
    * Unloads effects plugin with given implementation uid. If the plugin had created some additional 
    * windows to scene, those will be closed
    */
    IMPORT_C void UnloadTfxPlugin(const TUid& aPluginUid);
    
    /**
    * Sends message for a plugin, synhcronous method
    */
    IMPORT_C TInt SendSynchronousData(const TUid& aPluginUid, const TDesC8& aInBuf, TDes8& aOutBuf);
 
    /**
    * Sends message for a plugin, asynhcronous method. Calling implementation must ensure that buffers exist 
    * when command is being executed and completed in server.
    */   
    IMPORT_C void SendAsynchronousData(const TUid& aPluginUid, const TDesC8& aInBuf, TDes8& aOutBuf, TRequestStatus& aStatus);
 
    /**
    * Cancels asynchronous command, if the default values are used, all commands specific for this client will be cancelled
    * Message ids are implementation specific.
    */   
    IMPORT_C void CancelAsynchronousData(const TUid& aPluginUid = KNullUid, TInt aCommandId = 0);

    /**
     * Trigger effects plugins to prepare next frame. Only FW should use this method
     */
    IMPORT_C void PrepareFrame(TUint aEstimatedFrameInterval);

private:
    TInt iSpare1;
    TInt iSpare2;
    };
    
#endif