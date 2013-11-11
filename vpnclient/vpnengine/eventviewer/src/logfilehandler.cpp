/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Log file handler
*
*/



#include "eventviewer2.h"
#include "log_eventviewer.h"
//#include "logcommon.h"

///////////////////////////////////////////////////////////////////
//  OpenLogFile
//  This function checks the state of the LogFile and tries to open
//  the file if it is not yet open. It also reads the header section
//  of the file and checks that the file is a correct log file.
///////////////////////////////////////////////////////////////////
TInt CEventViewer::OpenLogFile()
    {
    TInt status = ReadLogFileHeader();
    if (status != KErrNone)
        {
        return status;
        }

    TakeCopyOfFileHeader(iCurrFileHeader, iPreviousFileHeader);   
    return KErrNone;
    }

///////////////////////////////////////////////////////////////////
//  CloseLogFile
//  This function closes the log file
///////////////////////////////////////////////////////////////////
void CEventViewer::CloseLogFile()
    {
    ;
    }

///////////////////////////////////////////////////////////////////
//  DoDeleteLogFileL
//  This function deletes the event log file
///////////////////////////////////////////////////////////////////
void CEventViewer::DoDeleteLogFileL()
    {
    CloseLogFile();
    TInt status = iEventMediator.ClearEventLog();
    LOG(Log::Printf(_L("Log cleared %d\n"), status));
    if (status != KErrNone)
        {
        User::Leave(status);
        }
    }
    
///////////////////////////////////////////////////////////////////
//  ReadWholeLogFileToMemoryBuffer()
//  
//  This function reads the whole log file to a memory buffer
//  and takes a new version of file header.
///////////////////////////////////////////////////////////////////
TInt CEventViewer::ReadWholeLogFileToMemoryBuffer()
    {
    TInt status;
    status = iEventMediator.GetEventLogSize(iLogFileSize);
    if (status != KErrNone)
        {
        return status;
        }
    if (iLogFileSize < EVENTLOG_FILE_HEADER_LTH)
        {
        return KErrNotFound;
        }

    delete iLogFileBuf;
    iLogFileBuf = 0;
    
    iLogFileBuf = HBufC8::New(iLogFileSize + 2048); // 2048: it can be bigger now
    if (iLogFileBuf == 0)
        {
        return KErrNoMemory;
        }

    TPtr8 ptr(iLogFileBuf->Des());
    status = iEventMediator.GetEventLogData(ptr);
    if (status != KErrNone)
        {
        return status;
        }

    status = CopyPackedFileHeaderToUnpackedObject(ptr);
    if (status != KErrNone) 
        {
        return status;
        }
    return KErrNone;
    }           

///////////////////////////////////////////////////////////////////
//  ReadLogFileHeader
//  This function reads the packed format file header section of the
//  event log file  and copies the contents of header to the
//  caller's unpacked object.
///////////////////////////////////////////////////////////////////
TInt CEventViewer::ReadLogFileHeader()
    {
    TBuf8<EVENTLOG_FILE_HEADER_LTH> fileHeaderBuf;
    TInt status = iEventMediator.GetEventLogHeader(fileHeaderBuf);
    if (status != KErrNone)
        {
        return status;
        }

    status = CopyPackedFileHeaderToUnpackedObject(fileHeaderBuf);
    if (status != KErrNone)
        {
        return status;
        }

    return KErrNone;
    }
    
///////////////////////////////////////////////////////////////////
//  TakeCopyOfFileHeader
//  This function takes a copy of the unpacked  file header.               
///////////////////////////////////////////////////////////////////
void CEventViewer::TakeCopyOfFileHeader(TUnpackedFileHeader& asrc,
                                        TUnpackedFileHeader& adest)
    {
    adest.iFileId                = asrc.iFileId;
    adest.iPositionOfNextFree    = asrc.iPositionOfNextFree;
    adest.iPositionOfWrapping    = asrc.iPositionOfWrapping;
    adest.iCurrEventNumber       = asrc.iCurrEventNumber;
    }

///////////////////////////////////////////////////////////////////
//  CopyPackedFileHeaderToUnpackedObject
//  This function copies the packet format file header to an
//  unpacked object.
///////////////////////////////////////////////////////////////////
TInt CEventViewer::CopyPackedFileHeaderToUnpackedObject( TDesC8& aFileHeader)
    {

    // Convert the TDesC8 parameter to TUint8* format

    TFileHeader* fileHeader = (TFileHeader*) aFileHeader.Ptr(); 

    // Copy the packet format header parameters to unpacked object

    iCurrFileHeader.iFileId                = fileHeader->GetFileId();
    iCurrFileHeader.iPositionOfNextFree    = fileHeader->GetPositionOfNextFree();
    iCurrFileHeader.iPositionOfWrapping    = fileHeader->GetPositionOfWrapping();
    iCurrFileHeader.iCurrEventNumber       = fileHeader->GetCurrEventNumber();

    // Check the file Id

    if (iCurrFileHeader.iFileId != LOG_FILE_ID)
       {
       return KErrNotFound;
       }
    
    return KErrNone;
    }

