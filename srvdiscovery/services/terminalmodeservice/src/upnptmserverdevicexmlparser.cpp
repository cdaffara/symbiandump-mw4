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
* Description: CUpnpTmServerDeviceXmlParser class implementation.
*
*/

// System Includes
#include <xml/parserfeature.h>
#include <xml/taginfo.h>

// User Includes
#include "upnptmserverdevicexmlparser.h"
#include "upnpterminalmodeicon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmserverdevicexmlparserTraces.h"
#endif


//Literals
_LIT8(KTextXml,           "text/xml");
_LIT8(KRootDeviceXmlLead, "<?xml version=\"1.0\" encoding=\"utf-8\"?><root xmlns=\"urn:schemas-upnp-org:device-1-0\"><specVersion><major>1</major><minor>0</minor></specVersion><device><deviceType>urn:schemas-upnp-org:device:TmServerDevice:1</deviceType><friendlyName>S60</friendlyName><manufacturer /><manufacturerURL>http://www.nokia.com</manufacturerURL><modelName /><serialNumber /><UDN /><X_connectivity><bluetooth><btAddr /><startConnection /></bluetooth></X_connectivity>");
_LIT8(KRootDeviceXmlTrail,"<serviceList><service><serviceType>urn:schemas-upnp-org:service:TmApplicationServer:1</serviceType><serviceId>urn:upnp-org:serviceId:TmApplicationServer1</serviceId><SCPDURL>/tmserverdevice1/tmapplicationserver1.xml</SCPDURL><controlURL>/tmserverdevice1/tmapplicationserver1/control</controlURL><eventSubURL>/tmserverdevice1/tmapplicationserver1/eventing</eventSubURL></service><service><serviceType>urn:schemas-upnp-org:service:TmClientProfile:1</serviceType>"
    "<serviceId>urn:upnp-org:serviceId:TmClientProfile1</serviceId><SCPDURL>/tmserverdevice1/tmclientprofile1.xml</SCPDURL><controlURL>/tmserverdevice1/tmclientprofile1/control</controlURL><eventSubURL>/tmserverdevice1/tmclientprofile1/eventing</eventSubURL></service></serviceList></device></root>");
_LIT8(KAppServerSrvXml,   "<?xml version=\"1.0\" encoding=\"utf-8\"?><scpd xmlns=\"urn:schemas-upnp-org:service-1-0\"><specVersion><major>1</major> <minor>0</minor> </specVersion><actionList><action><name>GetApplicationList</name><argumentList><argument><name>AppListingFilter</name><direction>in</direction> <relatedStateVariable>A_ARG_TYPE_String</relatedStateVariable> </argument><argument><name>ProfileID</name><direction>in</direction> <relatedStateVariable>A_ARG_TYPE_ProfileID</relatedStateVariable> "
    "</argument><argument><name>AppListing</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_AppList</relatedStateVariable></argument></argumentList></action><action><name>LaunchApplication</name><argumentList><argument><name>AppID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_AppID</relatedStateVariable></argument><argument><name>ProfileID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_ProfileID</relatedStateVariable></argument><argument><name>AppURI</name>"
    "<direction>out</direction><relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable></argument></argumentList></action><action><name>TerminateApplication</name><argumentList><argument><name>AppID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_AppID</relatedStateVariable></argument><argument><name>ProfileID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_ProfileID</relatedStateVariable></argument><argument><name>TerminationResult</name><direction>out</direction>"
    "<relatedStateVariable>A_ARG_TYPE_Bool</relatedStateVariable></argument></argumentList></action><action><name>GetApplicationStatus</name><argumentList><argument><name>AppID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_AppID</relatedStateVariable></argument><argument><name>AppStatus</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_AppStatus</relatedStateVariable></argument></argumentList></action></actionList><serviceStateTable><stateVariable sendEvents=\"yes\">"
    "<name>AppStatusUpdate</name> <dataType>string</dataType></stateVariable><stateVariable sendEvents=\"yes\"><name>AppListUpdate</name> <dataType>string</dataType></stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_AppStatus</name> <dataType>string</dataType> </stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_AppList</name><dataType>string</dataType></stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_AppID</name><dataType>string</dataType></stateVariable>"
    "<stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_ProfileID</name><dataType>ui4</dataType><defaultValue>0</defaultValue></stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_URI</name> <dataType>uri</dataType> </stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_String</name><dataType>string</dataType></stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_Bool</name><dataType>string</dataType><defaultValue>false</defaultValue><allowedValueList><allowedValue>false</allowedValue>"
    "<allowedValue>true</allowedValue></allowedValueList></stateVariable></serviceStateTable></scpd>");
