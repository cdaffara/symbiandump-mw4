/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmServerImpl class implementation.
*
*/

// Includes
#include "upnptmserverimpl.h"
#include "upnpiconfileservetransaction.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmserverimplTraces.h"
#endif


//Literals
_LIT8(KAppListStart,             "&lt;appList&gt;");
_LIT8(KAppListEnd,               "&lt;/appList&gt;");
_LIT8(KAppIdStart,               "&lt;appID&gt;");
_LIT8(KAppIdEnd,                 "&lt;/appID&gt;");
_LIT8(KNameStart,                "&lt;name&gt;");
_LIT8(KNameEnd,                  "&lt;/name&gt;");
_LIT8(KIconStart,                "&lt;icon&gt;");
_LIT8(KIconEnd,                  "&lt;/icon&gt;");
_LIT8(KMimeTypeStart,            "&lt;mimetype&gt;");
_LIT8(KMimeTypeEnd,              "&lt;/mimetype&gt;");
_LIT8(KWidthStart,               "&lt;width&gt;");
_LIT8(KWidthEnd,                 "&lt;/width&gt;");
_LIT8(KHeightStart,              "&lt;height&gt;");
_LIT8(KHeightEnd,                "&lt;/height&gt;");
_LIT8(KDepthStart,               "&lt;depth&gt;");
_LIT8(KDepthEnd,                 "&lt;/depth&gt;");
_LIT8(KUrlStart,                 "&lt;url&gt;");
_LIT8(KUrlEnd,                   "&lt;/url&gt;");
_LIT8(KAllowedProfileIdsStart,   "&lt;allowedProfileIDs&gt;");
_LIT8(KAllowedProfileIdsEnd,     "&lt;/allowedProfileIDs&gt;");
_LIT8(KResourceStatusStart,      "&lt;resourceStatus&gt;");
_LIT8(KResourceStatusEnd,        "&lt;/resourceStatus&gt;");
_LIT8(KAppStatusListStart,       "&lt;appStatusList&gt;");
_LIT8(KAppStatusListEnd,         "&lt;/appStatusList&gt;");
_LIT8(KAppStatusStart,           "&lt;appStatus&gt;");
_LIT8(KAppStatusEnd,             "&lt;/appStatus&gt;");
_LIT8(KStatusStart,              "&lt;status&gt;");
_LIT8(KStatusEnd,                "&lt;/status&gt;");
_LIT8(KProfileIdStart,           "&lt;profileID&gt;");
_LIT8(KProfileIdEnd,             "&lt;/profileID&gt;");
_LIT8(KStatusTypeStart,          "&lt;statusType&gt;");
_LIT8(KStatusTypeEnd,            "&lt;/statusType&gt;");
_LIT8(KStartTagOpenBracket,      "&lt;");
_LIT8(KEndTagOpenBracket,        "&lt;/");
_LIT8(KCloseBracket,             "&gt;");


