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
* Description:   Client interface for granting other processes to token to access
*                particular files in app directory 
*
*/



#include "alf/ftokenclient.h"
#include "ftokenconsts.h"

EXPORT_C TInt RFTokenClient::Connect()
    {
    TInt err = KErrNone;
    TFindServer serveFinder(FTOKEN_SERVER_NAME);
    TFullName fullName;
    if (serveFinder.Next(fullName) != KErrNone)
        {
 	    const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

	    RProcess server;
	    err = server.Create(_L("Z:\\sys\\bin\\!ftokenserver.exe"),FTOKEN_SERVER_NAME,serverUid);
	    if (!err)
	        {
        	TRequestStatus stat;
	        server.Rendezvous(stat);
	        if (stat!=KRequestPending)
	            {
		        server.Kill(0);         // abort startup
	            }
	        else
	            {
		        server.Resume();        // logon OK - start the server
	            }
	        User::WaitForRequest(stat);             // wait for start or death
	        err = stat.Int();
	        }
        }

    if ( !err )
        {
        err = CreateSession(FTOKEN_SERVER_NAME,TVersion(1,1,1)); 
        }

	return err;
    }

EXPORT_C TInt RFTokenClient::GenerateToken(const TDesC& aFileName, const TUid& aConsumerUid, TDes8& aToken) const
    {
    TSecureId consumerUid(aConsumerUid);
    return GenerateToken(aFileName, TSecurityPolicy(consumerUid), aToken);
    }
    
EXPORT_C TInt RFTokenClient::GenerateToken(const TDesC& aFileName, const TSecurityPolicy& aSecurityPolicy, TDes8& aToken) const
    {
    // unefficient, but this way we can make _easily_ sure that client is actually allowed to open a file
    // Not sure if we were allowed to just change CEikonEnv's file server session to shared, so we could acutally save
    // this extra session..
    RFs fs;
    TInt err = fs.Connect();
    if (!err) 
        {
        err = fs.ShareProtected();
        if (!err) 
            {
            RFile file;
            err = file.Open(fs, aFileName, EFileShareReadersOnly); // Todo: check the correct mode
            if (!err)
                {
                err = GenerateToken(file, aSecurityPolicy, aToken);
                }
            file.Close();
            }
        fs.Close();
        }
    
    return err;
    //return SendReceive(EGenerateTokenBasedOnName, TIpcArgs(&aFileName, aAllowedProcessUid.iUid, &aToken));
    }
    
EXPORT_C TInt RFTokenClient::GenerateToken(const RFile& aOpenFile, const TUid& aConsumerUid,TDes8& aToken) const
    {
    TSecureId consumerUid(aConsumerUid);
    return GenerateToken(aOpenFile, TSecurityPolicy(consumerUid), aToken);
    }

EXPORT_C TInt RFTokenClient::GenerateToken(const RFile& aOpenFile, const TSecurityPolicy& aSecurityPolicy,TDes8& aToken) const
    {
    TPtrC8 ptr = aSecurityPolicy.Package();
    TIpcArgs ipcArgs(&ptr, &aToken, 0, 0);
    TInt ret = aOpenFile.TransferToServer(ipcArgs, 2, 3);
    if (ret == KErrNone)
        {
        ret = SendReceive(EGenerateTokenBasedOnOpenHandle, ipcArgs);
        }
    return ret;
    }

EXPORT_C TInt RFTokenClient::GenerateToken(const TDesC8& aExistingToken, const TUid& aConsumerUid,TDes8& aToken) const
    {
    TSecureId consumerUid(aConsumerUid);
    return GenerateToken(aExistingToken, TSecurityPolicy(consumerUid), aToken);
    }

EXPORT_C TInt RFTokenClient::GenerateToken(const TDesC8& aExistingToken, const TSecurityPolicy& aSecurityPolicy,TDes8& aToken) const
    {
    TPtrC8 ptr = aSecurityPolicy.Package();
    return SendReceive(EGenerateTokenBasedOnAnotherToken, TIpcArgs(&ptr, &aExistingToken, &aToken));
    }

EXPORT_C TInt RFTokenClient::OpenFileForToken(RFile& aHandle, const TDesC8& aToken) const
    {
    TInt serverhandle = 0;
    TPckg<TInt> buf(serverhandle);
    TInt rfsHandle = SendReceive(EOpenHandleForToken, TIpcArgs(&buf, &aToken));
    return aHandle.AdoptFromServer(rfsHandle, serverhandle);
    }

// end of file
