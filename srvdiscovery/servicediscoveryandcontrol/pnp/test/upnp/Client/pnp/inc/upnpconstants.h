// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#ifndef __UPNPCONSTANTS_H_
#define __UPNPCONSTANTS_H_

/**
This is the tier id value which must be specified whenever the user wants to open a UPnP Service Point or Control Point. 
This is the parameter which must be specified in the RPnPServicePublisher::Open or RPnPServiceDiscovery::Open APIs
@publishedPartner
@prototype
*/

const TUint KUPnPInterface  = 0x2000D054;

/**
This is the Uid which is used to load in the underlying the pnp interface. New plugins which make use of the
pnp interface should make use of this as the interface id of their implementations in the rss file of the plugin
@publishedPartner
@prototype
*/
const TUid  KPnPInterfaceId = {0x2000D060};

/**
Base value for the UPnP error code range (-7700 to -7900)
@publishedPartner
@prototype
*/
const TInt KUpnpErrorBase = -7700;

/**
Error range  for client dll returned error code
@publishedPartner
@prototype
*/
const TInt KUpnpClientErrorBase=KUpnpErrorBase-50;

/**
Error code for invalid bundle passed by client application.
This error code is returned when the application fails to retrieve
either the callback (MPnPObserver) , or family or the parameter
set from the passed bundle

@publishedPartner
@prototype
*/
const TInt KErrClientDllInvalidBundle = KUpnpClientErrorBase-1;

//UpnP panic codes

enum TUPnPPanic
	{
	KUPnPErrNotOpen = 1 , //Set the starting error code.
	KUPnPErrNoObserver,
	KUPnPErrInvalidFamilyCount,
	KUPnPErrInvalidFamily,
	KUPnPErrNoParamSet,
	KUPnPErrDiscoveryInProgress,
	KUPnPErrPublish,
	KUPnPErrInvalidInput,
	KUPnPErrTimerError,
	KUPnPErrResponseUriNotFound,
	KUPnPErrInvalidFriendlyName,
	KUPnPErrInvalidManufactureName,
	KUPnPErrInvalidModelName,
	KUPnPErrInvalidModelNumber,
	KUPnPErrInvalidModelDescription,
	KUPnPErrInvalidSerialNumber,
	KUPnPErrInvalidUDN,
	KUPnPErrInvalidUPC,
	KUPnPErrMultiplePublishParamSet,
	KUPnPErrInvalidMimeType,
	};


//error strings
_LIT(KUPnPNoObserver,"Observer not set");
_LIT( KUPnPHandleNotOpen , "Handle not open");
_LIT( KUPnPInvalidFamilyCount , "Family count not equal to 1");
_LIT( KUPnPInvalidFamily , "Invalid Family");
_LIT( KUPnPNoParamSet , "No paramset");
_LIT(KUPnPDiscoveryInProgress,"Discovery in not set");
_LIT(KUPnPAlreadyPublished,"The service has already published");
_LIT(KUPnPServiceNotPublished,"The service is not yet published");
_LIT(KUPnPInvalidUserInput, "The input data/argument is invalid");
_LIT(KUPnPTimerUriNotFound, "Internal error : URI  in timer list not found");
_LIT(KUPnPResponseUriNotFound, "Internal error : URI  not found in resposne");
_LIT(KUPnPDeviceSendingNotification,"The device is sending notification");
_LIT(KUPnPInvalidFriendlyName,"The device friendly name is not valid");
_LIT(KUPnPInvalidManufactureName,"The device Manufacturer is name not valid");
_LIT(KUPnPInvalidModelName,"The model name is not valid");
_LIT(KUPnPInvalidModelNumber,"The model number is not valid");
_LIT(KUPnPInvalidModelDescription,"The model description is not valid");
_LIT(KUPnPInvalidSerialNumber,"The device serial number is not valid");
_LIT(KUPnPInvalidUPC,"The device UPC is not valid");
_LIT(KUPnPInvalidUDN,"The unique device name is not valid");
_LIT(KUPnPControlChannel,"UPnP Control Channel");
_LIT(KUPnPMultiplePublish,"Multiple devices/service publish in single service point");
_LIT(KUPnPInvalidIconInfo,"The device Icon Info is not valid");

/**
Error range  for upnpcodec dll returned error code
@publishedPartner
@prototype
*/
const TInt KUpnpCodecErrorBase=KUpnpErrorBase-150;

/**
Error encoding ST header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeST				= KUpnpCodecErrorBase - 1;

/**
Error encoding USN header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeUSN			= KUpnpCodecErrorBase - 2;

/**
Error encoding Timeout header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeTimeout		= KUpnpCodecErrorBase - 3;

/**
Error encoding EXT header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeEXT			= KUpnpCodecErrorBase - 4;

/**
Error encoding SID header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeSID			= KUpnpCodecErrorBase - 5;

/**
Error encoding NTS header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeNTS			= KUpnpCodecErrorBase - 6;

/**
Error encoding NT header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeNT				= KUpnpCodecErrorBase - 7;

/**
Error encoding MX header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeMX				= KUpnpCodecErrorBase - 8;

/**
Error encoding SEQ header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeSEQ			= KUpnpCodecErrorBase - 9;

/**
Error encoding MAN header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeMAN			= KUpnpCodecErrorBase - 10;

/**
Error encoding SoapAction header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeSoapAction		= KUpnpCodecErrorBase - 11;

/**
Error encoding Callback header
@publishedPartner
@prototype
*/
const TInt KErrUPnPEncodeCallback		= KUpnpCodecErrorBase - 12;

/**
Error decoding MAN header
@publishedPartner
@prototype
*/
const TInt KErrUPnPDecodeMAN	 		= KUpnpCodecErrorBase - 13;

#endif /*UPNPERR_H_*/


