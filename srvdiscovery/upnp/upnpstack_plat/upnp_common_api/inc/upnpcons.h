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
* Description:  Constants
*
*/


#ifndef C_UPNPCONS_H
#define C_UPNPCONS_H

// INCLUDES
#include <in_sock.h>
//#include <PathInfo.h>
#include "upnperrors.h"

#define D_LIT8(name,s) (TDesC8&)_LIT8(name, s)

//#define __DEFINES_IN_USE__

// CONSTANTS
_LIT( KSessionIdSemaphoreName,       "SessIdSemaphore" );
_LIT( KSessionIdAccessSemaphoreName, "SessIdAccessSemaphore" );


_LIT8( KPrinterDevice,       "urn:schemas-upnp-org:device:Printer:1" );
_LIT8( KMediaServerDevice,   "urn:schemas-upnp-org:device:MediaServer:1" );
_LIT8( KMediaRendererDevice, "urn:schemas-upnp-org:device:MediaRenderer:1" );

const TInt KMaxFixed144Left = 14;
const TInt KMaxFixed144Right = 4;
const TInt KDefaultHttpPortNumber = 80;
const TInt KMaxIntegerLength=10;

const TInt KErrHttpBadRequest = -400;
const TInt KRandomPort = 0;


// ENUMERATIONS
    enum TArgumentType
    {
        EUi1,
        EUi2,
        EUi4,
        EI1,
        EI2,
        EI4,
        EInt,
        ER4,
        ER8,
        ENumber,
        EFixed144,
        EFloat,
        EChar,
        EString,
        EDate,
        EDateTime,
        EDateTimeTz,
        ETime,
        ETimeTz,
        EBoolean,
        EBinBase64,
        EBinHex,
        EUri,
        EUuid,
        EUnknown
    };

    typedef TArgumentType TStateVariableType;
    


namespace UpnpString
{
    static const TInt KMaxFilenameLength = 100;
    static const TInt KShortStringLength = 30;
    static const TInt KDefaultStringLength = 150;
    static const TInt KMaxIntLength = 11;
    static const TInt KMaxTUintLength = 10;
    static const TInt KMaxAddrOutputLength = 20;

    static const TInt KMaxUuidLength = 43;

    static const TInt KMaxHeaderLength = 512;
    static const TInt KSocketBufferSize = 1024;
    
    static const TInt KDateTimeStringLen = 50;
    
    static const TInt KLineFeedLength = 2; 
    static const TInt KCRLFLength = 2; 
    static const TInt KDoubleLineFeedLength = 4;
    static const TInt KCRLFCRLength = 3;
    static const TInt KLinefeedLength = 1;
    static const TInt KCRLength = 1;
    
    

    
    _LIT8( KColon,              ":" );
    _LIT8( KEqual,              "=" );    
    _LIT8( KDblColon,           "::" );
    _LIT8( KSemiColon,          ";" );
    _LIT8( KSpace,              " " );
    _LIT8( KTab,                "\t" );
    _LIT8( KSlash,              "/" );
    _LIT8( KDoubleSlash,        "//" );
    _LIT8( KInvComma,           "\"" );
    _LIT8( KMinus,              "-" );
    _LIT8( KComa,               "," );

    _LIT( KColon16,             ":" );
    _LIT( KDblColon16,          "::" );
    _LIT( KSemiColon16,         ";" );
    _LIT( KSpace16,             " " );
    _LIT( KSlash16,             "/" );
    _LIT( KDoubleSlash16,       "//" );
    
    _LIT8(  KDot8,              "." );
    _LIT16( KDot16,             "." );
    _LIT16( KDoubleBackSlash16, "\\" );
    _LIT8( KDoubleBackSlash,    "\\" );    
    _LIT8( KLinefeed,           "\n" );
    _LIT8( KCRLF,               "\n\n" );
    
    _LIT8( KEmpty,              "" );
    _LIT( KEmpty16,             "" );
        
    _LIT8( KLineFeed,           "\r\n" );
    _LIT8( KCRLFSP,             "\r\n " );
    _LIT8( KCRLFHT,             "\r\n\t" );
    _LIT8( KCR,                 "\r" );
    _LIT8( KLFCRLF,             "\n\r\n" );    
    _LIT8( KCRLFCR,             "\r\n\r" ); 
    _LIT8( KDoubleLineFeed,     "\r\n\r\n" );    

    // used in HttpSession::CheckIfExistsL    
    _LIT( KUnderScore, "_" );

}

namespace UpnpPaths
{    
    // _LIT8( KDefaultMediaPath,    "c:\\My Files\\Image gallery\\");
    _LIT8( KDefaultMediaPath,       "c:\\nokia\\" );
    // _LIT8( KDefaultMediaPath,    "e:\\" ); 

    // #define KDefaultMediaPath PathInfo::PhoneMemoryRootPath()
    // _LIT( KDefaultMediaPath,     "C:\\My Files\\Image gallery\\" );  
    // #define KDefaultMediaPath PathInfo::PhoneMemoryRootPath()

