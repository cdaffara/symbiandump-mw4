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
* Description:   Event viewer
*
*/



#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>
#include <vpnlogmessages.rsg>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <utf.h>

#include "eventviewer.h"
#include "eventviewer2.h"
#include "log_eventviewer.h"

/////////////////////////////////////////////
// Method IDs
////////////////////////////////////////////
#define KGetMostRecentEvent           0
#define KGetNextEvent                 1
#define KGetPreviousEvent             2
#define KGetEventUsingEventNumber     3


//////////////////////////////////////////////////////////////
// Create EventViewer object
/////////////////////////////////////////////////////////////
EXPORT_C CEventViewer* CEventViewer::NewL()
    {
    CEventViewer *self = new (ELeave) CEventViewer();
    CleanupStack::PushL(self);
    self->ConstructL();
    LOG(Log::Printf(_L("Constructing EventViewer\n")));
    CleanupStack::Pop();        // self
    return self;
    }

/////////////////////////////////////////////////////////////
// CEventViewer::CEventViewer()
// C++ default constructor
/////////////////////////////////////////////////////////////
//
//  CEventViewer::CEventViewer() : iEikonEnv(*iCoeEnv)
//      {
//      }

//////////////////////////////////////////////////////////////
// Phase 2 constructor
/////////////////////////////////////////////////////////////
void CEventViewer::ConstructL()
    {
    TInt status = iFS.Connect();
    if (status != KErrNone)
        {
        LOG(Log::Printf(_L("iFS.Connect failed %d\n"), status));
        User::Leave(status);
        }

    status = iEventMediator.Connect();
    if (status != KErrNone)
        {
        LOG(Log::Printf(_L("iEventMediator.Connect failed %d\n"), status));
        User::Leave(status);
        }
    
    status = OpenLogFile();                             // Open log file and read file header
    if (status != KErrNone)
        {
        LOG(Log::Printf(_L("OpenLogFile failed %d\n"), status));
        User::Leave(status);
        }
    OpenResourceFileL();
    }
//////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////
CEventViewer::~CEventViewer()
    {
    LOG(Log::Printf(_L("Destructing EventViewer\n")));

    delete iLogFileBuf;
    iLogFileBuf = 0;
    CloseLogFile();
    delete iResultBuf;
    iResultBuf = 0;
    iResourceFile.Close();
    iFS.Close();
    iEventMediator.Close();
    }

//////////////////////////////////////////////////////////////
// OpenResourceFileL()
// Used in ConstructL to get resource file
//////////////////////////////////////////////////////////////
//
void CEventViewer::OpenResourceFileL()
   {
   //RResourceFile resourceFile;
   _LIT(KResourceFile, "\\resource\\vpnlogmessages.rsc");

   TFileName resourceFileName(KResourceFile);
   TFileName dllName;
   Dll::FileName(dllName);
   TBuf<2> drive = dllName.Left(2);
   resourceFileName.Insert(0, drive);
   iResourceFile.OpenL(iFS, resourceFileName);
   iResourceFile.ConfirmSignatureL();
   }

    
//////////////////////////////////////////////////////////////
// Release resources allocated for a call
/////////////////////////////////////////////////////////////
//void CEventViewer::ReleaseResourcesL()
//    {

//    }

//////////////////////////////////////////////////////////////////
//  
// GetMostRecentEvent
//
/////////////////////////////////////////////////////////////////
EXPORT_C TInt CEventViewer::GetMostRecentEvent( 
                             HBufC*&  aEventText,
                             TEventProperties& aEventProperties) 
    {
    TInt status = GetRequestedEvent(
                            aEventText,
                            aEventProperties,
                            0,         // Event number       
                            KGetMostRecentEvent);
    return status;
    }
//////////////////////////////////////////////////////////////////
//  
// GetNextEvent
//
/////////////////////////////////////////////////////////////////
EXPORT_C TInt CEventViewer::GetNextEvent( 
                                HBufC*&  aEventText,
                                TEventProperties& aEventProperties)
    {
    TInt status = GetRequestedEvent(
                             aEventText,
                             aEventProperties,
                             0,                           // Event number        
                             KGetNextEvent);
    return status;
    }

//////////////////////////////////////////////////////////////////
//  
// GetPrevousEvent
//
/////////////////////////////////////////////////////////////////
EXPORT_C TInt CEventViewer::GetPreviousEvent( 
                                         HBufC*&  aEventText,
                                         TEventProperties& aEventProperties)
    {
    TInt status = GetRequestedEvent(
                             aEventText,
                             aEventProperties,
                             0,                   // Event number 
                             KGetPreviousEvent);
    return status;
    }

