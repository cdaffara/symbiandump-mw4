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



// INCLUDE FILES
#include <connpref.h>
#include <commdbconnpref.h>
#include "senmobilityobserver.h"
#include "sentransportproperties.h"
#include "senwspattern.h"
#include "msenproperties.h"
#include "senlayeredtransportproperties.h"
#include "senlogger.h"
#include "senservicemanagerdefines.h"
#include <versioninfo.h>  // VersionInfo
#include <extendedconnpref.h>

// -----------------------------------------------------------------------------
// CALRObserver::NewL
// -----------------------------------------------------------------------------
//
CALRObserver* CALRObserver::NewL(MMobilityProtocolResp& aMobilityObserver, CSenXmlReader &aReader)
    {
    CALRObserver* self = new ( ELeave ) CALRObserver(aMobilityObserver, aReader);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CALRObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CALRObserver::ConstructL()
    {
#ifdef _SENDEBUG       
    TFileName logFile;
    logFile.Append( KSenMobilityLog().Left(KSenMobilityLog().Length()-4) ); // exclude ".log" file extension
    TDateTime time;
    TTime start;
    start.HomeTime();
    time=start.DateTime();
    
    logFile.AppendNum( time.Hour() );
    logFile.Append( KSenUnderline );
    logFile.AppendNum( time.Minute() );
    logFile.Append( KSenUnderline );
    logFile.AppendNum( time.Second() );
    logFile.Append( KSenMobilityLog().Right(4) ); // postfix with ".log" file extension
    TLSLOG_OPEN(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel, KSenMobilityDir, logFile);
#endif    
    User::LeaveIfError( iSocketServer.Connect());
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "CALRObserver::ConstructL()");
    IsOCCImplementedSDK();
    }

// -----------------------------------------------------------------------------
// CALRObserver::CALRObserver
// -----------------------------------------------------------------------------
CALRObserver::CALRObserver(MMobilityProtocolResp &aMobilityObserver, CSenXmlReader &aReader) 
    : CActive( CActive::EPriorityStandard ),
     iMobilityObserver( aMobilityObserver ),
     iMobility(NULL),
     iIapId(0),
     iNewIapId(0),
     iSnapId(0),
     iReader(aReader),
     iOCCenabled(EFalse)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CALRObserver::~CALRObserver
// -----------------------------------------------------------------------------
//
CALRObserver::~CALRObserver()
    {
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "CALRObserver::~CALRObserver() started");
    if (iMobility != NULL)
        {
        delete iMobility;
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "-iMobility deleted ");
        }
    Cancel();
    TUint openCount(0);
    TInt retVal = iConnection.EnumerateConnections(openCount);
    if(retVal == KErrNone)
    	{
    	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- connections count : [%d] "), openCount));	
    	}
    iConnection.Close();
    iSocketServer.Close();    
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "CALRObserver::~CALRObserver() completed");
#ifdef _SENDEBUG
    TLSLOG_CLOSE(KSenSenMobilityLogChannelBase);
#endif
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::DoCancel
// -----------------------------------------------------------------------------
//
void CALRObserver::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::RunL
// -----------------------------------------------------------------------------
//
void CALRObserver::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        TUint32 iap( 0 );
        iConnection.GetIntSetting( _L( "IAP\\Id" ), iap );
        iIapId = iap ;
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("-RunL registering mobility using IAPID: %d "), iIapId));
        TRAPD(err, iMobility = CActiveCommsMobilityApiExt::NewL( iConnection, *this )) ;
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- registered for mobility service returned : %d "), err));
        }
    else
        {
        //RunL status error: " ) );
        User::Leave(iStatus.Int()) ;
        }
    }
    
