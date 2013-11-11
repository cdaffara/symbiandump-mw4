/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of TDmAdEventLog.
*
*/


#include <utf.h>
#include <barsread.h>
#include "vpnlogger.h"

//#include <vpnmanagementui.rsg>
//#include <avkon.rsg>

#include "dmadeventlog.h"

// "dd/mm/yyyy0"
const TInt KDmAdMaxLengthTextDateString = 11;               

void TDmAdEventLog::DeleteLogL(void)
    {
    TRACE("TDmAdEventLog::DeleteLogL");
    
    CEventViewer* eventViewer = CEventViewer::NewL();
    CleanupStack::PushL(eventViewer);
    User::LeaveIfError(eventViewer->DeleteLogFile());
    CleanupStack::PopAndDestroy(); //eventViewer
    }

void TDmAdEventLog::EventLogL(CBufBase& aEventLog)
    {
    
    TRACE("TDmAdEventLog::EventLogL");
/*
   _LIT(KResourceFileVpnLog, "\\resource\\vpnmanagementui.rsc");
   _LIT(KResourceFileAvkon, "\\resource\\avkon.rsc");
*/

    CEventViewer* eventViewer = 0;
    TRAPD(err, eventViewer = CEventViewer::NewL());
    if (err == KErrNotFound)
        {
        return;
        }
    User::LeaveIfError(err);
    CleanupStack::PushL(eventViewer);

   RFs fs;
   CleanupClosePushL(fs);
   User::LeaveIfError(fs.Connect());

   RResourceFile resourceFileVpnLog;
   CleanupClosePushL(resourceFileVpnLog);

   RResourceFile resourceFileAvkon;
   CleanupClosePushL(resourceFileAvkon);

   //OpenResourceFileL(fs, resourceFileVpnLog, KResourceFileVpnLog);
   //OpenResourceFileL(fs, resourceFileAvkon, KResourceFileAvkon);
   
    TEventProperties eventProperties;
    HBufC* eventText = NULL;
      
    TInt ret = eventViewer->GetMostRecentEvent(eventText, eventProperties);
    while (ret == KErrNone)
        {
        CleanupStack::PushL(eventText);

        TBuf<2 * KDmAdMaxLengthTextDateString + 3> timeBuf;
        FormatTimeL(resourceFileAvkon, timeBuf, eventProperties.iTimeStamp);
        
        HBufC* categoryText = CategoryTextLC(resourceFileVpnLog, eventProperties.iCategory);
        
        _LIT(KDmAdCr, "\n");
        _LIT(KDmAdSpace, " ");
        HBufC* messageText = HBufC::NewLC(timeBuf.Length() +
                                          KDmAdCr().Length() +
                                          categoryText->Length() +
                                          KDmAdSpace().Length() +
                                          eventText->Length() +
                                          KDmAdCr().Length());
        messageText->Des().Copy(timeBuf);
        messageText->Des().Append(KDmAdCr);
        messageText->Des().Append(*categoryText);
        messageText->Des().Append(KDmAdSpace); 
        messageText->Des().Append(*eventText);
        messageText->Des().Append(KDmAdCr);

        HBufC8* messageText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*messageText);
        CleanupStack::PushL(messageText8);
        aEventLog.InsertL(aEventLog.Size(), *messageText8);
                
        CleanupStack::PopAndDestroy(4); // messageText8, messageText, categoryText, eventText

        ret = eventViewer->GetPreviousEvent(eventText, eventProperties);
        }
    CleanupStack::PopAndDestroy(4); //resourceFileAvkon, resourceFileVpnLog, fs, eventViewer
    }

