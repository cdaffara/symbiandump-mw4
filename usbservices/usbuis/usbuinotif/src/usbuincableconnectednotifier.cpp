/*
 * Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  USB Cable Connected notifier class.
 *
 */
// INCLUDE FILES
#include <utf.h>             // Unicode character conversion utilities
#include <usbman.h>
#include <centralrepository.h> 
#include <e32property.h>
#include <e32uid.h> // KExecutableImageUid
#include <hb/hbwidgets/hbdevicenotificationdialogsymbian.h>
#include <hb/hbcore/hbtextresolversymbian.h>
#include "usbuincableconnectednotifier.h"   // Own class definition
#include "usbuinotifdebug.h"                // Debugging macros
#include "UsbWatcherInternalCRKeys.h"
#include "usbuinotif.hrh"

// Literals

_LIT(KUSBExe, "USBSettingsApp");
const TInt KUSBUIUid = 0x2002E70C;
const TInt KTextIdLength = 40;

_LIT(KUSBUIconFileName, "qtg_large_usb");

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUSBUICableConnectedNotifier* CUSBUICableConnectedNotifier::NewL()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::NewL"));
    CUSBUICableConnectedNotifier* self =
            new (ELeave) CUSBUICableConnectedNotifier();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::NewL completed"));
    return self;
    }

// ----------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::CUSBUICableConnectedNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUSBUICableConnectedNotifier::CUSBUICableConnectedNotifier()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::default constructor"));
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUSBUICableConnectedNotifier::~CUSBUICableConnectedNotifier()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::destructor"));
    //Make sure that the request is completed. Note that inside the destructor,
    //this virtual function call is to the local CUSBUICableConnectedNotifier::
    //Cancel, not to any possibly derived class implementation. 
    Cancel();
           
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::destructor completed"));
    }

// ----------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::RegisterL
// Register notifier.
// ----------------------------------------------------------------------------
//
CUSBUICableConnectedNotifier::TNotifierInfo CUSBUICableConnectedNotifier::RegisterL()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::RegisterL"));
    iInfo.iUid = KCableConnectedNotifierUid;
    iInfo.iChannel = KQueriesNotifier; // work in the same channel with the other 
    // queries so that we can use priorities
    iInfo.iPriority = ENotifierPriorityLow; // must be smaller than queries notifier
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::RegisterL completed"));
    return iInfo;
    }

// ----------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::StartDialogL
// ----------------------------------------------------------------------------
//
void CUSBUICableConnectedNotifier::StartDialogL(const TDesC8& /*aBuffer*/,
        TInt aReplySlot, const RMessagePtr2& aMessage)
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::StartDialogL"));
    if ( iReplySlot != 0 || iNeedToCompleteMessage)
        {
        User::Leave(KErrInUse);
        }

    InitializeTextResolver();

    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;
    if (iDialog)
        {
        FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::StartDialogL() deleting previous dialog"));
        delete iDialog;
        iDialog = NULL;
        }
    iDialog = CHbDeviceNotificationDialogSymbian::NewL(this);
      
    HBufC* header = NULL;
    HBufC* description =NULL;   
    GetPersonalityStringLC(header, description);
   
    iDialog->SetTitleL(*header);
    iDialog->SetTextL(*description);
    iDialog->SetIconNameL(KUSBUIconFileName );
    iDialog->EnableTouchActivation(ETrue);
    iDialog->ShowL();
    
    CleanupStack::PopAndDestroy(header);
    CleanupStack::PopAndDestroy(description);
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::StartDialogL completed"));
    }

// ----------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::Cancel()
// Release all own resources (member variables)
// ----------------------------------------------------------------------------
//
void CUSBUICableConnectedNotifier::Cancel()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::Cancel() "));
    if (iDialog)
        {
        iDialog->Close();
        delete iDialog;
        iDialog = NULL;
        }
    CompleteMessage(KErrCancel);
    CUSBUINotifierBase::Cancel();

    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::Cancel() completed"));
    }

