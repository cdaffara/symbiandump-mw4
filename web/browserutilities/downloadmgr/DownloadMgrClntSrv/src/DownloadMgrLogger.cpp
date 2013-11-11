/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include    "HttpDownloadMgrLogger.h"

#ifdef __DOWNLOADMGR_LOG__
#include    "FileExt.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
_LIT( KDmEngLogDir, "DownloadMgr" );
_LIT( KDmEngLogFile,    "DownloadMgr.log" );
_LIT( KDeclareLoggerName, "<DECLARE_LOGGER_NAME>< %S >" );
_LIT( KAttachLogger, "<ATTACH_LOGGER><%x>to<%x>" );
_LIT( KOwnerLog, "- <%x> -\t" );
_LIT8( KOwnerLog8, "- <%x> -\t" );
_LIT( KLoggerCreated, "Logger created" );
_LIT( KLoggerClosed, "Logger closed" );

// MACROS
//#define _LOG_MICRO_TIME_

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//

// ============================ MEMBER FUNCTIONS ===============================

CFunctionEntryExit::CFunctionEntryExit( const TAny* aOwner, TRefByValue<const TDesC> aFunc )
    : iOwner( aOwner )
    {
    iFunc = HBufC::New( TDesC(aFunc).Length() );
    
    if( iFunc )
        {
        iFunc->Des().Copy(aFunc);
        }
        
    TDMLogger::LogIt( aOwner, _L("-> <%S>"), iFunc );
    }
    
CFunctionEntryExit::~CFunctionEntryExit() 
    {
    TDMLogger::LogIt( iOwner, _L("<- <%S>"), iFunc );
    
    delete iFunc;
    }

void TDMLogger::CreateFlogger( TAny* aOwner )
    {
    TDMLogger* logger = (TDMLogger*)Dll::Tls();
    
    if( !logger )
        {
        TDMLogger* logger = new TDMLogger;
        logger->iRefCounter = 1;
        
        Dll::SetTls( logger );
        }
    else
        {
        ++logger->iRefCounter;
        }     
        
    TDMLogger::LogIt( aOwner, KLoggerCreated );
    }
    
void TDMLogger::CloseFLogger( TAny* aOwner )
    {
    TDMLogger::LogIt( aOwner, KLoggerClosed );

    TDMLogger* logger = (TDMLogger*)Dll::Tls();
    
    if( !--logger->iRefCounter )
        {
        Dll::SetTls( NULL );
        delete logger;
        }
    }

void TDMLogger::AttachFLogger( const TAny* aOwner,
                               const TAny* aAttach1, 
                               const TAny* aAttach2 )
    {
    LogIt( aOwner, KAttachLogger, aAttach1, aAttach2 );
    }
    
void TDMLogger::NameIt( const TAny* aOwner, TRefByValue<const TDesC> aFmt, ...)
    {
    TDMLogger* logger = (TDMLogger*)Dll::Tls();

    VA_LIST list;
    VA_START(list,aFmt);
    
    TBuf<128> nameBuf;
    
    nameBuf.FormatList( aFmt, list );
    logger->iLogBuf.Format( KDeclareLoggerName, &nameBuf );

    VA_END(list);
    
    TBuf<32> tempBuf;
    
#ifdef _LOG_MICRO_TIME_
    TTime timeNull( 0 );
    TTime timeNow;
    
    timeNow.UniversalTime();
    
    tempBuf.Format( _L(" - %Li - "), timeNow.MicroSecondsFrom(timeNull));
    
    logger->iLogBuf.Insert( 0, timeBuf );
#endif    

    tempBuf.Format( KOwnerLog, aOwner );
    logger->iLogBuf.Insert( 0, tempBuf );

    RFileLogger::Write( KDmEngLogDir, KDmEngLogFile, EFileLoggingModeAppend, logger->iLogBuf );
    }
    
void TDMLogger::LogIt(const TAny* aOwner, TRefByValue<const TDesC> aFmt, VA_LIST aList )
    {
    TDMLogger* logger = (TDMLogger*)Dll::Tls();
            
    logger->iLogBuf.FormatList( aFmt, aList );

    TBuf<32> tempBuf;

#ifdef _LOG_MICRO_TIME_
    TTime timeNull( 0 );
    TTime timeNow;
    
    timeNow.UniversalTime();
    
    tempBuf.Format( _L(" - %Li - "), timeNow.MicroSecondsFrom(timeNull));
    
    logger->iLogBuf.Insert( 0, timeBuf );
#endif    

    tempBuf.Format( KOwnerLog, aOwner );
    logger->iLogBuf.Insert( 0, tempBuf );

    RFileLogger::Write( KDmEngLogDir, KDmEngLogFile, EFileLoggingModeAppend, logger->iLogBuf );
    }
    
void TDMLogger::LogIt(const TAny* aOwner, TRefByValue<const TDesC8> aFmt, VA_LIST aList )
    {
    TDMLogger* logger = (TDMLogger*)Dll::Tls();
            
    logger->iLogBuf8.FormatList( aFmt, aList );

    TBuf8<32> tempBuf8;

#ifdef _LOG_MICRO_TIME_
    TTime timeNull( 0 );
    TTime timeNow;
    
    timeNow.UniversalTime();
    
    
    tempBuf8.Format( _L8(" - %Li - "), timeNow.MicroSecondsFrom(timeNull));
    
    logger->iLogBuf8.Insert( 0, tempBuf );
#endif    

    tempBuf8.Format( KOwnerLog8, aOwner );
    logger->iLogBuf8.Insert( 0, tempBuf8 );
    
    RFileLogger::Write( KDmEngLogDir, KDmEngLogFile, EFileLoggingModeAppend, logger->iLogBuf8 );
    }

void TDMLogger::LogIt( const TAny* aOwner, TRefByValue<const TDesC> aFmt, ...)

    {
    VA_LIST list;
    VA_START(list,aFmt);
    
    TDMLogger::LogIt( aOwner, aFmt, list );

    VA_END(list);
    }

void TDMLogger::LogIt( const TAny* aOwner, TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    
    TDMLogger::LogIt( aOwner, aFmt, list );

    VA_END(list);
    }

void TDMLogger::WriteComment( const TAny* /*aOwner*/, const TDesC& /*aComment*/)
    {
    }

void TDMLogger::WriteComment( const TAny* /*aOwner*/, const TDesC8& /*aData*/)
    {
    }

void TDMLogger::DumpIt( const TAny* /*aOwner*/, const TDesC8& /*aData*/)
    {
    }

#endif  // __DOWNLOADMGR_LOG__

//  End of File  