// ================= MEMBER FUNCTIONS ==============================================

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerImpl* CUpnpTmServerImpl::NewL( CUpnpTmServerDeviceInfo& aDeviceInfo,
                                           MUpnpTmServerObserver& aTmServerObserver)
    {
    CUpnpTmServerImpl* self = new (ELeave) CUpnpTmServerImpl( aTmServerObserver );   
    CleanupStack::PushL(self);
    self->ConstructL( aDeviceInfo );
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::CUpnpTmServerImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerImpl::CUpnpTmServerImpl( MUpnpTmServerObserver& aTmServerObserver )
      :iTmServerObserver(aTmServerObserver)
     {

     }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::ConstructL( CUpnpTmServerDeviceInfo& aDeviceInfo )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CONSTRUCTL_ENTRY );
    iIapSetting = CUpnpSettings::NewL( KCRUidUPnPStack );
    /**
     *  Retrieve the IAP ID which has been set by DLNA and store it so that 
     *  it can be reverted back to the same when Automotive Server has stopped 
     *  making use of TM Service.
     */
    iIapId = CUpnpSettings::GetIapL(); 
    //Fetch the IAP ID set by the Automotive Server
    TInt iapId = aDeviceInfo.IapId();
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_CONSTRUCTL, "CUpnpTmServerImpl::ConstructL;iapId=%d", iapId );
    // writes the iap id to the cenrep
    User::LeaveIfError( iIapSetting->Set( CUpnpSettings::KUPnPStackIapId, iapId ) ); 
    iTmServerDevice = CUpnpTmServerDevice::NewL( aDeviceInfo, *this );
    iAppStore = CUpnpRemotableAppStore::NewL();
    iIconMapping = CUpnpTmIconMapping::NewL();
    User::LeaveIfError( iIconFileSession.Connect() );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::~CUpnpTmServerImpl
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmServerImpl::~CUpnpTmServerImpl()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CUPNPTMSERVERIMPL_ENTRY );
    //Restore the IAP ID for DLNA use
    RestoreIap();
    delete iIapSetting;
    delete iFilteredAppList;
    iIconFileSession.Close();
    delete iAppStore;
    delete iIconMapping;
    iXmSignature.Close();
    iResponseBuf.Close();
    delete iTmServerDevice;
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CUPNPTMSERVERIMPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::RestoreIap
// Method is used to restore IAP ID which has been previously retreived so that
// DLNA continues to make use of it.
// This method is mainly provided to avoid codescanner tool error.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::RestoreIap()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_RESTOREIAP_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_RESTOREIAP, "CUpnpTmServerImpl::RestoreIap;iIapId=%d", iIapId );
    iIapSetting->Set( CUpnpSettings::KUPnPStackIapId, iIapId );    
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_RESTOREIAP_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GetApplicationListL
// Method to create soap repsonse for GetApplicationList action.
// It generates a list of applications, which can be launched and terminated remotely.
// @param aAppListFilter Application Listing Filter. This is an optional parameter 
//                     used by the UPnP Control Point to limit the AppListing value to 
//                               those applications which meet the filter parameters.
// @param aProfileId  ProfileID of client profile whose parameter settings will be 
//                    applied for generating the application list
// @param aErr[out]   TerminalMode error code
// @return Returns soap response buffer 
// ---------------------------------------------------------------------------------
//
const TDesC8& CUpnpTmServerImpl::GetApplicationListL( const TDesC8& aAppListFilter,
                                     TUint aProfileId, TTerminalModeErrorCode& aErr )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GETAPPLICATIONLISTL_ENTRY );
    iResponseBuf.Close();
    iResponseBuf.CreateL(KActionBufferLength);
    aErr = iTmServerObserver.OnGetApplicationList( aProfileId );  
    if ( aErr == ETerminalModeSuccess )
        {
        // Check for availability of AppListingFilter input argument in the soap action
        TInt filterStringFound = aAppListFilter.Find(Keq);
        if ( filterStringFound != KErrNotFound )
            {
            delete iFilteredAppList;
            iFilteredAppList = NULL;
            
            // Constructs a filter object and parses the input filter string
            iFilteredAppList = CUpnpTmFilteredAppList::NewL(*this);
            iFilteredAppList->ParseAppFilterStringL( aAppListFilter, aErr );
            }
        if ( aErr == ETerminalModeSuccess )
            {
            iProfileId = aProfileId; // Stored for generating app icon path in the soap response
            AppendDataL(KAppListStart);
            for ( TInt i(0) ; i < iAppStore->AppIdArray().Count(); i++)
                 {
                 // There can be 2 conditions associated with this
                 // 1.  AppListingFilter value is "*" or empty 
                 // 2.  AppListingFilter has some filter string set
                 const RArray<TUint>& profileIdList = iAppStore->FetchRemotableApp(i).AllowedProfileIdList();
                 if ( ( profileIdList.Find(aProfileId) != KErrNotFound ) ||
                                                    ( profileIdList.Count() == KErrNone ))
                     {
                    
                     // If the provided profileID is allowed profileID for the app
                     if (  filterStringFound == KErrNotFound )                 
                         {
                         // Constructs a non-filtered soap response
                         ConstructActionResponseBufferL(i);             
                         }   
                     else                            
                         {  
                         // Constructs a filtered soap response
                         iFilteredAppList->ConstructFilteredGetAppListResponseL(i);
                         }
                     }
                 }
            if ( ( filterStringFound == KErrNotFound ) || ( (iFilteredAppList->SignatureElement()).Length() != KErrNone ))
                {
                // Append the xml signature value to the response buffer only if
                // 1. Filter string contains the signature element or
                // 2. Filter value is '*' or
                // 3. Filter value is empty
                AppendDataL(XmlSignature());
                }
            AppendDataL(KAppListEnd);        
            }
        }
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_GETAPPLICATIONLISTL_EXIT );
    return iResponseBuf;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::LaunchApp
// Method to create soap repsonse for LaunchApp action
// @param aAppId App ID of the application to be launched remotely
// @param aProfileId  ProfileID of client profile 
// @param aUrl[out] URI of the remote application where it is running
// @return Returns Terminal Mode error code
// ---------------------------------------------------------------------------------
//
TTerminalModeErrorCode CUpnpTmServerImpl::LaunchApp( TUint aAppId, TUint aProfileId, 
                                                                       RBuf8& aUrl )    
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_LAUNCHAPP_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_LAUNCHAPP, "CUpnpTmServerImpl::LaunchApp;aAppId=%d", aAppId );
    
    TTerminalModeErrorCode err = AuthorizeApp(aAppId);
    if ( err == ETerminalModeSuccess )
        {
        // Launching of the app succeeded
        err = iTmServerObserver.OnLaunchApp( aAppId,aUrl,aProfileId ); 
        }
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_LAUNCHAPP_EXIT );
    return err;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::TerminateApp
// Method to create soap repsonse for TerminateApp action
// @param aAppId App ID of the application to be terminated
// @param aProfileId  ProfileID of client profile 
// @return Returns Terminal Mode error code
// ---------------------------------------------------------------------------------
//
TTerminalModeErrorCode CUpnpTmServerImpl::TerminateApp( TUint aAppId, TUint aProfileId )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_TERMINATEAPP_ENTRY );
    TTerminalModeErrorCode err = AuthorizeApp(aAppId);
    if ( err == ETerminalModeSuccess )
        {
        // Termination of the app succeeded
        err = iTmServerObserver.OnTerminateApp( aAppId, aProfileId );
        } 
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_TERMINATEAPP, "CUpnpTmServerImpl::TerminateApp;err=%d", err );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_TERMINATEAPP_EXIT );
    return err;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GetAppStatusL
