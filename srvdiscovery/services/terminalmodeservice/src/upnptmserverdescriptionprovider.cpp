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
* Description: CUpnpTmServerDescriptionProvider class implementation.
*
*/

#include "upnptmserverdescriptionprovider.h"
#include <utf.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmserverdescriptionproviderTraces.h"
#endif


_LIT(KPublic,   "public\\");
_LIT(KIconPath, "icon");

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::NewL
//
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDescriptionProvider* CUpnpTmServerDescriptionProvider::NewL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_NEWL_ENTRY );
    CUpnpTmServerDescriptionProvider* self = new (ELeave) CUpnpTmServerDescriptionProvider();           
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    OstTraceFunctionExit0( CUPNPTMSERVERDESCRIPTIONPROVIDER_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::CUpnpTmServerDescriptionProvider
//
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDescriptionProvider::CUpnpTmServerDescriptionProvider()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::~CUpnpTmServerDescriptionProvider
//
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDescriptionProvider::~CUpnpTmServerDescriptionProvider()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_CUPNPTMSERVERDESCRIPTIONPROVIDER_ENTRY );
    iFs.Close();
    OstTraceFunctionExit0( CUPNPTMSERVERDESCRIPTIONPROVIDER_CUPNPTMSERVERDESCRIPTIONPROVIDER_EXIT );
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::ConstructL
//
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDescriptionProvider::ConstructL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_CONSTRUCTL_ENTRY );
    User::LeaveIfError( iFs.Connect() );        
    OstTraceFunctionExit0( CUPNPTMSERVERDESCRIPTIONPROVIDER_CONSTRUCTL_EXIT );
    }


// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::OnDeviceDescription
//
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerDescriptionProvider::OnDeviceDescription( 
                                       CUpnpDeviceDescriptionRequest& aRequest )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONDEVICEDESCRIPTION_ENTRY );
    return OnRequest( aRequest );
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::OnServiceDescription
//
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerDescriptionProvider::OnServiceDescription( 
                                        CUpnpDeviceDescriptionRequest& aRequest )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONSERVICEDESCRIPTION_ENTRY );
    return OnRequest( aRequest );
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::OnIcon
//
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerDescriptionProvider::OnIcon( CUpnpDeviceDescriptionRequest& aRequest )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONICON_ENTRY );
    return OnRequest( aRequest );
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::GetFile
//
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerDescriptionProvider::GetFile( const TDesC8& /*aUri*/, RFile& aFile )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_GETFILE_ENTRY );
    return ( aFile.Open( iFs, iFilePath, EFileShareReadersOnly | EFileRead ));  
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::OnRequest
//
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmServerDescriptionProvider::OnRequest( CUpnpDeviceDescriptionRequest& aRequest )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONREQUEST_ENTRY );
    TPtrC8 fileName;
    TInt error(KErrNone);
    TPtrC8 urlPath = UpnpFileUtil::ExtractUrlPath( aRequest.Uri(), fileName, error );
    if( error == KErrNone)
        {    
        TRAP( error, OnRequestL( urlPath,fileName ) );
        }
    OstTraceFunctionExit0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONREQUEST_EXIT );
    return error;
    }

// ---------------------------------------------------------------------------------
//  CUpnpTmServerDescriptionProvider::OnRequestL
//  @param aUrlPath   URL 
//  @param aFileName  FileName 
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDescriptionProvider::OnRequestL( const TDesC8& aUrlPath, 
                                                              const TDesC8& aFileName )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONREQUESTL_ENTRY );
    // 8-bit to 16-bit conversion of URL path and file path
    TBuf<UpnpString::KMaxFilenameLength> urlPath;  
    urlPath.Copy(aUrlPath);
    TBuf<UpnpString::KMaxFilenameLength> fileName;  
    fileName.Copy(aFileName);
    
    // Fetches the private directory
    User::LeaveIfError( iFs.PrivatePath( iFilePath ) );
    iFilePath.Append(KPublic());
    /** 
     * Appends the filename to the file path.
     * 1. In case of icon file, it first appends icon directory and then appends the filename.
     * 2. In case of device and service xml files, it merely appends filename to the file path.
     */
    if ( urlPath.Find(KIconPath()) != KErrNotFound )
        {
        // The request is for icon file
        iFilePath.Append(urlPath.Left(urlPath.Locate(KForwardSlash))); 
        iFilePath.Append(UpnpString::KDoubleBackSlash16());    
        iFilePath.Append(KIconPath());
        }
    else
        {
        iFilePath.Append(urlPath);   
        }
    iFilePath.Append(UpnpString::KDoubleBackSlash16());
    // Appends filename to the file path.
    iFilePath.Append(fileName);
    OstTraceFunctionExit0( CUPNPTMSERVERDESCRIPTIONPROVIDER_ONREQUESTL_EXIT );
    }

