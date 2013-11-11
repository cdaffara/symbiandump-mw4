/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for personality notifier
*
*/


#ifndef CUSBPERSONALITYNOTIFIER_H
#define CUSBPERSONALITYNOTIFIER_H

#include <e32base.h>
#include <musbnotifiercallback.h>

NONSHARABLE_CLASS(CUsbPersonalityNotifier) : public CActive
    {
public:
    IMPORT_C ~CUsbPersonalityNotifier();
        
    IMPORT_C static CUsbPersonalityNotifier* NewL();
    
    IMPORT_C TInt ShowQuery(TUid aNotifierUid, const TDesC8 &aBuffer, TDes8 &aResponse, 
        MUsbNotifierCallBack* aCallBack = NULL, TRequestStatus* aStatus = NULL);
    
    IMPORT_C TInt ShowNote(TUid aNotifierUid, const TDesC8 &aBuffer, TDes8 &aResponse);
    
    IMPORT_C void CancelAll();
    
    IMPORT_C void CancelQuery(TUid aNotifierUid);
    
    /** Cancel all queued queries and notes but the currently shown */
    IMPORT_C void CancelAllButCurrent();

    /** Return ETrue, if the notifier is showing currently */
    /** DEPRICATED */
    IMPORT_C TBool IsShowing(TUid aNotifierUid);

private:

    CUsbPersonalityNotifier();
    
    void ConstructL();
    
    enum TNotifierState
        {
        EUsbPersonalityNotifierIdle,
        EUsbPersonalityNotifierStarted
        };
        
    void DoCancel();
    void RunL();
    TInt RunError( TInt /*aError*/ );
        
    TInt DoShowQuery(MUsbNotifierCallBack* aCallBack, TUid aNotifierUid, 
        const TDesC8 &aBuffer, TDes8 &aResponse, TRequestStatus* aStatus);
    
    TInt DoShowNote(TUid aNotifierUid, const TDesC8 &aBuffer, TDes8 &aResponse);

    class TNotifierClient
        {
    public:
        TNotifierClient::TNotifierClient(MUsbNotifierCallBack* aCallBack, TUid aNotifierUid, 
            const TDesC8 &aBuffer, TDes8 &aResponse, TRequestStatus* iRequestStatus, 
            TBool aConfirmation);
            
        MUsbNotifierCallBack* iCallBack; 
        TUid iNotifierUid;
        const TDesC8 &iBuffer; 
        TDes8 &iResponse;
        TRequestStatus* iRequestStatus;
        TBool iConfirmation;
        };
    
    RPointerArray<TNotifierClient> iNotifierClient;
    
    MUsbNotifierCallBack* iCallBack;
    TUid iNotifierUid;
    RNotifier iNotifier;
    TNotifierState iState;
    TRequestStatus* iRequestStatus;
    };

#endif