// On return, aMajor and aMinor contain the version information
TBool CALRObserver::IsOCCImplementedSDK()
	{   
	TBool occ = EFalse;
	// Obtain the version number
	TUint major;
	TUint minor;	
	TLSLOG(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel,(_L("CSenHttpChannelImpl::IsOCCImplementedSDK()")));		
	TInt ret = GetS60PlatformVersion(major, minor);
	if (ret == KErrNone)
		{
		if(major == 5 && minor == 2)
			{
			occ = ETrue;
			iOCCenabled = ETrue;
			TLSLOG(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel,(_L("---- IsOCCImplementedSDK() returns TRUE")));
			}
		else
			{
			TLSLOG(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel,(_L("---- IsOCCImplementedSDK() returns False")));
			}
		}
	return occ;
	}
	
// On return, aMajor and aMinor contain the version information
TInt CALRObserver::GetS60PlatformVersion(TUint& aMajor, TUint& aMinor)
	{   
	TInt ret = KErrNone;
	// Connect to the file server session
	RFs fsSession;
	TLSLOG(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel,(_L("CSenHttpChannelImpl::GetS60PlatformVersion()")));	
	ret = fsSession.Connect();
	if(ret == KErrNone)
		{
		CleanupClosePushL(fsSession); // Obtain the version numberTUint major;
		VersionInfo::TPlatformVersion platformVersion;  
		TLSLOG_L(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, "--- GetS60PlatformVersion getting Version info");		
		ret = VersionInfo::GetVersion(platformVersion, fsSession);   
		CleanupStack::PopAndDestroy();  // fsSession		
		if (ret == KErrNone)       
			{     
			aMajor = platformVersion.iMajorVersion;  
			aMinor = platformVersion.iMinorVersion;   
		   	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, _L8("--- GetS60PlatformVersion Version: Major [%d], Minor[%d]"), aMajor, aMinor));
			}		
		}
	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, _L8("--- GetS60PlatformVersion returns [%d]"), ret));
	return ret;
	}	
	
// -----------------------------------------------------------------------------
// CALRObserver::RunError
// -----------------------------------------------------------------------------
//
TInt CALRObserver::RunError( TInt aError )
    {
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("-CALRObserver::RunError received with error = %d"), aError));
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::GetActiveIap
// -----------------------------------------------------------------------------
//
TUint32 CALRObserver::GetActiveIap()
    {
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- GetActiveIap (ID: %d) "), iIapId));
    return iIapId ;
    }    
    
TUint32 CALRObserver::GetActiveSnap()
    {
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- GetActiveSnap (ID: %d) "), iSnapId));
    return iSnapId ;
    }    
    
// -----------------------------------------------------------------------------
// CALRObserver::PreferredCarrierAvailable
// -----------------------------------------------------------------------------
//
void CALRObserver::PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                                 TAccessPointInfo aNewAPInfo,
                                                 TBool aIsUpgrade,
                                                 TBool aIsSeamless )
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- PreferredCarrierAvailable callback received ");
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- PreferredCarrierAvailable with old iapid = %d"), aOldAPInfo.AccessPoint()));
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- PreferredCarrierAvailable with new iapid = %d"), aNewAPInfo.AccessPoint()));
    if ( aIsUpgrade )
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- PreferredCarrierAvailable it is upgrade ");
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- PreferredCarrierAvailable it is not upgrade ");
        }
    if (aIsSeamless)
        {
          // in S60 3.2, this situation cannot occur.
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "-it is seemless in S60 3.2, this situation cannot occur. ");
        }        
	if (iIapId != aOldAPInfo.AccessPoint())
		{
		//we received callback for unwanted iap ids
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- ASSERT ERROR PreferredCarrierAvailable callback received ");
		}
	else
		{
        if (iIapId != aNewAPInfo.AccessPoint())
            {
            TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- forwarding call PreferredCarrierAvailable callback to observer with iapID %d"), aNewAPInfo.AccessPoint()));
            iMobilityObserver.PreferredCarrierAvailable( aOldAPInfo,
                                             aNewAPInfo,
                                             aIsUpgrade,
                                             aIsSeamless ) ;
           }
        else
            {
            TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- ASSERT ERROR iIapId cant be same with newIap as iIapId = %d, newIap = %d"), iIapId, aNewAPInfo.AccessPoint()));
            }
        }
	}

