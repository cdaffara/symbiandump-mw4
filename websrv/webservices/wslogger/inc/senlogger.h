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
* Description:           Central place for debug-type macros
*
*/











#ifndef SEN_LOGGER_H
#define SEN_LOGGER_H

// Note that _DEBUG is automatically defined for debug builds


#include "sendebug.h"
#include <senpointermap.h>
//  INCLUDE
#include <e32svr.h>
#include <flogger.h>

// CONSTANTS    

// Utilized by TLS-loggers:

const TInt KMaxLogLevel =2;
const TInt KNormalLogLevel =1;
const TInt KMinLogLevel =0;

const TInt KSenServiceConnectionLogChannelBase            = 1000;
const TInt KSenServiceConnectionLogChannel                =18000;
const TInt KSenServiceConnectionLogLevel                  =0;
const TInt KSenServiceConnectionStatusLogChannelBase      = 1100;
const TInt KSenServiceConnectionStatusLogLevel            = 0;
_LIT(KSenServiceConnectionLogDir,                         "WsLog");
_LIT(KSenServiceConnectionLogFile,                        "SenServiceConn.log");
_LIT(KSenServiceConnectionStatusLogFile,                  "SenServiceConnStatus.log");

const TInt KSenServiceManagerLogChannelBase           = 2000;
const TInt KSenServiceManagerLogChannel               = 19000;
const TInt KSenServiceManagerLogLevel                 = 0;
_LIT(KSenServiceManagerLogDir,                        "WsLog");
_LIT(KSenServiceManagerLogFile,                       "SenServiceMgr.log");

const TInt KSenHttpChannelLogChannelBase              = 3000;
const TInt KSenHttpChannelLogLevel                    = 0;

_LIT(KSenHttpChannelLogDir,                           "WsLog");
_LIT(KSenHttpChannelLogFile,                          "SenHttpChannel.log");


const TInt KSenHostletConnectionLogChannelBase              = 4000;
const TInt KSenHostletConnectionLogLevel                    = 0;
_LIT(KSenHostletConnectionLogDir,                           "WsLog");
_LIT(KSenHostletConnectionLogFile,                          "SenHostletConnection.log");


const TInt KSenCoreServiceManagerLogChannelBase            = 5000;
const TInt KSenCoreServiceManagerStatusLogChannelBase      = 1200;
const TInt KSenCoreServiceManagerLogLevel                  = 0;
_LIT(KSenCoreServiceManagerLogDir,                         "WsLog");
_LIT(KSenCoreServiceManagerLogFile,                        "SenCoreServiceManagerTLS.log");


const TInt KSenFaultsLogChannel                 = 6000;
const TInt KSenFaultsLogLevel                   = 0;
_LIT(KSenFaults,                                "WsLog");
_LIT(KSenFaultsLog,                             "SenFaults.log");


const TInt KSenUtilsLogChannel     =7000;
const TInt KSenUtilsLogLevel       = 0;
_LIT(KSenUtils,                    "WsLog");
_LIT(KSenUtilsLog,                 "SenUtils.log");


const TInt KSenServConnLogChannelBase         = 8000;
const TInt KSenServConnLogLevel               = 0;
 _LIT(KSenServConn, "WsLog");
 _LIT(KSenServConnLog, "SenServicePolicy.log");


const TInt KSenHttpChannelObserverLogChannelBase         = 9000;
const TInt KSenHttpChannelObserverLogLevel               = 0;
_LIT(KSenHttpChannelObserverLogDir,                   "WsLog");
_LIT(KSenHttpChannelObserverLogFile,                  "SenHttpChannelObserver.log");


const TInt KSenLocalTransportLogChannelBase         = 10000;
const TInt KSenLocalTransportLogLevel               = 0;
_LIT(KSenLocalTransportLogDir,  "WsLog");
_LIT(KSenLocalTransportLogFile, "SenLocalTransport.log");




