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
* Description:          
*
*/










#include "senlogger.h"
#include <f32file.h>
#include <s32file.h> 

namespace
    {
    const TInt KColumnWidth = 100; // used in WriteAll() methods.
     _LIT(KPath,"C:\\SenLogConfig.txt");
     _LIT(  KClientIdFmt,  "- Client[%d] %S");
     
     _LIT(  KClientIdFmtNextLine,  "Client(%d):");
     
     _LIT8( KClientIdFmt8, "- Client[%d] %S");
     _LIT8( KClientIdFmtNextLine8, "Client(%d):");
    }

EXPORT_C TInt CSenLogger::CreateL(TInt aChannel, TInt aLevel, const TDesC& aLogDir, const TDesC& aLogFileName)
     {
	 return CreateL( aChannel, aLevel, aLogDir, aLogFileName, EFileLoggingModeOverwrite );
     }
EXPORT_C TInt CSenLogger::CreateL(TInt aChannel, TInt aLevel, const TDesC& aLogDir, const TDesC& aLogFileName, TFileLoggingMode aMode)
    {
    TInt index(KErrNotFound);
    CSenLogger* pSenLogger = NULL;
    TWsTls* tls = (TWsTls*)Dll::Tls();
    if( !tls )
        {
        tls = new (ELeave) TWsTls();
        tls->iLogger = NULL;
        tls->iProxy = NULL;
        Dll::SetTls( tls );
        }
    pSenLogger = tls->iLogger;    
	if ( !pSenLogger )
  		{
  		pSenLogger = new (ELeave) CSenLogger();
        if ( pSenLogger )
            {
            tls->iLogger = pSenLogger;
      		pSenLogger->iCount = 0;
      		//reading the configuration file for the logging levels!!!
      		
      		/*the file has the  following format (IN ANSI)
      		* ==============================================================================
      		  * LogChannel LogLevel
      		    1000 0
      		    2000 1
      		      .
      		      .
      		      .
      		      .
      		      .
      		      ....
      		    18000 2
      		   this means first you need to write the channel followed by the corresponding logging level after a space
      		 
      		 the file has to be located under C:\Config.txt
      		 * ==============================================================================*/
      		 
      
      		TInt res;
      	    RFs fs;
      	    res=fs.Connect();
      	      if(res==KErrNone)
			{
			RFileReadStream readStream;
	      	res=readStream.Open(fs, KPath, EFileRead|EFileStreamText);
			if(res == KErrNone)
				{
				TInt error;
				TInt retVal;
				do
					{
					TBuf8<128> readLineBuf;
					TRAP(error, readStream.ReadL(readLineBuf, (TChar)'\n'));
					if(error == KErrNone)
						{      
						TInt32 length(0);
						length = readLineBuf.Length() ;
						TInt *key = new (ELeave) TInt;
						TLex8 lexBuf(readLineBuf);
						retVal = lexBuf.Val(*key);

						if(retVal!=0)
							{
							delete key;
							continue;
							}
						else
							{
							TInt *value = new (ELeave) TInt;
							lexBuf.SkipSpace();
							retVal=lexBuf.Val(*value);
							if(retVal==0)
								{
								retVal = pSenLogger->iMap.Append(key,value);
								if(retVal != KErrNone)
									{
									delete key;
									delete value;
									}
								}
							else
								{
								delete key;
								delete value;
								}
							}
						}
					}while(error == KErrNone);
				readStream.Close();
				}
			fs.Close();
			}
            }
        else
            {
            return KErrGeneral;
            }
  		}
  	else
  	    {
  	    index = pSenLogger->ChannelIndex(aChannel);
  	    }	
  	
  	if (index == KErrNotFound)
  	    {
  	    TInt channelIndex;
        TLog* pTLog = new (ELeave) TLog;
        CleanupStack::PushL(pTLog);
        pTLog->iChannel=aChannel;
        channelIndex=pSenLogger->iMap.Find(aChannel);
        if(channelIndex==KErrNotFound)
        {
        pTLog->iLevel=aLevel;
        }
        else
        {
        pTLog->iLevel=*(pSenLogger->iMap.ValueAt(channelIndex));
        }
        pTLog->iCount=1;
        pTLog->iLog.Connect();
        pTLog->iLog.CreateLog(aLogDir, aLogFileName, aMode);
        pTLog->iLog.Write(_L("SenTLSLogger - Log file opened"));
#ifdef EKA
        pSenLogger->iLogs.AppendL(pTLog);
#else
        User::LeaveIfError(pSenLogger->iLogs.Append(pTLog));
	  
#endif
        CleanupStack::Pop(pTLog);
  	    }
  	else
  	    {
  	    pSenLogger->iLogs[index]->iCount++;
  	    }
  	pSenLogger->iCount++;
    
    return KErrNone;
    }