// Method to create soap repsonse for GetAppStatus action
// @param aAppIdBuffer Buffer containing the appID or the wildcard character
// @param aAppStatusResponse[out] soap response buffer 
// @param aErr[out]  Terminal Mode error code
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::GetAppStatusL( const TDesC8& aAppIdBuffer,RBuf8& aAppStatusResponse,
                                                              TTerminalModeErrorCode& aErr )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GETAPPSTATUSL_ENTRY );
    RArray<TUint> registeredAppIdArray;
    CleanupClosePushL(registeredAppIdArray);
    if ( aAppIdBuffer.Compare( KWildCard()) != KErrNone )
        {
        // If the appID is not wildcard character(*)
        TInt appIdInt(0);
        TLex8 lex( aAppIdBuffer );
        lex.Val(appIdInt);  // No need to check the return code as it has been verified earlier
        aErr = AuthorizeApp( appIdInt );
        if ( aErr != ETerminalModeSuccess )
             {
             CleanupStack::PopAndDestroy(&registeredAppIdArray);
             OstTrace0( TRACE_ERROR, CUPNPTMSERVERIMPL_GETAPPSTATUSL, "CUpnpTmServerImpl::GetAppStatusL" );
             return;  // return if the requested appID is non-existent or un-authorized
             }    
        // add to the array for which get app status has to be retreived
        registeredAppIdArray.AppendL(appIdInt);   
        }
    else
        {
        // Fetch the list of registered apps from the App store
        for ( TInt i(0); i < iAppStore->AppIdArray().Count(); i++ )
            {
            registeredAppIdArray.AppendL(iAppStore->AppIdArray()[i]);       
            }
        }
    iResponseBuf.Close();
    iResponseBuf.CreateL(KActionBufferLength);
    AppendDataL(KAppStatusListStart);
    for ( TInt i(0); i < registeredAppIdArray.Count(); i++ )
        {
        CreateGetAppStatusResponseL( registeredAppIdArray[i], aErr );
        if ( aErr != ETerminalModeSuccess )
            {
            /* Should return an error code if it fails to retrieve the 
               status for even a single application */
            break;
            }         
        }
    AppendDataL(KAppStatusListEnd);
    aAppStatusResponse.CreateL(iResponseBuf);
    CleanupStack::PopAndDestroy(&registeredAppIdArray);
    OstTraceFunctionExit0( DUP2_CUPNPTMSERVERIMPL_GETAPPSTATUSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::CreateGetAppStatusResponseL
// Helper method for GetApplicationStatus response buffer construction
// @param aAppId AppID
// @param aErr[out]  Terminal Mode error code
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::CreateGetAppStatusResponseL( TUint aAppId,TTerminalModeErrorCode& aErr )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CREATEGETAPPSTATUSRESPONSEL_ENTRY );
    // Get the app statuses of all the requested apps one by one
    TUint profileId;
    RBuf8 statusType;
    aErr = iTmServerObserver.OnGetAppStatus( aAppId, profileId, statusType);    
    if ( aErr == ETerminalModeSuccess )
       {
       /* Append the status to the buffer only if the fetching of the actual status of the 
          application from the Automotive Server is successful ,else just return the error */
       CleanupClosePushL(statusType);
       AppendDataL(KAppStatusStart);
       AppendDataL(KAppIdStart);
       AppendDataL(ConvertIntToDescriptor(aAppId));
       AppendDataL(KAppIdEnd);
       AppendDataL(KStatusStart);
       AppendDataL(KProfileIdStart);
       AppendDataL(ConvertIntToDescriptor(profileId));
       AppendDataL(KProfileIdEnd);
       AppendDataL(KStatusTypeStart);
       AppendDataL(statusType);
       AppendDataL(KStatusTypeEnd);
       AppendDataL(KStatusEnd);
       AppendDataL(KAppStatusEnd);
       CleanupStack::Pop(&statusType);
       }
    statusType.Close();
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CREATEGETAPPSTATUSRESPONSEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GetMaxNumProfiles
// Method is used to return the value of the state variable MaxNumProfiles, 
// which denotes the maximum number of client profiles that are supported 
// simultaneously by the TmClientProfile service.
// @param aNumMaxProfiles[out] Maximum number of client profiles that are supported 
// @return Returns Terminal Mode error code
// ---------------------------------------------------------------------------------
//
TTerminalModeErrorCode CUpnpTmServerImpl::GetMaxNumProfiles( TUint& aNumMaxProfiles )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GETMAXNUMPROFILES_ENTRY );
    return iTmServerObserver.OnGetMaxNumProfiles(aNumMaxProfiles);
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::SetClientProfile
// Method to create soap repsonse for SetClientProfile action.
// Method allows the control point to register a client profile and
// notify the Terminal Mode device about its preferences, settings and capabilities.
// @param aProfileId     The identifier of the profile record where the client
//                       profile settings must be stored
// @param aInputClientProfile[out]  Profile information about Terminal Mode client and
//                    its capabilities which needs to be updated in the profile record
// @param aResultProfile[out] The updated client profile.
// @return Returns Terminal Mode error code
// ---------------------------------------------------------------------------------
//
TTerminalModeErrorCode CUpnpTmServerImpl::SetClientProfile( TUint aProfileId, 
                        const TDesC8& aInputClientProfile, RBuf8& aResultProfile )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_SETCLIENTPROFILE_ENTRY );
    return iTmServerObserver.OnSetClientProfile( aProfileId, aInputClientProfile,
                                                                aResultProfile );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GetClientProfile