//////////////////////////////////////////////////////////////////
//  
// GetEventUsingEventNumber
//
/////////////////////////////////////////////////////////////////
EXPORT_C TInt CEventViewer::GetEventUsingEventNumber( 
                                                HBufC*&  aEventText,
                                                TUint32   aEventNumber, 
                                                TEventProperties& aEventProperties)
    {
    TInt status = GetRequestedEvent(
                            aEventText,
                            aEventProperties,
                            aEventNumber, 
                            KGetEventUsingEventNumber);
    return status;
    }


//////////////////////////////////////////////////////////////////
//  
// GetRequestedEvent - Get Requested Event
// This is a common function for API calls. It searches the event
// that the API user has requested. The function returns a buffer
// containing the event text and TEventProperties structure that
// contains  some parameters relating to the event.
//
/////////////////////////////////////////////////////////////////
    TInt CEventViewer::GetRequestedEvent( 
                                          HBufC*&                 aEventText,
                                          TEventProperties&        aEventProperties,
                                          TUint32                  aEventNumber, 
                                          TInt                     aMethodId)
    {
    TInt status = 0;              
    TRAPD (err, status = GetRequestedEventL(
                                           aEventText,
                                           aEventProperties,
                                           aEventNumber, 
                                           aMethodId))
    
    if (err != KErrNone)
        {
        LOG(Log::Printf(_L("API request failed, status %d\n"), err));

        status = err;
        }
    return status;
    }
    
TInt CEventViewer::GetRequestedEventL( 
                                     HBufC*&                 aEventText,
                                     TEventProperties&        aEventProperties,
                                     TUint32                  aEventNumber, 
                                     TInt                     aMethodId)
    {
    TInt status;
    LOG(Log::Printf(_L("API request received, method =  %d\n"), aMethodId));

    //
    // Store the API parameters to this object
    //
    iEventText = &aEventText;
    iEventProperties = &aEventProperties;
    //
    // Produce the event number that the user is requesting
    //

    switch (aMethodId)
        {
        case KGetMostRecentEvent:
            {

            // Read the log file to memory buffer if not yet done

            if (iLogFileBuf == 0)
                {

                status = ReadWholeLogFileToMemoryBuffer();
                if (status != KErrNone)
                    {
                    break;
                    }
                TakeCopyOfFileHeader(iCurrFileHeader, iPreviousFileHeader);
                }

            // Check if the log file has been modified and read the log
            // file header

            else
                {
                status = ReadLogFileHeader();
                if (status != KErrNone)
                    {
                    break;
                    }
                if (IsLogFileModified())
                    {
                    TakeCopyOfFileHeader(iCurrFileHeader, iPreviousFileHeader);
                    }
                }
            iRequestedEventNumber = iCurrFileHeader.iCurrEventNumber;
            break;
            }
            
        case KGetNextEvent:
            {
            iRequestedEventNumber++;
            break;
            }
        case KGetPreviousEvent:
            {
            iRequestedEventNumber--;
            break;
            }
        case KGetEventUsingEventNumber:
            {
            iRequestedEventNumber = aEventNumber;
            break;
            }
        default:    
            {
            break;
            }
        }   
    //
    // Check the validity of requested event number
    //
    
    if (iRequestedEventNumber == 0 ||
        iRequestedEventNumber > iCurrFileHeader.iCurrEventNumber ||
        iLogFileBuf == 0)                // Logfile not yet in memory   
        {
        LOG(Log::Printf(_L("API request completed, requested event not found \n")));
        return KErrNotFound;
        }
    //
    // Search the event from the memory resident log file
    //
    status = EventLookup(iRequestedEventNumber);
    if (status != KErrNone)
        {
        return status;              // Event not found
        }
    //
    // Event found, copy the packed format log element parameters to an
    // unpacked object
    //

    CopyPackedLogElemToUnpackedObject(iPositionOfCurrLogElem);
                
    //
    // Search the event text relating to the MsgId
    //
    HBufC* eventTextBuf;
    eventTextBuf = SearchEventTextL(iUnpackedLogElem.iMsgId);
    if (eventTextBuf == 0)
        {
        return KErrNoMemory;
        }

    //
    // Modify the event text with the descriptor data
    // DescrPtr points to a list of following elements:
    //  TInt   DataLength
    //  TInt8  Data
    //

    TUint32 descrDataPosition = iPositionOfCurrLogElem + LOG_ELEM_HEADER_LTH; 
    HBufC* modifiedEventTextBuf = ModifyEventText(
                                     eventTextBuf,
                                    iUnpackedLogElem.iDescrCount,     // Count of descriptor elements
                                    descrDataPosition);               // Descriptors: lth,lth,...data,data...
    if (modifiedEventTextBuf == 0)
        {
        return KErrNoMemory;
        }
    //
    // An event is available, return data to the caller
    //
   *iEventText = modifiedEventTextBuf;           
    iResultBuf  = 0;
    iEventProperties->iEventNumber               = iUnpackedLogElem.iEventNumber;     
    iEventProperties->iMsgId                     = iUnpackedLogElem.iMsgId;
    TTime tmpTime(iUnpackedLogElem.iTimeStamp);  
    iEventProperties->iTimeStamp                 = tmpTime;
    iEventProperties->iSourceComponent           = iUnpackedLogElem.iSourceComponent;
    iEventProperties->iCategory                  = iUnpackedLogElem.iCategory;
    LOG(Log::Printf(_L("API request completed OK \n")));

    return KErrNone;

    }

    
    //////////////////////////////////////////////////////////////////////
    // EventLookup
    // This function searches an event from the log file buffer that
    // matches with the requested event number.
    //////////////////////////////////////////////////////////////////////