const TInt KSenPolicyLogChannelBase         = 11000;
const TInt KSenPolicyLogLevel               = 0;
 _LIT(KSenPolicyLogDir, " WsLog");
 _LIT(KSenPolicyLogFile, "SenPolicy.log");
 
 
 
const TInt KSenVtcpLogChannelBase         = 12000;
const TInt KSenVtcpLogLevel               = 0;
 _LIT(KSenVtcpLogFileDir, "WsLog");
 _LIT(KSenVtcpLogFile, "SenVtcp.log");
 
 
 
const TInt KSenVtcpObserverLogChannelBase         = 13000;
const TInt KSenVtcpObserverLogLevel               = 0;
 _LIT(KSenVtcpObserverLogDir,  "WsLog");
 _LIT(KSenVtcpObserverLogFile, "SenVtcpObserver.log");
 


const TInt KSenVtcpDispatcherLogChannel             = 13500;
const TInt KSenVtcpDispatcherLogLevel               = 0;
 _LIT(KSenVtcpDispatcherLogDir,  "WsLog");
 _LIT(KSenVtcpDispatcherLogFile, "SenVtcpDispatcher.log");



const TInt KSenMessagesLogChannel       = 14000;
const TInt KSenMessagesLogLevel         = 0;
_LIT(KSenMessages,                      "WsLog");
_LIT(KSenMessagesLog,                   "SenMessages.log");


const TInt KSenCredsLogChannel          = 15000;
const TInt KSenCredsLogLevelMin         = 1;
const TInt KSenCredsLogLevelNormal      = 2;
const TInt KSenCredsLogLevelMax         = 3;
_LIT(KSenCreds,                         "WsLog");
_LIT(KSenCredsLog,                      "SenCreds.log");


const TInt KSenXmlLogChannel       = 16000;
const TInt KSenXmlLogLevel         = 0;
_LIT(KSenXml,                      "WsLog");
_LIT(KSenXmlLog,                   "SenXml.log");

const TInt KSenClientSessionLogChannelBase              = 17000;
const TInt KSenClientSessionLogLevel                    =0;

const TInt KHostletTransportLogChannel = 20000;
const TInt KHostletTransportLogLevel   = 0;    
_LIT(KLogDir,  "WsLog");
_LIT(KLogFile, "SenHostletTransport.log");

const TInt KSenNcimLogChannelAsync          = 21000;
const TInt KSenNcimLogChannelMsg            = 21001;
const TInt KSenNcimLogChannelGeneral        = 21002;
const TInt KSenNcimLogLevel               = 0;
 _LIT(KSenNcimLogDir,  "WsLog");
 _LIT(KSenNcimLogFileAsync, "SenNcimAsync.log");
 _LIT(KSenNcimLogFileMsg, "SenNcimMsg.log");
 _LIT(KSenNcimLogFileGen, "SenNcimGen.log");
  
const TInt KSenDispatcherLogChannel     = 22000;
const TInt KSenDispatcherLogLevel       = 0;
_LIT(KSenDispactherThread,                    "WsLog");
_LIT(KSenDispactherThreadLog,                 "SenDispatcherThread.log");

/*
templates
 Reqest
_L8("                           | | | ---------->  Request(Invoke Public)"));

Complet
_L8("                           | | |                    ---<  Complete"));

Runl
_L8("                           | | |              <----  RunL          "));
*/
#ifdef __ENABLE_ALR__
const TInt KSenSenMobilityLogChannelBase              = 27000;
const TInt KSenSenMobilityLogLevel                    =0;
_LIT(KSenMobilityDir,  "WsLog");
_LIT(KSenMobilityLog, "SenMobility.log");
#endif // __ENABLE_ALR__

// WS PAOS HTTP filter
const TInt KPoasFilterLogChannel        = 30000;
const TInt KPoasFilterLogLevel          = 0;
_LIT( KPoasLogFileDir,                  "WsLog" );
_LIT( KPoasLogFileName,                 "WsPaosFilter.log" );    