// -----------------------------------------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::GetPersonalityStringL
// Get the strings for the discreet popup dialog
// -----------------------------------------------------------------------------------------------------------
//
void CUSBUICableConnectedNotifier::GetPersonalityStringLC(
       HBufC*& aHeader,HBufC*& aDescription )
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::GetPersonalityStringL"));
    _LIT(KFixedPart, "txt_usb_dpinfo_");
    _LIT(KSpace, " ");
    _LIT(KUnderline, "_");
    _LIT(KUsbConnected, "txt_usb_dpophead_usb_connected");
    TInt CurrentPersonalityID = 0; // The ID of current USB mode
    GetCurrentIdL(CurrentPersonalityID);
    RUsb usbman;
    User::LeaveIfError(usbman.Connect());
    FLOG(_L("[USBUINOTIF]\t usbman connect called"));     
    CleanupClosePushL(usbman);
    HBufC* description = NULL;
    User::LeaveIfError(usbman.GetDescription(CurrentPersonalityID,
            description));
     FLOG(_L("[USBUINOTIF]\t get description called"));  
    CleanupStack::PopAndDestroy(&usbman);
    
    CleanupStack::PushL(description);
    TPtr descriptionPtr = description->Des();
    TInt position = descriptionPtr.Find(KSpace);
    while (position != KErrNotFound)
        {
        descriptionPtr.Replace(position, 1, KUnderline);     
        position = descriptionPtr.Find(KSpace); 
        }
    HBufC* textId = HBufC::NewL( KTextIdLength );
    CleanupStack::PushL(textId);
    TPtr textIdPtr = textId->Des();
    textIdPtr.Append(KFixedPart);
    textIdPtr.Append(descriptionPtr);  

    aDescription = HbTextResolverSymbian::LoadL(textIdPtr);
    CleanupStack::PopAndDestroy(textId);
    CleanupStack::PopAndDestroy(description);
    CleanupStack::PushL(aDescription);

    FLOG(_L("[USBUINOTIF]\t get description called"));  
    aHeader = HbTextResolverSymbian::LoadL(KUsbConnected);
    FLOG(_L("[USBUINOTIF]\t get header called"));  
 
    CleanupStack::PushL(aHeader);
    
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::GetPersonalityStringL completed"));
    }

// ----------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::GetCurrentIdL
// get the current personality id
// ----------------------------------------------------------------------------
//
void CUSBUICableConnectedNotifier::GetCurrentIdL(TInt& aCurrentPersonality)
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::GetCurrentIdL"));
    // Connecting and initialization:
    CRepository* repository = CRepository::NewL(KCRUidUsbWatcher);
    CleanupStack::PushL(repository);
    // Get the current USB mode
    TInt ret = repository->Get(KUsbWatcherPersonality, aCurrentPersonality);
    FTRACE( FPrint(
        _L( "[USBWATCHER]\t CUSBUICableConnectedNotifier::GetCurrentIdL() ret = %d" ),
        ret ) );    
    CleanupStack::PopAndDestroy(repository);
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::GetCurrentIdL completed"));
    }



void CUSBUICableConnectedNotifier::NotificationDialogActivated(
        const CHbDeviceNotificationDialogSymbian* /*aDialog*/)
       {
       
           FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::NotificationDialogActivated()"));
           TUidType uidtype(KExecutableImageUid, TUid::Uid(0x00),
                                   TUid::Uid(KUSBUIUid));
           iDialog->Close();
           LaunchApplication(KUSBExe(), uidtype);
         
       }
   
void CUSBUICableConnectedNotifier::NotificationDialogClosed(
        const CHbDeviceNotificationDialogSymbian* /*aDialog*/, TInt /*aCompletionCode*/)
       {
       FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::NotificationDialogClosed()"));
       CompleteMessage( KErrCancel ); 
       FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::NotificationDialogClosed() complete"));

       }
// ---------------------------------------------------------------------------
// CUSBUICableConnectedNotifier::LaunchApplication()
// launches the application 
// ---------------------------------------------------------------------------
//  
void CUSBUICableConnectedNotifier::LaunchApplication(const TDesC & aProcessName, 
        const TUidType & aUidType) const
    {                 
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::LaunchApplication()"));
    RProcess usbUiProcess;                
    TInt result = usbUiProcess.Create(aProcessName, KNullDesC, aUidType);
    FTRACE( FPrint(
        _L( "[USBUINOTIF]\t CUSBUICableConnectedNotifier::LaunchApplication() result = %d" ),
        result ) );    
    usbUiProcess.Resume();
    usbUiProcess.Close();                     
    FLOG(_L("[USBUINOTIF]\t CUSBUICableConnectedNotifier::LaunchApplication() completed"));             
    }

// End of File
