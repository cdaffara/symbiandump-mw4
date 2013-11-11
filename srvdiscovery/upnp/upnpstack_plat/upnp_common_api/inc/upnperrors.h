/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares UPnP and Http error codes 
*
*/


#ifndef C_UPNPERRORS_H
#define C_UPNPERRORS_H

#include <e32base.h> 

	enum THttpStatusCode {

		// Informational 1xx
		EHttpContinue		    =	100,
		EHttpSwitchingProtocols =   101,

		// Successful 2xx
		EHttp200Ok			    =	200,
		EHttpCreated		    =	201,
		EHttpAccepted		    =	202,
		EHttpNonAuthoritative	=	203,
		EHttpNoContent			=	204,
		EHttpResetContent		=	205,
		EHttpPartialContent		=	206,

		// Redirection 3xx	
		EHttpMultipleChoises	=	300,
		EHttpMovedPermanently	=	301,
		EHttpFound				=	302,
		EHttpSeeOther			=	303,
		EHttpNotModified		=	304,
		EHttpUseProxy			=	305,
	    //	Unused				=	306,
		EHttpTemporaryRedirect	=	307,

		// Client error 4xx
		EHttpBadRequest		    =	400,
		EHttpUnAuthorized		=	401,
		EHttpPaymentRequired	=	402,
		EHttpForbidden			=	403,
		EHttpNotFound			=	404,
		EHttpMethodNotAllowed	=	405,
		EHttpNotAcceptable		=	406,
		EHttpProxyAuthenticationRequired	=	407,
		EHttpRequestTimeout		=	408,
		EHttpConflict			=	409,
		EHttpGone				=	410,
		EHttpLengthRequired		=	411,
		EHttpPreconditionFailed	=	412,
		EHttpEntityTooLarge		=	413,
		EHttpUriTooLong			=	414,
		EHttpUnsupportedMediaType=	415,
		EHttpRequestedRangeNotSatisfiable   =   416,
		EHttpExpectationFailed	=	417,
		
		// Server Error 5xx
		EHttpInternalServerError=	500,
		EHttpNotImplemented		=	501,
		EHttpBadGateway			=	502,
		EHttpServiceUnavailable	=	503,
		EHttpGatewayTimeout		=	504,
		EHttpVersionNotSupported=	505,
		EHttpInsufficientStorage=	507 

	};

	// Informational 1xx
	_LIT8( KHttpContinue,           "Continue" );	                // 100
	_LIT8( KHttpSwitchingProtocols, "Switching Protocols" );        // 101

          
	// Successful 2xx		
	_LIT8( KHttpCreated,            "Created" );                    // 201
	_LIT8( KHttpAccepted,           "Accepted" );                   // 202
	_LIT8( KHttpNonAutohorative,    "Non-Authoritative Information" );// 203
	_LIT8( KHttpNoContent,          "No Content" );                 // 204
	_LIT8( KHttpResetContent,       "Reset Content" );              // 205
	_LIT8( KHttpPartialContent,     "Partial Content" );            // 206

	// Redirection 3xx	
	_LIT8( KHttpMultipleChoises,    "Multiple Choices" );           // 300
	_LIT8( KHttpMovedPermanently,   "Moved Permanently" );          // 301
	_LIT8( KHttpFound,              "Found" );                      // 302
	_LIT8( KHttpSeeOther,           " See Other" );                 // 303
	_LIT8( KHttpNotModified,        "Not Modified" );               // 304
	_LIT8( KHttpUseProxy,           "Use Proxy" );                  // 305
	_LIT8( KHttpTemporaryRedirect,  "Temporary Redirect" );	        // 307

	// Client error 4xx
	_LIT8( KHttpBadRequest,         "Bad Request" );                 // 400
	_LIT8( KHttpUnauthorized,       "Unauthorized" );                // 401
	_LIT8( KHttpPaymentRequired,    "Payment Required" );            // 402
	_LIT8( KHttpForbidden,          "Forbidden" );                   // 403
	_LIT8( KHttpNotFound,           "Not Found" );                   // 404
	_LIT8( KHttpMethodNotAllowed,   "Method Not Allowed" );          // 405
	_LIT8( KHttpNotAcceptable,      "Not Acceptable" );              // 406
	_LIT8( KHttpProxyAuthenticationRequired, "Proxy Authentication Required");//407
	_LIT8( KHttpRequestTimeout,     "Request Timeout" );            // 408
	_LIT8( KHttpConflict,           "Conflict" );                    // 409
	_LIT8( KHttpGone,               "Gone" );                        // 410
	_LIT8( KHttpLengthRequired,     "Length Required" );             // 411
	_LIT8( KHttpPreconditionFailed, "Precondition Failed" );         // 412
	_LIT8( KHttpEntityTooLarge,     "Request Entity Too Large" );    // 413
	_LIT8( KHttpUriTooLarge,        "Request-URI Too Large" );       // 414
	_LIT8( KHttpUnsupprotedMediaType, "Unsupported Media Type" );    // 415
	_LIT8( KHttpRequestedRangeNotSatisfiable, "Requested range not satisfiable" );// 416
	_LIT8( KHttpExpectationFailed,  "Expectation Failed" );          // 417

		// Server Error 5xx
	_LIT8( KHttpInternalServerError,"Internal Server Error" );	    // 500
	_LIT8( KHttpNotImplemented,     "Not Implemented" );            // 501
	_LIT8( KHttpBadGateway,         "Bad Gateway" );                // 502
	_LIT8( KHttpServiceUnavailable, "Service Unavailable" );        // 503
	_LIT8( KHttpGatewayTimeout,     "Gateway Time-out" );           // 504
	_LIT8( KHttpVersionNotSupported,"HTTP Version not supported" );	// 505
	
	_LIT8( KHttpInsufficientStorage,"Insufficient Storage" );	// 507

	enum TUpnpErrorCode {
		EUpnpOk			=	0,
		EUpnpUndefined	=	-1,

		EHttpOk			=	200,

		EBadRequest		=	400,
		EInvalidAction	=	401,
		EInvalidArgs	=	402,
		EInvalidVar		=	404,
		EPreconditionFailed=412,

		EInternalServerError=500,
		EActionFailed	=	501,

		// 600-699 Common action errors
		EArgumentValue	=	600,
		EArgumentRange	=	601,
		ENotImplemented	=	602,
		EOutOfMemory	=	603,
		EHumanIntervention=	604,
		EStringTooLong	=	605,
		ENotAuthorized	=	606,
		ESignatureFailure =	607,
		ESignatureMissing =	608,
		ENotEncrypted	=	609,
		EInvalidSequence =	610,
		EInvalidUrl		=	611,
		ENoSession		=	612,

		// 700-799 Action specific errors
		ENoSuchObject	=	701,
		EInvalidCurrentTag= 702,
		EInvalidNewTag	=	703,
		ERequiredTag	=	704,
		EReadOnlyTag	=	705,
		EParameterMismatch = 706,
		EInvalidSearch	=	708,
		EInvalidSort	=	709,
		ENoContainer	=	710,
		ERestrictedObject=  711,
		EBadMetadata	=	712,
		ERestrictedParentObject=713,
		ENoSourceResource = 714,
		ESourceAccess	=	715,
		ETransferBusy	=	716,
		ENoFileTransfer	=	717,
		ENoDestinationResource=718,
		EDestinationAccess= 719,
		ECannotProcess	=	720,

		// 800-899 Action specific errors. Defined by vendor.
		EUndefined		=	808
		};
	

	_LIT8( KHttpOk, "OK" );	                                    /// 200
		
	_LIT8( KBadRequest,         "Bad Request" );	            /// 400
	_LIT8( KInvalidAction,      "Invalid Action" );	            /// 401
	_LIT8( KInvalidArgs,        "Invalid Args" );	            /// 402
	_LIT8( KInvalidVar,         "Invalid Var" );	            /// 404
	_LIT8( KPreconditionFailed, "Precondition Failed" );	    /// 412
		
	_LIT8( KInternalServerError,"Internal Server Error" );	    /// 500
	_LIT8( KActionFailed,       "Action Failed" );	            /// 501
		
	// 600-699 Common action errors	
	_LIT8( KArgumentValue,      "Argument Value Invalid" );	    /// 600
	_LIT8( KArgumentRange,      "Argument Value Out of Range" );/// 601
	_LIT8( KNotImplemented,     "Optional Action Not Implemented" );/// 602
	_LIT8( KOutOfMemory,        "Out of Memory" );	            /// 603
	_LIT8( KHumanIntervention,  "Human Intervention Required" );/// 604
	_LIT8( KStringTooLong,      "String Argument Too Long" );   /// 605
	_LIT8( KNotAuthorized,      "Action not authorized" );	    /// 606
	_LIT8( KSignatureFailure,   "Signature failure" );	        /// 607
	_LIT8( KSignatureMissing,   "Signature missing" );	        /// 608
	_LIT8( KNotEncrypted,       "Not encrypted" );	            /// 609
	_LIT8( KInvalidSequence,    "Invalid sequence" );	        /// 610
	_LIT8( KInvalidUrl,         "Invalid control URL" );        /// 611
	_LIT8( KNoSession,          "No such session" );	        /// 612
		
	// 700-799 Action specific errors	
	_LIT8( KNoSuchObject,       "No such object" );	            /// 701
	_LIT8( KInvalidCurrentTag,  "Invalid CurrentTagValue" );   	/// 702
	_LIT8( KInvalidNewTag,      "Invalid NewTagValue" );	    /// 703
	_LIT8( KRequiredTag,        "Required tag" );	            /// 704
	_LIT8( KReadOnlyTag,        "Read only tag" );              /// 705
	_LIT8( KInvalidConnectionReference,  "Invalid Connection Reference" );        	/// 706
	_LIT8( KParameterMismatch,  "Parameter Mismatch" );        	/// 706
	_LIT8( KInvalidSearch,      "Unsupported or invalid search criteria" );	/// 708
	_LIT8( KInvalidSort,	    "Unsupported or invalid sort criteria" );	/// 709
	_LIT8( KNoContainer,        "No such container" );	        /// 710
	_LIT8( KRestrictedObject,   "Restricted object" );	        /// 711
	_LIT8( KBadMetadata,        "Bad metadata" );	            /// 712
	_LIT8( KRestrictedParentObject, "Restricted parent object" ); /// 713
	_LIT8( KNoSourceResource,   "No such source resource" );	/// 714
	_LIT8( KSourceAccess,       "Source resource access denied" ); /// 715
	_LIT8( KTransferBusy,       "Transfer busy" );	            /// 716
	_LIT8( KNoFileTransfer,     "No such file transfer" );	    /// 717
	_LIT8( KNoDestinationResource,"No such destination resource" );	/// 718
	_LIT8( KDestinationAccess,  "Destination resource access denied" );	/// 719
	_LIT8( KCannotProcess,      "Cannot process the request" );	/// 720
	


#endif	// C_UPNPERRORS_H

// End Of File