// WS HTTP filter (pairs with PAOS filter)
const TInt KWsHttpFilterLogChannel      = 31000;
const TInt KWsHttpFilterLogLevel        = 0;
_LIT( KWsHttpFilterLogFileDir,          "WsLog" );
_LIT( KWsHttpFilterLogFileName,         "WsHttpFilter.log" );

class CSenLogger;
class CWSConnectionProxy;

typedef struct
    {
    RFileLogger iLog;
    TInt        iCount;
    TInt        iChannel;
    TInt        iLevel;
    } TLog;

typedef struct 
    {
    CSenLogger* iLogger;
    CWSConnectionProxy* iProxy;
    } TWsTls;

// CLASS DECLARATION
class CSenLogger : public CBase
    {
    public: // New functions
        /**
        * Creates the global ESenDebug object.
        */
        IMPORT_C static TInt CreateL(TInt aChannel, TInt aLevel, const TDesC& aLogDir, const TDesC& aLogFileName);
        IMPORT_C static TInt CreateL(TInt aChannel, TInt aLevel, const TDesC& aLogDir, const TDesC& aLogFileName, TFileLoggingMode aMode);

        /**
        * Destroys the global ESenDebug object.
        */
        IMPORT_C static void Destroy(TInt aChannel);
        
        // 8-bit UCS2 variants:
      IMPORT_C static void Write(TInt aChannel, TInt aLevel, const TDesC8& aText);
      IMPORT_C static void WriteAll(TInt aChannel, TInt aLevel, const TDesC8& aText);
      IMPORT_C static void WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt,...);
      IMPORT_C static void WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);

      IMPORT_C static void WriteWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC8& aText);
      IMPORT_C static void WriteAllWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC8& aText);
      IMPORT_C static void WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt,...);
      IMPORT_C static void WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
      IMPORT_C static void WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC8> aFmt,...);
      IMPORT_C static void WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);


        // 16-bit Unicode variants:
      IMPORT_C static void Write(TInt aChannel, TInt aLevel, const TDesC16& aText);
      IMPORT_C static void WriteAll(TInt aChannel, TInt aLevel, const TDesC16& aText);
      IMPORT_C static void WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt,...);
      IMPORT_C static void WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList);

        // 16-bit Unicode variants:      
			IMPORT_C static void WriteWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC16& aText);
	    IMPORT_C static void WriteAllWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC16& aText);

    	IMPORT_C static void WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt,...);
	    IMPORT_C static void WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList);
    	IMPORT_C static void WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC16> aFmt,...);
		IMPORT_C static void WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList);
        
    private: // Constructor and destructor
        TInt ChannelIndex(TInt aChannel) const;
        static RFileLogger* Logger(TInt aChannel, TInt aLevel);
    
        /**
        * Constructor.
        */
        IMPORT_C CSenLogger();

        /**
        * Destructor.
        */
        virtual ~CSenLogger();
        
    private: // Data
        TInt                iCount;
        RPointerArray<TLog> iLogs;
        RSenPointerMap<TInt,TInt> iMap;
        
    };

#ifdef _SENDEBUG
    #define TLSLOG_OPEN(channel, level, logdir, logfile)    CSenLogger::CreateL(channel, level, logdir, logfile)
    #define TLSLOG_OPEN_TO_MODE(channel, level, logdir, logfile, loggingmode)  CSenLogger::CreateL(channel, level, logdir, logfile, loggingmode)
    #define TLSLOG(channel, level, string)                  CSenLogger::Write(channel, level, string)
    #define TLSLOG_L(channel, level, string)                CSenLogger::Write(channel, level, _L(string))
    #define TLSLOG_FORMAT(parameters)                       CSenLogger::WriteFormat parameters
    #define TLSLOG_ALL(channel, level, string)              CSenLogger::WriteAll(channel, level, string)
    #define TLSLOG_CLOSE(channel)                           CSenLogger::Destroy(channel)