_LIT8(KClientProfSrvXml,  "<?xml version=\"1.0\" encoding=\"utf-8\"?><scpd xmlns=\"urn:schemas-upnp-org:service-1-0\"><specVersion><major>1</major> <minor>0</minor> </specVersion><actionList><action><name>GetMaxNumProfiles</name><argumentList><argument><name>numProfilesAllowed</name><direction>out</direction> <relatedStateVariable>MaxNumProfiles</relatedStateVariable> </argument></argumentList></action><action><name>SetClientProfile</name><argumentList><argument><name>profileID</name><direction>in</direction>"
    "<relatedStateVariable>A_ARG_TYPE_ProfileID</relatedStateVariable></argument><argument><name>clientProfile</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_ClientProfile</relatedStateVariable></argument><argument><name>resultProfile</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_ClientProfile</relatedStateVariable></argument></argumentList></action><action><name>GetClientProfile</name><argumentList><argument><name>profileID</name><direction>in</direction>"
    "<relatedStateVariable>A_ARG_TYPE_ProfileID</relatedStateVariable></argument><argument><name>clientProfile</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_ClientProfile</relatedStateVariable></argument></argumentList></action></actionList><serviceStateTable><stateVariable sendEvents=\"yes\"><name>UnusedProfileIDs</name> <dataType>string</dataType></stateVariable><stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_ClientProfile</name> <dataType>string</dataType></stateVariable>"
    "<stateVariable sendEvents=\"no\"><name>A_ARG_TYPE_ProfileID</name><dataType>ui4</dataType><defaultValue>0</defaultValue></stateVariable><stateVariable sendEvents=\"no\"><name>MaxNumProfiles</name> <dataType>ui2</dataType> <defaultValue>1</defaultValue> </stateVariable> </serviceStateTable></scpd>");