///////////////////////////////////////////////////////////////////
//  IsLogFileModified
//  Check if the logfile has been modified (file header is no more
//  the same as previously)
///////////////////////////////////////////////////////////////////
TBool CEventViewer::IsLogFileModified()
    {

    if (iCurrFileHeader.iFileId == iPreviousFileHeader.iFileId
        &&
        iCurrFileHeader.iPositionOfNextFree == iPreviousFileHeader.iPositionOfNextFree)
        {
        return EFalse;
        }
    return ETrue;
    }



///////////////////////////////////////////////////////////////////
//  GetStartPositionOfLogElem
//  This function calculates the start position of a log element.
//  As input parameter it has the position of the first byte
//  after the element.
///////////////////////////////////////////////////////////////////
TInt CEventViewer::GetStartPositionOfLogElem(TUint32 aPositionOfLogElemEnd,       
                                             TUint32* aPositionOfLogElem)
    {

    // Set base for the most recent log element trailer

    TPtr8 logFileBuf (iLogFileBuf->Des()); // Log file in memory
    TUint32  logElemTrailerPos = aPositionOfLogElemEnd - LOG_ELEM_TRAILER_LTH;

    if (iWrappingOccured && logElemTrailerPos <= iCurrFileHeader.iPositionOfNextFree)
        {

        return KErrNotFound;
        }

    TPtr8 elemTrailer (const_cast<TUint8*>(logFileBuf.Ptr())+ logElemTrailerPos, // Data ptr
                       LOG_ELEM_TRAILER_LTH,    // Data length
                       LOG_ELEM_TRAILER_LTH);   // Max length
    // Convert the TPtr8 parameter to TUint8* format

    TLogElemTrailer* elemTrailerPtr = (TLogElemTrailer*) elemTrailer.Ptr(); 

    // Copy the packet format trailer parameters to unpacked object

    iLogElemTrailer.iEndMark1             = elemTrailerPtr->GetEndMark1();
    iLogElemTrailer.iEndMark2             = elemTrailerPtr->GetEndMark2();
    iLogElemTrailer.iEventLength          = elemTrailerPtr->GetEventLength();

    // Check the validity of trailer

    if (iLogElemTrailer.iEndMark1 != END_MARK_1
        ||
        iLogElemTrailer.iEndMark2 != END_MARK_2
        ||
        iLogElemTrailer.iEventLength > aPositionOfLogElemEnd +
        EVENTLOG_FILE_HEADER_LTH)
        {

        return KErrGeneral;
        }

    // Set position for the current log element

    TUint32  logElemPos = aPositionOfLogElemEnd - iLogElemTrailer.iEventLength;

    if (iWrappingOccured && logElemPos < iCurrFileHeader.iPositionOfNextFree)
        {

        return KErrNotFound;
        }

    TPtr8 logElem (const_cast<TUint8*>(logFileBuf.Ptr())+ logElemPos, // Data ptr
                   iLogElemTrailer.iEventLength,   // Data length
                   iLogElemTrailer.iEventLength);

    // Convert the TPtr8 parameter to TUint8* format

    TLogElem* logElemPtr = (TLogElem*) logElem.Ptr(); 

    // Copy the event number and event length parameters
    // from packed format log element to unpacked object

    iUnpackedLogElem.iEventLength                 = logElemPtr->GetEventLength();
    iUnpackedLogElem.iEventNumber                 = logElemPtr->GetEventNumber();

    // Verify the extracted data

    if (iUnpackedLogElem.iEventNumber > iCurrFileHeader.iCurrEventNumber
        ||
        iUnpackedLogElem.iEventLength  != iLogElemTrailer.iEventLength)
        {

        return KErrGeneral;
        }

    *aPositionOfLogElem = logElemPos;

    return KErrNone;

    }

    //
    // Event found, copy the packed format log element parameters to an
    // unpacked object
    //


///////////////////////////////////////////////////////////////////
//  CopyPackedFileHeaderToUnpackedObject
//  This function copies the packet format log file element to an
//  unpacked object.
///////////////////////////////////////////////////////////////////
void CEventViewer::CopyPackedLogElemToUnpackedObject(TUint32 aPositionOfCurrLogElem)
    {
    // Build the log element pointer

    TPtr8 logFileBuf (iLogFileBuf->Des()); // Log file in memory
    TPtr8 logElem (const_cast<TUint8*>(logFileBuf.Ptr())+ aPositionOfCurrLogElem, // Data ptr
                   iUnpackedLogElem.iEventLength,   // Data length
                   iUnpackedLogElem.iEventLength);  // Max length

    // Convert the TPtr8 parameter to TUint8* format

    TLogElem* logElemPtr = (TLogElem*) logElem.Ptr(); 


    // Copy the packet format header parameters to unpacked object

    iUnpackedLogElem.iMsgId                = logElemPtr->GetMsgId();
    iUnpackedLogElem.iTimeStamp            = logElemPtr->GetTimeStamp();
    iUnpackedLogElem.iSourceComponent      = logElemPtr->GetSourceComponent();
    iUnpackedLogElem.iCategory             = logElemPtr->GetCategory();
    iUnpackedLogElem.iDescrCount           = logElemPtr->GetDescrCount();

    }

