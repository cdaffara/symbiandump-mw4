/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This module defines event logger.
*
*/



#include <eikenv.h>
#include <bautils.h>
//#include "filesdef.h"
#include "eventlogger.h"

//
//Class that writes logs events into a binary file

CEventLogger::CEventLogger(CEventMediatorServer* aServer)
{
    iServer = aServer;
    iFileMaxLength = KBYTES_TO_BYTES * LOGFILE_MAX_LTH;           
}

CEventLogger* CEventLogger::NewL(CEventMediatorServer* aServer)
    {
    CEventLogger* server = new (ELeave) CEventLogger(aServer);
    return server;
    }

CEventLogger::CEventLogger(TInt aFileMaxLength, CEventMediatorServer* aServer)
{
    iServer = aServer;
    if ( aFileMaxLength == 0 || aFileMaxLength > LOGFILE_MAX_LTH )
         iFileMaxLength = KBYTES_TO_BYTES * LOGFILE_MAX_LTH;           
    else iFileMaxLength = KBYTES_TO_BYTES * aFileMaxLength; 
}

CEventLogger::~CEventLogger()
{
}



void CEventLogger::LogEvent(const TDesC8& aEvent)
{
    TInt      position;
    TInt      log_elem_lth;  
    TBuf8<EVENTLOG_FILE_HEADER_LTH> file_hdr;

/*-------------------------------------------------------------------
 *  Log an event into the binary file. File format:
 *  0                    3                    7                   CL  
 *  +--------------------+--------------------+---------------------+
 *  ! File Id        (FI)! Current length (CL)! Wrapping offset (WO)!      
 *  +--------------------+--------------------+---------------------+
 *
 *  +-----------------------+                                          
 *  ! Curr event number (EN)
 *  +-----------------------+                                          
 *  where;
 *
 *  FI = File Id 0x474f4c45 = ELOG
 *  CL = Current free position in file (=offset value)
 *  WO = Wrapping offset 
 *  EN = Event number; events are numbered starting from 1
 *       Number of the last event to be logged
 *
 *  Log event data format:
 *
 *  Log event data format:
 *  0           3          7            15                 19
 *  +-----------+----------+------------+------------------+----------------+     
 *  ! Length(L) ! Event    ! TimeStamp) ! Source           ! Catogory (EC)) !          
 *  !           ! number(EN) (TS)       ! Component (SC)   !               
 *  +-----------+----------+------------+------------------+----------------+ - - - - +
 *  23          27         31           n                  n+12
 *  +-----------+----------+------------+------------------+ 
 *  ! MsgId     ! Descriptor Descriptors! Trailer (TR)     ! 
 *  ! (MI)      ! count (DC) (DE)       !                  !               
 *  +-----------+----------+------------+------------------+
 *  *  where;
 * 
 *  L  = Length of log element
 *  EN = Event number
 *  TS = Time stamp (TTime in TInt64 (two TUint32)
 *  SC = UID of source component
 *  EC = Event catogory  (Info, warning, error, debug)
 *  MI = Message Id, defines a text string in localisation file
 *  DC = Descriptor count, number of lth-data pairs
 *  DE = Descriptors: lth,lth...,data,data,... These are used to modify
 *       the text avaibale in localisation file
 *  TR = Element trailer: endmark1, endmark2, length of log element
 *       same value as in (L)  
 *
 *---------------------------------------------------------------------*/
    if ( OpenLogFile() ) 
    {
       file_hdr.SetLength(EVENTLOG_FILE_HEADER_LTH);             
       if ( GetFileHeader(file_hdr) ) {
          position = GetElemDataPosition(file_hdr, aEvent.Length());
          if ( position ) {
             log_elem_lth = WriteLogElem(position, aEvent);
             if ( log_elem_lth ) {
                position += log_elem_lth;
                SaveFileHeader(position, file_hdr);
             }          
          }   
       }
       
       CloseLogFile();
    }
    
}

