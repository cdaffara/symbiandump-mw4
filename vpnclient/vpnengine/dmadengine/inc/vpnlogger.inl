/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging code fot the dmadpki
*
*/




#if defined(_DEBUG)


_LIT(KTraceOutOfMemory, "TRACE out of memory");

_LIT(KInMark, ">");
_LIT(KOutMark, "<");
_LIT(KDepthMark, "-");
_LIT(KSeparator, " ");
_LIT(KLeaveFrom, "LEAVE FROM: ");



void TTraceItem::TraceCleanupOperation(TAny* aItem)
    {           
    
    TTraceItem* traceCleanupItem = static_cast<TTraceItem*>(aItem);
    CVpnDebugLogger* logger = CVpnDebugLogger::VpnDebugLogger();
    logger->iCallDepth--;
        
    logger->iDebugString.Zero();                
    
    CVpnDebugLogger::TimeStamp(logger->iDebugString);
    logger->iDebugString.Append(KLeaveFrom);
    logger->iDebugString.Append(*(traceCleanupItem->iTraceName));  
    logger->WriteLogRaw(logger->iDebugString);
    
    delete traceCleanupItem->iTraceName;
    
    traceCleanupItem->iMethodHasLeft = ETrue;
    }


TTraceItem::TTraceItem(const TDesC& aTraceName)
    {
    iMethodHasLeft = EFalse;
    
    CVpnDebugLogger* logger = CVpnDebugLogger::VpnDebugLogger();
    
    iTraceName = aTraceName.Alloc();
    if (iTraceName != NULL)
        {                
        if (logger != NULL)
            {            
            logger->iCallDepth++;        
            
            logger->iDebugString.Zero();
            CVpnDebugLogger::TimeStamp(logger->iDebugString);
            
            for (TInt i = 0; i < logger->iCallDepth; ++i)
                {
                logger->iDebugString.Append(KDepthMark);
                }            
                    
            logger->iDebugString.Append(KInMark);
            logger->iDebugString.Append(KSeparator);        
            logger->iDebugString.Append(*iTraceName);        
            logger->WriteLogRaw(logger->iDebugString);
            }
        CleanupStack::PushL(TCleanupItem(TTraceItem::TraceCleanupOperation, this));
        }
    else
        {
        if (logger != NULL)
            {            
            logger->WriteLogRaw(KTraceOutOfMemory);
            }
        }
    }


TTraceItem::~TTraceItem()
    {
    CVpnDebugLogger* logger = CVpnDebugLogger::VpnDebugLogger();
    
    
    if (!iMethodHasLeft)
        {        
        if (iTraceName != NULL)
            {       
            if (logger != NULL)
                {                
                logger->iDebugString.Zero();
                CVpnDebugLogger::TimeStamp(logger->iDebugString);
                logger->iDebugString.Append(KOutMark);
                for (TInt i = 0; i < logger->iCallDepth; ++i)
                    {
                    logger->iDebugString.Append(KDepthMark);
                    }                

                logger->iDebugString.Append(KSeparator);
                logger->iDebugString.Append(*iTraceName);        
                logger->WriteLogRaw(logger->iDebugString);
                
                logger->iCallDepth--;                
                }
            delete iTraceName;        
            CleanupStack::Pop(this);                
            
            }
        else
            {
            if (logger != NULL)
                {                
                logger->WriteLogRaw(KTraceOutOfMemory);
                }
            }
        }
    }
    
    

void CVpnDebugLogger::InitializeDebugLoggerL(const TDesC& aFolder, const TDesC& aFileName)
    {
    
    CVpnDebugLogger* self = static_cast<CVpnDebugLogger*>(Dll::Tls());
    if (self == NULL)
        {            
        self = new (ELeave) CVpnDebugLogger;
        CleanupStack::PushL(self);
        self->ConstructL(aFolder, aFileName);
        User::LeaveIfError(Dll::SetTls(self));
        CleanupStack::Pop(self);    
        }
    self->iReferenceCount++;
    }


void CVpnDebugLogger::ConstructL(const TDesC& aFolder, const TDesC& aFileName)
    {            
    User::LeaveIfError(iFileLogger.Connect());
    iFileLogger.SetDateAndTime(EFalse, EFalse);
    iFileLogger.CreateLog(aFolder, aFileName, EFileLoggingModeAppend);	
    }
    
    
CVpnDebugLogger::~CVpnDebugLogger()
    {
    if ( iFileLogger.Handle() != 0 )
        {    
        iFileLogger.Write(_L("Logger delete"));        
        iFileLogger.CloseLog();
        }
    
    iFileLogger.Close();
    }


void CVpnDebugLogger::FinalizeDebugLogger()
    {
    CVpnDebugLogger* self = static_cast<CVpnDebugLogger*>(Dll::Tls());
    __ASSERT_ALWAYS(self != NULL, User::Invariant());
    
    self->iReferenceCount--;
    
    if (self->iReferenceCount == 0)
        {        
        Dll::SetTls(NULL);    
        delete self;
        }
    }