EXPORT_C void CSenLogger::Destroy(TInt aChannel)
    {
    //	CSenLogger* pSenLogger = (CSenLogger*)Dll::Tls();
    CSenLogger* pSenLogger = NULL;
    TWsTls* tls = (TWsTls*)Dll::Tls();
    if( tls )
        {
        pSenLogger = tls->iLogger;
        }
	if ( pSenLogger )
	    {
	    
	    TInt index = pSenLogger->ChannelIndex(aChannel);
	    if (index != KErrNotFound)
	        {
	        pSenLogger->iLogs[index]->iCount--;
	        if (pSenLogger->iLogs[index]->iCount <= 0)
	            {
                pSenLogger->iLogs[index]->iLog.Write(_L("SenTLSLogger - Log file closed."));
                pSenLogger->iLogs[index]->iLog.CloseLog();
                pSenLogger->iLogs[index]->iLog.Close();
                delete pSenLogger->iLogs[index];
                pSenLogger->iLogs.Remove(index);
	            }
            }
	        pSenLogger->iCount--;
	        if (pSenLogger->iCount <= 0)
	            {
	            pSenLogger->iLogs.Close();
	            delete pSenLogger;
            tls->iLogger = NULL;
            if( !tls->iLogger && !tls->iProxy )
                {
	            delete tls;
                Dll::FreeTls();	            
	            }
	        }
	    }
    }

// 16-bit variants    
    
EXPORT_C void CSenLogger::Write(TInt aChannel, TInt aLevel, const TDesC16& aText)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if ( pLogger )
        {
        pLogger->Write(aText);
        }
    }

EXPORT_C void CSenLogger::WriteAll(TInt aChannel, TInt aLevel, const TDesC16& aText)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if ( pLogger )
        {
        TInt i = 0;
        TInt lineAmount(KColumnWidth);
        TInt length = aText.Length();
        while(i<aText.Length())
            {
            if ( length - i < KColumnWidth )
                {
                lineAmount = length - i;    
                }
            pLogger->Write(aText.Mid(i, lineAmount));
            i += KColumnWidth;
            }
        }
    }

EXPORT_C void CSenLogger::WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt,...)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if (pLogger)
        {
        VA_LIST list;
        VA_START(list,aFmt);
        pLogger->WriteFormat(aFmt, list);
        }    
    }
    
EXPORT_C void CSenLogger::WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if (pLogger)
        {
        pLogger->WriteFormat(aFmt, aList);
        }    
    }


// Client ID spesific logging methods    
EXPORT_C void CSenLogger::WriteWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC16& aText)
    {
    WriteFormat( aChannel, aLevel, KClientIdFmt, aClientId, &aText );
    }

EXPORT_C void CSenLogger::WriteAllWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC16& aText)
    {
    // Write client id on separate line, for clarity & simplicity
    WriteFormat( aChannel, aLevel, KClientIdFmtNextLine, aClientId);
    WriteAll( aChannel, aLevel, aText );
    }

EXPORT_C void CSenLogger::WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt, ...)
    {
    // Write client id on separate line, for clarity & simplicity
    WriteFormat( aChannel, aLevel, KClientIdFmtNextLine, aClientId);
    VA_LIST list;
    VA_START(list,aFmt);
    WriteFormat( aChannel, aLevel, aFmt, list);
    }
    
EXPORT_C void CSenLogger::WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
    {
    WriteFormat( aChannel, aLevel, KClientIdFmtNextLine, aClientId);
    WriteFormat( aChannel, aLevel, aFmt, aList);
    }

EXPORT_C void CSenLogger::WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC16> aFmt,...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    WriteFormatWithClientId( aClientId, KSenClientSessionLogChannelBase+aClientId, aLevel, aFmt, list );
    WriteFormatWithClientId( aClientId, KSenCoreServiceManagerLogChannelBase, aLevel, aFmt, list );
    }

