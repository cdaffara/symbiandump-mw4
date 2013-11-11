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
 * Description:  Declares USB UI notifiers base class.
 *
 */

// INCLUDE FILES

#include <bautils.h>         // BAFL utils (for language file)
#include <data_caging_path_literals.hrh> 
#include <hb/hbcore/hbtextresolversymbian.h>

#include "usbnotifier.h"     // Own class 
#include "usbuinotifdebug.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::CBTNotifierBase
// C++ default constructor can NOT contain any code, that
// might leave. Sets the AOs priority and puts
// itself to the active scheduler stack.
// ----------------------------------------------------------------------------
//
CUSBUINotifierBase::CUSBUINotifierBase() 
    {
        FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::Default constructor()"));
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::ConstructL
// Symbian 2nd phase constructor can leave.
// Create registry object and open resource file.
// ----------------------------------------------------------------------------
//
void CUSBUINotifierBase::ConstructL()
    {
    iEikEnv = CEikonEnv::Static();
   
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::ConstructL()"));
        
    // use TLS as a boolean, set to EFalse
    TBool initialized = EFalse;
    TInt error;
    error = Dll::SetTls((TAny* )initialized); 
    error = error; // to suppress a compiler warning
    FTRACE(FPrint(_L("[USBUINOTIF]\t Dll:SetTls returned %d"), error ));       
   
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::ConstructL() completed"));
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CUSBUINotifierBase::~CUSBUINotifierBase()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::Destructor"));
    //Make sure that the request is completed. Note that inside the destructor,
    //this virtual function call is to the local CUSBUINotifierBase::Cancel, 
    //not to any possibly derived class implementation. 
    Cancel();
 
    // Complete the RMessage2 if needed
    //
    CompleteMessage( KErrDied );

    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::Destructor completed"));
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::Release
// Release itself. Call to destructor.
// ----------------------------------------------------------------------------
//
void CUSBUINotifierBase::Release()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::Release()"));
    delete this;
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::Info
// Return registered information.
// ----------------------------------------------------------------------------
//
CUSBUINotifierBase::TNotifierInfo CUSBUINotifierBase::Info() const
    {
    FTRACE(FPrint(_L("[USBUINOTIF]\t CUSBUINotifierBase::Info() id %d channel %d priority %d"), iInfo.iUid, iInfo.iChannel, iInfo.iPriority ));
    return iInfo;
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::StartL
// Synchronic notifier launch. Does nothing
// ----------------------------------------------------------------------------
//
TPtrC8 CUSBUINotifierBase::StartL(const TDesC8& /*aBuffer*/)
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::StartL()"));

    TPtrC8 ret( KNullDesC8 );
    return (ret);
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::StartL
// Asynchronic notifier launch.
// ----------------------------------------------------------------------------
//
void CUSBUINotifierBase::StartL(const TDesC8& aBuffer, TInt aReplySlot,
        const RMessagePtr2& aMessage)
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::StartL()"));
    TRAPD( err, StartDialogL( aBuffer, aReplySlot, aMessage ));
    if (err)
        {
        aMessage.Complete( err );
        iNeedToCompleteMessage = EFalse;
        User::Leave( err );
        }
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::StartL completed()"));
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::Cancel
// Cancelling method.
// ----------------------------------------------------------------------------
//
void CUSBUINotifierBase::Cancel()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::Cancel()"));
    //The message box closed callback gets not run in subclass cancel calls.
    CompleteMessage(KErrCancel);
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::Cancel completed"));
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::UpdateL
// Notifier update. Not supported.
// ----------------------------------------------------------------------------
//
TPtrC8 CUSBUINotifierBase::UpdateL(const TDesC8& /*aBuffer*/)
    {
    TPtrC8 ret( KNullDesC8 );
    return (ret);
    }

// ----------------------------------------------------------------------------
// CUSBUINotifierBase::CompleteMessage
// Check if message needs to be completed and complete it. 
// ----------------------------------------------------------------------------
//
void CUSBUINotifierBase::CompleteMessage(TInt aReason)
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::CompleteMessage()"));
    if (iNeedToCompleteMessage)
        {
        iMessage.Complete( aReason );
        iNeedToCompleteMessage = EFalse;
        }
    iReplySlot = 0;
    FLOG(_L("[USBUINOTIF]\t CUSBUINotifierBase::CompleteMessage() completed"));
    }

void CUSBUINotifierBase::InitializeTextResolver()
    {
    _LIT(KFileName, "usbdialogs_");
    _LIT(KPath, "z:/resource/qt/translations/");         
    TInt error = KErrNone;
        
    iTranslator = (TBool )Dll::Tls();
    
    FTRACE(FPrint(_L("[USBUINOTIF]\t Dll:Tls returned %d"), iTranslator ));    
    
    if (!iTranslator)
        {
        iTranslator = HbTextResolverSymbian::Init(KFileName, KPath);
        FTRACE(FPrint(_L("[USBUINOTIF]\t HbTextResolverSymbian::Init returned %d"), iTranslator ));    
        if (iTranslator)
            {
            error = Dll::SetTls((TAny* )iTranslator);      
            }
        FTRACE(FPrint(_L("[USBUINOTIF]\t Dll:SetTls returned %d"), error ));    
        }    
       
     error = error;   // to suppress a compiler warning
     FTRACE( FPrint(
        _L( "[USBUINOTIF]\t CUSBUINotifierBase::InitializeTextResolver result = %d" ),
        iTranslator ) );                     
        
    }


// End of File
