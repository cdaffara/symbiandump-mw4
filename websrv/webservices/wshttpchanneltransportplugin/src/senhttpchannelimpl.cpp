/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include <uri8.h>
#include <es_sock.h>
#include <in_sock.h>
#include <http.h>

#include "sendebug.h" // filelogging and debugging MACROS
#include "SenServiceConnection.h" // KErrSenNoHttpResponseBody
#include "SenElement.h"
#include "SenXmlUtils.h"
#include "SenHttpTransportProperties.h"
#include "senhttpchannelimpl.h"
#include "senhttpeventhandler.h"
#include "sentxnstate.h"
#include "senatomtxnstate.h"
#include "senrfiletxnstate.h"
#include "senhttpchannel.h"
#include "senlayeredhttptransportproperties.h"
#include "msenidentitymanager.h"
#include "senlogger.h"

#include "senhttpchanneltransportplugin.h"
//For HTTPProxyFilter
#include <httpfilterproxyinterface.h>
#include <httpfilteracceptheaderinterface.h>
#include <httpfiltercommonstringsext.h>
#include <deflatefilterinterface.h>

#ifndef __ENABLE_ALR__
#include <versioninfo.h>  // VersionInfo
#include <extendedconnpref.h>
#endif

// CONSTANTS
namespace
    {
    _LIT(KTxnStateNullPanicText,            "TxnState is NULL");

    // Minimum granularity for array initialization
    const TInt KMinimumArrayGranularity =   1;

#ifdef _SENDEBUG
    // logging constants
    //_LIT(KLogFileDir, "SenHttpChannel");
   // _LIT(KLogFileName, "SenHttpChannel.log");
    _LIT(KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");
    // Size of buffer used when submitting request bodies
    const TInt KMaxHeaderNameLen = 16;
    const TInt KMaxHeaderValueLen = 128;
    const TInt KMaxFilterNameLen = 16;
#endif
    }

//
// Implementation of CSenHttpChannelImpl
//
CSenHttpChannelImpl::CSenHttpChannelImpl(MSenIdentityManager& aManager)
:
    iIapId(KErrNone),
    iManager(aManager),
    iSessionAuthentication(NULL),
    iPasswordFromUser(EFalse),
    iExplicitIapDefined(EFalse),
    iProxyHostPort(NULL),
    iXopResponse(EFalse),
    iHasHttpContentType(ETrue)
#ifndef __ENABLE_ALR__
    , iOCCenabled(EFalse)
#endif    
    {
//    IsOCCImplementedSDK();
    }

CSenHttpChannelImpl* CSenHttpChannelImpl::NewL(MSenIdentityManager& aManager)
    {
    CSenHttpChannelImpl* pNew = NewLC(aManager);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenHttpChannelImpl* CSenHttpChannelImpl::NewLC(MSenIdentityManager& aManager)
    {
    CSenHttpChannelImpl* pNew = new (ELeave) CSenHttpChannelImpl(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenHttpChannelImpl* CSenHttpChannelImpl::NewL(MSenIdentityManager& aManager,
                                               TUint32 aIapId)
    {
    CSenHttpChannelImpl* pNew = NewLC(aManager, aIapId);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenHttpChannelImpl* CSenHttpChannelImpl::NewLC(MSenIdentityManager& aManager,
                                                TUint32 aIapId)
    {
    CSenHttpChannelImpl* pNew = new (ELeave) CSenHttpChannelImpl(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aIapId); 
    return pNew;
    }

#ifndef __ENABLE_ALR__
// On return, aMajor and aMinor contain the version information
TInt CSenHttpChannelImpl::GetS60PlatformVersion(TUint& aMajor, TUint& aMinor)
	{   
	TInt ret = KErrNone;
	// Connect to the file server session
	RFs fsSession;
	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::GetS60PlatformVersion()")));	
	ret = fsSession.Connect();
	if(ret == KErrNone)
		{
		CleanupClosePushL(fsSession); // Obtain the version numberTUint major;
		VersionInfo::TPlatformVersion platformVersion;  
		TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "--- GetS60PlatformVersion getting Version info");		
		ret = VersionInfo::GetVersion(platformVersion, fsSession);   
		CleanupStack::PopAndDestroy();  // fsSession		
		if (ret == KErrNone)       
			{     
			aMajor = platformVersion.iMajorVersion;  
			aMinor = platformVersion.iMinorVersion;   
		   	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("--- GetS60PlatformVersion Version: Major [%d], Minor[%d]"), aMajor, aMinor));
			}		
		}
	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("--- GetS60PlatformVersion returns [%d]"), ret));
	return ret;
	}

// On return, aMajor and aMinor contain the version information
TBool CSenHttpChannelImpl::IsOCCImplementedSDK()
	{   
	TBool occ = EFalse;
	// Obtain the version number
	TUint major;
	TUint minor;	
	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::IsOCCImplementedSDK()")));		
	TInt ret = GetS60PlatformVersion(major, minor);
	if (ret == KErrNone)
		{
		if(major == 5 && minor == 2)
			{
			occ = ETrue;
			iOCCenabled = ETrue;
			TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("---- IsOCCImplementedSDK() returns TRUE")));
			}
		else
			{
			TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("---- IsOCCImplementedSDK() returns False")));
			}
		}
	return occ;
	}
#endif	

// Ask IAP from user
void CSenHttpChannelImpl::ConstructL()
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL()")));
    	
    // Open the RHTTPSession
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - Opening HTTP/TCP session.")));

    iSess.OpenL();

    // Store the string pool for this HTTP session
    iStringPool = iSess.StringPool();
	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - Calling InstallAuthenticationL()")));
    // Install this class as the callback for authentication requests
    InstallAuthenticationL( iSess );
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - Completed InstallAuthenticationL()")));
    //Install Proxy Filter  
    iDeflateFilter = EFalse;

#ifdef EKA2
  //#ifndef _DEBUG 
  	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - Installing  HTTPProxyFilter for EKA2 build.")));
    CHttpFilterProxyInterface::InstallFilterL( iSess );
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - HTTPProxyFilter installed for EKA2 build.")));
    iSess.StringPool().OpenL(HttpFilterCommonStringsExt::GetLanguageTable());
    iSess.StringPool().OpenL(HttpFilterCommonStringsExt::GetTable());
	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - Installing  HTTPAcceptProxyFilter for EKA2 build.")));
    //CHttpFilterAcceptHeaderInterface::InstallFilterL(iSess); //There is issue with installing this filter. so disabling it temporarly
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() - HTTPAcceptProxyFilter installed for EKA2 build.")));
  //#else
  //  LOG_WRITE_L("HTTPProxyFilter is NOT in use with EKA2 debug builds.");
  //#endif
#else // __INSTALL_HTTP_PROXY_FILTER__ is not defined by macro in .mmp
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"HTTPProxyFilter is NOT in use with EKA1.");
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"HTTPAcceptProxyFilter is NOT in use with EKA1.");
#endif // __INSTALL_HTTP_PROXY_FILTER__

    iTransObs = CSenHttpEventHandler::NewL(this);//, &iLog);
    iTxnStates =
        new (ELeave) CArrayPtrFlat<CSenTxnState>(KMinimumArrayGranularity);

    iBasicConnectionTries = 0;
    
#ifndef __ENABLE_ALR__
    IsOCCImplementedSDK();
#endif    

#ifdef _SENDEBUG
    ListFilters();
#endif // _SENDEBUG
	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::ConstructL() Completed")));
    }

// Forces preselected IAP to be used!
void CSenHttpChannelImpl::ConstructL( TUint32 aIapId )
    {
    // Initialize
//    SetupConnectionWithIapPrefsL( aIapId, iConnection, iSockServ );
        
    ConstructL();

//    AttachSocketServerAndConnectionWithHttpSession( aIapId, iConnection, iSockServ );
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::ConstructL - Setting IAP id");


//    Use local variable (new RSocketServer each time; handle is given to RHTTPSession via RConnectionInfo..)
//    RSocketServ server; // with "anon" (local) socket servers, should we keep array of open ones in case of pending txns?
//    SetIapPrefsL(aIapId, iConnection, server);
#ifndef __ENABLE_ALR__
	IsOCCImplementedSDK();
#endif	
	if(aIapId > 0)
		{
    	const TInt result = SetIapPrefsL(aIapId, ETrue, iConnection, iSockServ);
    	User::LeaveIfError( result );
    	}
    }

