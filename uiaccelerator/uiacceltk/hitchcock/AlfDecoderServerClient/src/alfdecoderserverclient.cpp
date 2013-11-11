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
* Description:   
*
*/


#include <alf/alfappui.h>
#include "alfdecoderserverclient.h"
#include "alfstreamerconsts.h"
#include "alfuids.h"

_LIT(KAlfServerThreadName, "alfredserver");


// Implements just Error() to avoid panic
NONSHARABLE_CLASS(CSimpleScheduler) : public CActiveScheduler
    {
    void Error( TInt ) const{} // From CActiveScheduler
    };


static void RunServerL()
    {
    CActiveScheduler* s=new(ELeave) CSimpleScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    CAlfAppUi* appUiStub = new (ELeave) CAlfAppUi();
    CleanupStack::PushL(appUiStub);
    appUiStub->ConstructL(); 
     
    CActiveScheduler::Start();
        
    CleanupStack::PopAndDestroy(2); 
    }

TInt AlfThreadFucntion(TAny*)
    {
    TInt err = User::RenameThread(KAlfServerThreadName);
    if ( err != KErrNone )
        {
        return err;
        }
    
    User::SetCritical(User::ESystemCritical); // reset the device if we die

    // create CCleanup
    CTrapCleanup * cleanup = CTrapCleanup::New();
    if (cleanup)
        {
        TRAP(err, RunServerL());
        delete cleanup;
        }

    return err;
    }

EXPORT_C void AlfServerStarter::StartL(TRequestStatus& aStatus, TBool aCreateProcess)
	{
    TName serverName;
    TUid appUid = TUid::Uid(KAlfAppServerCoreUid3);
    _LIT(KServerNameFormat, "%08x_%08x_AppServer");
    serverName.Format( 
            KServerNameFormat, 
            appUid, 
            appUid.iUid );
    TFindServer serverFinder(serverName);
    TFullName fullName;
    if (serverFinder.Next(fullName) != KErrNone)
        {
 	    const TUidType serverUid(TUid::Uid(0x1000007a),TUid::Uid(0x100039ce),TUid::Uid(0xdabadaba));

		if (aCreateProcess)
			{
	    	RProcess server;
	    	User::LeaveIfError(server.Create(_L("z:\\sys\\bin\\alfserver.exe"),_L(""),serverUid));
	        server.Rendezvous(aStatus);
	        if (aStatus!=KRequestPending)
	            {
		        server.Kill(0);         // abort startup
	            }
	        else
	            {
		        server.Resume();        // logon OK - start the server
	            }
	        }
	    else
	    	{
    	    RThread serverThread;
    	    User::LeaveIfError(serverThread.Create(
	    	        KAlfServerThreadName,
	    	        AlfThreadFucntion,
	    	        16384, // magic
	    	        4*1024*1024, // uses own heap for now
	    	        10*1024*1024,
	    	        0,
	    	        EOwnerThread));
	    	    
	    	serverThread.Rendezvous(aStatus);
	    	serverThread.Resume();
	    	serverThread.Close();
	    	}
        }
	else
		{
		// Someone else already started the server, weird
		User::Leave(KErrAlreadyExists);
		}
	}

// ---------------------------------------------------------------------------
// Connect
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfBridgerClient::Connect()
    {
	return CreateSession(ALFSTREAMER_SERVER_NAME,TVersion(1,1,1));
    }

// ---------------------------------------------------------------------------
// SendSynch
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfBridgerClient::SendSynch(TInt aOp, const TDesC8& aBuffer)
    {
    return SendReceive(aOp, TIpcArgs(&aBuffer));
    }

// ---------------------------------------------------------------------------
// SendSynch
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfBridgerClient::SendSynch(TInt aOp, const TIpcArgs& aIPCArgs )
    {
    return SendReceive(aOp, aIPCArgs );
    }

// ---------------------------------------------------------------------------
// SendAsyncronous
// ---------------------------------------------------------------------------
//
EXPORT_C void RAlfBridgerClient::SendAsynchronous(TInt aOp, const TIpcArgs& aIPCArgs,TRequestStatus& aStatus )
    {
    SendReceive(aOp, aIPCArgs, aStatus );
    }