void TDmAdEventLog::FormatTimeL(RResourceFile& aResourceFile, TDes& aDateTimeText, TTime aTime)
    {
    TRACE("TDmAdEventLog::FormatTimeL");
    
    (void)aResourceFile;
   _LIT(KDmAdDateFromat, "%D%M%Y%/0%1%/1%2%/2%3%/3");
   _LIT(KDmAdTimeFromat, "%-B%:0%J%:1%T%:3%+B");

     // Date
   HBufC* dateFormat = KDmAdDateFromat().AllocLC();
    //HBufC* dateFormat = ReadResourceLC(aResourceFile, R_QTN_DATE_USUAL_WITH_ZERO);
    //HBufC* dateFormat = StringLoader::LoadLC(R_QTN_DATE_USUAL_WITH_ZERO);
    TBuf<KDmAdMaxLengthTextDateString> dateString;
    aTime.FormatL(dateString, *dateFormat);
    CleanupStack::PopAndDestroy(); //dateFormat

    // Time
    dateFormat = KDmAdTimeFromat().AllocLC();
    //dateFormat = ReadResourceLC(aResourceFile, R_QTN_TIME_USUAL_WITH_ZERO);
    //dateFormat = StringLoader::LoadLC(R_QTN_TIME_USUAL_WITH_ZERO);
    TBuf<KDmAdMaxLengthTextDateString> timeString;
    aTime.FormatL(timeString, *dateFormat);
    CleanupStack::PopAndDestroy(); //dateFormat
      
    TBuf<2 * KDmAdMaxLengthTextDateString + 3> timeBuf;

    /*
    TTime now;
    now.HomeTime();
    if ( (now.YearsFrom(aTime).Int() > 0) ||
        (aTime.DayNoInYear() < now.DayNoInYear()))
        {
        timeBuf.Append(dateString);
        _LIT(KDmAdSpaceHyphenSpace, " - ");
        timeBuf.Append(KDmAdSpaceHyphenSpace);
        }
    */

    timeBuf.Append(dateString);
    _LIT(KDmAdSpaceHyphenSpace, " - ");
    timeBuf.Append(KDmAdSpaceHyphenSpace);
        
    timeBuf.Append(timeString);
    aDateTimeText = timeBuf;
    }

HBufC* TDmAdEventLog::CategoryTextLC(RResourceFile& aResourceFile, TLogCategory2 aCategory)
    {
    (void)aResourceFile;
    HBufC* categoryText = NULL;
    _LIT(KDmAdInfo, "Information:");
    _LIT(KDmAdWarning, "Warning:");
    _LIT(KDmAdError, "Error:");
    
    if (aCategory == ELogInfo)
        {
        categoryText = KDmAdInfo().AllocLC();
        //categoryText = ReadResourceLC(aResourceFile, R_VPN_DETAIL_LOG_ENTRY_INFO);
        //categoryText = StringLoader::LoadLC(R_VPN_DETAIL_LOG_ENTRY_INFO);
        }
    else if (aCategory == ELogWarning)
        {
        categoryText = KDmAdWarning().AllocLC();
        //categoryText = ReadResourceLC(aResourceFile, R_VPN_DETAIL_LOG_ENTRY_WARNING);
        //categoryText = StringLoader::LoadLC(R_VPN_DETAIL_LOG_ENTRY_WARNING);
        }
    else // ELogError (or ELogDebug)
        {
        categoryText = KDmAdError().AllocLC();
        //categoryText = ReadResourceLC(aResourceFile, R_VPN_DETAIL_LOG_ENTRY_ERROR);
        //categoryText = StringLoader::LoadLC(R_VPN_DETAIL_LOG_ENTRY_ERROR);
        }
    return categoryText;
    }
    
void TDmAdEventLog::OpenResourceFileL(RFs& aFs, RResourceFile& aResourceFile, const TDesC& aFilename)
   {
   TRACE("TDmAdEventLog::OpenResourceFileL");
   
   TFileName resourceFileName;
   TFileName dllName;

   Dll::FileName(dllName);
   TBuf<2> drive = dllName.Left(2); // always z: for ... ?
   
   resourceFileName.Copy(drive);
   resourceFileName.Append(aFilename);
   
   aResourceFile.OpenL(aFs, resourceFileName);
   aResourceFile.ConfirmSignatureL();
   }

HBufC* TDmAdEventLog::ReadResourceLC(RResourceFile& aResourceFile, TInt aMsgId)
   {
   HBufC8* resourceBuf = aResourceFile.AllocReadLC(aMsgId);

   TResourceReader resourceReader;
   resourceReader.SetBuffer(resourceBuf);

   HBufC* textDataBuf = HBufC::NewL(resourceBuf->Length());
   resourceReader.Read((void*)textDataBuf->Ptr(), resourceBuf->Length());
   textDataBuf->Des().SetLength(resourceBuf->Length()/2);
   
   CleanupStack::PopAndDestroy(); // resourceBuf
   CleanupStack::PushL(textDataBuf);
   return textDataBuf;
   }