    _LIT8( KLoggingPath,                    "c:\\logs\\upnp\\" );
    _LIT8( KDefaultPostedFilesPath,         "c:\\System\\data\\upnp\\posted\\" );
    _LIT8( KMediaserverHttpDocumentRoot,    "/Media/MediaServerContent/" );
    _LIT16( KLoggingPath16,                 "c:\\logs\\upnp\\" );
    _LIT16( KDefaultPostedFilesPath16,      "c:\\System\\data\\upnp\\posted\\" );
    _LIT16( KMediaserverHttpDocumentRoot16, "/Media/MediaServerContent/" );
    
    //patterny for checking if current folder/filename is allowed to share
    _LIT(KForbiddenPrivateFolderPattern,    "?:\\private\\*"); 
    _LIT(KForbiddenZDrivePattern,           "z:\\*");     

    static const TInt KDriveLength = 1;    // C or E
    
}

namespace UpnpSocket
{
    static const TInt KMaxConnectionsInQueue = 2;
}

namespace UpnpGENA
{
    _LIT8( KGenaSubscribe,      "SUBSCRIBE" );
    _LIT8( KGenaUnSubscribe,    "UNSUBSCRIBE" );
    _LIT8( KGenaNotify,         "NOTIFY" );
    
    _LIT8( KCallback,           "CALLBACK" );
    _LIT8( KTimeout,            "TIMEOUT" );
    _LIT8( KContentType,        "Content-Type" );
    _LIT8( KSeq,                "SEQ" );
    _LIT8( KSid,                "SID" );

    _LIT8( KDefaultNt,          "upnp:event" );
    _LIT8( KDefaultNts,         "upnp:propchange" );
    _LIT8( KDefaultContentType, "text/xml" );
    _LIT8( KTextXmlUtf8,        "text/xml; charset=\"utf-8\"");

    _LIT8( KGenaEvent,          "schemas-upnp-org:event-1-0" );
}

namespace UpnpHTTP
{
    static const TInt KDefaultHTTPPort = 50001;
    _LIT8( KSchemeSeparator,    "://" );
    _LIT( KSchemeSeparator16,   "://" );
    
    _LIT8( TransferCoding,      "Transfer-Coding:" );
    _LIT8( TransferEncoding,    "Transfer-Encoding:" );
    _LIT8( KConnection,         "Connection" );
    _LIT8( Chunked,             "Chunked" );
    _LIT8( KClose,              "close" );

    _LIT8( KHTTPUrl,            "http://" );
    _LIT8( KHdrContentLength,   "Content-Length" );    
    _LIT8( KHdrContentType,     "Content-Type" );    
    _LIT8( KHdrDate,            "Date" );
    _LIT8( KHdrExt,             "EXT:" );
    _LIT8( KHdrExtWithoutColon, "EXT" );
    
    _LIT8( KHdrExpect,          "Expect" );
    _LIT8( K100Continue,        "100-continue" );
    
    _LIT8( KHdrRange,           "Range" );
    _LIT8( KHdrContentRange,    "Content-Range" );
    _LIT8( KBytes,              "bytes" );
    _LIT8( KHTTPPartialContent, "HTTP/1.1 206 Partial Content");
    _LIT8( KHTTPNoContent,      "HTTP/1.1 204 No Content");

    _LIT8( KHdrPragma,          "Pragma" );
    _LIT8( KHdrCacheControl,    "Cache-Control" );
    _LIT8( KNoCache,            "no-cache" );
    
    _LIT8( KHdrUserAgent,       "User-Agent" );
    _LIT8( KCPUserAgentValue,   "DLNADOC/1.50 UPnP/1.0" );
    _LIT8( KHdrServer,          "Server" );
    
    _LIT( TransferCoding16,     "Transfer-Coding:" );
    _LIT( TransferEncoding16,   "Transfer-Encoding:" );
    _LIT( KConnection16,        "Connection" );
    _LIT( Chunked16,            "Chunked" );

    _LIT( KHTTPUrl16,           "http://" );
    _LIT( KHdrContentLength16,  "Content-Length" );
    _LIT( KHdrDate16,           "Date" );
    _LIT( KHdrExt16,            "EXT:" );

    _LIT8( KHTTPOk,             "HTTP/1.1 200 OK" );
    
    _LIT8( KDefaultDate,        "Sun, 06 Nov 1994 08:49:37 GMT" );
    //schemes
    _LIT( KSchemeFile,          "file" );
    _LIT8( KSchemeFile8,        "file" );
    _LIT( KSchemeHTTP,          "http" );
    _LIT8( KSchemeHTTP8,        "http" );

    _LIT8( KHdrAcceptLanguage,  "Accept-Language" );
    _LIT8( KHdrContentLanguage, "Content-Language" );
    _LIT8( KLanguageEn,         "en" );
    _LIT8( KContentLength,      "Content-Length:" );
    _LIT8( KContentType,        "Content-Type:" );
}

namespace UpnpSSDP
{
    static const TInt KDefaultSSDPPort = 50101;
    static const TInt KMaxMessageLength = 2048;
    