// Method to create soap repsonse for GetClientProfile action.
// Method allows the control point to access the contents of a client profile stored 
// in the Terminal Mode device.
// @param aProfileId  The identifier of the profile record where the client 
//                     profile settings must be stored
// @param aClientProfile[out] Client profile corresponding to the profileID input variable
// @return Returns Terminal Mode error code
// ---------------------------------------------------------------------------------
//
TTerminalModeErrorCode CUpnpTmServerImpl::GetClientProfile( TUint aProfileId, RBuf8& aClientProfile )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GETCLIENTPROFILE_ENTRY );
    return iTmServerObserver.OnGetClientProfile( aProfileId,aClientProfile );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GetIconInfoL
// Method to fetch the icon file associated with the particular application. 
// @param aUrl Relative URL of the application icon to be retreived
// @param aResultTrans CUpnpHttpServerTransaction object
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::GetIconInfoL( const TDesC8& aUrl, CUpnpHttpServerTransaction*& aResultTrans )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GETICONINFOL_ENTRY );
    RBuf svgFilePath;   
    // Fetch the original file path which has been mapped to the requesting file.
    TInt fileIndex = iIconMapping->GetSvgIconPath( aUrl,svgFilePath );
    if ( fileIndex == KErrNotFound )
        {
        // In case the requested icon file is not available, should return EHttpNotFound(404). 
        // Sets the file source as an empty descriptor.
        aResultTrans = CUpnpIconFileServeTransaction::NewL( KNullDesC(), iIconFileSession );  
        svgFilePath.Close();
        OstTraceFunctionExit0( CUPNPTMSERVERIMPL_GETICONINFOL_EXIT );
        return;
        }
    CleanupClosePushL(svgFilePath);
    /* Extracts the profileID from the relative URL.
     * The relative URL will be of the form ( /<profileID>/<iconName> )
     */
    const TDesC8& urlWithoutLeadingSlash = aUrl.Mid((aUrl.Locate(KForwardSlash))+1);
    TInt secondSlashPos = urlWithoutLeadingSlash.Locate(KForwardSlash);
    User::LeaveIfError(secondSlashPos);
    const TDesC8& profileIdBuf = urlWithoutLeadingSlash.Left(secondSlashPos);
    // Converts descriptor to int
    TLex8 lex( profileIdBuf );
    TUint profileIdInt(KErrNone);
    lex.Val(profileIdInt);
    // Fetch the reference to client icon preference object from AutomotiveServer
    const CUpnpTmClientIconPref& clientIconPref = iTmServerObserver.GetClientIconPrefsL(profileIdInt);
    
    TInt servingIconWidth(KErrNone);
    TInt servingIconHeight(KErrNone);
    /* 
     * Checks whether the conversion of the icon file needed.
     * If conversion is required, it also determines the dimension(width and height) 
     * of the serving file.
     */ 
    TBool iconConversionNeeded = iIconMapping->IsConversionNeeded( fileIndex, clientIconPref, servingIconWidth,servingIconHeight );
    if ( iconConversionNeeded )
        {
        // Icon file conversion is needed( svg to bmp )
        CUpnpSvgImageConverter* decoder = CUpnpSvgImageConverter::NewL( servingIconWidth, servingIconHeight );
        CleanupStack::PushL(decoder);
        RBuf bitmapBuf;
        CleanupClosePushL(bitmapBuf);
        decoder->ConvertToBitmapL( svgFilePath, bitmapBuf );  // Method which in turn invokes the conversion
        aResultTrans = CUpnpIconFileServeTransaction::NewL( bitmapBuf, iIconFileSession ); /* Sets the data source( file ) 
                                                                                           which needs to be serverd*/
        CleanupStack::PopAndDestroy( 2,decoder);
        }
    else
        {
        // No icon file conversion needed ( svg icon is served )
        aResultTrans = CUpnpIconFileServeTransaction::NewL( svgFilePath, iIconFileSession );
        }
    CleanupStack::PopAndDestroy(&svgFilePath);
    OstTraceFunctionExit0( DUP1_CUPNPTMSERVERIMPL_GETICONINFOL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::StartDeviceL
// Method is used to start publishing the terminal mode server device and 
// its services
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::StartDeviceL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_STARTDEVICEL_ENTRY );
    iTmServerDevice->StartTmServerDeviceL();
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_STARTDEVICEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::StopDeviceL
// Method is used to stop publishing the terminal mode server device and 
// its services
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::StopDeviceL()
    {   
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_STOPDEVICEL_ENTRY );
    iTmServerDevice->StopTmServerDeviceL();
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_STOPDEVICEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GetRemotableApp
// Method is invoked by the Automotive Server to get the access to the particular
// instance of the remotable app corresponding to the app ID provided.
// This is needed in case AS wishes to modify any paramters of the already 
// registered remotable app
// @param aAppId App ID of the application whose access is needed
// @param aErr[out]  Error code
// @returns Returns reference to the Remotable App object
// ---------------------------------------------------------------------------------
//
CUpnpRemotableApp& CUpnpTmServerImpl::GetRemotableApp( TUint aAppId, TInt& aErr )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GETREMOTABLEAPP_ENTRY );
    aErr = KErrNotFound;
    TInt appIndex = iAppStore->AppIdArray().Find(aAppId);
    CUpnpRemotableApp* remotableApp = NULL;
    if ( appIndex != KErrNotFound )
        {
        // The requested application is available
        aErr = KErrNone;    
        remotableApp = &(iAppStore->FetchRemotableApp(appIndex));  
        }
    OstTraceExt2( TRACE_NORMAL, CUPNPTMSERVERIMPL_GETREMOTABLEAPP, "CUpnpTmServerImpl::GetRemotableApp;aAppId=%d;aErr=%d", aAppId, aErr );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_GETREMOTABLEAPP_EXIT );
    return *remotableApp;
    }

