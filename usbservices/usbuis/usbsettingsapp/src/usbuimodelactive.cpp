/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include "usbuimodelactive.h"
#include "usbdebug.h"



/*!
    Constructor.
 */
UsbUiModelActive::UsbUiModelActive( int priority, QObject *parent )
:   QObject( parent )
{
    myDebug() << ">>> UsbUiModelActive::UsbUiModelActive";
    d = new UsbUiModelActivePrivate( this, priority );
    int err= iUsbWatcher.Connect();    
    myDebug() << "iUsbWatcher.Connect() returned ";
    myDebug() << err;  
    myDebug() << "<<< UsbUiModelActive::UsbUiModelActive";
}

/*!
    Destructor.
 */
UsbUiModelActive::~UsbUiModelActive()
{        
    myDebug() << ">>> UsbUiModelActive::~UsbUiModelActive";
    delete d;
    iUsbWatcher.Close(); 
    myDebug() << "<<< UsbUiModelActive::~UsbUiModelActive";
}
/*!
 * emits a signal when the request is completed
 */
void UsbUiModelActive::emitRequestCompleted( int status )
{
    myDebug() << ">>> UsbUiModelActive::emitRequestCompleted status: ";
    myDebug() << status; 
    emit requestCompleted( status );
    myDebug() << "<<< UsbUiModelActive::emitRequestCompleted";
}

/*!
 * Sets the the selected personality through usbwatcher
 */
void UsbUiModelActive::SetUsbPersonality(int personality)
    {
    myDebug() << ">>> UsbUiModelActive::SetUsbPersonality";
    myDebug() << "requested personality is ";
    myDebug() << personality;   
    // Change the personality asynchrously, result checked in RunL()
    if( IsActive() ) 
        {
        Cancel();
        }
    myDebug() << "setting new personality";    
    iUsbWatcher.SetPersonality(RequestStatus(), personality);
    SetActive();
    myDebug() << "<<< UsbUiModelActive::SetUsbPersonality";

    }
/*!
 * cancles the personality set in usbwatcher
 */
void UsbUiModelActive::CancelSetPersonality()
    {
    iUsbWatcher.CancelSetPersonality();    
    }

/*!
    Constructor.
 */
UsbUiModelActivePrivate::UsbUiModelActivePrivate( 
        UsbUiModelActive *parent, int priority )
:   CActive( (TInt) priority ), q( parent )
{
    myDebug() << ">>> UsbUiModelActivePrivate::UsbUiModelActivePrivate";
    CActiveScheduler::Add( this );
    myDebug() << "<<< UsbUiModelActivePrivate::UsbUiModelActivePrivate";
}


/*!
    Destructor.
 */
UsbUiModelActivePrivate::~UsbUiModelActivePrivate()
{
    Cancel();
}


/*!
 *  Called by the active scheduler when the request has been completed.
 */
void UsbUiModelActivePrivate::RunL()
{
    myDebug() << ">>> UsbUiModelActivePrivate::RunL";
    q->emitRequestCompleted( iStatus.Int() );
    myDebug() << "<<< UsbUiModelActivePrivate::RunL";
}


/*!
    Called by the active scheduler when the request has been cancelled.
 */
void UsbUiModelActivePrivate::DoCancel()
{
    q->CancelSetPersonality();
}


/*!
    Called by the active scheduler when an error in RunL has occurred.
 */
TInt UsbUiModelActivePrivate::RunError( TInt aError )
{
    q->emitRequestCompleted( aError );
    return KErrNone;
}