CVpnDebugLogger* CVpnDebugLogger::VpnDebugLogger()
    {
    return static_cast<CVpnDebugLogger*>(Dll::Tls());
    }
    

void CVpnDebugLogger::WriteLogRaw(const TDesC& aLogMessage)
    {
    iFileLogger.Write(aLogMessage);
    }
    
    
void CVpnDebugLogger::LogWrite(const TDesC& aText)
    {
    CVpnDebugLogger* logger = VpnDebugLogger();
    logger->iDebugString.Zero();
    CVpnDebugLogger::TimeStamp(logger->iDebugString);
    
    if (logger->iCallDepth > 0)
        {        
        for (TUint i = 0; i < logger->iCallDepth; ++i)
            {
            logger->iDebugString.Append(_L(" "));
            }
        logger->iDebugString.Append(_L("  "));
        }
        
    logger->iDebugString.Append(aText);
    logger->WriteLogRaw(logger->iDebugString);
    }


void CVpnDebugLogger::LogWrite(const TDesC8& aText)
    {
    CVpnDebugLogger* logger = VpnDebugLogger();
    logger->iDebugString.Zero();
    CVpnDebugLogger::TimeStamp(logger->iDebugString);
    
    if (logger->iCallDepth > 0)
        {        
        for (TUint i = 0; i < logger->iCallDepth; ++i)
            {
            logger->iDebugString.Append(_L(" "));
            }
        logger->iDebugString.Append(_L("  "));
        }
                
    TInt stringLength = logger->iDebugString.Length(); 
    TUint spaceLeft = logger->iDebugString.MaxLength() - stringLength;
       
    TUint16* ptr = const_cast<TUint16*>(logger->iDebugString.Ptr() + stringLength);      
    TPtr buffer(ptr, spaceLeft);       
    buffer.Copy(aText);
        
    logger->iDebugString.SetLength(stringLength + buffer.Length());
    logger->WriteLogRaw(logger->iDebugString);    
    }


void CVpnDebugLogger::LogWriteF(TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);


    CVpnDebugLogger* logger = VpnDebugLogger();
    logger->iDebugString.Zero();
    CVpnDebugLogger::TimeStamp(logger->iDebugString);

    if (logger->iCallDepth > 0)
        {        
        for (TUint i = 0; i < logger->iCallDepth; ++i)
            {
            logger->iDebugString.Append(_L(" "));
            }
        logger->iDebugString.Append(_L("  "));
        }
        
        
    const TDesC& format = aFmt;    
    
    logger->iDebugString.AppendFormatList(format, list);
	logger->WriteLogRaw(logger->iDebugString);
    }



void CVpnDebugLogger::LogWriteF(TRefByValue<const TDesC8> aFmt, ...)
    {
    TBuf8<512> buf;

    VA_LIST list;
    VA_START(list,aFmt);
    
    const TDesC8& format = aFmt;
    buf.FormatList(format, list);
    
    LogWrite(buf);    
    }


void CVpnDebugLogger::HexWrite(const TDesC8& aData)
    {
    const TUint KRowLength = 16; //16 bytes in one row
    
    TBuf<128> row;    
    
    TUint rowCount = aData.Length() / KRowLength;
    if (aData.Length() % KRowLength != 0)
        {
        rowCount++;
        }
    
    for (TInt i = 0; i < rowCount; ++i)
        {
        _LIT(KRowStartFormat, "%04x: ");
        TPtrC8 rowData(aData.Mid(KRowLength*i));
        
        row.Zero();        
        row.Format(KRowStartFormat, i*KRowLength);  
        TInt j;      
        for (j = 0; j < KRowLength; ++j)
            {                                    
            if ( j < rowData.Length())
                {
                _LIT(KDataByteFormat, "%02x ");
                TUint8 byte = rowData[j];
                row.AppendFormat(KDataByteFormat, byte);
                }
            else
                {
                row.Append(_L("   "));
                }
            }                

        for (j = 0; j < KRowLength && j < rowData.Length(); ++j)
            {
            TUint8 byte = rowData[j];
            if (byte >= ' ' && byte <= '~')
                {
                row.Append(byte);
                }
            else
                {
                row.Append(_L("."));
                }
            }                

        
        LogWrite(row);
        }           
    }

    
void CVpnDebugLogger::TimeStamp(TDes& aBuffer)
    {
    TTime time;
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    
    
    _LIT(KFormatTxt,"%02d/%02d/%d    %02d:%02d:%02d.%03d    ");
    aBuffer.Format(KFormatTxt,
                   dateTime.Day()+1, 
                   TInt(dateTime.Month()+1), 
                   dateTime.Year(),
                   
                   dateTime.Hour(), dateTime.Minute(), dateTime.Second(),
                   dateTime.MicroSecond()
                  );

    
    }


#endif //defined(_DEBUG)