CSenHttpChannelImpl::~CSenHttpChannelImpl()
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::~CSenHttpChannelImpl()");
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"Closing http session.");
    iSess.Close();
    if (iTransObs) //might be NULL if constructor failed ---
        {
        delete iTransObs;   
        iTransObs = NULL;
        }
    if (iTxnStates) //might be NULL if constructor failed--- 
        {
        iTxnStates->ResetAndDestroy();
        delete iTxnStates;
        iTxnStates = NULL;
        }

    if(iProxyHostPort)
        {
        delete iProxyHostPort;
        iProxyHostPort = NULL;
        }
    
    if(iMultiPartContentType.params.Count())
        {
        iMultiPartContentType.params.Close();
        }
    if(iDeflateFilter)
        {
        REComSession::DestroyedImplementation(KDeflateFilterUid); 	
        }
        
    iConnection.Close();
    iSockServ.Close();
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::~CSenHttpChannelImpl() Completed");
    }

TInt CSenHttpChannelImpl::SetIapPrefsL( TUint32 aIapId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer )
   	{
   	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- SetIapPrefsL	, aIapId (%d)"), aIapId));
   	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- SetIapPrefsL	, iIapId (%d)"), iIapId));
   	
    // Check whether IAP ID is not equal with the one that is currently in effect:
    if(iExplicitIapDefined || iIapId == aIapId && iIapId > 0)
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetIapPrefsL: Iap Id is same as currently in effect");
        return KErrNone;
        }
	else
		{
		TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetIapPrefsL: Iap Id different with the currently in effect");
		}    
    

    // Check if socket server (connection) is already open..
    if( iExplicitIapDefined )
        {
         // Socket server opened once before for some other IAP
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetIapPrefsL: Re-using existing RConnection => calling RConnection::Stop");
        aConnection.Stop();
        }
    else
        {
        if(aConnection.SubSessionHandle())
        	{
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Re-using existing RConnection => calling RConnection::Stop");
        	aConnection.Stop();
        	}
        else
            {
            // Connect to a socket server    
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetIapPrefsL: Connecting to new socket server");    
            User::LeaveIfError( aSocketServer.Connect() );       
    
            // Open new connection
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetIapPrefsL: Opening new RConnection using the socket server.");       
            User::LeaveIfError( aConnection.Open(aSocketServer) );        
            }
        }
        
    // Set the IAP selection preferences (IAP ID, do not prompt)    
	TInt retVal = SetID(aIapId, aDialogPref, aConnection, aSocketServer);	        
	
    // Get the connection "handle" from the HTTP session    
    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
     
    // Attach socket server
    connInfo.SetPropertyL(iStringPool.StringF(  HTTP::EHttpSocketServ,
                                                RHTTPSession::GetTable()),
                                                THTTPHdrVal(aSocketServer.Handle()));
    
    // Attach connection  
    TInt connPtr = REINTERPRET_CAST(TInt, &aConnection);
    connInfo.SetPropertyL(iStringPool.StringF(  HTTP::EHttpSocketConnection, 
                                                RHTTPSession::GetTable()), 
                                                THTTPHdrVal(connPtr));                                                    
    return retVal;
    }

TInt CSenHttpChannelImpl::SetID(TUint32 aId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer, TBool aSNAP)
{
   /*
    * Single click connectivity feature has been implemented by CSock (RConnection class).
    * According to this client no need to set the IAP ID. 
    * Automatically RConnection will use the suitable IAP
    */
    TInt retVal = KErrNone;
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl  called with ID [%d]"), aId));
	    
#ifndef __ENABLE_ALR__	    
	if (iOCCenabled == EFalse)	    
		{
#endif		
		if (aSNAP == EFalse)
			{
			TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- SetIAPID");
		    TCommDbConnPref pref;		
			pref.SetIapId( aId );
			TCommDbDialogPref dialogPref;
		    if (aDialogPref)
		        {
		        dialogPref = ECommDbDialogPrefDoNotPrompt;
		        }
		    else
		        {
		        dialogPref = ECommDbDialogPrefPrompt;
		        }
	    	pref.SetDialogPreference(dialogPref);
	    	retVal = aConnection.Start(pref);
			TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("RConnection->Start retVal [%d]"), retVal));
			}
		else
			{
			TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- SetSNAPID");
			// Set the SNAP selection preferences (SNAP ID)
		    TConnSnapPref pref;
		    pref.SetSnap(aId);		    
		    // Start the connection with the new preferences
		    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Calling RConnection::Start with new SNAP prefs");
		    TInt retVal = aConnection.Start(pref);
			}
#ifndef __ENABLE_ALR__			
		}   
	else
		{
		if (aSNAP == EFalse)
			{
			/* If IAP id is zero, it is interpreted as
		    * client's request for not requesting any specific IAP.
		    * Default value is 0.
		    * 
		    * If IAP id is set, SNAP id shall be zero.
		    * If IAP id is set, SNAP purpose shall be CMManager::ESnapPurposeUnknown.
		    * If IAP id is set, Connection selection dialog shall be disabled.
		    * If IAP id is set, bearer set shall be EExtendedConnBearerUnknown.
		    * If IAP id is set, forced roaming is disabled automatically.
		    * Either SNAP purpose, SNAP id, or IAP id shall be given, or Connection
		    * selection dialog shall be enabled.
		    */
		    
		    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- Setting up OCC with IAP Settings");
			TConnPrefList prefList;
			TExtendedConnPref extPrefs;
			//extPrefs.SetSnapPurpose( CMManager::ESnapPurposeUnknown);
			//TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- Setting up OCC Silent Connection");
			//extPrefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
			//extPrefs.SetBearerSet(TExtendedConnBearer::EExtendedConnBearerUnknown);
			if(aId != 0)
				{
				extPrefs.SetIapId(aId);
				extPrefs.SetSnapId(0);
				}
			else
				{
				extPrefs.SetSnapPurpose(CMManager::ESnapPurposeInternet);
				extPrefs.SetConnSelectionDialog(ETrue);
				}
			prefList.AppendL(&extPrefs);
			retVal = aConnection.Start(prefList);
			if(retVal == KErrNone)
			    {
			    if(aId == 0)
			    	{
                    aConnection.GetIntSetting( _L("IAP\\Id"), iIapId);
                    TName name;
                    retVal = aConnection.Name(name);
                    if(retVal == KErrNone)
                        {
                        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("IAP Name [%S]"), &name));
                        }
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("GetIntSetting returned IAP [%d]"), iIapId));			    
			    	}
			    else
			    	{	
			    	iIapId = aId;
			    	}
			    }
			 else
			    {
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("IAP ->Start retVal [%d]"), retVal));
			    }
		    }
		else
		    {
		    /**
		    * Sets SNAP id. If SNAP id is zero, it is interpreted as 
		    * client's request for not requesting any specific SNAP.
		    * Default value is 0.
		    *
		    * If SNAP id is set, IAP id shall be zero.
		    * If SNAP id is set, SNAP purpose shall be CMManager::ESnapPurposeUnknown.
		    * If SNAP id is set, Connection selection dialog shall be disabled.
		    * Either SNAP purpose, SNAP id, or IAP id shall be given, or Connection
		    * selection dialog shall be enabled.
		    */
			TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- Setting up OCC with SNAP Settings");
			TConnPrefList prefList;
			TExtendedConnPref extPrefs;
			//extPrefs.SetSnapPurpose( CMManager::ESnapPurposeUnknown);
			//TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- Setting up OCC Silent Connection");
			//extPrefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
			//extPrefs.SetBearerSet(TExtendedConnBearer::EExtendedConnBearerUnknown);
			if(aId == 0)
				{
				TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- Setting SnapPurpose Internet");
				if(aDialogPref) //If it fails because of unavailibility of access points
					{
					TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "- Fails because of unavailibility of access points");
					extPrefs.SetSnapPurpose(CMManager::ESnapPurposeUnknown);
					extPrefs.SetConnSelectionDialog(ETrue);
					}
				else
					{
					extPrefs.SetSnapPurpose(CMManager::ESnapPurposeInternet);
					//extPrefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);					
					}
				}
			else
				{
				extPrefs.SetSnapId(aId);
				extPrefs.SetIapId(0);	
				extPrefs.SetSnapPurpose(CMManager::ESnapPurposeUnknown);
				extPrefs.SetConnSelectionDialog(EFalse);
				}
			prefList.AppendL(&extPrefs);
			TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel, "SetID: RConnection Start");
			retVal = aConnection.Start(prefList);
			TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("RConnection Start retval[%d]"), retVal));
			if(retVal == KErrNone)
				{
				aConnection.GetIntSetting( _L("IAP\\Id"), iIapId);
				TName name;
				retVal = aConnection.Name(name);
				if(retVal == KErrNone)
					{
					TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("IAP Name [%S]"), &name));
					}
				TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("GetIntSetting returned IAP [%d]"), iIapId));
				}
			 else
			    {
				TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("SNAP ->Start retVal [%d]"), retVal));
			    }				
		    }
		}    
#endif    
    if (!retVal && !aSNAP)
        {
        iExplicitIapDefined = ETrue;
        }	
	return retVal;
}

