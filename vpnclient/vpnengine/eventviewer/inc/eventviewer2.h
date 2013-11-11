/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Log File Handler.
*
*/


#ifndef __EVENTVIEWER2__
#define __EVENTVIEWER2__

#include <gdi.h>
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>

////////////////////////////////////////////////////////
// Literals
////////////////////////////////////////////////////////
#include "eventmediatorapi.h"

#define LOG_ELEM_TRAILER_LTH  12
#define LOG_ELEM_HEADER_LTH   32
#define KBYTES_TO_BYTES     1024
//efine ELEM_HEADER_LTH     8
#define FILE_BEGIN          0    
#define END_MARK_1          0xefbeadde
#define END_MARK_2          0xeeabedfe
#define LOG_FILE_ID         0x474F4C45    // ELOG


/////////////////////////////////////////////////////////////////
//  Enumeration of log event categories.
////////////////////////////////////////////////////////////////

enum TLogCategory2
   {
   ELogInfo,
   ELogWarning,
   ELogError,
   ELogDebug
   };


/////////////////////////////////////////////////////////
// TEventProperties - data to be returned with the event
// text to the caller
/////////////////////////////////////////////////////////
struct TEventProperties

{
    TUint32       iSourceComponent; // Uid of source component
    TTime         iTimeStamp;       // 64 bit time stamp                           
    TLogCategory2 iCategory;        // Info, Warning, Error, Debug 
    TUint32       iMsgId;           // MsgId in localization file
    TUint32       iEventNumber;     // Event number
};


/////////////////////////////////////////////////////////
// Packed File Header
/////////////////////////////////////////////////////////
class TFileHeader
{
    public: // Methods
        inline TInt32 GetFileId()                const {return ((TInt32)(u.iData32[0]));};
        inline TInt32 GetPositionOfNextFree()    const {return ((TInt32)(u.iData32[1]));};
        inline TInt32 GetPositionOfWrapping()    const {return ((TInt32)(u.iData32[2]));};
        inline TInt32 GetCurrEventNumber()       const {return ((TInt32)(u.iData32[3]));};

    private: 
        union
            {
            TUint32 iData32[4];
            } u;
};



/////////////////////////////////////////////////////////
// Unpacked File Header
/////////////////////////////////////////////////////////

class TUnpackedFileHeader
    {
    public:
        TUint32                 iFileId;    // ELOG
        TUint32                 iPositionOfNextFree;
        TUint32                 iPositionOfWrapping;
        TUint32                 iCurrEventNumber;
    };

/////////////////////////////////////////////////////////
// Packed Log Element
///////////////////////////////////////////////////////
class TLogElem
    {
    public: // Methods
    inline TInt32        GetEventLength()           const {return ((TInt32)(u.iData32[0]));};
    inline TInt32        GetEventNumber()           const {return ((TInt32)(u.iData32[1]));};
    inline TInt64        GetTimeStamp()             const
        {
        return MAKE_TINT64(u.iData32[3], u.iData32[2]);;
        };
    inline TInt32        GetSourceComponent()       const {return ((TInt32)(u.iData32[4]));};
    inline TLogCategory2 GetCategory()              const {return ((TLogCategory2)(u.iData32[5]));};
    inline TInt32        GetMsgId()                 const {return ((TInt32)(u.iData32[6]));};
    inline TInt32        GetDescrCount()            const {return ((TInt32)(u.iData32[7]));};

private: 
    union
        {
        TUint32 iData32[8];
        } u;
    };

/////////////////////////////////////////////////////////
// Unpacked Log Element         
/////////////////////////////////////////////////////////

class TUnpackedLogElem
{
    public:
        TUint32                 iEventLength;             
        TUint32                 iEventNumber;             
        TInt64                  iTimeStamp;               
        TUint32                 iSourceComponent;         
        TLogCategory2           iCategory;                
        TUint32                 iMsgId;                   
        TUint32                 iDescrCount;              
};

/////////////////////////////////////////////////////////
// Packed Log Element trailer
/////////////////////////////////////////////////////////
class TLogElemTrailer
{
    public: // Methods
        inline TInt32 GetEndMark1()              const {return ((TInt32)(u.iData32[0]));};
        inline TInt32 GetEndMark2()              const {return ((TInt32)(u.iData32[1]));};
        inline TInt32 GetEventLength()           const {return ((TInt32)(u.iData32[2]));};

    private: 
        union
            {
            TUint32 iData32[3];
            } u;
};

/////////////////////////////////////////////////////////
// Unpacked Element trailer
/////////////////////////////////////////////////////////

class TUnpackedLogElemTrailer
{
    public:
        TUint32                 iEndMark1;
        TUint32                 iEndMark2;               
        TUint32                 iEventLength;           
};


