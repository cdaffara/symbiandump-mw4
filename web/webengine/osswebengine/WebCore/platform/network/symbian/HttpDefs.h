/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef HTTPLOADERDEFS_H
#define HTTPLOADERDEFS_H

// Encoding
// IMPORTANT: Sync with \browser\settingsinc\settingscontainer.h
// The order of these enums can NOT be changed 
enum TEncoding
    {
    EGb2312,
    EISO8859_1,
    EBig5,
    EUTF8,
    EISO8859_2,
    EISO8859_4,
    EISO8859_5,
    EISO8859_7,
    EISO8859_9,
    EISO8859_8i,
    EISO8859_8,
    EISO8859_6,
    EISO8859_15,
    EWindows1256,
    EWindows1255,
    EWindows1250,
    EWindows1251,
    EWindows1252,
    EWindows1253,
    EWindows1254,
    EWindows1257,
    EAutomatic,
    EISO10646_UCS_2,
    ETis620,
    EShiftJis,
    EJisX0201_1997,
    EJisX0208_1997,
    EEucJp,
    EIso2022Jp,
    EWindows874,
    EEucKr,
    EKsc5601,
    EDummyLast
    };

enum TUrlCacheMode
    {
    ECacheNormal,      // Normal Http load, use cache if not stale
    ECacheHistPrev,    // History Prev load, use cache even if stale unless must-revalidate
    ECacheNoCache,     // Do not use cache, even if fresh
    ECacheOnlyCache    // Only load from cache, for loading saved deck
    } ;

// HTTP Default Headers 
enum TUrlDefaultHeaderType
    {
    EDefaultHeadersNotAny = 0x00, 
    EDefaultHeadersContent = 0x01, 
    EDefaultHeadersCharset = 0x02, 
    EDefaultHeadersLang = 0x04, 
    EDefaultHeadersAll = 0x07
    };

// These types are for optimized accept headers 
enum TUrlLoadType 
    {
    EUrlRequestTypeAny,     // Any type               
    EUrlRequestTypeImage,   // Load an image type     
    EUrlRequestTypeCss,     // Load a stylesheet type 
    EUrlRequestTypeJavaScript, // JavaScript type        
    EUrlRequestTypePlugin,     // Load plugin resource     
    EUrlRequestTypeNone        // No content types - related to WMLSCRIPT
    };

enum TUrlLoadMethod
    {
    EUrlGet,     // get
    EUrlPost,    // post
    EUrlHead     // head
    };

enum THttpStatus
	{
	ESuccess = 200,
	ECreated = 201,
	EAccepted = 202,
	ENonAuthInfo = 203,
	ENoContent = 204,
	EResetContent = 205,
	EPartialContent = 206
	};