#else // _SENDEBUG not defined:
    #define TLSLOG_OPEN(channel, level, logdir, logfile)
    #define TLSLOG_OPEN_TO_MODE(channel, level, logdir, logfile, loggingmode)
    #define TLSLOG_L(channel, level, string)
    #define TLSLOG(channel, level, string)
    #define TLSLOG_ALL(channel, level, string)
    #define TLSLOG_FORMAT(parameters)
    #define TLSLOG_CLOSE(channel)
    #define CSLOG(clientid, level, string)
    #define CSLOG_L(clientid, level, string)
    #define CSLOG_FORMAT(parameters)
    #define CSLOG_ALL(clientid, level, string)
#endif // _DEBUG

#ifdef _SENDEBUG
//#ifndef TRUE // disabling CS logs (for testing purposes)
    //  Core-side, Client Session logging methods:
//    #define CSLOG_OPEN(clientid, level, logdir, logfile)    CSenLogger::CreateWithClientIdL(channel, level, logdir, logfile)
    #define CSLOG(clientid, level, string)                  CSenLogger::WriteWithClientId(clientid, KSenClientSessionLogChannelBase+clientid, level, string);\
                                                            CSenLogger::WriteWithClientId(clientid, KSenCoreServiceManagerLogChannelBase, level, string)
                                                            
    #define CSLOG_L(clientid, level, string)                CSenLogger::WriteWithClientId(clientid, KSenClientSessionLogChannelBase+clientid, level, _L(string));\
                                                            CSenLogger::WriteWithClientId(clientid, KSenCoreServiceManagerLogChannelBase, level, _L(string))

    // aClientId, TInt aLevel, <format_string>, ...
    #define CSLOG_FORMAT(parameters)                        CSenLogger::WriteFormatWithClientIdToMainLogs parameters
    
    #define CSLOG_ALL(clientid, level, string)              CSenLogger::WriteAllWithClientId(clientid, KSenClientSessionLogChannelBase+clientid, level, string);\
                                                            CSenLogger::WriteAllWithClientId(clientid, KSenCoreServiceManagerLogChannelBase, level, string)
//    #define CSLOG_CLOSE(clientid)                           CSenLogger::DestroyWithClientId(channel)
#else // _SENDEBUG not defined:
//    #define CSLOG_OPEN(clientid, level, logdir, logfile)
    #define CSLOG(clientid, level, string)
    #define CSLOG_L(clientid, level, string)
    #define CSLOG_FORMAT(parameters)
    #define CSLOG_ALL(clientid, level, string)
//    #define CSLOG_CLOSE(clientid)   
#endif // _DEBUG



#ifdef _SENDEBUG
    #define CREDLOG_OPEN()                                   CSenLogger::CreateL(KSenCredsLogChannel, KSenCredsLogLevelMax, KSenCreds, KSenCredsLog)
    #define CREDLOG_L(level, string)                         CSenLogger::Write(KSenCredsLogChannel, level, _L(string))
    #define CREDLOG(level, string)                           CSenLogger::Write(KSenCredsLogChannel, level, string)
    #define CREDLOG_ALL(level, string)                       CSenLogger::WriteAll(KSenCredsLogChannel, level, string)
    #define CREDLOG_FORMAT(parameters)                       CSenLogger::WriteFormat parameters
    #define CREDLOG_CLOSE()                                  CSenLogger::Destroy(KSenCredsLogChannel)
#else // _SENDEBUG not defined in <codebase>inc\webservices.hrh file:
    #define CREDLOG_OPEN()
    #define CREDLOG_L(level, string)
    #define CREDLOG(level, string)
    #define CREDLOG_ALL(level, string)
    #define CREDLOG_FORMAT(parameters)
    #define CREDLOG_CLOSE()
#endif // _DEBUG

#endif // SEN_LOGGER_H

// End of File