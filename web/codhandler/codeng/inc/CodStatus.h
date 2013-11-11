/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      HTTP status codes for COD Engine.   
*      
*
*/


#ifndef COD_STATUS_H
#define COD_STATUS_H

// Status codes.

// (Common for COD and DD)
LOCAL_D const TInt KHttp900Success = 900;
LOCAL_D const TInt KHttp901InsufficientMemory =  901;
LOCAL_D const TInt KHttp902UserCancelled = 902;
LOCAL_D const TInt KHttp903LossOfService = 903;
LOCAL_D const TInt KHttp905AttributeMismatch = 905;
LOCAL_D const TInt KHttp906InvalidDescriptor = 906;
LOCAL_D const TInt KHttp907InvalidType = 907;
LOCAL_D const TInt KHttp909RequestedRangeNotSatisfiable = 909;
LOCAL_D const TInt KHttp910NoMemory = 910;
LOCAL_D const TInt KHttp970MixedStatus = 970;
LOCAL_D const TInt KHttp981DownloadCompletion = 981;

// (COD only)
LOCAL_D const TInt KHttp921UserAborted = 921;
LOCAL_D const TInt KHttp922DeviceAborted = 922;
LOCAL_D const TInt KHttp923NonAcceptableContent = 923;
LOCAL_D const TInt KHttp924LoaderError = 924;

// (DD only)
LOCAL_D const TInt KHttp951InvalidDdVersion = 951;
LOCAL_D const TInt KHttp952DeviceAborted = 952;
LOCAL_D const TInt KHttp953NonAcceptableContent = 953;
LOCAL_D const TInt KHttp954LoaderError = 954;
LOCAL_D const TInt KHttp955PreconditionFailed = 955;
LOCAL_D const TInt KHttp956LicenseSuccess = 956;
LOCAL_D const TInt KHttp957LicenseFailed = 957;

// Status strings.

// (Common for COD and DD)
_LIT8( KText900Success,                 "900 Success" );
_LIT8( KText901InsufficientMemory,      "901 Insufficient Memory" );
_LIT8( KText902UserCancelled,           "902 User Cancelled" );
_LIT8( KText903LossOfService,           "903 Loss of Service" );
_LIT8( KText905AttributeMismatch,       "905 Attribute Mismatch" );
_LIT8( KText906InvalidDescriptor,       "906 Invalid Descriptor" );
_LIT8( KText907InvalidType,             "907 Invalid Type" );
_LIT8( KText910NoMemory,                "910 No Memory" );
_LIT8( KText970MixedStatus,             "970 Mixed Status" );
_LIT8( KText981DownloadCompletion,      "981 Download Completion" );
// (COD only)
_LIT8( KText921UserAborted,             "921 User Aborted" );
_LIT8( KText922DeviceAborted,           "922 Device Aborted" );
_LIT8( KText923NonAcceptableContent,    "923 Non-acceptable Content" );
_LIT8( KText924LoaderError,             "924 Loader Error" );

// (DD only)
_LIT8( KText951InvalidDdVersion,        "951 Invalid DD Version" );
_LIT8( KText952DeviceAborted,           "952 Device Aborted" );
_LIT8( KText953NonAcceptableContent,    "953 Non-acceptable Content" );
_LIT8( KText954LoaderError,             "954 Loader Error" );
_LIT8( KText955MediaObjectUpdated,      "955 Media Object Updated" );

// OMA2 (5.3.3): second line should be added conaining the License status code and status message
_LIT8( KText956LicenseSuccess,          "900 Sussess \r\n 956 License Retrieval Succeeded" );
	                                     
_LIT8( KText957LicenseFailed,           "957 License Retrieval Failed" );

#endif /* def COD_STATUS_H */