TBool CEventLogger::OpenLogFile()
{
    TBool file_created = EFalse;
    TInt  position     = 0;
/*--------------------------------------------------------
 *  Open event log file
 *--------------------------------------------------------*/
    if(iFS.Connect()!=KErrNone)
        return EFalse;
    if ( iLogFile.Create(iFS, iServer->EventLogFileName(), EFileWrite|EFileShareAny) != KErrNone )
    {
       if (iLogFile.Open(iFS, iServer->EventLogFileName(), EFileWrite|EFileShareAny) != KErrNone)
       {
//          DEB(iEngine->PrintText(_L("Error opening Trace file\n"));)
            iFS.Close();
            return EFalse;
       }
    }
    else file_created = ETrue;

    if ( file_created ) {
      /*--------------------------------------------------------
       *  Write file header initial values to created file
       *  (Current_offset = First_elem_offset = 16)     
       *--------------------------------------------------------*/
       TBuf8<EVENTLOG_FILE_HEADER_LTH> file_hdr;
       file_hdr.SetLength(EVENTLOG_FILE_HEADER_LTH);        
       StoreFileHeaderInfo(EVENTLOG_FILE_HEADER_LTH, 0, file_hdr);
       if ( SaveFileHeader(EVENTLOG_FILE_HEADER_LTH, file_hdr) == EFalse ) {
//          DEB(iEngine->PrintText(_L("File header creation error\n"));)
          return EFalse;
       }       
       iLogFile.Seek(ESeekStart, position);  // return file pointer to file start
    }       

    return ETrue;
}

void CEventLogger::CloseLogFile()
{
/*--------------------------------------------------------
 *  Close event log file
 *--------------------------------------------------------*/
    iLogFile.Close();
    iFS.Close();
}

TBool CEventLogger::GetFileHeader(TDes8 &aBuf)
{
/*--------------------------------------------------------
 *  Read current log file length from file start
 *--------------------------------------------------------*/
    if ( iLogFile.Read(aBuf, EVENTLOG_FILE_HEADER_LTH) ) {
       return EFalse;               
    }

    return ETrue;
}


TInt CEventLogger::GetElemDataPosition(TDes8 &aFileHdr, TInt aLength)
{
    TInt position;
/*---------------------------------------------------------------
 *  Get log element position value from file header
 *  If (position + aLength) > file max length, wrap file
 *---------------------------------------------------------------*/
    TFileHeader *file_hdr = (TFileHeader *)aFileHdr.Ptr();      
    position = file_hdr->GetPosition();
    if ( ( position + aLength +
           ELEM_HEADER_LTH + ELEM_TRAILER_LTH ) > iFileMaxLength )
        {
       position = EVENTLOG_FILE_HEADER_LTH;  // Wrapping occurs  
        }
    iEventNumber = file_hdr->GetEventNumber() + 1;

    return position;
}

/*
TUint32 CEventLogger::GetTimeStamp()
{
TTime     tmp_time; 
TDateTime date;
TUint32   time_stamp = 0;
*/
/*---------------------------------------------------------------
 *  Get time stamp for log element.
 *  Map the year-month-day-hour-minutes-seconds information to 
 *  32 bits into the following way:
 *    3                   2                   1                   0    
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  Y Y Y Y M M M M D D D D D D H H H H H H M M M M M M S S S S S S
 *  where; 
 *  Bits 31-28 = Current Year - 2000  (0-15)
 *  Bits 27-24 = Current Month index (0-11))
 *  Bits 23-18 = Current Day (1-31)
 *  Bits 17-12 = Current Hour (0-23)
 *  Bits 11-7  = Current Minutes (0-59)
 *  Bits  6-0  = Current Seconds (0-59)    
 *  
 *--------------------------------------------------------------- /
    tmp_time.HomeTime();
    date = tmp_time.DateTime();
    
    time_stamp |= (((TUint32)(date.Year() - 2000)) << 28);
    time_stamp |= ((TUint32)date.Month() << 24);
    time_stamp |= ((TUint32)date.Day() << 18);
    time_stamp |= ((TUint32)date.Hour() << 12);
    time_stamp |= ((TUint32)date.Minute() << 6);
    time_stamp |= (TUint32)date.Second();
    
    return time_stamp;
}   
*/