TInt CEventViewer::EventLookup(TUint32 aEventNumber)
    {
    TUint32 positionOfLogElemEnd = iCurrFileHeader.iPositionOfNextFree;
    TUint32 positionOfLogElem;
    TInt    searchGoing = ETrue;
    TInt status = KErrNone;              
    iWrappingOccured = EFalse;
    //
    // Loop here until matching event found or end of elements reached
    // or any error found
    //
    
    while (searchGoing && status == KErrNone)
        {
    //
    // Check if wrapping 1
    //
        if (positionOfLogElemEnd == EVENTLOG_FILE_HEADER_LTH)
            {


            if (iCurrFileHeader.iPositionOfWrapping != 0)
                {
                positionOfLogElemEnd = iCurrFileHeader.iPositionOfWrapping;  // Wrapping occurs
                iWrappingOccured = ETrue;
                }
            else
                {
                status = KErrNotFound;
                continue;
                }
            }

    //
    // Produce the start position of an event   
    //

        status = GetStartPositionOfLogElem( positionOfLogElemEnd,
                                            &positionOfLogElem);
        if (status != KErrNone)
            {
            continue;
            }

    //
    // Check if requested event has been found
    //

        if (iUnpackedLogElem.iEventNumber != aEventNumber)
            {
            positionOfLogElemEnd = positionOfLogElem;
            continue;                           // Not found, continue
            }
    //
    // Requested event found, stop the loop  
    //
        status = KErrNone;
        iPositionOfCurrLogElem = positionOfLogElem;
        searchGoing = EFalse;

        }

    return status;
    }

    
/////////////////////////////////////////////////////////////////////
// SearchEventText
// This function searches from the localization file the text string
// relating to the MsgId parameter and allocates an buffer in which
// it returns the data to the caller.
//
/////////////////////////////////////////////////////////////////////
    
HBufC*  CEventViewer::SearchEventTextL( TUint32 aMsgId)
    {
    HBufC8* resourceBuf = NULL;
    HBufC* textDataBuf = NULL;

    //
    // MsgId is a code defined in epoc32\include\eventviewererr.rsg
    // file. It refers to a message in .rsc file.
    //
    TInt msgNumber = STATIC_CAST(TInt, aMsgId); 
    resourceBuf = iResourceFile.AllocReadL( msgNumber);
    
    TResourceReader resourceReader;

    resourceReader.SetBuffer(resourceBuf);

    textDataBuf = HBufC::New(resourceBuf->Length());
    if (textDataBuf)
        {
        TPtr textDataPtr(textDataBuf->Des());

        resourceReader.Read((void*)textDataBuf->Ptr(), resourceBuf->Length());
        textDataPtr.SetLength(resourceBuf->Length()/2);
        }

    delete resourceBuf;
    return textDataBuf;
    }

/////////////////////////////////////////////////////////////////////
// ModifyEventText
// This function modifies the event text by the descriptors.
// Both the event text and descriptors are parameters of the
// function.
//
/////////////////////////////////////////////////////////////////////