TInt CSenHttpChannelImpl::SetSnapPrefsL( TUint32 aSnapId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer )
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- CSenHttpChannelImpl::SetSnapPrefsL, SNAP ID (%d)"), aSnapId));
		// Check whether SNAP ID is not equal with the one that is currently in effect:
    if(iExplicitIapDefined && iSnapId == aSnapId)
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Sanp is same as currently in effect");
        return KErrNone;
        }
	else
		{
		TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Sanp is different with currently in effect");
		}
	
    // Check if socket server (connection) is already open..
    if( iExplicitIapDefined )
        {
         // Socket server opened once before for some other IAP
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Re-using existing RConnection => calling RConnection::Stop");
        aConnection.Stop();
        }
    else
        {
        if(aConnection.SubSessionHandle())
        	{
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Re-using existing RConnection => calling RConnection::Stop");
        	aConnection.Stop();
        	}
        else
            {
            // Connect to a socket server    
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Connecting to new socket server");    
            User::LeaveIfError( aSocketServer.Connect() );       
    
            // Open new connection
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- SetSnapPrefsL: Opening new RConnection using the socket server.");       
            User::LeaveIfError( aConnection.Open(aSocketServer) );         
            }
        }
        
        
	// Set the SNAP selection preferences (SNAP ID, do not prompt)    
	TInt retVal = SetID(aSnapId, aDialogPref, aConnection, aSocketServer, ETrue);
    
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- SetSnapPrefsL, RConnection::Start returned: (%d)"), retVal));
    
    // Get the connection "handle" from the HTTP session    
    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
     
    // Attach socket server
    connInfo.SetPropertyL(iStringPool.StringF(  HTTP::EHttpSocketServ,
                                                RHTTPSession::GetTable()),
                                                THTTPHdrVal(aSocketServer.Handle()));
    
    // Attach connection  
    TInt connPtr = REINTERPRET_CAST(TInt, &aConnection);
    connInfo.SetPropertyL(iStringPool.StringF(  HTTP::EHttpSocketConnection, 
                                                RHTTPSession::GetTable()), 
                                                THTTPHdrVal(connPtr));
                                                
    // Remember the SNAP id that is being set, because *at the moment*,
    // there is NO direct API to query effective SNAP ID from CommsDB.
    if (!retVal)
        {
        //iExplicitIapDefined = ETrue;
        iSnapId = aSnapId;
        }
    return retVal;
    }

TInt CSenHttpChannelImpl::SendL( MSenResponseObserver& aObserver,
                                 const TDesC8& aUri,
                                 const TDesC8& aContent,
                                 CSenLayeredHttpTransportProperties& aProps )
    {
#ifdef _SENDEBUG
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::SendL:");
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- Endpoint URI: %S"), &aUri));
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"- Content(msg):");
    TLSLOG_ALL(KSenHttpChannelLogChannelBase , KMaxLogLevel,(aContent));
#endif

    TPtrC8 contentType;
    TInt retVal = aProps.ContentTypeL(contentType);
    if(retVal!=KErrNone)
        {
        // Use the default
        contentType.Set(KDefaultContentType);
        }

    CSenTxnState* pTxnState = CSenTxnState::NewL(aObserver,
                                                 //Log(),
                                                 &aUri,
                                                 contentType,
                                                 &aContent);

    CleanupStack::PushL(pTxnState);
    AppendNewTxnStateL(pTxnState);
    CleanupStack::Pop(); // pTxnState

    return InvokeHttpMethodL(pTxnState, aProps);
    }
    
TInt CSenHttpChannelImpl::SendL( MSenResponseObserver& aObserver,
                                 const TDesC8& aUri,
                                 const RFile& aFile,
                                 CSenLayeredHttpTransportProperties& aProps )
    {
#ifdef _SENDEBUG
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::SendL:");
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- Endpoint URI: %S"), &aUri));
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"- Content(msg):");
#endif

    TPtrC8 contentType;
    TInt retVal = aProps.ContentTypeL(contentType);
    if(retVal != KErrNone)
        {
        // Use the default
        contentType.Set(KDefaultContentType);
        }

    CSenRfileTxnState* pTxnState = CSenRfileTxnState::NewL(aObserver,
                                                 //Log(),
                                                 &aUri,
                                                 this,
                                                 contentType,
                                                 aFile);

    CleanupStack::PushL(pTxnState);
    AppendNewTxnStateL(pTxnState);
    CleanupStack::Pop(); // pTxnState

    return InvokeHttpMethodL(pTxnState, aProps);
    }
    
    
TInt CSenHttpChannelImpl::SendL(MSenResponseObserver& aObserver,
                           	    const TDesC8& aUri,
                               	CSenSoapEnvelope2& aContent, 
                               	CSenLayeredHttpTransportProperties& aProps)
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::SendL:");
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- Endpoint URI: %S"), &aUri));
    
    TPtrC8 soapAction;
    TInt retVal = aProps.SoapActionL(soapAction);
    
    CSenMtomTxnState* pMtomTxnState = CSenMtomTxnState::NewL(aObserver,
                                             				 //Log(),
                                             				 &aUri,
                                             				 soapAction,
                                             				 aContent);
                                             				 
    if (aContent.SoapVersion() == ESOAP12)
      {
    aProps.RemovePropertyL(KSoapActionLocalName, MSenLayeredProperties::ESenMessageLayer);
      }

	CleanupStack::PushL(pMtomTxnState);
	AppendNewTxnStateL(pMtomTxnState);
    CleanupStack::Pop(); // pMtomTxnState

	return InvokeHttpMethodL(pMtomTxnState, aProps);
    }
TInt CSenHttpChannelImpl::SendL(MSenResponseObserver& aObserver,
                           	    const TDesC8& aUri,
                               	CSenAtomEntry& aContent, 
                               	CSenLayeredHttpTransportProperties& aProps)
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::SendL:");
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- Endpoint URI: %S"), &aUri));
    CSenAtomTxnState* pAtomTxnState = CSenAtomTxnState::NewL(aObserver,
                                             				 &aUri,
                                             				 aContent);
                                             				 
    CleanupStack::PushL(pAtomTxnState);
	AppendNewTxnStateL(pAtomTxnState);
    CleanupStack::Pop(); // pAtomTxnState

	return InvokeHttpMethodL(pAtomTxnState, aProps);
    }    
/** Invoke the http method
This actually creates the transaction,
sets the headers and body and then starts the transaction
*/
TInt CSenHttpChannelImpl::InvokeHttpMethodL(CSenTxnState* aTxnState,
                                            CSenLayeredHttpTransportProperties& aProps)
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::InvokeHttpMethodL");
    TUriParser8 uri;
    RStringF method;
    TPtrC8 deflate;
    // Set IAP preferences, if such exist in properties (and not already in effect):
    TUint32 id(KErrNone);
    TBool prompt(ETrue); 
    
    TInt retVal = aProps.IAPDialogL( prompt );
    if ( retVal != KErrNone )
        {
        // by default, do not prompt (even if property does not exist!) 
        // => only if property is set, and has value "FALSE", show PROMPT            
        prompt = ETrue;
        }
	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::InvokeHttpMethodL - iIapId [%d]"), iIapId)); 
#ifndef __ENABLE_ALR__
		TInt propRetVal = aProps.IapIdL(id);
    // Independent of dialog preference (property's existance), if IAP was predefined, it must be set        
    if(propRetVal == KErrNone)
        {
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::InvokeHttpMethodL SetIapPrefs - id [%d]"), id));
        retVal = SetIapPrefsL(id, prompt, iConnection, iSockServ);
        }
    else if(iIapId > 0)
        {
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::InvokeHttpMethodL SetIapPrefs - iIapId [%d]"), iIapId)); 
        retVal = SetIapPrefsL(iIapId, prompt, iConnection, iSockServ);
        }
    else if(((aProps.SnapIdL(id)) == KErrNone))
        {
        retVal = SetSnapPrefsL(id, prompt, iConnection, iSockServ);
        }
    else//to better control RConnection, we have to call Start by ourselve
        {
        retVal = SetSnapPrefsL(0, EFalse, iConnection, iSockServ);
        if(retVal == KErrNotFound)
            {
            if(iOCCenabled == EFalse)
		        {
	    	    retVal = SetIapPrefsL(0, EFalse, iConnection, iSockServ);	
	        	}
        	else
	        	{
	        	retVal = SetSnapPrefsL(0, prompt, iConnection, iSockServ);	
	        	}
            }
        }
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Set Snap/IAP prefs retVal [%d]"), retVal));    
    User::LeaveIfError(retVal);
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::InvokeHttpMethodL After User::Leave");
    //TInt ret= iConnection.GetIntSetting(_L("IAP\\Id"), iUsedIapId);
    iUsedIapId = iIapId;
    // Check transport properties
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("IAP/SNAP %d"), id));
#else
	retVal = SetIapPrefsL(id, prompt, iConnection, iSockServ);
	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("IAP/SNAP %d"), id));	
	iUsedIapId = id;
