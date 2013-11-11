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


#ifndef HTTPDOWNLOADMGRLOGGER_H
#define HTTPDOWNLOADMGRLOGGER_H

// INCLUDE FILES
#include <e32svr.h>

#ifdef _DEBUG

#define __DOWNLOADMGR_LOG__
#include <flogger.h>

// CONSTANTS

class CFunctionEntryExit;

//Logging to file in THUMB UDEB
class TDMLogger
	{
    public: // methods

        static void CreateFlogger( TAny* aOwner );
        static void CloseFLogger( TAny* aOwner );

        static void AttachFLogger( const TAny* aOwner, 
                                   const TAny* aAttach1,
                                   const TAny* aAttach2 );
       
        static void NameIt( const TAny* aOwner, 
                            TRefByValue<const TDesC> aNameFmt, ...);
        
	    static void LogIt(const TAny* aOwner, TRefByValue<const TDesC> aFmt, ...);
        static void LogIt(const TAny* aOwner, TRefByValue<const TDesC8> aFmt, ...);
	    static void LogIt(const TAny* aOwner, TRefByValue<const TDesC> aFmt, VA_LIST aList );
        static void LogIt(const TAny* aOwner, TRefByValue<const TDesC8> aFmt, VA_LIST aList );

	    static void WriteComment(const TAny* aOwner, const TDesC& aComment);
	    static void WriteComment(const TAny* aOwner, const TDesC8& aData);

	    static void DumpIt(const TAny* aOwner, const TDesC8& aData);

    private:       

        TBuf<2048>      iLogBuf;
        TBuf8<2048>     iLogBuf8;
        TInt            iRefCounter;
	};

class CFunctionEntryExit : public CBase
    {
    public:

        CFunctionEntryExit( const TAny* aOwner, TRefByValue<const TDesC> aFunc );
        ~CFunctionEntryExit();

    public:
    
        HBufC*      iFunc;
        const TAny* iOwner;
    };

#define _T8( a )                        TPtrC8( (const TText8*)(a) )
#define LOGGER_ENTERFN( a )       CFunctionEntryExit entryExit( this, _L(a) )

#define CLOG_CREATE                      TDMLogger::CreateFlogger( this )
#define CLOG_CREATE_NULL                 TDMLogger::CreateFlogger( NULL )
#define CLOG_ATTACH( attach1, attach2 )  TDMLogger::AttachFLogger( this, attach1, attach2 )
#define CLOG_NAME( a )                   TDMLogger::NameIt( this, a )
#define CLOG_NAME_1( a, b )             TDMLogger::NameIt( this, a, b )
#define CLOG_NAME_2( a, b, c )          TDMLogger::NameIt( this, a, b, c )
#define CLOG_NAME_3( a, b, c, d )       TDMLogger::NameIt( this, a, b, c, d )
#define CLOG_CLOSE                      TDMLogger::CloseFLogger( this )
#define CLOG_WRITE( a )                   TDMLogger::LogIt( this, _L(a) )
#define CLOG_WRITE_1( a, b )              TDMLogger::LogIt( this, _L(a), b )
#define CLOG_WRITE_2( a, b, c )           TDMLogger::LogIt( this, _L(a), b, c )
#define CLOG_WRITE_3( a, b, c, d )        TDMLogger::LogIt( this, _L(a), b, c, d )
#define CLOG_WRITE_4( a, b, c, d, e )     TDMLogger::LogIt( this, _L(a), b, c, d, e )
#define CLOG_WRITE8( a )                  TDMLogger::LogIt( this, _T8(a) )
#define CLOG_WRITE8_1( a, b )             TDMLogger::LogIt( this, _T8(a), b )
#define CLOG_WRITE8_2( a, b, c )          TDMLogger::LogIt( this, _T8(a), b, c )
#define CLOG_WRITE8_3( a, b, c, d )       TDMLogger::LogIt( this, _T8(a), b, c, d )
#define CLOG_WRITE8_4( a, b, c, d, e )    TDMLogger::LogIt( this, _T8(a), b, c, d, e )

#else   // _DEBUG

#define LOGGER_ENTERFN( a )
#define CLOG_CREATE                
#define CLOG_ATTACH( attach1, attach2 )
#define CLOG_NAME( a )
#define CLOG_NAME_1( a, b )
#define CLOG_NAME_2( a, b, c )
#define CLOG_NAME_3( a, b, c, d )
#define CLOG_CLOSE
#define CLOG_WRITE( a )                   
#define CLOG_WRITE_1( a, b )              
#define CLOG_WRITE_2( a, b, c )           
#define CLOG_WRITE_3( a, b, c, d )        
#define CLOG_WRITE_4( a, b, c, d, e )     
#define CLOG_WRITE8( a )                  
#define CLOG_WRITE8_1( a, b )             
#define CLOG_WRITE8_2( a, b, c )          
#define CLOG_WRITE8_3( a, b, c, d )       
#define CLOG_WRITE8_4( a, b, c, d, e )    

#endif // _DEBUG
#endif	// HTTPDOWNLOADMGRLOGGER_H

// End of file