_LIT8(KStartLine,         "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
_LIT8(KRootAttrb,         " xmlns=\"urn:schemas-upnp-org:device-1-0\"");
_LIT8(KScpdUrl,           "/tmserverdevice1/");
_LIT8(KUpnpDeviceRoot,    "root" );
_LIT8(KStartIconList,     "<iconList>" );
_LIT8(KEndIconList,       "</iconList>" );
_LIT8(KStartIcon,         "<icon>" );
_LIT8(KEndIcon,           "</icon>" );
_LIT8(KStartMimeType,     "<mimetype>" );
_LIT8(KEndMimeType,       "</mimetype>" );
_LIT8(KStartWidth,        "<width>" );
_LIT8(KEndWidth,          "</width>" );
_LIT8(KStartHeight,       "<height>" );
_LIT8(KEndHeight,         "</height>" );
_LIT8(KStartDepth,        "<depth>" );
_LIT8(KEndDepth,          "</depth>" );
_LIT8(KStartUrl,          "<url>" );
_LIT8(KEndUrl,            "</url>" );
_LIT8(KPhoneManufacturer, "manufacturer" );
_LIT8(KPhoneModel,        "modelName" );
_LIT8(KPhoneSerialNumber, "serialNumber" );
_LIT8(KBtAddress,         "btAddr" );
_LIT8(KStartConnection,   "startConnection" );
_LIT8(KIconPath,          "icon/");
_LIT8(KConnTrue,          "true");
_LIT8(KConnFalse,         "false");
_LIT(KPublicDevicePath,   "public\\TmServerDevice1\\");
_LIT(KDeviceName,         "TmServerDevice1.xml");
_LIT(KAppServerSrvName,   "TmApplicationServer1.xml");
_LIT(KClientProfSrvName,  "TmClientProfile1.xml");
_LIT(KIconDirectory,      "icon\\");


// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDeviceXmlParser* CUpnpTmServerDeviceXmlParser::NewL( CUpnpTmServerDeviceInfo& aDeviceInfo )
	{
	OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_NEWL_ENTRY );
	CUpnpTmServerDeviceXmlParser* self = new (ELeave) CUpnpTmServerDeviceXmlParser( aDeviceInfo );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_NEWL_EXIT );
	return self;	
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::~CUpnpTmServerDeviceXmlParser
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDeviceXmlParser::~CUpnpTmServerDeviceXmlParser()	
	{
	OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_CUPNPTMSERVERDEVICEXMLPARSER_ENTRY );
	DeleteDirectory();
	iPhone.Close();
	iEtelSrv.Close();
	delete iParser;
	iAppServerSrvDescription.Close();
	iClientProfSrvDescription.Close();
	iReadBuf.Close();
	iDeviceDescription.Close();
	iDescFilePath.Close();
	iClientProfSrvFilePath.Close();
	iAppServerSrvFilePath.Close();
	iDescriptionUri.Close();
	iFs.Close();
	iDeviceDir.Close();
	iIconDirectory.Close();
	delete iFileMan;
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_CUPNPTMSERVERDEVICEXMLPARSER_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::DeleteDirectory
// Deletes device and service description files and device icon files related to  
// Terminal Mode Service located in the private directory.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceXmlParser::DeleteDirectory()   
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_DELETEDIRECTORY_ENTRY );
    iFs.Delete(iDescFilePath);
    iFs.Delete(iAppServerSrvFilePath);
    iFs.Delete(iClientProfSrvFilePath); 
    iFileMan->RmDir(iIconDirectory);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_DELETEDIRECTORY_EXIT );
    }
// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::CUpnpTmServerDeviceXmlParser
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDeviceXmlParser::CUpnpTmServerDeviceXmlParser( CUpnpTmServerDeviceInfo& aDeviceInfo )
    :iDeviceInfo(aDeviceInfo)
	{		
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceXmlParser::ConstructL( )
	{
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_CONSTRUCTL_ENTRY );
    User::LeaveIfError(iEtelSrv.Connect());
    User::LeaveIfError(iEtelSrv.LoadPhoneModule( KMmTsyModuleName ));
    User::LeaveIfError(iPhone.Open( iEtelSrv, KMmTsyPhoneName ));
    TRequestStatus status;
    iPhone.GetPhoneId(status, iPhoneId); 
    User::WaitForRequest(status);
	User::LeaveIfError(iFs.Connect());
    iAppServerSrvDescription.CreateL( KAppServerSrvXml() );
    iClientProfSrvDescription.CreateL( KClientProfSrvXml() );
	iParser = CParser::NewL(KTextXml(), *this);
	iParser->EnableFeature(ESendFullContentInOneChunk);
	iReadBuf.CreateMaxL(KBufSize);
	iDeviceDescription.CreateMaxL(KBufSize);
	iDeviceDescription.SetLength(KErrNone);
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_CONSTRUCTL_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnStartDocumentL
// Called when parser hits the start of the document
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceXmlParser::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/,
                                                                             TInt aErrorCode )
	{
	OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONSTARTDOCUMENTL_ENTRY );
	User::LeaveIfError(aErrorCode);
    
    TBuf<KMaxBufLength> tmpBuf(iDeviceDir);
    tmpBuf.Append(KDeviceName());
    iDescFilePath.CreateL(tmpBuf);
    tmpBuf.Zero();
    
    // Create TmAppServer service xml
    tmpBuf.Copy(iDeviceDir);
    tmpBuf.Append(KAppServerSrvName());
    RFile appServerSrvFile;
    CleanupClosePushL(appServerSrvFile);
    TInt err(KErrNone);
    err = appServerSrvFile.Create(iFs, tmpBuf, EFileWrite);
    if ( err == KErrAlreadyExists )
        {
        User::LeaveIfError(appServerSrvFile.Open(iFs, tmpBuf, EFileWrite));
        }
    else
        {
        User::LeaveIfError(err);
        }
    User::LeaveIfError(appServerSrvFile.Write(iAppServerSrvDescription));
    CleanupStack::PopAndDestroy( &appServerSrvFile );
    iAppServerSrvFilePath.CreateL(tmpBuf);
    tmpBuf.Zero();
    
    // Create TmClientProfile service xml
    tmpBuf.Copy(iDeviceDir);
    tmpBuf.Append(KClientProfSrvName());
    RFile clientProfSrvFile;
    CleanupClosePushL(clientProfSrvFile);
    err = clientProfSrvFile.Create(iFs, tmpBuf, EFileWrite);
    if ( err == KErrAlreadyExists )
        {
        User::LeaveIfError(clientProfSrvFile.Open(iFs, tmpBuf, EFileWrite));
        }
    else
        {
        User::LeaveIfError(err);
        }
    User::LeaveIfError(clientProfSrvFile.Write(iClientProfSrvDescription));
    CleanupStack::PopAndDestroy( &clientProfSrvFile );
    iClientProfSrvFilePath.CreateL(tmpBuf);
    
    iDeviceDescription.Append(KStartLine());
    iDescriptionUri.CreateMaxL(UpnpString::KDefaultStringLength);
    iDescriptionUri.SetLength(KErrNone);
    iDescriptionUri.Append(KScpdUrl());
    iDescriptionUri.Append(KDeviceName());
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONSTARTDOCUMENTL_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnEndDocumentL
// Called when parser hits the end of the document
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceXmlParser::OnEndDocumentL(TInt aErrorCode)
	{
	OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONENDDOCUMENTL_ENTRY );
	User::LeaveIfError(aErrorCode);
	
	RFile deviceFile;
	CleanupClosePushL(deviceFile);
	TInt err = deviceFile.Create(iFs, iDescFilePath, EFileWrite);
	OstTrace1( TRACE_NORMAL, CUPNPTMSERVERDEVICEXMLPARSER_ONENDDOCUMENTL, "CUpnpTmServerDeviceXmlParser::OnEndDocumentL;err=%d", err );
    if ( err == KErrAlreadyExists )
        {
        User::LeaveIfError(deviceFile.Open(iFs, iDescFilePath, EFileWrite));
        }
    else
        {
        User::LeaveIfError(err);
        }
	User::LeaveIfError(deviceFile.Write(iDeviceDescription));
	CleanupStack::PopAndDestroy(&deviceFile);
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONENDDOCUMENTL_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnStartElementL
// Called when parser hits the opening tag
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceXmlParser::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/, TInt aErrorCode)
	{
	OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONSTARTELEMENTL_ENTRY );
	User::LeaveIfError(aErrorCode);

	iDeviceDescription.Append(UpnpCD::KKorp);
	iDeviceDescription.Append(aElement.LocalName().DesC());
	
    if(aElement.LocalName().DesC().Compare(KUpnpDeviceRoot) == 0)
        {
        iDeviceDescription.Append(KRootAttrb());    
        }       
    iDeviceDescription.Append(UpnpCD::KProk);

    if(aElement.LocalName().DesC().Compare(KPhoneManufacturer) == 0)
        {
        iDeviceDescription.Append(iPhoneId.iManufacturer);   
        }
	else if(aElement.LocalName().DesC().Compare(KPhoneModel) == 0)
        {
        iDeviceDescription.Append(iPhoneId.iModel);   
        }
	else if(aElement.LocalName().DesC().Compare(KPhoneSerialNumber) == 0)
        {
        iDeviceDescription.Append(iPhoneId.iSerialNumber);   
        }
    else if(aElement.LocalName().DesC().Compare(KBtAddress) == 0)
        {
        iDeviceDescription.Append(iDeviceInfo.BtAddress());   
        }
    else if(aElement.LocalName().DesC().Compare(KStartConnection) == 0)
        {
        if ( iDeviceInfo.StartConn() )
            {
            iDeviceDescription.Append(KConnTrue);    
            }
        else
            {
            iDeviceDescription.Append(KConnFalse);    
            }       
        }
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONSTARTELEMENTL_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnEndElementL
// Called when parser hits the closing tag
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceXmlParser::OnEndElementL( const RTagInfo& aElement, TInt aErrorCode )
	{
	OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONENDELEMENTL_ENTRY );
	User::LeaveIfError(aErrorCode);
	
	iDeviceDescription.Append( UpnpCD::KKorp );
	iDeviceDescription.Append( UpnpString::KSlash );
	iDeviceDescription.Append( aElement.LocalName().DesC() );
	iDeviceDescription.Append( UpnpCD::KProk );
	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONENDELEMENTL_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnContentL
// Provides the content/value of the element/tag
// ---------------------------------------------------------------------------------
//	
void CUpnpTmServerDeviceXmlParser::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
	{
	User::LeaveIfError(aErrorCode);
	iDeviceDescription.Append(aBytes);
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::StartL
// Method which inputs xml formatted buffer content to the XML Parser 
// and invokes parsing
// ---------------------------------------------------------------------------------
// 
void CUpnpTmServerDeviceXmlParser::StartL()
	{
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_STARTL_ENTRY );
    TDriveNumber drive = RFs::GetSystemDrive();   //Find system's drive
    TBuf<UpnpString::KMaxFilenameLength> privatePath;  
    TInt err = iFs.PrivatePath(privatePath); //Find the application's private path
    // a) If the Private Path is not found (for whatever reasons),
    //      Create a private path in the System Drive
    if( err == KErrNotFound )
        {
        User::LeaveIfError( iFs.CreatePrivatePath(drive) );
        }
    // b) If Private Path is found but is a read-only or non-persistent drive
    //      Create a private path in the system drive else use it
    else if( err == KErrNone )
        {
        TDriveInfo driveInfo;
        User::LeaveIfError( iFs.Drive(driveInfo));
        TUint driveAttr = driveInfo.iDriveAtt;
        if ( driveAttr == KDriveAttRom )
            {
            User::LeaveIfError( iFs.CreatePrivatePath(drive) );
            }
        }
    else
        {
        OstTrace1( TRACE_ERROR, CUPNPTMSERVERDEVICEXMLPARSER_STARTL, "CUpnpTmServerDeviceXmlParser::StartL;err=%d", err );     
        User::Leave(err);
        }
    privatePath.Append(KPublicDevicePath());
    err = iFs.MkDirAll(privatePath);
    if( err && err != KErrAlreadyExists )
        {
        OstTrace1( TRACE_ERROR, DUP1_CUPNPTMSERVERDEVICEXMLPARSER_STARTL, "CUpnpTmServerDeviceXmlParser::StartL;err=%d", err );
        User::LeaveIfError(err);
        }
    iDeviceDir.CreateL(privatePath);
    RBuf8 rootDeviceBuf;
    CleanupClosePushL(rootDeviceBuf);
    rootDeviceBuf.CreateL(KBufSize);
    rootDeviceBuf.Append(KRootDeviceXmlLead());
    // Appends the device icon list to the device xml buffer
    const RPointerArray<CUpnpTerminalModeIcon>& deviceList = iDeviceInfo.DeviceIconList();
    TInt iconCount = deviceList.Count();
    if ( iconCount > KErrNone )
        {
        privatePath.Append(KIconDirectory());
        err = iFs.MkDir(privatePath);  // Creates icon directory 
        if( err && err != KErrAlreadyExists )
            {
            OstTrace1( TRACE_ERROR, DUP2_CUPNPTMSERVERDEVICEXMLPARSER_STARTL, "CUpnpTmServerDeviceXmlParser::StartL;err=%d", err );
            User::LeaveIfError(err);
            }
        iIconDirectory.CreateL(privatePath);
        OstTrace1( TRACE_ERROR, DUP3_CUPNPTMSERVERDEVICEXMLPARSER_STARTL, "CUpnpTmServerDeviceXmlParser::StartL;iconCount=%d", iconCount );
        rootDeviceBuf.Append(KStartIconList);
        for ( TInt i(0); i < iconCount; i++ )
            {
            rootDeviceBuf.Append(KStartIcon);
            rootDeviceBuf.Append(KStartMimeType);
            rootDeviceBuf.Append(deviceList[i]->MimeType());
            rootDeviceBuf.Append(KEndMimeType);
            rootDeviceBuf.Append(KStartWidth);
            rootDeviceBuf.AppendNum(deviceList[i]->Width());
            rootDeviceBuf.Append(KEndWidth);
            rootDeviceBuf.Append(KStartHeight);
            rootDeviceBuf.AppendNum(deviceList[i]->Height());
            rootDeviceBuf.Append(KEndHeight);
            rootDeviceBuf.Append(KStartDepth);
            rootDeviceBuf.AppendNum(deviceList[i]->Depth());
            rootDeviceBuf.Append(KEndDepth);
            rootDeviceBuf.Append(KStartUrl);
            
            TBuf8<KMaxPath> iconBuf;
            const TDesC& fileName = deviceList[i]->IconFilename();
            iconBuf.Copy(fileName);
            TBuf8<UpnpString::KDefaultStringLength> iconRelativeUrl(KScpdUrl());
            // Extracts the actual input filepath and creates the relative url for the icon
            // to be provided in the device xml file
            iconRelativeUrl.Append(KIconPath());
            // Extracts only filename and appends the same to url
            iconRelativeUrl.Append(iconBuf.Mid((iconBuf.LocateReverse(KDirectorySeparator))+1));
            rootDeviceBuf.Append(iconRelativeUrl);
            delete iFileMan;
            iFileMan = NULL;
            iFileMan = CFileMan::NewL(iFs) ;
            // copies icon files to the private device directory
            User::LeaveIfError(iFileMan->Copy(fileName,iIconDirectory));  
            rootDeviceBuf.Append(KEndUrl);
            rootDeviceBuf.Append(KEndIcon);
            }
        rootDeviceBuf.Append(KEndIconList);
        }
    rootDeviceBuf.Append( iDeviceInfo.DeviceInfo());
    rootDeviceBuf.Append( KRootDeviceXmlTrail() );
       
	ParseL(*iParser, rootDeviceBuf );
	CleanupStack::PopAndDestroy(&rootDeviceBuf);

	OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_STARTL_EXIT );
	}

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnStartPrefixMappingL
// ---------------------------------------------------------------------------------
//  
void CUpnpTmServerDeviceXmlParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONSTARTPREFIXMAPPINGL_ENTRY );
    User::LeaveIfError(aErrorCode);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONSTARTPREFIXMAPPINGL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnEndPrefixMappingL
// ---------------------------------------------------------------------------------
// 
void CUpnpTmServerDeviceXmlParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONENDPREFIXMAPPINGL_ENTRY );
    User::LeaveIfError(aErrorCode);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONENDPREFIXMAPPINGL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnIgnorableWhiteSpaceL
// ---------------------------------------------------------------------------------
// 
void CUpnpTmServerDeviceXmlParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONIGNORABLEWHITESPACEL_ENTRY );
    User::LeaveIfError(aErrorCode);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONIGNORABLEWHITESPACEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnSkippedEntityL
// ---------------------------------------------------------------------------------
// 
void CUpnpTmServerDeviceXmlParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONSKIPPEDENTITYL_ENTRY );
    User::LeaveIfError(aErrorCode);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONSKIPPEDENTITYL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnProcessingInstructionL
// ---------------------------------------------------------------------------------
// 
void CUpnpTmServerDeviceXmlParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEXMLPARSER_ONPROCESSINGINSTRUCTIONL_ENTRY );
    User::LeaveIfError(aErrorCode);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEXMLPARSER_ONPROCESSINGINSTRUCTIONL_EXIT );
    }


