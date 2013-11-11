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
* Description: Header declaration
*
*/








#ifndef __SENNOTPLUGINDLGIMPL__
#define __SENNOTPLUGINDLGIMPL__

// INCLUDES
#include <eiknotapi.h> // MEikSrvNotifierBase

#include "sennotplugindlg.h"

// FORWARD DECLARATIONS
class CAknDialog;
class TAuthenticationDlgRequest;
class TAuthenticationDlgResponse;
class TAllowRegisterDlgRequest;
class TAllowRegisterDlgResponse;
class TAllowUnRegisterDlgRequest;
class TAllowUnRegisterDlgResponse;

// CLASS DECLARATION
#ifdef EKA2
IMPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();
class CSenNotPluginDlgImpl : public CActive, public MEikSrvNotifierBase2
#else
IMPORT_C CArrayPtr<MEikSrvNotifierBase>* NotifierArray();
class CSenNotPluginDlgImpl : public CActive, public MEikSrvNotifierBase
#endif
    {
    public: // Constructors and destructor

        static CSenNotPluginDlgImpl* NewL();

        ~CSenNotPluginDlgImpl();

        // Functions from base classes

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

        // From MEikSrvNotifierBase
        /**
        * Called when all resources allocated by notifiers shoudl be freed.
        */
        virtual void Release();
    
        /**
        * Called when a notifier is first loaded to allow any initial construction that is required.
        */
        #ifdef EKA2
            virtual MEikSrvNotifierBase2::TNotifierInfo RegisterL();
        #else
            virtual MEikSrvNotifierBase::TNotifierInfo RegisterL();
        #endif
        
        /**
        * @return the priority a notifier takes and the channels it acts on.  The return value may be varied
        * at run-time.
        */
        #ifdef EKA2
            virtual MEikSrvNotifierBase2::TNotifierInfo Info() const;
        #else
            virtual MEikSrvNotifierBase::TNotifierInfo Info() const;
        #endif
    
        /**
        * Start the notifier with data aBuffer and return an initial response.
        */
        virtual TPtrC8 StartL(const TDesC8& aBuffer);
    
        /**
        * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
        * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
        * responsible for completing aMessage.
        */
        #ifdef EKA2
            virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
        #else
            virtual void StartL(const TDesC8& aBuffer, const TAny* aReturnVal, RMessage aMessage);
        #endif
    
        /**
        * The notifier has been deactivated so resources can be freed and outstanding messages completed.
        */
        virtual void Cancel();

        /**
        * Update a currently active notifier with data aBuffer.
        */
        virtual TPtrC8 UpdateL(const TDesC8& aBuffer);

    private: 
        
        /**
        * C++ default constructor.
        */
        CSenNotPluginDlgImpl();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions

        // Helpers
        void HandleAsyncRequestL();
        TInt ShowAuthenticationDialogL(TAuthenticationDlgRequest* aRequest,
                                       TAuthenticationDlgResponse* aResponse);

        TInt ShowAllowRegisterDialogL(TAllowRegisterDlgRequest* aRequest,
                                     TAllowRegisterDlgResponse* aResponse);

        TInt ShowAllowUnRegisterDialogL(TAllowUnRegisterDlgRequest* aRequest,
                                        TAllowUnRegisterDlgResponse* aResponse);

        TInt ShowAskSaveDialogL(TAskSaveDlgRequest* aRequest,
                                TAskSaveDlgResponse* aResponse);

        void Cleanup();

    private: // Data
        TInt iMethod; // See TSenNotPluginRequest::TSenNotPluginMethod
        #ifdef EKA2
            RMessagePtr2    iMessage;
            TInt            iReplySlot;
            MEikSrvNotifierBase2::TNotifierInfo  iInfo;
        #else
            RMessage        iMessage;
            const TAny*     iReplyPtr;
            MEikSrvNotifierBase::TNotifierInfo  iInfo;
        #endif
        TPckgBuf<TSenNotPluginRequest>* iRequest;
        TPckgBuf<TSenNotPluginResponse>* iResponse;
        // Valid during showing a dialog
        CAknDialog* iDialog; 
        TBool iCancelled;
    };

#endif // __SENNOTPLUGINDLGIMPL__

// End of File
