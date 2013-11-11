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

#ifndef USBUIMODELACTIVE_H
#define USBUIMODELACTIVE_H

#include <QObject>
#include <e32base.h>
#include <usbwatcher.h>

class UsbUiModelActivePrivate;

/*!
    \class UsbUiModelActive
    \brief Helper class for using active objects in Qt classes.

    UsbUiModelActive is a helper class for using active objects 
    from any Qt class. It wraps a CActive-derived class in an interface
    that uses Qt's signal-slot mechanism for communicating status changes 
    of the active object.

 */
class UsbUiModelActive : public QObject
{
    Q_OBJECT
    friend class UsbUiModelActivePrivate;

public:
    explicit UsbUiModelActive( int priority = CActive::EPriorityStandard, 
            QObject *parent = NULL );
    ~UsbUiModelActive();

    inline TRequestStatus &RequestStatus();
    inline void SetActive();
    inline void Cancel();
    inline bool IsActive();
    
public:
    /*
     * Sets the the selected personality through usbwatcher
     * @param personality is the personality to be set
     */
    void SetUsbPersonality(int personality);
    /*
     * cancels the personality set in usbwatcher
     */
    void CancelSetPersonality();
    
signals:
    void requestCompleted( int status );

private:
   /*!
    * emits a signal when the request is completed
    * @param status is the error
    */
    void emitRequestCompleted( int status );

private:
    UsbUiModelActivePrivate *d;
     
    /** Handle to USBWatcher for setting the personality */
    RUsbWatcher iUsbWatcher; 
};


/*!
    \class UsbUiModelActivePrivate
    \brief Private class of UsbUiModelActive, for using active objects in Qt classes.

    UsbUiModelActivePrivate is a helper class for using active objects 
    from any Qt class. It derives from CActive and allows other classes to use 
    it for passing to asynchronous function calls through its RequestStatus method.

 */
class UsbUiModelActivePrivate : public CActive
{
    friend class UsbUiModelActive;

public:
    explicit UsbUiModelActivePrivate( UsbUiModelActive *parent, int priority );
    ~UsbUiModelActivePrivate();

private:
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError( TInt aError );

private:
    UsbUiModelActive *q;

};

inline bool UsbUiModelActive::IsActive()
{
    return d->IsActive();
}

inline TRequestStatus &UsbUiModelActive::RequestStatus()
{
    return d->iStatus;
}

inline void UsbUiModelActive::SetActive()
{
    d->SetActive();
}

inline void UsbUiModelActive::Cancel()
{
    d->Cancel();
}

#endif /* USBUIMODELACTIVE_H */