#endif
    
    CSenLayeredHttpTransportProperties::TSenHttpMethod httpMethod;
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("PROMPT %d"), prompt));
    TInt retValHttpMethod = aProps.HttpMethodL(httpMethod);
    
    
    /*Installing the deflate filter based on the property values */
    /*Once installed, the filter will be alive for on session lifetime */   
    if(KErrNone == aProps.PropertyL(KWsPropertyValueHttpFilterDeflate,deflate))
       {
        if(!iDeflateFilter)
          {
           TRAPD(err, CHttpDeflateFilter::InstallFilterL(iSess));
           if(err == KErrNone) 
           	{
		     		iDeflateFilter = ETrue;  
	          TLSLOG_L(KSenHttpChannelLogChannelBase , KNormalLogLevel,"HTTPDeflateFilter installed for EKA2 build.");	
						}
					else
		     		{
		      	TLSLOG_L(KSenHttpChannelLogChannelBase , KNormalLogLevel,"ERROR:HTTPDeflateFilter Not installed for EKA2 build.");
		     		}
          }
       }


    if(!aTxnState->HasRequestBody())
        {
        if (retValHttpMethod) httpMethod = CSenLayeredHttpTransportProperties::ESenHttpGet;
        User::LeaveIfError( uri.Parse(aTxnState->RequestUri()) );
        switch(httpMethod)
            {
            case CSenLayeredHttpTransportProperties::ESenHttpPost:
                {
                method = iStringPool.StringF(HTTP::EPOST, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EPOST");
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttpPut:
                {
                method = iStringPool.StringF(HTTP::EPUT, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EPUT");
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttpDelete:
                {
                method = iStringPool.StringF(HTTP::EDELETE, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EDELETE");
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttpGet:
            default:
                {
                // Default to HTTP GET, if no body has been provided
                method = iStringPool.StringF(HTTP::EGET, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EGET");
                }
            }
        }
    else 
        {   
        // Some request "body"; or a query string for GET or DELETE was provided
        if (retValHttpMethod) httpMethod = CSenLayeredHttpTransportProperties::ESenHttpPost;
        switch(httpMethod)
            {
            case CSenLayeredHttpTransportProperties::ESenHttpGet:
                {
                method = iStringPool.StringF(HTTP::EGET, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EGET");
                // The responsibility of correct query string for GET URI 
                // is on the shoulders of the original invoker (public API
                // caller):
                aTxnState->TransformBodyToUriL();

                // NOTE: warning from Uri16.h:
                // @warning The descriptor that is parsed by an object of this class will be referenced 
                // by that object. If the original descriptor is no longer in scope there will be undefined 
                // behaviour.
                User::LeaveIfError( uri.Parse(aTxnState->RequestUri()) );
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttpPut:
                {
                method = iStringPool.StringF(HTTP::EPUT, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EPUT");
                User::LeaveIfError( uri.Parse(aTxnState->RequestUri()) );
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttpDelete:
                {
                method = iStringPool.StringF(HTTP::EDELETE, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EDELETE");
                // The responsibility of correct query string for DELETE URI 
                // argument is on the shoulders of the original invoker 
                // (public API caller):
                aTxnState->TransformBodyToUriL();
                User::LeaveIfError( uri.Parse(aTxnState->RequestUri()) );
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttpPost:
            default:
                {
                // default to HttpPost
                method = iStringPool.StringF(HTTP::EPOST, RHTTPSession::GetTable());
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- HTTP::EPOST");
                User::LeaveIfError( uri.Parse(aTxnState->RequestUri()) );
                }
            }
        }


    RHTTPTransaction transaction = iSess.OpenTransactionL(uri, *iTransObs, method);
    iHttpTransaction = transaction ;
    aTxnState->SetId(transaction.Id());
    aTxnState->SetTransaction(transaction);
    aTxnState->SetSession(iSess); 
    RHTTPHeaders hdr = transaction.Request().GetHeaderCollection();

    // Add request headers: Accept, User-Agent, any other user defined ones
    AddRequestHeadersL(hdr, aProps);

    // Add Content type header and request body if available
    if(aTxnState->HasRequestBody())
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- Adding content type header and request body");

        // Set the "Content-Type" HTTP header here, and *only* here(!)
        aTxnState->SetContentTypeHeaderL(iSess, hdr);

        MHTTPDataSupplier* pDataSupplier = aTxnState;
        transaction.Request().SetBody(*pDataSupplier);
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- This transaction does not carry a request body.");
        }
#endif

    // Submit the transaction
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- Submitting the transaction.");
    transaction.SubmitL();
    return aTxnState->Id();
    }

void CSenHttpChannelImpl::NotifyMoreBodyL()    
{
	iHttpTransaction.NotifyNewRequestBodyPartL();
}

void CSenHttpChannelImpl::SetProxyL(const TDesC8& aProxyHostBaseAddr, 
                                    TInt aProxyPort)
    {
    TBuf8<64> port;
    port.AppendNum(aProxyPort);
    HBufC8* pProxyHostPort = HBufC8::NewLC(aProxyHostBaseAddr.Length()
                                           +KColon().Length()
                                           +port.Length());

    pProxyHostPort->Des().Append( aProxyHostBaseAddr );
    pProxyHostPort->Des().Append( KColon );
    pProxyHostPort->Des().AppendNum( aProxyPort );

    TPtrC8 proxy = pProxyHostPort->Des();
    SetProxyL( proxy );
    CleanupStack::PopAndDestroy(pProxyHostPort);
    }

void CSenHttpChannelImpl::SetProxyL(const TDesC8& aProxyAddrStr)
    {
    if(iProxyHostPort && iProxyHostPort->Compare(aProxyAddrStr)==0)
        {
        // nothing to do, proxy address is already in use
        return;
        }

    delete iProxyHostPort;
    iProxyHostPort = NULL;

    iProxyHostPort = aProxyAddrStr.AllocL();

    RStringF proxyAddr = iSess.StringPool().OpenFStringL(*iProxyHostPort);

    CleanupClosePushL(proxyAddr);

    // Set the proxy here...
    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
    THTTPHdrVal proxyUsage(
        iSess.StringPool().StringF(HTTP::EUseProxy, RHTTPSession::GetTable()));
    connInfo.SetPropertyL(
        iSess.StringPool().StringF(HTTP::EProxyUsage, RHTTPSession::GetTable()),
                                    proxyUsage);
    connInfo.SetPropertyL(
        iSess.StringPool().StringF(
                    HTTP::EProxyAddress, RHTTPSession::GetTable()), proxyAddr);

    CleanupStack::PopAndDestroy(); // proxyAddr
    }

void CSenHttpChannelImpl::SetHttpVersionL(TInt aVersion)
    {
    HTTP::TStrings httpVersion = HTTP::EHttp11;
    if(0 == aVersion)
        {
        httpVersion = HTTP::EHttp10;
        }

    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
    RStringPool p=iSess.StringPool();
    connInfo.SetPropertyL(p.StringF(
                            HTTP::EHTTPVersion,RHTTPSession::GetTable()),
                          THTTPHdrVal(
                          p.StringF(httpVersion,RHTTPSession::GetTable())));
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KNormalLogLevel, _L8("Http version is set to HTTP 1.%d"), aVersion));
    }

void CSenHttpChannelImpl::SetProxyUsageL(TBool aProxyUsage)
    {
    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
    RStringPool strPool=iSess.StringPool();
    if ( aProxyUsage )
        {
        connInfo.SetPropertyL
            ( 
            strPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ), 
            THTTPHdrVal( strPool.StringF(HTTP::EUseProxy, RHTTPSession::GetTable() ) )
            );    
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Http Proxy usage is set to TRUE")));
        }
    else
        {
        connInfo.SetPropertyL
            ( 
            strPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ), 
            THTTPHdrVal( strPool.StringF(HTTP::EDoNotUseProxy, RHTTPSession::GetTable() ) )
            );    
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Http Proxy usage is set to FALSE")));
        
        }
    }

void CSenHttpChannelImpl::SetSecureDialogL(TBool aSecureDialog)
    {
    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
    RStringPool strPool=iSess.StringPool();
    if ( aSecureDialog )
        {
        connInfo.SetPropertyL
            ( 
            strPool.StringF( HTTP::ESecureDialog, RHTTPSession::GetTable() ), 
            THTTPHdrVal( strPool.StringF(HTTP::EDialogPrompt, RHTTPSession::GetTable() ) )
            );    
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Http Secure dialog prompt is set to TRUE")));
        }
    else
        {
        connInfo.SetPropertyL
            ( 
            strPool.StringF( HTTP::ESecureDialog, RHTTPSession::GetTable() ), 
            THTTPHdrVal( strPool.StringF(HTTP::EDialogNoPrompt, RHTTPSession::GetTable() ) )
            );    
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Http Secure dialog prompt is set to FALSE")));
        
        }
    }
    
// ----------------------------------------------------------------------------
// CSenHttpChannelImpl::AddRequestHeadersL
// Adds headers to the request.
// ----------------------------------------------------------------------------
void CSenHttpChannelImpl::AddRequestHeadersL(RHTTPHeaders& aHeaders,
                                             CSenLayeredHttpTransportProperties& aProps)
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::AddRequestHeadersL")));


    //TPtrC8 useragent;
    TInt retVal;// = aProps.UserAgentL(useragent);

    // It is MANDATORY, that User-Agent header exists:
 //   if ( useragent.Length()== 0 )
 //       {
 //       //iProperties->SetPropertyL(KUserAgentLocalName, KSenHttpChannelUserAgentHeaderDefault, KHttpHeaderType);            
 //       useragent.Set( KSenHttpChannelUserAgentHeaderDefault );
 //       }

    // Fetch the accepted content types:
    MSenProperty* acceptHeaders = NULL;
    retVal = aProps.AcceptL(acceptHeaders);
    RPointerArray<TPtrC8> tokens;
    CleanupClosePushL(tokens);

    if(retVal == KErrNone)
        {
        if(acceptHeaders)
            {
            TInt retVal = acceptHeaders->ValueTokensL(KSenHttpAcceptHeaderDelimiter, tokens);
            // Sanity check
            if(retVal == KErrNone && tokens.Count()==0)
                {
                retVal = KErrNotFound; // should not happen
                }
            }
        else
            {
            retVal = KErrNotFound; // should not happen
            }
        }

    // It is MANDATORY, that at least one Accept header TOKEN exists:
  if ( tokens.Count()==0 )
        {
        TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::AddRequestHeadersL Default Token")));
        TPtrC8* pDefaultToken = new (ELeave) TPtrC8( KSenHttpChannelAcceptHeaderDefault );
        TInt error = tokens.Append( pDefaultToken );
        if ( error )
            {
            delete pDefaultToken;
            pDefaultToken = NULL;
            }
        }
    // Http headers
    TInt headerCount(0);
    RPointerArray<MSenProperty> headersArray;
    retVal = aProps.HttpHeadersL(headersArray);
    if ( retVal == KErrNone )
        {
        CleanupClosePushL(headersArray);
        headerCount = headersArray.Count();
        for(TInt i=0; i<headerCount; i++)
            {
            MSenProperty* property = headersArray[i];
            if(property)
                {
                TPtrC8 name = property->Name();

                // Check if header name is "Content-Type"
                if(!name.Compare(KContentTypeLocalName) ||
                   !name.Compare(KAcceptLocalName))
                    {
                    // Skip Content-Type header(s) in here:
                    // - otherwise HTTP GET might get confused

                    // NOTE(!): Content-Type header is added separately
                    // through CSenTxnState -object:
                    // @InvokeHttpMethodL()
                    continue;
                    }

                TPtrC8 value = property->Value();
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- Adding HTTP HEADER, name: (%S), value: (%S)"), &name, &value));

                // Open stringpool strings
                RStringF headerName = iStringPool.OpenFStringL(name);
                CleanupClosePushL(headerName);
                RStringF headerValue = iStringPool.OpenFStringL(value);
                CleanupClosePushL(headerValue);
                // Create header name field
                THTTPHdrVal headerFieldVal;
                headerFieldVal.SetStrF(headerValue);
                // Check if the header field value already exists
                TBool fieldValueExists = EFalse;
                TInt fieldCount = aHeaders.FieldPartsL(headerName);
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Header < %S > field count: %d"), &name, fieldCount));

                for(TInt j=0; j<fieldCount; j++)
                    {
                    THTTPHdrVal hVal;
                    TInt retCode = aHeaders.GetField(headerName, j, hVal);
                    if(KErrNotFound != retCode)
                        {
                        if(hVal == headerFieldVal)
                            {
                            TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Header < %S: %S > already exists"),
                                               &name, &value));

                            fieldValueExists = ETrue;
                            }
                        }
                    }

                // Add header field
                if(!fieldValueExists)
                    {
                    aHeaders.SetFieldL(headerName, headerFieldVal);
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Header < %S: %S > added"),&name, &value));
                    }

                // Close stringpool strings
                CleanupStack::PopAndDestroy(2); // headerValue, headerName
                }
            }
        headersArray.Reset();
        CleanupStack::Pop(); // headersArray
        }
    else
        {
        // No HTTP headers were found(!)
        User::Leave( retVal );
        }

    for (TInt k = 0; k < tokens.Count(); k++)
        {
        TPtrC8 token = *tokens[k]; // accept header value
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("- Adding token to Accept header (%S)"), tokens[k]));

        //const TDesC8& acceptHeaderValueStr
        //        = iAcceptedContentTypes->MdcaPoint(k);

        // Open stringpool string
        RStringF acceptHeaderValue 
            = iStringPool.OpenFStringL(token);

        CleanupClosePushL(acceptHeaderValue);

        // Create header name field
        THTTPHdrVal headerFieldVal;
        headerFieldVal.SetStrF(acceptHeaderValue);

        // Check if the header field value already exists
        TBool fieldValueExists = EFalse;

        TInt fieldCount = aHeaders.FieldPartsL(
            iStringPool.StringF(HTTP::EAccept, RHTTPSession::GetTable()));
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KNormalLogLevel, _L8("Accept header field count: %d"), fieldCount));


        for(TInt j = 0; j < fieldCount; j++)
            {
            THTTPHdrVal hVal;
            TInt retCode = aHeaders.GetField(
                iStringPool.StringF(HTTP::EAccept, 
                                    RHTTPSession::GetTable()),
                                    j, 
                                    hVal);

            if(KErrNotFound != retCode)
                {
#ifdef _SENDEBUG
                const TDesC8& hValStr = hVal.StrF().DesC();
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Accept: %S"), &hValStr));
#endif // _SENDEBUG
                if(hVal == headerFieldVal)
                    {
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Header < Accept: %S > already exists"), &token));
                    fieldValueExists = ETrue;
                    }
                }
            }

        // Add header field
        if(!fieldValueExists)
            {
            aHeaders.SetFieldL(
                iStringPool.StringF(HTTP::EAccept, 
                                    RHTTPSession::GetTable()),
                                    headerFieldVal);
            TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Header < Accept: %S > added"), &token));

            }

        // Close stringpool string
        CleanupStack::PopAndDestroy(); // acceptHeaderValue
        }
    tokens.ResetAndDestroy(); // TPtrC8 is allocated with "new" keyword and thus owned by this array, eventhough
                              // the actual descriptors inside TPtrC8 tokens are NOT owned, of course (NORMAL).
    CleanupStack::PopAndDestroy(); // token array
    }