#define	KBrowserFailure          	                -26000
#define	KBrowserFileNotFound     	                -26003
#define	KBrowserFileWriteError   	                -26008
#define	KBrowserFileDiskFullError	                -26009
#define	KBrowserOutOfMemory      	                -26011
#define	KBrowserUnexpectedError                    -26012
#define	KBrowserNotImplemented   	                -26013
#define	KBrowserWaitTimeout      	                -26014
#define	KBrowserBufferTooSmall   	                -26015
#define	KBrowserBadInputParam    	                -26016
#define	KBrowserMissingInputParam	                -26017
#define	KBrowserMalformedUrl                       -26018
#define	KBrowserNotFound         	                -26021
#define	KBrowserBufferEnd	                        -26022
#define	KBrowserIterateMore                        -26023
#define	KBrowserIterateDone                        -26024
#define	KBrowserUnknownScheme	                    -26026
#define	KBrowserBadRedirect                        -26027
#define	KBrowserTooManyRedirects                   -26028
#define	KBrowserHttpStatus	                        -26035
#define	KBrowserMissingAuthHeader	                -26037
#define	KBrowserCancelled        	                -26038
#define	KBrowserWpsaNoActiveTransactions           -26067
#define	KBrowserNoContentLength                    -26068
#define	KBrowserTransactionCompete	                -26069
#define	KBrowserWimiInvalidPin	                    -26072
#define	KBrowserWimiPinBlocked	                    -26073
#define	KBrowserWimiNoCard	                        -26075
#define	KBrowserWimiNoInit	                        -26084
#define	KBrowserWimiBadCert                        -26085
#define	KBrowserWimiWimErr	                        -26089
#define	KBrowserWimiSessionNotSet	                -26090
#define	KBrowserWimiCertNotFound                   -26091
#define	KBrowserWimiKeyNotFound                    -26092
#define	KBrowserWimiDuplicateCert	                -26093
#define	KBrowserWtlsHandshakeFail	                -26102
#define	KBrowserWtlsUnknowCertAuth	                -26110
#define	KBrowserWtlsUcNotYetValid	                -26111
#define	KBrowserWtlsUcExpired	                    -26112
#define	KBrowserWtlsUnsupCert	                    -26114
#define	KBrowserWtlsDecodeFail	                    -26116
#define	KBrowserScriptCardNotInDeck                -26131
#define	KBrowserScriptNoAccess	                    -26132
#define	KBrowserScriptBadContent                   -26133
#define	KBrowserScriptErrorUserExit                -26134
#define	KBrowserScriptErrorUserAbort               -26135
#define	KBrowserScriptErrorStackUnderflow	        -26136
#define	KBrowserScriptErrorStackOverflow           -26137
#define	KBrowserScriptFatalLibFuncError            -26138
#define	KBrowserScriptInvalidFuncArgs	            -26139
#define	KBrowserScriptVerificationFailed           -26140
#define	KBrowserScriptExternalFunctionNotFound	    -26141
#define	KBrowserScriptUnableToLoadCompilationUnit	-26142
#define	KBrowserScriptAccessViolation	            -26143
#define	KBrowserWmlbrowserCardNotInDeck            -26144
#define	KBrowserWmlbrowserNoAccess	                -26145
#define	KBrowserBadContent                          -26146
#define	KBrowserBadContentType                      -26148
#define	KBrowserWmlbrowserInputNonconformingToMask	-26149
#define	KBrowserWmlbrowserOnPickEx	                -26150
#define	KBrowserXhtmlBadContent                    -26151
#define	KBrowserCryptoUserCancel                   -26152
#define	KBrowserCryptoNoCert                       -26153
#define	KBrowserUnsupportedFormCharset	            -26156
#define	KBrowserScrproxyScrAbort                   -26163
#define	KBrowserScrproxyScrBusy                    -26164
#define	KBrowserScrproxyScrNotResponding           -26165
#define	KBrowserWaeNavigationCancelled	            -26167
#define	KBrowserConnCancelled	                    -26172
#define	KBrowserConnFailed	                        -26173
#define	KBrowserLmgrConstraintTooSmall	            -26189
#define	KBrowserLmgrNotConstrained	                -26190
#define	KBrowserLmgrFormatOverflow	                -26191
#define	KBrowserLmgrSplitOk                        -26192
#define	KBrowserLmgrNoSplit                        -26194
#define	KBrowserLmgrNotClear                       -26196
#define	KBrowserDomNoStringReturned                -26199
#define	KBrowserDomNodeTypeErr	                    -26200
#define	KBrowserDomNoValuePrefix                   -26201
#define KBrowserWbxmlErrorBytecode                 -26202
#define	KBrowserWbxmlErrorCharsetUnsupported       -26203
#define	KBrowserHedNoCurrentHistEntry	            -26211
#define	KBrowserHedNoPreviousHistEntry	            -26212
#define	KBrowserHedNoNextHistEntry	                -26213
#define	KBrowserHedContentDispatched               -26215
#define	KBrowserTcpHostUnreachable	                -26223
#define	KBrowserSavedPageFailed                    -26224
#define	KBrowserRestartParsing                     -26225
#define KBrowserMvcConverterNotFound               -26226
#define KBrowserContinue                           -26227
#define KBrowserLastErrorCode		                -26999

// HTTP status codes
#define KBrowserHTTP100                            -25100
#define KBrowserHTTP101                            -25101
#define KBrowserHTTP102                            -25102
#define KBrowserHTTP200                            -25200
#define KBrowserHTTP201                            -25201
#define KBrowserHTTP202                            -25202
#define KBrowserHTTP203                            -25203
#define KBrowserHTTP204                            -25204
#define KBrowserHTTP205                            -25205
#define KBrowserHTTP206                            -25206
#define KBrowserHTTP300                            -25300
#define KBrowserHTTP301                            -25301
#define KBrowserHTTP302                            -25302
#define KBrowserHTTP303                            -25303
#define KBrowserHTTP304                            -25304
#define KBrowserHTTP305                            -25305
#define KBrowserHTTP400                            -25400
#define KBrowserHTTP401                            -25401
#define KBrowserHTTP402                            -25402
#define KBrowserHTTP403                            -25403
#define KBrowserHTTP404                            -25404
#define KBrowserHTTP405                            -25405
#define KBrowserHTTP406                            -25406
#define KBrowserHTTP407                            -25407
#define KBrowserHTTP408                            -25408
#define KBrowserHTTP409                            -25409
#define KBrowserHTTP410                            -25410
#define KBrowserHTTP411                            -25411
#define KBrowserHTTP412                            -25412
#define KBrowserHTTP413                            -25413
#define KBrowserHTTP414                            -25414
#define KBrowserHTTP415                            -25415
#define KBrowserHTTP500                            -25500
#define KBrowserHTTP501                            -25501
#define KBrowserHTTP502                            -25502
#define KBrowserHTTP503                            -25503
#define KBrowserHTTP504                            -25504
#define KBrowserHTTP505                            -25505

#endif // HTTPLOADERDEFS_H