// ---------------------------------------------------------------------------------
// Method is used to un-register the individual applications 
// @param aRemotableApp  Pointer to Remotable app object which needs to 
 //                      be registered.Ownership is passed.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::RegisterAppL( CUpnpRemotableApp* aRemotableApp )
    {   
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_REGISTERAPPL_ENTRY );
    ASSERT( aRemotableApp );   
    iAppStore->AddRemotableAppL( aRemotableApp );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_REGISTERAPPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::RegisterAppsL
// Method is used to register multiple applications in one go
// @param aRemotableAppList Array of Remotable App objects which needs to be
//                          registered. Ownership is passed.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::RegisterAppsL( const RPointerArray<CUpnpRemotableApp>& aRemotableAppList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_REGISTERAPPSL_ENTRY );
    for ( TInt i(0); i < aRemotableAppList.Count(); i++ )
        {
        iAppStore->AddRemotableAppL( aRemotableAppList[i] );
        }
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_REGISTERAPPSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::UnRegisterApp
// Method is used to un-register the individual applications 
// @param aAppId App ID which needs to be un-registered
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerImpl::UnRegisterApp( TUint aAppId )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_UNREGISTERAPP_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVER_GETREMOTABLEAPP, "CUpnpTmServer::GetRemotableApp;aAppId=%u", aAppId );
    return iAppStore->RemoveRemotableApp(aAppId);
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::UnRegisterApps
// Method is used to un-register multiple applications in one go 
// param aAppIdArray an array of appIDs which needs to be un-registered
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerImpl::UnRegisterApps( const RArray<TUint>& aAppIdArray )
    {   
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_UNREGISTERAPPS_ENTRY );
    /**
     * Create a copy of the array.
     * Only distinct and unique app IDs are copied into the array.
     */
    RArray<TUint> appIdList ;
    TInt i;
    for ( i = 0; i < aAppIdArray.Count(); i++ )
        {
        OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_UNREGISTERAPPS, "CUpnpTmServerImpl::UnRegisterApps;aAppIdArray[i]=%d", aAppIdArray[i] );
        if ( appIdList.Find(aAppIdArray[i]) == KErrNotFound )
            {
            appIdList.Append(aAppIdArray[i]);
            }
        }
    for ( i = 0; i < appIdList.Count(); i++ )
        { 
        // Now remove the remotable app from the array one by one
        if ( ( iAppStore->RemoveRemotableApp( appIdList[i] )) == KErrNotFound )
            {
            /** 
              * Even if a single app from the requested array is unable 
              * to get un-registered the method should return KErrNotFound 
              */
            OstTrace1( TRACE_ERROR, DUP1_CUPNPTMSERVERIMPL_UNREGISTERAPPS, "CUpnpTmServerImpl::UnRegisterApps;appIdList[i]=%d", appIdList[i] );
            appIdList.Close();
            OstTraceFunctionExit0( CUPNPTMSERVERIMPL_UNREGISTERAPPS_EXIT );
            return KErrNotFound;
            }
        }  
    appIdList.Close();
    OstTraceFunctionExit0( DUP1_CUPNPTMSERVERIMPL_UNREGISTERAPPS_EXIT );
    return KErrNone;      
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::SetXmlSignatureL
// Method is used to add xml formatted signature value to the list of remotable 
// apps which is provided in response to the GetApplicationList action.
// XML signature is applied over entire contents of the appList element
// @param aSignature XML formatted signature value
//                              objects
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::SetXmlSignatureL( const TDesC8& aSignature )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_SETXMLSIGNATUREL_ENTRY );
    iXmSignature.Close();
    iXmSignature.CreateL(aSignature);
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_SETXMLSIGNATUREL_EXIT );
    }

// --------------------------------------------------------------------------------------------------------------------
// CUpnpTmServerImpl::XmlSignature
// Method is used to fetch xml formatted signature value 
// @return Returns xml formatted signature buffer 
// --------------------------------------------------------------------------------------------------------------------
//
const TDesC8& CUpnpTmServerImpl::XmlSignature( )const
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_XMLSIGNATURE_ENTRY );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_XMLSIGNATURE_EXIT );
    return iXmSignature;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::UpdateAppStatusL