/** Called when a authenticated page is requested
Asks the user for a username and password that would be appropriate for the
url that was supplied.
*/
TBool CSenHttpChannelImpl::GetCredentialsL(const TUriC8& aURI,
                                           RString aRealm,
                                           RStringF /*aAuthenticationType*/,
                                           RString& aUsername,
                                           RString& aPassword)

    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("GetCredentialsL(),  for URL (%S), realm (%S)"),
                            &aURI.UriDes(), &aRealm.DesC()));

    if (iBasicConnectionTries > 2)
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() max number of tries with auth-dialog reached (3)");
        // In the next attempt start with a fresh identityprovider
        iSessionAuthentication = NULL;
        iPasswordFromUser = EFalse;
        iBasicConnectionTries = 0;
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() returning EFalse");
        return EFalse;  // Invalid login attempts have reached the defined 
                        // maximum, abort this request
        }

    if (!iSessionAuthentication)
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() reading auth-pair from database (senidentites.xml)");
        iSessionAuthentication = iManager.IdentityProviderL(aURI.UriDes());
        if(!iSessionAuthentication)
            {
            // create and register new IDP
            iSessionAuthentication = CSenIdentityProvider::NewL(aURI.UriDes(),
                                                                KNullDesC8);
            iSessionAuthentication->SetFrameworkIdL(KNullDesC8); 
            TInt retVal = iManager.RegisterIdentityProviderL(iSessionAuthentication);
            if(retVal!=KErrNone)
                {
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("GetCredentials: Register IDP failed! Error code: (%d)"),
                    retVal));
                iSessionAuthentication = NULL;
                TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() returning EFalse");
                return EFalse; // decision: we could not save info into database, abort
                }
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"New IDP registeration OK. Proceeding.");
            }
        // we have credentials
        TRAPD(err, aUsername = 
            aRealm.Pool().OpenStringL(iSessionAuthentication->AuthzID()));
        TRAP(err, aPassword = 
            aRealm.Pool().OpenStringL(iSessionAuthentication->Password()));
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() returning ETrue");
        return ETrue;
        }
    else
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() prompting auth-info from user via dialog.");
        // Remove the old password before asking the new one.
        CSenElement& element = iSessionAuthentication->AsElement();
        delete element.RemoveElement(KSenIdpPasswordLocalname);
        
        TPckgBuf<TSenAuthentication> authInfo;
        iManager.AuthenticationForL(*iSessionAuthentication, authInfo);
        iPasswordFromUser = ETrue;

        TRAPD(err, aUsername = 
                aRealm.Pool().OpenStringL(authInfo().iUsername));
        TRAP(err, aPassword = 
                aRealm.Pool().OpenStringL(authInfo().iPassword));
        iBasicConnectionTries++;
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"GetCredentialsL() returning ETrue");
        return ETrue;
        }
    }

