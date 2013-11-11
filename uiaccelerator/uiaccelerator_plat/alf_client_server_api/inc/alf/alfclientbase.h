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
* Description:   Client class for the alfredserver.exe
*
*/



#ifndef R_ALFCLIENTBASE_H
#define R_ALFCLIENTBASE_H

#include <AknServerApp.h>
#include <f32file.h>

class CAlfEnv;
class RApaLsSession;
class CApaCommandLine;
/**
 *  Alfred client base.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class RAlfClientBase : public RAknAppServiceBase
    {
    
public:

    /**
     * Constructor
     */
    IMPORT_C RAlfClientBase(TInt aServiceUid);
     
     /**
     * Opens connection to the server.
     * Must be called before any other activity!
     */
    IMPORT_C void OpenL();
    
    /**
	* Grants server access to given private file
	* This access is valid as long as the session remains alive 
	*
	* @param aFileName          Full filename of file to be shared
    * @param aObjectIdentfier   To which object the generated token will be passed
    * @param aCommandId         Command indetifier that allows server implementation to distinguish this particular file
    *
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GrantAccessToPrivateFile( const TDesC& aFileName, 
                                            TInt aObjectIdentfier, 
                                            TInt aCommandId );
    /**
	* Grants server access to given private file
	* This access is valid as long as the session remains alive 
	*
	* @param aOpenFile          Open filehandle, can reside other process gage as well
    * @param aObjectIdentfier   To which object the generated token will be passed
    * @param aCommandId         Command indetifier that allows server implementation to distinguish this particular file
    *
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GrantAccessToPrivateFile( const RFile& aFile, 
                                            TInt aObjectIdentfier, 
                                            TInt aCommandId );

    /**
    *! internal
    */
    void SetAlfEnv(CAlfEnv& aAlf);
    
        
// from base class RAknAppServiceBase

    /**
     * From RAknAppServiceBase.
     * Returns the service UID which the server implements.
     *
     * @return Service UID supported.
     */
    IMPORT_C TUid ServiceUid() const;

    void StartAsyncL(TRequestStatus* aStatus);
    void CompleteAsynchConstructionL();

protected:

    /**
     * Construct the server name.
     */
    IMPORT_C static void ConstructServerName( 
        TName& aServerName, 
        TUid aAppServerUid, 
        TUint aServerDifferentiator );
        
    /**
     * Starts the server.
     */
    IMPORT_C TUint StartServerL( TUid aAppUid );
    
    /**
     * Launches the application.
     */
    IMPORT_C void LaunchAppL( 
        TUid aAppUid, 
        TUint aServerDifferentiator, 
        TThreadId& aThreadId );                                  

private:
    TInt iServiceUid;
    CAlfEnv* iEnv;
    CApaCommandLine* iCmdLine;
   	RApaLsSession* iApa;
    };

NONSHARABLE_CLASS(CAlfAsynchStartup): public CActive 
    {
public:
        
    /**
     * Starts the server. Does not return client for caller, so to be utilized only by FW to trigger server creation
     */
    IMPORT_C static void StartL();

private:
    CAlfAsynchStartup();
    void RunL();
    void DoCancel();
    RAlfClientBase iClient;
    };


#endif // R_ALFCLIENT_H