// -----------------------------------------------------------------------------
// CALRObserver::NewCarrierActive
// -----------------------------------------------------------------------------
//
void CALRObserver::NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless )
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- NewCarrierActive callback received ");
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- NewCarrierActive with new iapid = %d"), aNewAPInfo.AccessPoint()));     
    if ( aIsSeamless )
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- NewCarrierActive it is seemless ");
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- NewCarrierActive it is not seemless ");
        }        
	if (iIapId != aNewAPInfo.AccessPoint())
		{
        iNewIapId = aNewAPInfo.AccessPoint() ;
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- frowarding NewCarrierActive callback to observer with new IAPID = %d"), iNewIapId));
        iMobilityObserver.NewCarrierActive( aNewAPInfo, aIsSeamless ) ;
		}
	else
		{
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- ASSERT ERROR NewCarrierActive callback received with oldIapId = %d, newIapInfo = %d "), iIapId, aNewAPInfo.AccessPoint() ));
    	//we received callback for wrong PreferredCarrierAvailable call
		}
    }
    
// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::DoCancel
// -----------------------------------------------------------------------------
//
void CALRObserver::Error(TInt aError)
    {
    //iConnection.Stop();
    if (aError != KErrCancel)
        {
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("-CALRObserver::Error callback received with error = %d"), aError));        
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "-forwarding mobility Error callback to consumer");
        iMobilityObserver.Error(aError) ;            
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "-User Canceled mobility Service");
        }
    }    

void CALRObserver::MigrateToPreferredCarrier()
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::MigrateToPreferredCarrier ");
    if (iMobility)    
        {
	    iMobility->MigrateToPreferredCarrier();            
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- iMobility object is NULL");
        }
	}

void CALRObserver::IgnorePreferredCarrier()
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::IgnorePreferredCarrier ");
    if (iMobility)    
        {
	    iMobility->IgnorePreferredCarrier();
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- iMobility object is NULL");
        }	
	}

void CALRObserver::NewCarrierAccepted() 
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::NewCarrierAccepted ");
    if (iMobility)    
        {
	    iMobility->NewCarrierAccepted();
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- iMobility object is NULL");
        }	
	}

void CALRObserver::NewCarrierRejected()
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::NewCarrierRejected ");	    	
    if (iMobility)    
        {
	    iMobility->NewCarrierRejected();
        }
    else
        {
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- iMobility object is NULL");
        }
	}

// -----------------------------------------------------------------------------
// CALRObserver::OpenSocketSever
// -----------------------------------------------------------------------------
//
TInt CALRObserver::OpenSocketSever()
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- OpenSocketSever ");
    TInt err( KErrNone );
    // If iConnection is already open don't reopen it.
	if( !iConnection.SubSessionHandle() )
        {     
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- calling iConnection.Open ");
        err = iConnection.Open(iSocketServer);
        }     
    return err;
	}

HBufC8* CALRObserver::GetNewIapAsTransportPropertyL()
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- GetNewIapAsTransportPropertyL ");
	CSenTransportProperties * pTransportProperties = CSenTransportProperties::NewL();
	if (iIapId != iNewIapId)
		{
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("-GetNewIapAsTransportPropertyL filling up transport properties with iNewIapId = %d"), iNewIapId));
        pTransportProperties->SetIapIdL(iNewIapId) ;
        iIapId = iNewIapId ;    //reseting current iapid as the new iapid for internal purpose
        return pTransportProperties->AsUtf8L() ;
		}
	else
		{
		delete pTransportProperties ;
		return NULL ;		
		}
	}