/////////////////////////////////////////////////////////
// EventViewer  Class
/////////////////////////////////////////////////////////
class CEventViewer : public CBase
{


    //
    // EventViewer API in EventViewer.cpp
    //
    public: 
        IMPORT_C static CEventViewer* NewL();
        IMPORT_C ~CEventViewer();
        void ConstructL();

        IMPORT_C TInt GetMostRecentEvent (HBufC*&                            aEventText,
                                          TEventProperties&                   aEventProperties);

        IMPORT_C TInt GetNextEvent       (HBufC*&                            aEventText,
                                          TEventProperties&                   aEventProperties);

        IMPORT_C TInt GetPreviousEvent   (HBufC*&                            aEventText,
                                          TEventProperties&                   aEventProperties);

        IMPORT_C TInt GetEventUsingEventNumber(
                                         HBufC*&                            aEventText,
                                         TUint32                             aEventNumber,        
                                         TEventProperties&                   aEventProperties);

        IMPORT_C TInt GetOldestEvent     (HBufC*&                            aEventText,
                                          TEventProperties&                   aEventProperties);

        IMPORT_C TInt DeleteLogFile();

    //
    // EventViewer internal functions in EventViewer.cpp
    //
    private: 
        TInt GetRequestedEvent( HBufC*&              aEventText,
                                TEventProperties&     aEventProperties,
                                TUint32               aEventNumber,
                                TInt                  aMethodId);

        TInt GetRequestedEventL( HBufC*&              aEventText,
                                TEventProperties&     aEventProperties,
                                TUint32               aEventNumber,
                                TInt                  aMethodId);

    //
    // EventViewer internal functions in LogFileHandler.cpp
    //
    private:
        TInt OpenLogFile();
        void CloseLogFile();
        void TakeCopyOfFileHeader(TUnpackedFileHeader& asrc,
                                  TUnpackedFileHeader& adest);
        void OpenResourceFileL();
        
        TInt ReadWholeLogFileToMemoryBuffer();
        TInt ReadLogFileHeader();
        TInt CopyPackedFileHeaderToUnpackedObject( TDesC8& aFileHeader);
        TBool IsLogFileModified();
        TInt EventLookup(TUint32 aEventNumber);
        TInt GetStartPositionOfLogElem(TUint32 aEndOfLogElemPosition,    
                                       TUint32* aLogElemPosition);       
        void CopyPackedLogElemToUnpackedObject(TUint32 iPositionOfCurrLogElem);
        HBufC*  SearchEventTextL( TUint32 aMsgId);
        HBufC*  ModifyEventText(HBufC* aEventTextSrc,
                                 TUint32 aDescrCount,         // Count of descriptor elements
                                 TUint32 aDescrDataPosition); // Descriptors: lth data lth data.... 
        HBufC*   FormatEvent(TInt aDescrCount,                // Count of lth/data pairs 
                             const TDesC8&  aDescrList,       // lth lth ... data data ... 
                             const TDesC&   aEventString);    // string got from localisation file
        TInt     AnalyseConvSpec(
                                 const TDesC& SourceBuf,     //    
                                 TInt&     aOffset,         // offset of conversion spec element
                                 TUint32&  aConvType,       // U or N (KConvTypex)
                                 TUint32&  aSeqNumber);     // Seq number included to conv spec elem
        HBufC*  GetDescriptorData (
                                   TUint32 aDescrCount,         // Count of lth/data pairs
                                   const TDesC8& aDescrList,    // lth lth ... data data ...
                                   TUint32 aConvType,           // KConvTypeN or KConvTypeU
                                   TUint32 aDescrNumber);       // Seq number of lth/data pair
        TInt    BufferAppend(HBufC*& aDestBuf,
                             const TDesC& aString);

        void    DoDeleteLogFileL();                             // In LogFileHandler.cpp

    private:
        RFs                         iFS;
        REventMediator              iEventMediator;
        TUnpackedFileHeader         iCurrFileHeader;
        TUnpackedFileHeader         iPreviousFileHeader;
        TUnpackedLogElem            iUnpackedLogElem;
        TUnpackedLogElemTrailer     iLogElemTrailer;
        TUint32                     iRequestedEventNumber;
        HBufC8*                     iLogFileBuf;
        TInt                        iLogFileSize;
        TInt                        iWrappingOccured;
        TUint32                     iPositionOfCurrLogElem;
        RResourceFile               iResourceFile;
        HBufC*                      iResultBuf;    // Formatted event is build to this buffer 
        
    //
    // Current API call parameters
    //
    HBufC** iEventText;
    TEventProperties* iEventProperties;
};
#endif