// This is a debug logging method:
void CSenHttpChannelImpl::ListFilters()
    {
#ifdef _SENDEBUG
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"List HTTP filters started.");
    TLSLOG(KSenHttpChannelLogChannelBase , KMaxLogLevel,(_L(
    "      Name       | Pos'n | Event  |      Header      | Status | Handle ")
    ));
    TLSLOG(KSenHttpChannelLogChannelBase , KMaxLogLevel,(_L(
    "-----------------+-------+--------+------------------+--------+--------")
    ));

    RHTTPFilterCollection filtColl = iSess.FilterCollection();
    THTTPFilterIterator iter = filtColl.Query();

    THTTPFilterRegistration regInfo;
    iter.First();
    TBuf<KMaxFilterNameLen> earlierName;
    while (!iter.AtEnd())
        {
        // Get next filter registration info
        regInfo = iter();

        TBuf<KMaxFilterNameLen> name;
        name.Copy(iSess.StringPool().StringF(regInfo.iName).DesC().Left(
                                                        KMaxFilterNameLen));
        TBuf<KMaxHeaderNameLen> header;
        header.Copy(iSess.StringPool().StringF(regInfo.iHeader).DesC().Left(
                                                        KMaxHeaderNameLen));
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMaxLogLevel, _L8("%16S | %4d  | %4d   | %16S |   %3d  | %2d"),
               &name, regInfo.iPosition, regInfo.iEvent.iStatus, &header,
                regInfo.iStatus, regInfo.iHandle));

        earlierName.Format(name);
        ++iter;
        }
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"List HTTP filters done.");
#endif    
    }

void CSenHttpChannelImpl::HandleResponseHeadersL(RHTTPTransaction aTransaction)
    {
    TInt txnId = aTransaction.Id();
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::HandleResponseHeadersL( %d )"),
                        txnId));

    RHTTPResponse resp = aTransaction.Response();
    TInt status = resp.StatusCode();
#ifdef _SENDEBUG
    RStringF statusStr = resp.StatusText();
    const TDesC8& statusStrDesC = statusStr.DesC();
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Response status: %d ( %S )"), status, &statusStrDesC));
#endif // _SENDEBUG
    CSenTxnState* pTxnState = FindTxnState(txnId);

    __ASSERT_ALWAYS(pTxnState != NULL, User::Panic(KTxnStateNullPanicText, CSenHttpChannel::ETxnStateNull));

    if (status == 401)  // basic authentication needed
        {
        CSenHttpTransportProperties* tp = CSenHttpTransportProperties::NewLC();
        RHTTPHeaders responseHeaders = aTransaction.Response().GetHeaderCollection();
        THTTPHdrFieldIter fields = responseHeaders.Fields();
        while (!fields.AtEnd())
             {
             RStringTokenF fieldName = fields();
             RStringF fieldNameStr = iStringPool.StringF(fieldName);
             THTTPHdrVal fieldVal;
             if (responseHeaders.GetField(fieldNameStr,0,fieldVal) == KErrNone)
                 {
                  TPtrC8 ptr(KNullDesC8);
                  responseHeaders.GetRawField(fieldNameStr, ptr);
                  TPtrC8 ptr2(fieldNameStr.DesC());
                  if (fieldVal.Type() == THTTPHdrVal::KDateVal)
                      {
                      TDateTime date = fieldVal.DateTime();
                      RStringF dateS = iStringPool.StringF(HTTP::EDate, RHTTPSession::GetTable());
                      TTimeIntervalMinutes interval;
                      TTime begin(0);
                      TTime time(date);
                      time.MinutesFrom(begin, interval);
                      tp->SetIntPropertyL(dateS.DesC(), interval.Int());
                      pTxnState->SetTP(tp);//ownership transfered
                      pTxnState->HttpChannelPropertiesL().SetIntPropertyL(dateS.DesC(), interval.Int());
                      }
                  }
              ++fields;
             }
        CleanupStack::Pop(tp);
        return;
        }
        
    else if (status == 200)
        {
        RHTTPHeaders responseHeaders = aTransaction.Response().GetHeaderCollection();

        THTTPHdrVal fieldVal;
        TPtrC8 fieldValPtr;
        THTTPHdrVal paramVal;
            
        // get the Content-Type string
        RStringF content = iStringPool.StringF(HTTP::EContentType,
                                               RHTTPSession::GetTable());

        if (responseHeaders.GetField(content,0,fieldVal) == KErrNone)
            {
            // get the field value
            switch (fieldVal.Type())
                {
                case THTTPHdrVal::KStrFVal:
                    {
                    fieldValPtr.Set(iStringPool.StringF(fieldVal.StrF()).DesC());
                    }
                break;
                case THTTPHdrVal::KStrVal:
                    {
                    fieldValPtr.Set(iStringPool.String(fieldVal.Str()).DesC());
                    }
                break;
                default:
                    {
                    User::Panic(KUnrecognisedValueTypeOfContentTypePanicText,
                                EContentTypeUnrecognisedValueType);
                    }
                }
            _LIT8(KMultipartRelated,"Multipart/Related");	//CodeScannerWarning
            _LIT8(KBoundry,"boundary");
            _LIT8(KType,"type");
            _LIT8(KStart,"start");
            _LIT8(KStartInfo,"start-info");
            _LIT8(KAction,"action");
            _LIT8(KApplication,"application/xop+xml");
            if (fieldValPtr == KMultipartRelated)
                {
                //SenMultiPartUtils::TMultiPartContentType MultiPartContentType;
                iMultiPartContentType.fieldValue.Set(fieldValPtr);

                // get the param values
                TPtrC8 paramName(KBoundry);
                if(KErrNone == ContentTypeParamValueL(responseHeaders, content, paramName,  iMultiPartContentType))     
                    {
                    paramName.Set(KType);
                    if (KErrNone == ContentTypeParamValueL(responseHeaders, content, paramName,  iMultiPartContentType))     
                        {
                        paramName.Set(KStart);
                        if (KErrNone == ContentTypeParamValueL(responseHeaders, content, paramName,  iMultiPartContentType))     
                            {
                            paramName.Set(KStartInfo);
                            if (KErrNone == ContentTypeParamValueL(responseHeaders, content, paramName,  iMultiPartContentType))     
                                {
                                paramName.Set(KAction);
                                if (KErrSenNoHttpContentType == ContentTypeParamValueL(responseHeaders, content, paramName,  iMultiPartContentType))     
                                    {
                                    SenMultiPartUtils::TMultiPartContentTypeParam contentTypeParam = {_L8("action"), _L8("")};
                                    iMultiPartContentType.params.Append(contentTypeParam);
                                    }
                                TUint i(0);
                                while (iMultiPartContentType.params[i].paramName != KType)
                                    {
                                     ++i;
                                    }
                                if (iMultiPartContentType.params[i].paramValue == KApplication)   
                                    {
                                    iXopResponse = ETrue;
                                    content.Close();
                                    return;
                                    }
                                }
                            }
                        }
                    }
                }
           else 
                {
                iContentType.Set(fieldValPtr);
                content.Close();
                return;
                }
            }
        iHasHttpContentType = EFalse;
        content.Close();
        }
    }