    static const TUint KMulticastAddress = INET_ADDR(239,255,255,250);
    static const TInt KMulticastPort = 1900;
    static const TInt KHdrSTLen = 2;
    static const TInt KIPAddBuffLen = 100;
    static const TInt KDefaultMaxAgeValue = 1800;


    _LIT8( KNotify,               "NOTIFY" );
    _LIT8( KMSearch,              "M-SEARCH" );
 
    _LIT8( KMethodNotify,         "NOTIFY * HTTP/1.1" );
    _LIT8( KMethodMSearch,        "M-SEARCH * HTTP/1.1" );
    
    _LIT8( KHdrHost,              "Host" );
    _LIT8( KHdrCacheControl,      "Cache-Control" );
    _LIT8( KHdrLocation,          "Location" );
    _LIT8( KHdrLOCATION,          "LOCATION" );
    _LIT8( KHdrSearchTarget,      "NT" );
    _LIT8( KHdrSEARCHTARGET,      "Nt" );
    _LIT8( KHdrNotificationType,  "NTS" );
    _LIT8( KHdrNOTIFICATIONTYPE,  "Nts" );
    _LIT8( KHdrServer,            "Server" );
    _LIT8( KHdrUuid,              "USN" );
    _LIT8( KHdrUUID,              "Usn" );
    _LIT8( KHdrMan,               "Man" );
    _LIT8( KHdrMx,                "MX" );
    _LIT8( KHdrSt,                "ST" );
        
    _LIT8( KNotificationAlive,    "ssdp:alive" );
    _LIT8( KNotificationByebye,   "ssdp:byebye" );
    _LIT8( KNotificationDiscover, "\"ssdp:discover\"" );
    
    _LIT8( KDefaultOS,            "Symbian OS 7.0s" );    
    _LIT8( KDefaultProduct,       "Nokia phone" );
    
    _LIT8( KDefaultMaxAge,        "max-age=1800" );
    _LIT8( KSsdpMaxAge,           "max-age" );
    _LIT8( KDefaultHost,          "239.255.255.250:1900" );
    _LIT8( KDefaultLocation,      "239.255.255.250:1900" );
    _LIT8( KDefaultResponseDelay, "2" );
    
    _LIT8( KUPnPRootDevice,       "upnp:rootdevice" );
    _LIT8( KSearchTargetAll,      "ssdp:all" );
    
    _LIT8( KUPnPUuid,             "uuid:" );
    _LIT8( KUPnPDeviceSchema,     "urn:schemas-upnp-org:device:" );
    _LIT8( KUPnPServiceSchema,    "urn:schemas-upnp-org:service:" );
    
    _LIT8( novalue,               "NoValue" );
}

namespace UpnpCD
{
    _LIT8( KQuotation,          "\""); 
    _LIT8( KQuotationWithSpace, "\" ");
    _LIT8( KObjectID,           "ObjectID");
    _LIT8( KSpace,              " ");
    _LIT8( KSpace8,             " ");
    _LIT8( KKorp,               "<"); 
    _LIT8( KKorpSlash,          "</"); 
    _LIT8( KProk,               ">");
    _LIT8( KOne,                "1");
    _LIT8( KZero,               "0");
    _LIT8( KMinusOne,           "-1");
    _LIT8( KClosingBracket,     ")");
    _LIT8( KClosingBracketWithQuotation, "\")");
    _LIT8( KPlainLineFeed,      "\r\n");
    _LIT8( Keq,                 "=\"");
    _LIT8( KOneNumber,          "%i");
    _LIT8( KComma,              ",");
    _LIT8( KEmptyString,        "");
    _LIT8( KColon,              ":");
    _LIT8( KErrorHere,          "Here is an ERROR!");
    _LIT8( KOneNumber8,         "%i");
    _LIT8( Ktrue,               "true");
    _LIT8( Kfalse,              "false");
    _LIT8( KRequired,           "r");    // stands for required
    _LIT8( kElement,            "e"); // stands for element
    _LIT8( KProperty,           "p");
    _LIT8( kYes,                "1");
    _LIT8( KProperties,         "ps");
    _LIT8( kName,               "n");
    _LIT8( kInherited,          "i");
    _LIT8( kItem,               "i");
    _LIT8( KType,               "type");
    _LIT8( kAttribute,          "Attribute");
    _LIT8( kNo,                 "0");
}

namespace UpnpDLNA
{
    _LIT8( KHdrTransferMode,            "transferMode.dlna.org");
    _LIT8( KTransferModeStreaming,      "Streaming");
    _LIT8( KTransferModeInteractive,    "Interactive");
    _LIT8( KTransferModeBackground,     "Background");
    
    _LIT8( KHdrTimeSeekRange,           "TimeSeekRange.dlna.org");
    _LIT8( KHdrPlaySpeed,               "PlaySpeed.dlna.org");
    _LIT8( KHdrRealTimeInfo,            "realTimeInfo.dlna.org");

    _LIT8( KHdrContentFeatures,         "contentFeatures.dlna.org");
    _LIT8( KHdrGetcontentFeatures,      "getcontentFeatures.dlna.org");
    _LIT8( KHdrGetcontentFeaturesValue, "1");
}

#endif // C_UPNPCONS_H

// End of File