EXPORT_C void CSenLogger::WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
    {
    WriteFormatWithClientId( aClientId, KSenClientSessionLogChannelBase+aClientId, aLevel, aFmt, aList );
    WriteFormatWithClientId( aClientId, KSenCoreServiceManagerLogChannelBase, aLevel, aFmt, aList );
    }
    
// 8-bit variants    

EXPORT_C void CSenLogger::Write(TInt aChannel, TInt aLevel, const TDesC8& aText)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if (pLogger)
        {
        pLogger->Write(aText);
        }
    }

EXPORT_C void CSenLogger::WriteAll(TInt aChannel, TInt aLevel, const TDesC8& aText)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if ( pLogger )
        {
        TInt i = 0;
        TInt lineAmount(KColumnWidth);
        TInt length = aText.Length();
        while(i<aText.Length())
            {
            if ( length - i < KColumnWidth )
                {
                lineAmount = length - i;    
                }
            pLogger->Write(aText.Mid(i, lineAmount));
            i += KColumnWidth;
            }
        }
    }


EXPORT_C void CSenLogger::WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt,...)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if (pLogger)
        {
        VA_LIST list;
        VA_START(list,aFmt);
        pLogger->WriteFormat(aFmt, list);
        }    
    }

EXPORT_C void CSenLogger::WriteFormat(TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    RFileLogger* pLogger = Logger(aChannel, aLevel);
    if (pLogger)
        {
        pLogger->WriteFormat(aFmt, aList);
        }    
    }

EXPORT_C void CSenLogger::WriteWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC8& aText)
    {
    WriteFormat( aChannel, aLevel, KClientIdFmt8, aClientId, &aText );    	
    }

EXPORT_C void CSenLogger::WriteAllWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, const TDesC8& aText)
    {
    // Write client id on separate line, for clarity & simplicity
    WriteFormat( aChannel, aLevel, KClientIdFmtNextLine8, aClientId);
    WriteAll( aChannel, aLevel, aText );
    }

EXPORT_C void CSenLogger::WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt,...)
    {
    // Write client id on separate line, for clarity & simplicity
    WriteFormat( aChannel, aLevel, KClientIdFmtNextLine8, aClientId);
    VA_LIST list;
    VA_START(list,aFmt);
    WriteFormat( aChannel, aLevel, aFmt, list);
    }
    
EXPORT_C void CSenLogger::WriteFormatWithClientId(TInt aClientId, TInt aChannel, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    WriteFormat( aChannel, aLevel, KClientIdFmtNextLine8, aClientId);
    WriteFormat( aChannel, aLevel, aFmt, aList);
    }

EXPORT_C void CSenLogger::WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    WriteFormatWithClientId( aClientId, KSenClientSessionLogChannelBase+aClientId, aLevel, aFmt, list );
    WriteFormatWithClientId( aClientId, KSenCoreServiceManagerLogChannelBase, aLevel, aFmt, list );
    }

EXPORT_C void CSenLogger::WriteFormatWithClientIdToMainLogs(TInt aClientId, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    WriteFormatWithClientId( aClientId, KSenClientSessionLogChannelBase+aClientId, aLevel, aFmt, aList );
    WriteFormatWithClientId( aClientId, KSenCoreServiceManagerLogChannelBase, aLevel, aFmt, aList );
    }


RFileLogger* CSenLogger::Logger(TInt aChannel, TInt aLevel)
    {
//    CSenLogger* pSenLogger = (CSenLogger*)Dll::Tls();
    CSenLogger* pSenLogger = NULL;
    TWsTls* tls = (TWsTls*)Dll::Tls();
    if( tls )
        {
        pSenLogger = tls->iLogger;
        }
	if ( pSenLogger )
	    {
        TInt index = pSenLogger->ChannelIndex(aChannel);
        if (index != KErrNotFound)
            {
            if (pSenLogger->iLogs[index]->iLevel >= aLevel)
                {
                return &pSenLogger->iLogs[index]->iLog;
                }
            }
	    }
	return NULL;
    }

TInt CSenLogger::ChannelIndex(TInt aChannel) const
    {
    TInt index(KErrNotFound);
    TInt count=iLogs.Count();
    for (TInt i=0; i<count; i++)      	    
        {
        if (iLogs[i]->iChannel == aChannel)
            {
            index = i;
            break;
            }
        }
    return index;    
    }

EXPORT_C CSenLogger::CSenLogger()
    : iCount(0),iMap(1,1)
    {
    }

CSenLogger::~CSenLogger()
    {
   
    iMap.Reset();
    }

// END OF FILE