TInt CSenHttpChannelImpl::ContentTypeParamValueL(const RHTTPHeaders& aResponseHeaders,
                                const RStringF& aContent, 
                                const TPtrC8& aParamName,
                                SenMultiPartUtils::TMultiPartContentType& aMultiPartContentType)
    {
    RStringF paramNameStr = iStringPool.OpenFStringL(aParamName);            
    
    THTTPHdrVal paramVal;
    TPtrC8 paramValPtr;
    if (aResponseHeaders.GetParam(aContent,paramNameStr,paramVal) == KErrNone)
        {
        switch (paramVal.Type())
            {
            case THTTPHdrVal::KStrFVal:
                {
                paramValPtr.Set(iStringPool.StringF(paramVal.StrF()).DesC());
                }
            break;
            case THTTPHdrVal::KStrVal:
                {
                paramValPtr.Set(iStringPool.String(paramVal.Str()).DesC());
                }
            break;
            default:
                User::Panic(KUnrecognisedValueTypeOfContentTypePanicText,
                            EContentTypeUnrecognisedValueType);
            break;
            }

        SenMultiPartUtils::TMultiPartContentTypeParam contentTypeParam = {_L8(""), _L8("")};
        contentTypeParam.paramName.Set(aParamName);
        contentTypeParam.paramValue.Set(paramValPtr);
        aMultiPartContentType.params.Append(contentTypeParam);
        paramNameStr.Close();
        return KErrNone;
        }
    else
        {
        paramNameStr.Close();
        return KErrSenNoHttpContentType;
        }
    }

void CSenHttpChannelImpl::HandleResponseBodyDataL(RHTTPTransaction aTransaction)
    {
    TInt txnId = aTransaction.Id();
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::HandleResponseBodyDataL( %d )"), txnId));

    CSenTxnState* pTxnState = FindTxnState(txnId);
    __ASSERT_ALWAYS(pTxnState != NULL,
                    User::Panic(KTxnStateNullPanicText,
                    CSenHttpChannel::ETxnStateNull));

    // Get the body data supplier
    MHTTPDataSupplier* pRespBody = aTransaction.Response().Body();

    if(pRespBody)
        {
        TPtrC8 dataChunk;
#ifdef _SENDEBUG
        TBool isLast = pRespBody->GetNextDataPart(dataChunk);
        if (isLast)
            TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("Got last data chunk.")));
#else
        pRespBody->GetNextDataPart(dataChunk);
#endif // _SENDEBUG
        
        if (!iXopResponse)
            {
            pTxnState->CollectResponseBodyL(dataChunk);
            }
        else
            {
            ((CSenMtomTxnState*)pTxnState)->ParseMultiPartResponseL(dataChunk);
            }


        // Done with that bit of body data
        pRespBody->ReleaseData();
        }
    else
        {
        //pTxnState->ResponseError(-20000);
        pTxnState->ResponseErrorL(KErrSenNoHttpResponseBody); //SenServiceConnection.h // was -20000
        DeleteTxnState(txnId);
        aTransaction.Close();
        }
    }

void CSenHttpChannelImpl::HandleResponseL(RHTTPTransaction aTransaction)
    {
    TInt txnId = aTransaction.Id();
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::HandleResponseL( %d )"), txnId));
    CSenTxnState* pTxnState = FindTxnState(txnId);
    __ASSERT_ALWAYS(pTxnState != NULL,
                    User::Panic(KTxnStateNullPanicText,
                    CSenHttpChannel::ETxnStateNull));

    // Propagate http status codes                                    
    RHTTPResponse resp = aTransaction.Response();
    TInt status = resp.StatusCode();
    pTxnState->StateChanged(status);    
                                           
    if (iHasHttpContentType)
        {
         if (!iXopResponse)
            {
            pTxnState->ResponseReceivedL(iContentType);
            }
        else
            {
            ((CSenMtomTxnState*)pTxnState)->ResponseReceivedL(iMultiPartContentType);
            }
        }
    else
        {
        pTxnState->ResponseErrorL(KErrSenNoHttpContentType); // was: -20001
        }

    DeleteTxnState(txnId);
    aTransaction.Close();
    }

void CSenHttpChannelImpl::HandleRunErrorL(RHTTPTransaction aTransaction,
                                          TInt aError)
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"CSenHttpChannelImpl::HandleRunErrorL()");
    TInt txnId = aTransaction.Id();
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::HandleRunErrorL( %d ): %d"),txnId, aError));
    CSenTxnState* pTxnState = FindTxnState(txnId);
    __ASSERT_ALWAYS(pTxnState != NULL,
                    User::Panic(KTxnStateNullPanicText,
                    CSenHttpChannel::ETxnStateNull));

    pTxnState->ResponseErrorL(aError);
    DeleteTxnState(txnId);
    aTransaction.Close();
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"CSenHttpChannelImpl::HandleRunErrorL() Completed");
    }

void CSenHttpChannelImpl::HandleRedirectRequiresConfirmationL(
                                                RHTTPTransaction aTransaction)
    {
    TInt txnId = aTransaction.Id();
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::HandleRedirectRequiresConfirmationL( %d )"),
                txnId));
    CSenTxnState* pTxnState = FindTxnState(txnId);
    __ASSERT_ALWAYS(pTxnState != NULL,
                    User::Panic(KTxnStateNullPanicText,
                    CSenHttpChannel::ETxnStateNull));
    pTxnState->ResponseErrorL(KErrSenHttpRedirectRequiresConfirmation); // was: -20002
    DeleteTxnState(txnId);
    aTransaction.Close();
    }
/*
RFileLogger* CSenHttpChannelImpl::Log() const
    {
    return (RFileLogger*) &iLog;
    }
*/
// This is a debug logging method
#ifdef _SENDEBUG
void CSenHttpChannelImpl::DumpRespHeadersL(RHTTPTransaction& aTrans)
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::DumpRespHeadersL")));
    RHTTPResponse resp = aTrans.Response();
    RHTTPHeaders hdr = resp.GetHeaderCollection();
    THTTPHdrFieldIter it = hdr.Fields();

    TBuf<KMaxHeaderNameLen>  fieldName16;
    TBuf<KMaxHeaderValueLen> fieldVal16;
    TBuf<KMaxHeaderNameLen>  paramName16;
    TBuf<KMaxHeaderValueLen> paramVal16;

    while (!it.AtEnd())
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = iStringPool.StringF(fieldName);
        THTTPHdrVal fieldVal;
        THTTPHdrVal paramVal;
        if (hdr.GetField(fieldNameStr,0,fieldVal) == KErrNone)
            {
            const TDesC8& fieldNameDesC = fieldNameStr.DesC();
            fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
            switch (fieldVal.Type())
                {
            case THTTPHdrVal::KTIntVal:
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: %d"), &fieldName16, fieldVal.Int()));
                break;
            case THTTPHdrVal::KStrFVal:
                {
                RStringF fieldValStr = iStringPool.StringF(fieldVal.StrF());
                const TDesC8& fieldValDesC = fieldValStr.DesC();
                fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: %S"), &fieldName16, &fieldVal16));
                }
                break;
            case THTTPHdrVal::KStrVal:
                {
                RString fieldValStr = iStringPool.String(fieldVal.Str());
                const TDesC8& fieldValDesC = fieldValStr.DesC();
                fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: %S"), &fieldName16, &fieldVal16));
                }
                break;
            case THTTPHdrVal::KDateVal:
                {
                TDateTime date = fieldVal.DateTime();
                TBuf<40> dateTimeString;
                TTime t(date);
                t.FormatL(dateTimeString,KDateFormat);
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: %S"), &fieldName16, &dateTimeString));

                }
                break;
            default:
                TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: <unrecognised value type>"), &fieldName16));
            break;
            }
            
        RStringF paramNameStr = iStringPool.OpenFStringL(_L8("type"));            
        const TDesC8& paramNameDesC = paramNameStr.DesC();
        paramName16.Copy(paramNameDesC.Left(KMaxHeaderNameLen));
        if (hdr.GetParam(fieldNameStr,paramNameStr,paramVal) == KErrNone)
            {
            switch (paramVal.Type())
                {
                case THTTPHdrVal::KStrFVal:
                    {
                    RStringF paramValStr = iStringPool.StringF(paramVal.StrF());
                    const TDesC8& paramValDesC = paramValStr.DesC();
                    paramVal16.Copy(paramValDesC.Left(KMaxHeaderValueLen));
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: %S"), &paramName16, &paramVal16));
                    }
                break;
                case THTTPHdrVal::KStrVal:
                    {
                    RString paramValStr = iStringPool.String(paramVal.Str());
                    const TDesC8& paramValDesC = paramValStr.DesC();
                    paramVal16.Copy(paramValDesC.Left(KMaxHeaderValueLen));
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: %S"), &paramName16, &paramVal16));
                    }
                break;
                default:
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("%S: <unrecognised value type>"), &paramName16));
                break;
                }
            }
		_LIT(KMultipartRelated,"Multipart/Related");
		_LIT(KApplication,"application/xop+xml");
        if ((fieldVal16 == KMultipartRelated) && (paramVal16 == KApplication))
            {
            iXopResponse = ETrue;    
            }
            
            // Display realm for WWW-Authenticate header
            RStringF wwwAuth =
                iStringPool.StringF(
                    HTTP::EWWWAuthenticate,RHTTPSession::GetTable());
            if (fieldNameStr == wwwAuth)
                {
                // check the auth scheme is 'basic'
                RStringF basic =
                    iStringPool.StringF(HTTP::EBasic,RHTTPSession::GetTable());
                RStringF realm =
                    iStringPool.StringF(HTTP::ERealm,RHTTPSession::GetTable());
                THTTPHdrVal realmVal;
                if ((fieldVal.StrF() == basic) &&
                    (!hdr.GetParam(wwwAuth, realm, realmVal)))
                    {
                    RStringF realmValStr =
                        iStringPool.StringF(realmVal.StrF());
                    const TDesC8& realmValDesC = realmValStr.DesC();
                    fieldVal16.Copy(realmValDesC.Left(KMaxHeaderValueLen));
                    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("Realm is: %S"), &fieldVal16));
                    }
                }
            paramNameStr.Close();
            }
        ++it;
        }
    }