HBufC*  CEventViewer::ModifyEventText(
                         HBufC*  aEventTextSrc,
                         TUint32 aDescrCount,         // Count of descriptor elements
                         TUint32 aDescrDataPosition)  // Descriptors: lth data lth data.... 
    {
    HBufC* modifiedEventText;

    //
    // If descriptors exist, build pointer to the first
    // length definition. The format of descriptor data
    // is as follows:
    //   -- TUInt32 Length of parameter 1 (lth1)
    //   -- TUInt32 Length of parameter 2 (lth2)
    //   -- TUInt32 Length of parameter n (lthx)
    //   -- TInt8[lth1] Data 1
    //   -- TInt8[lth2] Data 2
    //   -- TInt8[lthx] Data n
    //              Variable length data, The format of 
    //              data is defined by the conversion
    //              specification characters in the text string
    //

    //
    // No descriptors, return localisation data  buffer
    //
    if (aDescrCount == 0)
        {
        return aEventTextSrc;
        }

    //
    // Build TPtr8 pointer for descriptor list
    //
    
    TInt descrLth = iUnpackedLogElem.iEventLength -
                           LOG_ELEM_HEADER_LTH -
                           LOG_ELEM_TRAILER_LTH;
    TPtr8 logFileBuf (iLogFileBuf->Des()); // Log file in memory
    TPtr8 descrListPtr (const_cast<TUint8*>(logFileBuf.Ptr())+ aDescrDataPosition, // Data ptr
                       descrLth,   // Data length
                       descrLth);  // Max length
    //
    // Modify the localisation data buffer with  the descriptor data 
    //
    modifiedEventText = FormatEvent(
                                 aDescrCount,
                                 descrListPtr,
                                 aEventTextSrc->Des());


    delete aEventTextSrc;
    return modifiedEventText;
    }


/////////////////////////////////////////////////////////////////////
// GetIapName
// These functions are used to convert an IAP ID to IAP name.
//
////////////////////////////////////////////////////////////////////
EXPORT_C TInt EventViewer::GetIapName(TUint32 aIapId, TIapName& aIapName)
    {
    TRAPD(err, DoGetIapNameL(aIapId, aIapName));
    return err;
    }

EXPORT_C TInt EventViewer::GetSnapName(TUint32 aSnapId, TIapName& aSnapName)
    {
    TRAPD(err, DoGetSnapNameL(aSnapId, aSnapName));
    return err;
    }

EXPORT_C TInt EventViewer::GetIapNames(TUint32 aIapId1, TIapName& aIapName1,
                                        TUint32 aIapId2, TIapName& aIapName2)
    {
    TRAPD(err, DoGetIapNamesL(aIapId1, aIapName1, aIapId2, aIapName2));
    return err;
    }

void EventViewer::DoGetIapNameL(TUint32 aIapId, TIapName& aIapName)
    {
    using namespace CMManager;
    
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL(cmManagerExt);
    
    RCmConnectionMethodExt connectionMethod = cmManagerExt.ConnectionMethodL( aIapId );
    CleanupClosePushL(connectionMethod);
    
    HBufC* cmName = connectionMethod.GetStringAttributeL(ECmName); // Ownership passed
    
    aIapName.Copy(*cmName);
    delete cmName;
    cmName = NULL;
    
    CleanupStack::PopAndDestroy(&connectionMethod);
    CleanupStack::PopAndDestroy(&cmManagerExt);
    }

void EventViewer::DoGetIapNamesL(TUint32 aIapId1, TIapName& aIapName1,
                                  TUint32 aIapId2, TIapName& aIapName2)
    {
    using namespace CMManager;
    
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL(cmManagerExt);
    
    RCmConnectionMethodExt connectionMethod1 = cmManagerExt.ConnectionMethodL( aIapId1 );
    CleanupClosePushL(connectionMethod1);
    
    HBufC* cmName1 = connectionMethod1.GetStringAttributeL(ECmName);
    CleanupStack::PushL(cmName1);
    
    aIapName1.Copy(*cmName1);

    RCmConnectionMethodExt connectionMethod2 = cmManagerExt.ConnectionMethodL( aIapId2 );
    CleanupClosePushL(connectionMethod2);
    
    HBufC* cmName2 = connectionMethod2.GetStringAttributeL(ECmName); // Ownership passed
    
    aIapName2.Copy(*cmName2);
    
    delete cmName2;
    cmName2 = NULL;
    CleanupStack::PopAndDestroy(4);
    }


void EventViewer::DoGetSnapNameL(TUint32 aSnapId, TIapName& aSnapName)
    {
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL(cmManagerExt);
    
    RCmDestinationExt destination = cmManagerExt.DestinationL( aSnapId );
    CleanupClosePushL(destination);
    
    HBufC* snapName16bit = destination.NameLC();            
    CnvUtfConverter::ConvertFromUnicodeToUtf8(aSnapName, *snapName16bit);    
    CleanupStack::PopAndDestroy(snapName16bit);
    
    CleanupStack::PopAndDestroy(); //destination
    CleanupStack::PopAndDestroy(); //cmManagerExt
    }

/////////////////////////////////////////////////////////////////////
// DeleteLogFile
// This function deletes the eventlog.bin file
//
////////////////////////////////////////////////////////////////////
EXPORT_C TInt CEventViewer::DeleteLogFile()
    {
    TRAPD(err, DoDeleteLogFileL());
    return err;
    }