TBool CEventLogger::SaveFileHeader(TInt aPosition, TDes8 &aBuf)
{
    TInt old_position;
    TInt wrapping_offset;
/*---------------------------------------------------------------
 *  Save event log element position value to file header
 *  If current position < position in file header, file has wrapped.
 *  Store then wrapping offset in file header, too
 *---------------------------------------------------------------*/
    TFileHeader *file_hdr = (TFileHeader *)aBuf.Ptr();
    old_position    = file_hdr->GetPosition();
    wrapping_offset = file_hdr->GetWrappingOffset();    
    if ( old_position > aPosition ) {
       wrapping_offset = old_position; /* wrapping occurred */
    }
    else {
       if ( wrapping_offset && ( wrapping_offset < aPosition ) ) {
          wrapping_offset = 0;
       }       
    }   
          
    StoreFileHeaderInfo(aPosition, wrapping_offset, aBuf);

    old_position = 0;         // not old position really, just integer with value 0 needed
    iLogFile.Seek(ESeekStart, old_position);  // return file pointer to file start   
    if ( iLogFile.Write(aBuf, EVENTLOG_FILE_HEADER_LTH) )
       return EFalse;
    
    return ETrue;   
}


void CEventLogger::StoreFileHeaderInfo(TInt aPosition, TInt aWrappingOffset, TDes8 &aBuf)
{

    TFileHeader *file_hdr = (TFileHeader *)aBuf.Ptr();
    file_hdr->SetFileId();
    file_hdr->SetPosition(aPosition);    
    file_hdr->SetWrappingOffset(aWrappingOffset);
    file_hdr->SetEventNumber(iEventNumber);
    
}

TInt CEventLogger::WriteLogElem(TInt aPosition, const TDesC8& aEvent)  

//TInt CEventLogger::WriteLogElem(TInt aPosition, const TLogEvent& aEvent, const TDesC8& aData)  
{
    TBuf8<ELEM_HEADER_LTH> header;
    TBuf8<ELEM_TRAILER_LTH>  trailer;  
//    TUint32   time_stamp;
    TInt      data_lth=aEvent.Length();
    TInt eventLength = data_lth + ELEM_HEADER_LTH + ELEM_TRAILER_LTH;
/*---------------------------------------------------------------
 *  Write log event data into the log file 
 *  Build first log element header:
 *  log elem length + iEventNumber + time stamp
 *---------------------------------------------------------------*/
//  time_stamp  = GetTimeStamp();
    header.SetLength(ELEM_HEADER_LTH);  

    TElemHeader *elem_hdr = (TElemHeader *)header.Ptr();      
    elem_hdr->SetEventLength((TUint32) eventLength);
    elem_hdr->SetEventNumber((TUint32) iEventNumber);

    TTime tmpTime;
    const TInt64* currTime;
    tmpTime.HomeTime();
    currTime = &tmpTime.Int64();
    elem_hdr->SetTime(*currTime);
    
    trailer.SetLength(ELEM_TRAILER_LTH);    
    TElemTrailer *elem_trailer = (TElemTrailer *)trailer.Ptr();
    elem_trailer->SetLogEndMark();    
    elem_trailer->SetEventLength((TUint32) eventLength);
    iLogFile.Seek(ESeekStart, aPosition);  // Set file pointer    
/*---------------------------------------------------------------
 *  Write log event into log element
 *---------------------------------------------------------------*/
    if ( iLogFile.Write(header) )         //Element header 
       return 0;
    
    if ( iLogFile.Write(aEvent))            //log event data
       return 0;
    
    if ( iLogFile.Write(trailer) )         //Element trailer 
       return 0;
    
    return eventLength;
    
}