// Method is used to update the status of applications which have undergone some 
// change. This method is invoked by Automotive Server
// @param aUpdatedAppIdList An array of app IDs corresponding to the applications
//                          whose status have have undergone change
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::UpdateAppStatusL( const RArray<TUint>& aUpdatedAppIdList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_UPDATEAPPSTATUSL_ENTRY );
    CreateEventMessageL(aUpdatedAppIdList);
    iTmServerDevice->AppStatusUpdateL(iResponseBuf);
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_UPDATEAPPSTATUSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::UpdateAppListL
// Method is used to update the list of applications which have been either added,
// removed or modified. This method is invoked by Automotive Server
// @param aUpdatedAppIdList An array of app IDs corresponding to the applications
//                          which have added, removed or modified to/from the list
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::UpdateAppListL( const RArray<TUint>& aUpdatedAppIdList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_UPDATEAPPLISTL_ENTRY );
    CreateEventMessageL(aUpdatedAppIdList);
    iTmServerDevice->AppListUpdateL(iResponseBuf);    
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_UPDATEAPPLISTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::UpdateUnusedProfileIdsL
// Method is used to update the list of profile IDs for profiles which are currently  
// not being used by any Terminal Mode service hosted.on the Terminal Mode device
// This method is invoked by Automotive Server
// @param aUnusedProfileIdList List of profile IDs for profiles which are currently 
//                             not being used
//                             
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::UpdateUnusedProfileIdsL( const RArray<TUint>& aUnusedProfileIdList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_UPDATEUNUSEDPROFILEIDSL_ENTRY );
    CreateEventMessageL(aUnusedProfileIdList);
    iTmServerDevice->UnUsedProfileIdUpdateL(iResponseBuf);     
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_UPDATEUNUSEDPROFILEIDSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::AuthorizeApp
// This method is used to verify whether the requested appID exists and whether
// the client is authorized to perform the oprations on that App ID
// @param aAppID  appID which for which operation is requested
// @return Returns ETerminalModeSuccess if appID is valid and authorized 
//         else the error code
// ---------------------------------------------------------------------------------
//
TTerminalModeErrorCode CUpnpTmServerImpl::AuthorizeApp( TUint aAppId )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_AUTHORIZEAPP_ENTRY );
    OstTrace1( TRACE_NORMAL, DUP2_CUPNPTMSERVERIMPL_AUTHORIZEAPP, "CUpnpTmServerImpl::AuthorizeApp;aAppId=%u", aAppId );
    
    TTerminalModeErrorCode err;
    if ( iAppStore->AppIdArray().Find(aAppId) != KErrNotFound )
        {
        // requested appID is a valid one
        err = ETerminalModeSuccess;
        }
    else
        {
        // Requested appID does not exist
        err = ETerminalModeBadAppId;
        }
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_AUTHORIZEAPP, "CUpnpTmServerImpl::AuthorizeApp;err=%d", err );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_AUTHORIZEAPP_EXIT );
    return err;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::AppendDataL
// Appends data to the buffer and also reallocates when needed
// @param aData data to be appended to the buffer
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::AppendDataL( const TDesC8& aData )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_APPENDDATAL_ENTRY );
    TInt actionBufMaxLen = iResponseBuf.MaxLength();
    TInt actionBufLen = iResponseBuf.Length();
    TInt dataLen = aData.Length();
    if ( actionBufMaxLen < ( actionBufLen + dataLen ) )
        {
        TInt lenToAllocate = (dataLen > KActionBufferLength) ? dataLen : KActionBufferLength;
        iResponseBuf.ReAllocL ( actionBufLen + lenToAllocate );
        }   
    iResponseBuf.Append( aData );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_APPENDDATAL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::ConstructActionResponseBufferL