TInt CALRObserver::OpenConnectionL(TDesC8& aAppTransportProperties,
								 MSenTransport &aTransport,
								 CSenWSDescription& aInitializer,
								 HBufC8*& apNewTransportProperties)
	{
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::OpenConnectionL ");	
	CSenTransportProperties * pTransportProperties = CSenTransportProperties::NewL(aAppTransportProperties, iReader);
	TUint32 id(0);
	TBool isSnap(EFalse) ;
    TBool isStartConnectionRequired(EFalse) ;

    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get SNAPID from pTransportProperties ");
	TInt error = pTransportProperties->SnapIdL(id) ;
	if(error || id >= (TUint)KErrNotFound) //SNAP not found
		{
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get IAPID from pTransportProperties ");
	    error = pTransportProperties->IapIdL(id);		
		}
	else
		{
		isSnap = ETrue ;
		}  
		
    if(error || id >= (TUint)KErrNotFound) //IAP also not found
	    {
    	//Application Did not provide IAP ID through transport properties
    	//Now we need to check consumer and provider policy of service description
    	
    	if( aInitializer.DescriptionClassType() == MSenServiceDescription::EWSPattern )
            {
            // Check if <ConsumerPolicy/> was defined in SC aInitializer (constructor argument)
            // and if IAP was predefined
            CSenWSPattern* pConsumerPolicy = (CSenWSPattern*)&aInitializer;
		    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get SNAPID from pConsumerPolicy ");
			error = pConsumerPolicy->ConsumerSnapId( id );
			if(error || id >= (TUint)KErrNotFound) //SNAP not found
			    {
        		TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get IAPID from pConsumerPolicy ");
				error = pConsumerPolicy->ConsumerIapId( id );			 	
			    }
			else
    			{
    			isSnap = ETrue ;
    			}			    
           }
        if( error || id >= (TUint)KErrNotFound) // IAP ID must be 0...INT_MAX, since (TUInt)KErrNotFound == INT_MAX+1
            {
    		// Check if <ProviderPolicy/> was defined in SC aInitializer (constructor argument)
            // and if IAP was predefined
            TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get SNAPID from aInitializer ");
            error = aInitializer.SnapId( id );
			if(error || id >= (TUint)KErrNotFound) 
			    {
                TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get IAPID from aInitializer ");
				error = aInitializer.IapId( id );		 	
			    }            
			else
    			{
    			isSnap = ETrue ;
    			}			    
            }            
        if( error || id >= (TUint)KErrNotFound) // IAP ID must be 0...INT_MAX, since (TUInt)KErrNotFound == INT_MAX+1
	    	{
	    	//initialiser didnot provide iap/snap id
	    	//if another SC has already set up a transport we dont want to prompt
	        MSenProperties& property = aTransport.PropertiesL() ;
	    	switch (property.PropertiesClassType())
		    	{    		
	    		case MSenProperties::ESenLayeredTransportProperties:
	    		case MSenProperties::ESenLayeredHttpTransportProperties:
	    		case MSenProperties::ESenLayeredVtcpTransportProperties:
	    			{
	    			CSenLayeredTransportProperties& layerdPropery = (CSenLayeredTransportProperties&)property ;
                    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get SNAPID from layerdPropery ");
	    			error = layerdPropery.SnapIdL(id) ;
	    			if(error || id >= (TUint)KErrNotFound) 
		    			{
                        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- trying to get IAPID from layerdPropery ");
		    			error = layerdPropery.IapIdL(id) ;    				
		    			}
	    			else
		    			{
		    			isSnap = ETrue ;
		    			}
	    			}    			
		    		break;
		    	}
		    }	        	
	    }
        
    TUint32 openId(id);
    if (iIapId == 0 && iSnapId == 0)
        {
        //check with first Snapid
        isStartConnectionRequired = ETrue ;
        if(iOCCenabled != EFalse)
	        {
	        isSnap = ETrue; //We must start with SNAP for OCC
			TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- isSnap Modified for OCC Support ");     
			}
     	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- StartConnection required as (iIapId == 0 && iSnapId == 0) openId = %d, isSnap = %d, iIapId = %d and iSnapId = %d "), openId, isSnap, iIapId, iSnapId));
        }
    else if(isSnap != EFalse && openId != iSnapId)
        {
        isStartConnectionRequired = ETrue ;
     	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- StartConnection required as (isSnap == EFalse && openId != iIapId) openId = %d, isSnap = %d and iSnapId = %d "), openId, isSnap, iSnapId));        
        }
    else if(isSnap == EFalse && openId != iIapId)
        {
        isStartConnectionRequired = ETrue ;
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("-StartConnection required aws (isSnap == EFalse && openId != iIapId) openId = %d, isSnap = %d and iIapId = %d"), openId, isSnap, iIapId));
        }
    else
        {
        isStartConnectionRequired = EFalse ;
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- call StartConnection not required ");
        }
   if (isStartConnectionRequired != EFalse)
		{
        TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- calling StartConnection ");		
    	error = StartConnection(openId, isSnap) ;
	    if (openId != id)
		    {
        	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- OpenConnectionL setting up transport properties with iapid %d "), openId));
		    pTransportProperties->SetIapIdL(openId) ;
		    apNewTransportProperties = pTransportProperties->AsUtf8LC() ;
		    //CleanupStack::PopAndDestroy(pTransportProperties);
		    delete pTransportProperties;
		    pTransportProperties = NULL;	
		    CleanupStack::Pop();
		    }
		else
		    {
		    apNewTransportProperties = NULL ;
		    }
    	}
	else
    	{
    	TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- OpenConnectionL returning apNewTransportProperties = NULL ");	
		apNewTransportProperties = NULL ;
		if (iMobility)
    		{
    		//Already Registered for mobility service for the iap
    	    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- OpenConnectionL Already Registered for mobility service for the iap");	
    		error = KErrNone ;
    		}
    	}		    
     	TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- CALRObserver::OpenConnectionL returnd %d "), error));
        return error ;
	}
	