#else
// do nothing
void CSenHttpChannelImpl::DumpRespHeadersL(RHTTPTransaction& /* aTrans */) {  } 
#endif


// This is second debug logging method
//Do a formatted dump of binary data
#ifdef _SENDEBUG
void CSenHttpChannelImpl::DumpBodyData(const TDesC8& aData)
    {
    // Iterate the supplied block of data in blocks of cols=80 bytes
    const TInt cols=16;
    TInt pos = 0;
    TBuf<KMaxFileName - 2> logLine;
    TBuf<KMaxFileName - 2> anEntry;
    const TInt dataLength = aData.Length();
    while (pos < dataLength)
        {
        //start-line hexadecimal( a 4 digit number)
        anEntry.Format(TRefByValue<const TDesC>_L("%04x : "), pos);
        logLine.Append(anEntry);

        // Hex output
        TInt offset;
        for (offset = 0; offset < cols; ++offset)
            {
            if (pos + offset < aData.Length())
                {
                TInt nextByte = aData[pos + offset];
                anEntry.Format(TRefByValue<const TDesC>_L("%02x "), nextByte);
                logLine.Append(anEntry);
                }
            else
                {
                // fill the remaining spaces with
                // blanks untill the cols-th Hex number
                anEntry.Format(TRefByValue<const TDesC>_L("   "));
                logLine.Append(anEntry);
                }
            }
        anEntry.Format(TRefByValue<const TDesC>_L(": "));
        logLine.Append(anEntry);

        // Char output
        for (offset = 0; offset < cols; ++offset)
            {
            if (pos + offset < aData.Length())
                {
                TInt nextByte = aData[pos + offset];
                if ((nextByte >= ' ') && (nextByte <= '~'))
                    {
                    anEntry.Format(TRefByValue<const TDesC>_L("%c"), nextByte);
                    logLine.Append(anEntry);
                    }
                else
                    {
                    anEntry.Format(TRefByValue<const TDesC>_L("."));
                    logLine.Append(anEntry);
                    }
                }
            else
                {
                anEntry.Format(TRefByValue<const TDesC>_L(" "));
                logLine.Append(anEntry);
                }
            }
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, TRefByValue<const TDesC>_L("%S"), &logLine));
            logLine.Zero();

        // Advance to next  byte segment (1 seg= cols)
        pos += cols;
        }
    }
#else
// do nothing
void CSenHttpChannelImpl::DumpBodyData(const TDesC8& /* aData */) { } 
#endif

// ----------------------------------------------------------------------------
// CSenHttpChannelImpl::AppendNewTxnStateL
// ----------------------------------------------------------------------------
//
void CSenHttpChannelImpl::AppendNewTxnStateL(CSenTxnState* aTxnState)
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenHttpChannelImpl::AppendNewTxnStateL")));
    iTxnStates->AppendL(aTxnState);
    }

// ----------------------------------------------------------------------------
// CSenHttpChannelImpl::DeleteTxnState
// ----------------------------------------------------------------------------
void CSenHttpChannelImpl::DeleteTxnState(TInt aTxnId)
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::DeleteTxnState( %d )"), aTxnId));
    TInt index;
    CSenTxnState* pTxnState = FindTxnState(aTxnId, &index);
    if(pTxnState)
        {
        iTxnStates->Delete(index);
        delete pTxnState;
        pTxnState = NULL;
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("TxnState ( %d ) deleted"), aTxnId));
        }
    }

// ----------------------------------------------------------------------------
// CSenHttpChannelImpl::FindTxnState
// ----------------------------------------------------------------------------
CSenTxnState* CSenHttpChannelImpl::FindTxnState(TInt aTxnId, TInt* aIndex)
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::FindTxnState( %d )"), aTxnId));
    for(TInt i = 0; i < iTxnStates->Count(); ++i)
        {
        if((*iTxnStates)[i]->Id() == aTxnId)
            {
            if(aIndex)
                {
                *aIndex = i;
                }
            return (*iTxnStates)[i];
            }
        }
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("TxnState ( %d ) not found"), aTxnId));
    return NULL;
    }


// @return KErrNone, if cancel was performed
//        KErrNotFound if transaction was not found 
TInt CSenHttpChannelImpl::CancelTransaction(const TInt aTxnId)
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KSenHttpChannelLogLevel,"");
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::CancelTransaction - TxnId: %d"),
        aTxnId));

    CSenTxnState* pTxnState = FindTxnState(aTxnId);
    if(pTxnState)
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"- Txn found, calling Cancel():");
        return pTxnState->Cancel();
        }
    return KErrNotFound;
    }

void CSenHttpChannelImpl::CancelAllTransactions()
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::CancelAllTransaction");
    
    for(TInt i = 0; i < iTxnStates->Count(); ++i)
        {
        (*iTxnStates)[i]->Cancel();
        }
    }

void CSenHttpChannelImpl::SetExplicitIapDefined(TBool aExplicitIapDefined)
    {
    iExplicitIapDefined = aExplicitIapDefined;
    }


TBool CSenHttpChannelImpl::EffectiveIapId( TUint32 &aIapId )
    {
    /*
		TInt handle = iConnection.SubSessionHandle();
		if (handle>0)
		    {
				TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::EffectiveIapId(): Current RConnection's subsession handle(%d)"), handle ));	    
        TUint connEnum(0);
        TInt err = iConnection.EnumerateConnections(connEnum);
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::EffectiveIapId -  EnumerateConnections retVal [%d]"), err ));
        if (!err && !connEnum)
            {
            TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::EffectiveIapId returns");
            return EFalse;
            }
		    }
	    else
		    {
		    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel," -> RConnection has not been initialized.");	
		    }
		*/
	if( iExplicitIapDefined )
		{
		TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8(" - IAP ID is known: %d"), iIapId ));
		}
	else
		{
		TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel," - IAP ID is not known.");
		}
		    
  if(/* handle && */!iExplicitIapDefined )
  	{
		// Eventhough IAP was not explicitely set (through Serene API), this
		// code can check what IAP end-user provided via IAP selection dialog:
		TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::EffectiveIapId: about to call RConnection::GetIntSetting()");
	//_LIT( KIapIdKey, "IAP\\Id" );
    //iConnection.GetIntSetting( KIapIdKey, iIapId);
    if ( iIapId > 0 )
    	{
			TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("---- GetIntSetting(): retuens IAP(%d)"), iIapId ));
    	// Treat the end-user selection as "explicit" definition as well(!):
    	iExplicitIapDefined = ETrue; 
    	}
    }
  if( iExplicitIapDefined )
      {
      aIapId = iIapId;
      }
  	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenHttpChannelImpl::EffectiveIapId() returns(%d)"), iExplicitIapDefined ));
    return iExplicitIapDefined;
    }

void CSenHttpChannelImpl::EnableTimeOutL(TInt aTxnId, TInt aTimeOutSec)
    {
    CSenTxnState* txn = FindTxnState(aTxnId);
    if (txn)
        txn->EnableTimeOutL(aTimeOutSec);
    }
void CSenHttpChannelImpl::DisableTimeOutL(TInt aTxnId)
    {
    CSenTxnState* txn = FindTxnState(aTxnId);
    if (txn)
        txn->DisableTimeOutL();
    }

TInt32 CSenHttpChannelImpl::UsedIap()
    {
    return iUsedIapId;
    }
    
void CSenHttpChannelImpl::ResetIapId()
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::ResetIapId()");
    iIapId = 0; //Reset to Zero in case of errors to enable other IAP
    }
		
void CSenHttpChannelImpl::ResetUsedIapId()
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenHttpChannelImpl::ResetUsedIapId()");
    iUsedIapId = 0; //Reset to Zero in case of errors to enable other IAP
    }
    
// END OF FILE