// Constructs the actual soap response buffer
// @param aAppIndex Index of the registered App
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::ConstructActionResponseBufferL( TInt aAppIndex )    
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CONSTRUCTACTIONRESPONSEBUFFERL_ENTRY );
    ConstructLeadingSoapBufferL(aAppIndex);
    const TDesC8& appDesc = iAppStore->FetchRemotableApp(aAppIndex).AppDescription();
    if ( appDesc.Length() != KErrNone )
        {
        // Appends the application desciption
        AppendDataL(KStartNameDesc);
        AppendDataL(appDesc);
        AppendDataL(KEndNameDesc);   
        } 
    // Appends the optional icon information to the soap response buffer only 
    // if its available.
    const RPointerArray<CUpnpTerminalModeIcon>& iconList = iAppStore->FetchRemotableApp(aAppIndex).IconList();
    TInt iconCount = iconList.Count();
    if ( iconCount > KErrNone )
        {
        // Icon information is present
        AppendDataL(KStartIconList);                     
        for ( TInt j(0); j < iconCount; j++ )
            {
            // Adds all the icons associated with this app one by one
            SetRemotableAppIconInfoL( *iconList[j] );
            }
        AppendDataL(KEndIconList);
        }
    //Apends the allowedProfileIDs element
    SetAllowedProfileIdListL(aAppIndex);
    //Appends the different terminalmode info elements
    const RPointerArray<CUpnpTmInfoElement>& tmInfoElementList = 
                                 iAppStore->FetchRemotableApp(aAppIndex).TmInfoElementList();
    for ( TInt k(0); k < tmInfoElementList.Count(); k++)
        {
        // Called for each terminal mode element associated with the App
        SetTerminalModeInfoElementL( *tmInfoElementList[k] );
        }
    // Appends the resource status element for the app if it is available 
    SetResourceStatusL(aAppIndex);
    // End of an application
    AppendDataL(KEndApp);
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CONSTRUCTACTIONRESPONSEBUFFERL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::ConstructLeadingSoapBufferL
// Constructs leading soap buffer
// It adds appID and appName to the soap response
// @param aAppIndex Index of the remotable app
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::ConstructLeadingSoapBufferL( TInt aAppIndex )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CONSTRUCTLEADINGSOAPBUFFERL_ENTRY );
    AppendDataL(KStartApp);
    AppendDataL(KAppIdStart);
    AppendDataL(ConvertIntToDescriptor(iAppStore->FetchRemotableApp(aAppIndex).AppId()));
    AppendDataL(KAppIdEnd);
    AppendDataL(KNameStart);
    AppendDataL(iAppStore->FetchRemotableApp(aAppIndex).AppName());
    AppendDataL(KNameEnd);
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CONSTRUCTLEADINGSOAPBUFFERL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::SetRemotableAppIconInfoL
// Appends application icon information to the soap response buffer.
// It stores the icon information in Tm Icon Mapping object for further reference.
// @param aTmIcon  Reference to icon object 
// @param aWidth   Width of the application icon.
// @param aHeight  Height of the application icon.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::SetRemotableAppIconInfoL( const CUpnpTerminalModeIcon& aTmIcon )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_SETREMOTABLEAPPICONINFOL_ENTRY );
    AppendDataL(KIconStart);
    AppendDataL(KMimeTypeStart);
    AppendDataL(aTmIcon.MimeType());
    AppendDataL(KMimeTypeEnd);
    AppendDataL(KWidthStart);
    TInt width = aTmIcon.Width();
    AppendDataL(ConvertIntToDescriptor(width));
    AppendDataL(KWidthEnd);
    AppendDataL(KHeightStart);
    TInt height = aTmIcon.Height();
    AppendDataL(ConvertIntToDescriptor(height));
    AppendDataL(KHeightEnd);
    AppendDataL(KDepthStart);
    AppendDataL(ConvertIntToDescriptor(aTmIcon.Depth()));
    AppendDataL(KDepthEnd);
    //Application icon URL creation
    AppendDataL(KUrlStart);
    AppendDataL(UpnpHTTP::KHTTPUrl());        
    AppendDataL(iAddrBuf);  // Adds IP address
    AppendDataL(KColon);
    AppendDataL(iPortBuf);  // Adds port number
    
    // Fetch the icon file path
    const TDesC& filePath = aTmIcon.IconFilename();
    // Extract just filename from the complete file path
    const TDesC& fileName = filePath.Mid((filePath.LocateReverse(KDirectorySeparator))+1);
    TBuf8<UpnpString::KDefaultStringLength> fileName8;
    fileName8.Copy(fileName);  // 16-bit to 8-bit conversion( icon filename )
    
    TBuf8<UpnpString::KDefaultStringLength> iconRelativeUrl(UpnpString::KSlash);
    // Appends the client profileID
    iconRelativeUrl.Append(ConvertIntToDescriptor(iProfileId));
    iconRelativeUrl.Append(UpnpString::KSlash);   
    // Appends the icon filename and creates the relative url
    iconRelativeUrl.Append(fileName8);
    AppendDataL(iconRelativeUrl);
    /** Icon information is stored for making decision during http request for app icon file.
     *  Depth is not taken into consideration while deciding on the final icon dimension
     *  because it is not a requirement and playing with the depth of the icon may lead
     *  to a poorly converted image which may not be displayed properly in the car 
     *  kit screen.
     */
    iIconMapping->AddIconFileInfoL( iconRelativeUrl,filePath,width,height );
    AppendDataL(KUrlEnd);
    AppendDataL(KIconEnd);
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_SETREMOTABLEAPPICONINFOL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::SetAllowedProfileIdListL
// Appends  the list of allowed profile IDs information to the soap response buffer
// @param aAppIndex Index of the registered app
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::SetAllowedProfileIdListL( TInt aAppIndex )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_SETALLOWEDPROFILEIDLISTL_ENTRY );
    const RArray<TUint>& profileIdList = iAppStore->FetchRemotableApp(aAppIndex).AllowedProfileIdList();
    TInt profileIdCount = profileIdList.Count();
    if ( profileIdCount > KErrNone )
        {
        AppendDataL(KAllowedProfileIdsStart);
        for ( TInt i(0); i < profileIdCount; i++ )
            {
            AppendDataL(ConvertIntToDescriptor(profileIdList[i]));
            if ( i < (profileIdCount-1) )
                {
                // Adds comma after each profile ID except last 
                AppendDataL(UpnpString::KComa);
                }
            }
        AppendDataL(KAllowedProfileIdsEnd);
        }
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_SETALLOWEDPROFILEIDLISTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::SetTerminalModeInfoElementL
// Appends terminal mode info elements to the soap buffer
// @param aTmInfoElement Reference to CUpnpTmInfoElement object
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::SetTerminalModeInfoElementL( const CUpnpTmInfoElement&  aTmInfoElement )                                                              
    {  
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_SETTERMINALMODEINFOELEMENTL_ENTRY );
    AppendDataL(KStartTagOpenBracket);
    AppendDataL(aTmInfoElement.TerminalModeInfoType());
    AppendDataL(KCloseBracket);
    for ( TInt i(0); i < aTmInfoElement.TmInfoElementCount(); i++ )
        {
        RBuf8 elementName;
        CleanupClosePushL(elementName);
        RBuf8 elementValue;
        CleanupClosePushL(elementValue);
        // Fetch the key-value pair
        aTmInfoElement.GetTmInfoElementL( elementName, elementValue, i );
        AppendDataL(KStartTagOpenBracket);
        AppendDataL(elementName);
        AppendDataL(KCloseBracket);
        AppendDataL(elementValue); 
        AppendDataL(KEndTagOpenBracket);
        AppendDataL(elementName);
        AppendDataL(KCloseBracket);
        CleanupStack::PopAndDestroy(2,&elementName);
        }   
    AppendDataL(KEndTagOpenBracket);
    AppendDataL(aTmInfoElement.TerminalModeInfoType());
    AppendDataL(KCloseBracket);
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_SETTERMINALMODEINFOELEMENTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::SetResourceStatusL
// Appends resource status element(optional element)to the soap response buffer.
// Provides the status of the resource of the application.
// @param aAppIndex Index of the registered app
// --------------------------------------------------------------------------------- 
//
void CUpnpTmServerImpl::SetResourceStatusL( TInt aAppIndex )
    {  
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_SETRESOURCESTATUSL_ENTRY );
    const TDesC8& rscStatus = iAppStore->FetchRemotableApp(aAppIndex).ResourceStatus();
    if ( rscStatus.Length() > KErrNone ) 
        {
        AppendDataL(KResourceStatusStart);
        AppendDataL(rscStatus);
        AppendDataL(KResourceStatusEnd);
        }
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_SETRESOURCESTATUSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::GenerateUrlComponents
// Utility method to generate Server's IP address and port number which is used 
// in constructing the icon url in action responses
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::GenerateUrlComponents( CUpnpHttpServerSession& aHttpServerSession )   
   {
   OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_GENERATEURLCOMPONENTS_ENTRY );
   TInetAddr servAddr;
   servAddr.SetFamily(KAfInet);
   aHttpServerSession.GetAddress( servAddr );
   TBuf<UpnpString::KShortStringLength> addrBuf8;
   servAddr.Output( addrBuf8 );
   iAddrBuf.Copy(addrBuf8); //Sets the IP address of the server
   iPortBuf.Num(servAddr.Port()); //Sets the port number
   OstTraceFunctionExit0( CUPNPTMSERVERIMPL_GENERATEURLCOMPONENTS_EXIT );
   }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::CreateEventMessageL
