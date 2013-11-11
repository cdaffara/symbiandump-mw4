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



#ifndef __FTOKENCLIENT_H__
#define __FTOKENCLIENT_H__

#include <e32std.h>
#include <f32file.h>

const TInt KMaxTokenLenght(8);

/**
* Utility class for for giving another process permission (token) to
* access on demand a specific file from another process. The class
* permits process (provider) to grant another process (consumer) ability to
* open, read and close the named file on demand as the consumer so wishes, 
* without repeated interaction between provider and consumer. 
*
* Tokens are valid as long the session used for grating the permission is connected 
* 
* @code
*
*    // Provider should usually instantiate session as member variable as session lifetime 
*    // defines the tokens lifetime also, consumer may generate session on-demand based as well
*
*    RFTokenClient client;
*    User::LeaveIfError(client.Connect());
*    CleanupClosePushL(client);
*
*   // Provider, generating token based on known uid (made up uid in this example ;)
*
*   TInt64 token = 0;
*   TPckg<TInt64> tokenbuf(token); 
*    
*   _LIT(KMYPRIVATEFILE, "z:\\private\\1028289F\\focus.png"); // full path with drive letter must be given
*   
*   TSecureId consumerSecId(0x10282845); 
*
*   User::LeaveIfError(iClient.GenerateToken(),TSecurityPolicy(consumerSecId), tokenbuf));
*
*   // Generating token based on existing token
*   // Nb. Consumer may ensure extended lifetime for token by "cloning" it
*   // (i.e. make sure that token remains valid even the session of orifial provider
*   // is closed )  
*
*   TInt64 token2 = 0;
*   TPckg<TInt64> tokenbuf2(token2); 
*
*    User::LeaveIfError(client.GenerateToken(tokenbuf, Application()->AppDllUid(), tokenbuf2));
*
*  // Consumer, opening file based on token
*
*   RFile file;
*   User::LeaveIfError(client.OpenFileForToken(file, tokenbuf2));
*    
* @endcode
*/    
NONSHARABLE_CLASS(RFTokenClient): public RSessionBase
    {
public:
    /**
	* Connects to server. If the server does not exist, starts the server .
	*/
    IMPORT_C TInt Connect();
    
    /* Methods for granting permission */
    
    /**
	* Generates token based on filename. Method is synchronous.
	*
	* @param aFileName          Filename of file to be shared
	* @param aSecurityPolicy    Security policy that a process that process must fullfill to gain access to file 
	* @param aToken             Generated token, 64-bit integer as a 8-bit descriptor              
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GenerateToken(const TDesC& aFileName, const TSecurityPolicy& aSecurityPolicy, TDes8& aToken) const;    
    
    /**
	* Generates token based on open file handle. Method is synchronous.
	*
	* @param aOpenFile          Open filehandle, can reside other process gage as well
	* @param aSecurityPolicy    Security policy that a process that process must fullfill to gain access to file 
	* @param aToken             Generated token, 64-bit integer as a 8-bit descriptor              
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GenerateToken(const RFile& aOpenFile, const TSecurityPolicy& aSecurityPolicy, TDes8& aToken) const;    
    
    /**
	* Generates token based on existing token. Method is synchronous.
	*
	* @param aExistingToken     Token that is valid inside this process (SecId matches)
	* @param aSecurityPolicy    Security policy that a process that process must fullfill to gain access to file 
	* @param aToken             Generated token, 64-bit integer as a 8-bit descriptor              
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GenerateToken(const TDesC8& aExistingToken, const TSecurityPolicy& aSecurityPolicy, TDes8& aToken) const;    

    
    /* Methods for granting permission, for convenience */
    
    /**
	* Generates token based on filename. Method is synchronous.
	*
	* @param aFileName          Filename of file to be shared
	* @param aConsumerUid       Secure id of process that gains access to file 
	* @param aToken             Generated token, 64-bit integer as a 8-bit descriptor              
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GenerateToken(const TDesC& aFileName, const TUid& aConsumerUid, TDes8& aToken) const;
    
    /**
	* Generates token based on open file handle. Method is synchronous.
	*
	* @param aOpenFile          Open filehandle, can reside other process gage as well
	* @param aConsumerUid       Secure id of process that gains access to file 
	* @param aToken             Generated token, 64-bit integer as a 8-bit descriptor              
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GenerateToken(const RFile& aOpenFile, const TUid& aConsumerUid, TDes8& aToken) const;    
    
    /**
	* Generates token based on existing token. Method is synchronous.
	*
	* @param aExistingToken     Token that is valid inside this process (SecId matches)
	* @param aConsumerUid       Secure id of process that gains access to file 
	* @param aToken             Generated token, 64-bit integer as a 8-bit descriptor              
	* @return                   Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt GenerateToken(const TDesC8& aExistingToken, const TUid& aConsumerUid, TDes8& aToken) const;    
    
    
    /* Methods for accessing file based on token */
    
    /**
	* Opens filehandle based on token. Method is synchronous.
	*
	* @param aHandle     Unattached handle 
	* @param aToken      Token that is valid in for this process              
	* @return            Any system wide error code, KErrNone on success
	*/
    IMPORT_C TInt OpenFileForToken(RFile& aHandle, const TDesC8& aToken) const;

private:
    TInt iSpare1;
    TInt iSpare2;
    };
    
#endif