TInt CALRObserver::SetID(TUint32 aId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer, TBool aSNAP)
	{
   /*
    * Single click connectivity feature has been implemented by CSock (RConnection class).
    * According to this client no need to set the IAP ID. 
    * Automatically RConnection will use the suitable IAP
    */
    TInt retVal = KErrNone;
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, _L8("CSenHttpChannelImpl  called with ID [%d]"), aId));
	    
	if (iOCCenabled == EFalse)	    
		{
		if(aSNAP != EFalse && aId)
		    {
		    TConnSnapPref SNAPPrefs;
		    iSnapId = aId ;
		    SNAPPrefs.SetSnap( aId ); 
		    // Start connecting with Snap
		    retVal = iConnection.Start(SNAPPrefs);						
		    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::StartConnection iConnection Started with SNAP");
		    }
		else //default is iap
			{
		    if( aId )
		        {
		        TCommDbConnPref iapPrefs ;
		        iapPrefs.SetIapId( aId );
				iapPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
				TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::SetDialogPref is FALSE");					
				//iapPrefs.SetDialogPreference( ECommDbDialogPrefPrompt );					
		        // Start connecting with IAP
		        retVal = iConnection.Start(iapPrefs);
		        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- CALRObserver::StartConnection connection started with iapid %d and preference"), aId));
		        }
		    else
		    	{
		    	if (iIapId != 0)
		        	{
		        	//OpenSocketSever is already called for this iIapId When multiple
		        	//sendL are calld for the same IAPId StartConnection may be called
		        	//unintentionaly and may create crash
		        	//strange case StartConnection must not be called in this case
		        	TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::StartConnection connection StartConnection must not be called as aId == 0 and iIapId != 0");
		        	}
		    	else
		        	{
		        	//Using Default Connection
		            TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::StartConnection connection started Using Default Connection");
		        	retVal = iConnection.Start();	            	
		        	}
		    	}
			}
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
		    
		    TLSLOG_L(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, "- Setting up OCC with IAP Settings");
			TConnPrefList prefList;
			TExtendedConnPref extPrefs;
			//extPrefs.SetSnapPurpose( CMManager::ESnapPurposeUnknown);
			//TLSLOG_L(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, "- Setting up OCC Silent Connection");
			//extPrefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
			//extPrefs.SetBearerSet(TExtendedConnBearer::EExtendedConnBearerUnknown);
			extPrefs.SetIapId(aId);
			extPrefs.SetSnapId(0);
			prefList.AppendL(&extPrefs);
			retVal = aConnection.Start(prefList);
			if(retVal == KErrNone)
			    {
			    iIapId = aId;
			    }
			 else
			    {
				TLSLOG_FORMAT((KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, _L8("IAP ->Start retVal [%d]"), retVal));
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
			TLSLOG_L(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, "- Setting up OCC with SNAP Settings");
			TConnPrefList prefList;
			TExtendedConnPref extPrefs;
			//extPrefs.SetSnapPurpose( CMManager::ESnapPurposeUnknown);
			//TLSLOG_L(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, "- Setting up OCC Silent Connection");
			//extPrefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
			//extPrefs.SetBearerSet(TExtendedConnBearer::EExtendedConnBearerUnknown);
			if(aId == 0)
				{
				TLSLOG_L(KSenSenMobilityLogChannelBase , KSenSenMobilityLogLevel, "- Setting SnapPurpose Internet");
				if(aDialogPref) //If it fails because of unavailibility of access points
					{
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
			retVal = aConnection.Start(prefList);							
		    }
		}    
    
	return retVal;
	}

// -----------------------------------------------------------------------------
// CALRObserver::StartConnection
// -----------------------------------------------------------------------------
//
TInt CALRObserver::StartConnection(TUint32& aId, TBool aIsSnapId)
    {
    TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::StartConnection ");
    TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- CALRObserver::StartConnection aId(%d) aIsSnapId(%d) "),  aId, aIsSnapId));
	TInt errRet = KErrNone;
	if( !IsActive() )
	    {
	    if (aIsSnapId == EFalse)
			{
			if (aId !=0 && aId != iIapId)
				{
		        // IAP ID is provided and the new IAPID is not equal to the old IAP ID 
		        //we need to close old Connection and start the Connection with new
		        //IAPID as application is interested to change it on fly        
				//but openId must not be Zero (0)
				iConnection.Close();  
				TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::StartConnection iConnection closed as new IAPID for the same connection");
				}
			}
		else
			{
			if (aId !=0 && aId != iSnapId)
				{
				// SNAP ID is provided and the new SNAPID is not equal to the old SNAP ID 
				//we need to close old Connection and start the Connection with new
				//SNAPID as application is interested to change it on fly
				//but openId must not be Zero (0)
				iConnection.Close();  
				TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::StartConnection iConnection closed as new SNAPID for the same connection");				      
				}
			}
			
		errRet = OpenSocketSever();
		
		if (!errRet)
		    {
			errRet = SetID(aId, EFalse, iConnection, iSocketServer, aIsSnapId);
			
			if (errRet == KErrNone)
    			{	
     			//iConnection.GetIntSetting( _L( "IAP\\Id" ), aId );
     			_LIT( KIapId, "IAP\\Id");
                iConnection.GetIntSetting( KIapId, aId );
                TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- CALRObserver::StartConnection iConnection.GetIntSetting returned iapid = %d"), aId));                
     			TRAPD(err, RefreshAvailabilityL()) ;
                if(err == KErrNone)
                    {
     			    iIapId = aId ;
                    }
    			}
	    	//SetActive();
		    }
	    }
	return errRet;
    }	

// -----------------------------------------------------------------------------
// CALRObserver::RefreshAvailability
// -----------------------------------------------------------------------------
//
void CALRObserver::RefreshAvailabilityL()
    {
   TLSLOG_L(KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , "- CALRObserver::RefreshAvailabilityL");
    if ( iMobility )
        {
        delete iMobility;
        iMobility = NULL;
        }
        TRAPD(err, iMobility = CActiveCommsMobilityApiExt::NewL( iConnection, *this )) ;
        TLSLOG_FORMAT((KSenSenMobilityLogChannelBase, KSenSenMobilityLogLevel , _L8("- registered for mobility service returned : %d "), err));
    }

//End of File