// Utility method to generate GENA notificatiion messages.
// Its is called in case of updateapplist, updateappstatuslist and
// unusedprofileidlist events.
// @param aIdList Reference to the changed ID list. It could be appID list or 
//                profileID list.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerImpl::CreateEventMessageL( const RArray<TUint>& aIdList )   
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CREATEEVENTMESSAGEL_ENTRY );
    iResponseBuf.Close();
    iResponseBuf.CreateL(KActionBufferLength);
    /**
     * Create a copy of the array.
     * Only distinct and unique elements are copied into the array.
     */
    RArray<TUint> idList;
    CleanupClosePushL(idList);
    TInt i;
    for ( i = 0; i < aIdList.Count(); i++ )
        {
        OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_CREATEEVENTMESSAGEL, "CUpnpTmServerImpl::CreateEventMessageL;aIdList[i]=%d", aIdList[i] );
        if ( idList.Find(aIdList[i]) == KErrNotFound )
            {
            idList.AppendL(aIdList[i]);
            }
        }
    TInt idCount = idList.Count();
    for ( i = 0; i < idCount; i++ )
        {  
        AppendDataL(ConvertIntToDescriptor( idList[i]));
        if ( i < ( idCount-1) )
            {
            //Add comma(,) after each ID except the last
            AppendDataL(UpnpString::KComa);
            }
    }
   CleanupStack::PopAndDestroy(&idList);
   OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CREATEEVENTMESSAGEL_EXIT );
   }

// ---------------------------------------------------------------------------------
// CUpnpTmServerImpl::ConvertIntToDescriptor
// Utility method to convert integer to descriptor
// @param  aIntVal  Integer value
// @return  Returns corresponding descriptor value
// ---------------------------------------------------------------------------------
//
const TDesC8& CUpnpTmServerImpl::ConvertIntToDescriptor( TUint aIntVal )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERIMPL_CONVERTINTTODESCRIPTOR_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERIMPL_CONVERTINTTODESCRIPTOR, "CUpnpTmServerImpl::ConvertIntToDescriptor;aIntVal=%u", aIntVal );
    iBufVal.Num( aIntVal );
    OstTraceFunctionExit0( CUPNPTMSERVERIMPL_CONVERTINTTODESCRIPTOR_EXIT );
    return iBufVal;
    }

//End of File
