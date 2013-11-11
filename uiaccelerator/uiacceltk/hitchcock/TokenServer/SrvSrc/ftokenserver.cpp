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
#include "ftokenserver.h"
#include "ftokenconsts.h"

void CFsTokenServer::NewLC()
    {
    CFsTokenServer* pS = new (ELeave) CFsTokenServer;
    CleanupStack::PushL(pS);
    pS->ConstructL();
    }

void CFsTokenServer::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );
    StartL(FTOKEN_SERVER_NAME);
    }

CSession2* CFsTokenServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
    {
	return new(ELeave) CFsTokenServerSession();   
    }

void CFsTokenServer::HandleClientExit(const CSession2* aClient)
    { // Todo: We could invent somekind notify here to notify 
      // possible user of a file to release its handle as well..
    for(TInt index = iTokens.Find(*aClient, TToken::CompareOwner); 
        index != KErrNotFound; 
        index = iTokens.Find(*aClient, TToken::CompareOwner) )
        {
        if (iTokens[index].iHasSubToken == 0)
            {
            delete iTokens[index].iFileName;
            }
        iTokens.Remove(index);
        }
    }

CFsTokenServer::~CFsTokenServer()
    {
    for(TInt count = iTokens.Count(); count > 0; count-- )
        {
        if (iTokens[0].iHasSubToken == 0)
            {
            delete iTokens[0].iFileName;
            }
        iTokens.Remove(0);
        }
        
    iFs.Close();
	iTokens.Close();
    }
	    
void CFsTokenServer::AddTokenL(const RMessage2& aMessage)
    {
	HBufC* fileName = 0;
    TKeyArrayFix tokenKey(_FOFF(TToken, iToken), ECmpTInt64);	  	    
	
	TInt function = aMessage.Function();
	switch(aMessage.Function())
        {
        case EGenerateTokenBasedOnName:
        //==================================================
        // 1) filename
        //===================================================
           {
           User::Leave(KErrNotSupported);
/*                TInt msgLenght = aMessage.GetDesLength(1);
                if (length > 0)
                    {
                    fileName = HBufC::NewLC(length);
                    TPtr ptr = fileName->Des();
                    aMessage.Read(1,ptr);
                    if (iFs.IsValidName(ptr))
                        {

                        }
                    else
                        {
                        User::Leave(KErrArgument);
                        }
                    }
                else 
                    {
                    User::Leave(KErrArgument);
                    }    
*/	    
            break;
            }
        case EGenerateTokenBasedOnOpenHandle:
        //===================================================
        // 2) handle
        //===================================================
            {
            // 2.1) check that we are able to open the file	
            RFile file;
            User::LeaveIfError(file.AdoptFromClient(aMessage,2,3));
            // 2.2) resolve full filename
            TFileName fn;
            file.FullName(fn);
            file.Close();
               
            fileName = fn.AllocLC();
            break;
            }
        case EGenerateTokenBasedOnAnotherToken:
        // ==================================================        
        // 3) token
        //===================================================
            {
            TInt64 token = 0;
            TPckg<TInt64> buf(token); 
    
            // 3.1) just check that client uid and token match     
            aMessage.ReadL(1,buf);
            TInt index = iTokens.Find(token, TToken::CompareToken);
            User::LeaveIfError( index );
    
	        if (iTokens[index].iPolicy.CheckPolicy(aMessage))
	            {
	            // set a pointer to existing buffer, when deleting
	            // this must be handled slightly different way
                // but theres no need to spend lots of mem 
                // for duplicate filenames
                fileName = iTokens[index].iFileName;
                
                // there shouldn't be more than one subtoken per token ever 
                ASSERT(iTokens[index].iHasSubToken==EFalse);
                iTokens[index].iHasSubToken = ETrue;
                }
            else 
                {
                User::Leave(KErrPermissionDenied);
                }    
 
            break;
            }
        default:
            break;
        }
    // add token to array
    TInt64 newToken = User::NTickCount();

    for(newToken = Math::FRand(newToken);
        iTokens.Find(newToken, TToken::CompareToken) != KErrNotFound;
        newToken = User::NTickCount())
        {
        newToken = Math::FRand(newToken);
        }
    
    TToken entry;
    entry.iToken = newToken; 
    entry.iFileName = fileName;
    entry.iOwner = aMessage.Session();

    TPckg<TSecurityPolicy> policyBuf(entry.iPolicy);
    aMessage.ReadL(0, policyBuf);

    User::LeaveIfError(iTokens.Append(entry));
    if (function == EGenerateTokenBasedOnOpenHandle)
        {
        CleanupStack::Pop(); // filename
        }
    }
	
void CFsTokenServer::OpenFileL(const RMessage2& aMessage)
    {
	// find token based on client uid and given token 
    TInt64 token = 0;
    TPckg<TInt64> buf(token); 
    aMessage.ReadL(1,buf);
    
    TInt index = iTokens.Find(token, TToken::CompareToken);
	User::LeaveIfError(index);
	
	if (iTokens[index].iPolicy.CheckPolicy(aMessage))        
	    { // authorized client requests file 
        RFile file;
	    // Todo: what should the mode be ??
	    User::LeaveIfError(file.Open(iFs, *(iTokens[index].iFileName), EFileShareReadersOnly)); 
        CleanupClosePushL(file);
        User::LeaveIfError(file.TransferToClient(aMessage, 0));
        CleanupStack::PopAndDestroy(); //file
        }
    else 
        {
        User::Leave(KErrPermissionDenied);    
        } 
    }


CFsTokenServerSession::CFsTokenServerSession()
    {
    }

CFsTokenServerSession::~CFsTokenServerSession()
    {
    ((CFsTokenServer*)(Server()))->HandleClientExit(this);
    }

void CFsTokenServerSession::ServiceL(const RMessage2& aMessage)
    {
    CFsTokenServer* server = (CFsTokenServer*)( Server() );
    switch(aMessage.Function())
        {
        case EGenerateTokenBasedOnName:
        case EGenerateTokenBasedOnOpenHandle:
        case EGenerateTokenBasedOnAnotherToken:
            {
            server->AddTokenL(aMessage);
            break;
            }
        case EOpenHandleForToken:
            {
            server->OpenFileL(aMessage);
            // Todo: We could set somekind penalty to client that continuously
            // passes wrong tokens (i.e. seems to be guessing), e.g. panic it...
            break;
            }

        default:
            aMessage.Complete(KErrNotSupported);
            break;
        }
    if (!aMessage.IsNull())
        {
        aMessage.Complete(KErrNone);
        }
    }

static void RunServerL()
	{
    // Create Scheduler, obsolete nowdays ?
    CActiveScheduler* s=new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    CFsTokenServer::NewLC(); 
 
    RProcess::Rendezvous(KErrNone);
    
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(2);	
	}


TInt E32Main()
	{
	TInt err = User::RenameThread(FTOKEN_SERVER_NAME);
    if ( err != KErrNone )
        {
        return err;
        }
    
    // create CCleanup
    CTrapCleanup * cleanup = CTrapCleanup::New();
    if (cleanup)
        {
        TRAP(err, RunServerL());
        delete cleanup;
        }

	return err;
	}

