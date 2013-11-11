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



#ifndef _EVENTLOGGER_H_
#define _EVENTLOGGER_H_

// INCLUDES
#include <eikenv.h>
#include <coeutils.h>
#include "eventmediatorapi.h"
#include "eventmediator.h"

_LIT(KEventLogFile,  "eventlog.bin");


// CLASS DECLARATION

/**
* Contains general information about a log event. When reporting a log event an object of this class
* Should be in the begining of the data descriptor.
*/
class TLogEvent
{
/*  public:
        enum TLogCategory
        {   EInfo,
            EWarning,
            EError,
            EDebug
        };
*/  
    public: // Methods
        /**
         * Standard constructors
         */
        TLogEvent(){};
        TLogEvent(TUid aSource, TLogCategory aCategory, TUint aMsgId, TInt aDesCount):iSource(aSource), iCategory(aCategory),iMsgId(aMsgId),iDesCount(aDesCount){};
    
    public: // Data
        // enumeration of log event categories
    
        // source of the log event
        TUid    iSource;
        // category of the log event
        TLogCategory iCategory;
        // id of the log message
        TUint iMsgId;
        // number of data descriptors related to this iMsgId
        TInt iDesCount;
    };






#define LOGFILE_MAX_LTH  20   // 20 kb 
#define KBYTES_TO_BYTES     1024
#define ELEM_HEADER_LTH     16
#define ELEM_TRAILER_LTH    12
//#define FILE_HEADER_LTH     16 NOTE! moved to eventmediatorapi.h
#define FILE_BEGIN          0    
#define END_MARK_1          0xefbeadde
#define END_MARK_2          0xeeabedfe
#define FILE_ID             0x474f4c45  // ELOG


// CLASS DECLARATION

/**
* Class used to write the log file.
*/
class CEventLogger : public CBase
{
    public: //Methods

    // Constructors and destructor

        /**
        * Default constructor, maximum length of log file set to LOGFILE_MAX_LTH
        */
        static CEventLogger* NewL(CEventMediatorServer* aServer);

        /**
        * Constructor
        * @param aFileMaxLength: maximum length of log file in kbytes
        */
        CEventLogger(TInt aFileMaxLength, CEventMediatorServer* aServer);

        /**
        * Destructor
        */
        ~CEventLogger();

        /**
        * Writes new event to the log file.
        * @param aEvent: descriptor containing instance of class TLogEvent 
        * and data descriptor of the event appended to it
        */
        void LogEvent(const TDesC8& aEvent);

    private: //Methods

        /**
        * Writes new event to the log file.
        * @param aInfo: general info on the event.
        * @param aData: event data.
        */
//      void    LogEvent(const TLogEvent& aInfo, const TDesC8& aData);   

        /**
        * Opens the log file for reading and writing.
        * @return: Etrue if Successfull, EFalse if not
        */    
        TBool   OpenLogFile();

        /**
        * Closes the log file.
        */    
        void    CloseLogFile();

        /**
        * Reads the file header from the log file to a descriptor.
        * @param aFileHdr: descriptor to which the header is read.
        * @return: Etrue if Successfull, EFalse if not
        */    
        TBool   GetFileHeader(TDes8 &aFileHdr);

        /**
        * Returns the position in the file to which a new element can be written.
        * @param aFileHdr: log file header.
        * @param aLength: length of the element about to be written.
        * @return: position to write the element to.
        */    
        TInt    GetElemDataPosition(TDes8 &aFileHdr, TInt aLength);   

        /**
        * Saves the log file header to the log file.
        * @param aPosition: the position to which new elements can be written.
        * @param aFileHdr: log file header.
        * @return: Etrue if Successfull, EFalse if not
        */    
        TBool   SaveFileHeader(TInt aPosition, TDes8 &aFileHdr);

        /**
        * Stores the log file header to a descriptor.
        * @param aPosition: the position to which new elements can be written.
        * @param aFirstOffset: wrapping offset
        * @param aBuf: log file header.
        */    
        void    StoreFileHeaderInfo(TInt aPosition, TInt aFirstOffset, TDes8 &aBuf);   

        /**
        * Writes new event to the log file.
        * @param aPosition: the position to write the element to.
        * @param aEvent: descriptor containning the element
        * @return: the total length of the written element.
        */
        TInt    WriteLogElem(TInt aPosition, const TDesC8& aEvent);

        /**
        * Gets the time and formats it into a TUint32 object.
        * @return: the time stamp as an unsigned integer.
        */
//        TUint32 GetTimeStamp();  

    private:
        /**
        * Default constructor.
        */
        CEventLogger(CEventMediatorServer* aServer);
    private: //Data
        // file server
        RFs        iFS;
    
        // log file
        RFile      iLogFile;      
    
        // log file max length
        TInt       iFileMaxLength;

        // event number, starting from 1
        TUint32    iEventNumber;

        CEventMediatorServer* iServer;
};


// CLASS DECLARATION

/**
* Contains log file header, basically two integers position and wrapping offset. 
*/
class TFileHeader
    {
    public: // Methods
        inline TInt32 GetPosition() const {return ((TInt32)(u.iData32[1]));};
        inline TInt32 GetWrappingOffset() const {return ((TInt32)(u.iData32[2]));};   
        inline TInt32 GetEventNumber() const {return ((TInt32)(u.iData32[3]));};   
        inline void SetFileId()  {u.iData32[0] = FILE_ID; };
        inline void SetPosition(TInt32 aPosition) {u.iData32[1] = (TUint32)aPosition;};
        inline void SetWrappingOffset(TInt32 aWO) {u.iData32[2] = (TUint32)aWO;};
        inline void SetEventNumber(TInt32 aEventNumber) {u.iData32[3] = (TUint32)aEventNumber;};

    private: // Data
        union // Currently union is not needed, only iData32 is used
        {
            TUint32 iData32[4];
            TUint8  iData8[8];
        } u;
    };



// CLASS DECLARATION

/**
* Contains log element header, consisting of four integer values:
*       length: length of the data beloging to the element
*       timestamp: the time the element was written.
*       source: Uid of the reporter of the element.
*       aCategory: the gategory of the log event.
*/
class TElemHeader
    {
    public: // Methods

        inline void SetEventLength(TUint32 aLength) {u.iData32[0] = (TUint32)aLength;};
        inline void SetEventNumber(TUint32 aEventNumber) {u.iData32[1] = (TUint32)aEventNumber;};
//     inline void SetTime(TUint32 aTimeStamp) {u.iData32[2] = (TUint32)aTimeStamp;};
        inline void SetTime(const TInt64& aTimeStamp) {u.iData32[2] = I64LOW(aTimeStamp); u.iData32[3] = I64HIGH(aTimeStamp);};

    private: // Data
        union 
        {
            TUint32 iData32[4];
            TUint8  iData8[8];
        } u;
    };


// CLASS DECLARATION

/**
* Contains a constant log element trailer, consisting of two integer values.
*/
class TElemTrailer
    {
    public: // Methods
        inline void SetLogEndMark() {u.iData32[0] = END_MARK_1;
                                     u.iData32[1] = END_MARK_2;};
        inline void SetEventLength(TUint32 aLength) {u.iData32[2] = (TUint32)aLength;};
    private: // Data
        union 
        {
            TUint32 iData32[3];
            TUint8  iData8[8];
        } u;
    };


#endif