TInt RAlfBridgerClient::GetListOfWindowGroups(RArray<TInt>* aWindowGroups, TInt aType)
    {
    TInt err = KErrNone;
    TInt array[10];
    TPtr8 ptr((TUint8*)array,0,40);

    err = SendReceive(EAlfGetListOfWGsHavingInactiveSurfaces, TIpcArgs(&ptr, aType));

    if (!err)
        {
        for(TInt i = 0; i < 10; i++)
            {
            if (array[i])
                {
                aWindowGroups->Append(array[i]);
                }
            else
                {
                break;
                }        
            }    
        }
    return err; 
    }

EXPORT_C TInt RAlfBridgerClient::GetListOfInactiveWindowGroupsWSurfaces(RArray<TInt>* aWindowGroups)
    {
    return GetListOfWindowGroups(aWindowGroups, EAlfInactiveWgs);
    }

EXPORT_C TInt RAlfBridgerClient::GetListOfWindowGroupsWSurfaces(RArray<TInt>* aWindowGroups)
    {
    return GetListOfWindowGroups(aWindowGroups, EAlfAllWgsWithSurface);
    }

EXPORT_C void RAlfBridgerClient::GetOptionalGraphicsMemUsers(RArray<TInt>* aOptionalCandidates)
    {
    GetListOfWindowGroups(aOptionalCandidates, EAlfVolunteersForCommonGood);
    }    


// ---------------------------------------------------------------------------
// EffectsCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfBridgerClient::EffectsCount()
    {
    return SendReceive(EAlfGetNumberOfActiveEffects, TIpcArgs());
    }


// Open
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfTfxClient::Open()
    {
    return RAlfBridgerClient::Connect();
    }
    
// ---------------------------------------------------------------------------
// LoadTfxPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfTfxClient::LoadTfxPlugin(const TUid& aPluginUid)
    {
    return SendReceive(EAlfDecodSLoadPlugin, TIpcArgs(aPluginUid.iUid));
    }
    
// ---------------------------------------------------------------------------
// UnloadTfxPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C void RAlfTfxClient::UnloadTfxPlugin(const TUid& aPluginUid)
    {
    SendReceive(EAlfDecodSUnloadPlugin, TIpcArgs(aPluginUid.iUid));
    }
    
// ---------------------------------------------------------------------------
// SendSynchronousData
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfTfxClient::SendSynchronousData(const TUid& aPluginUid, const TDesC8& aInBuf, TDes8& aOutBuf)
    {
    return SendReceive(EAlfDecodSSendSynch, TIpcArgs(aPluginUid.iUid, &aInBuf, &aOutBuf));
    }

// ---------------------------------------------------------------------------
// SendAsynchronousData
// ---------------------------------------------------------------------------
//
EXPORT_C void RAlfTfxClient::SendAsynchronousData(const TUid& aPluginUid, const TDesC8& aInBuf, TDes8& aOutBuf, TRequestStatus& aStatus)
    {
    SendReceive(EAlfDecodSSendAsynch, TIpcArgs(aPluginUid.iUid, &aInBuf, &aOutBuf), aStatus);    
    }
    
// ---------------------------------------------------------------------------
// CancelAsynchronousData
// ---------------------------------------------------------------------------
//
EXPORT_C void RAlfTfxClient::CancelAsynchronousData(const TUid& aPluginUid,TInt aCommandId)
    {
    SendReceive(EAlfDecodSCancelAsynch, TIpcArgs(aPluginUid.iUid, aCommandId));
    }

// ---------------------------------------------------------------------------
// PrepareFrame
// ---------------------------------------------------------------------------
//
EXPORT_C void RAlfTfxClient::PrepareFrame(TUint aEstimatedFrameInterval)
    {
    Send(EAlfDecodSPrepareFrame, TIpcArgs(aEstimatedFrameInterval));
    }

// ---------------------------------------------------------------------------
// SendSynch
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfBridgerClient::SendBlind(TInt aOp, const TIpcArgs& aIPCArgs )
    {
    return Send(aOp, aIPCArgs);
    }

// end of file
