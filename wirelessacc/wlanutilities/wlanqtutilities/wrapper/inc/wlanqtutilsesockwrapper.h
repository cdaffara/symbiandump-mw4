/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Wrapper for Symbian Esock library.
*/

#ifndef WLANQTUTILSESOCKWRAPPER_H
#define WLANQTUTILSESOCKWRAPPER_H

// System includes

#include <QObject>
#include <QScopedPointer>

// User includes

// Forward declarations

class WlanQtUtilsEsockWrapperPrivate;

// External data types

// Constants

// Class declaration

class WlanQtUtilsEsockWrapper: public QObject
{
    Q_OBJECT
    
public:
    
    // Data types

    WlanQtUtilsEsockWrapper(QObject *parent = 0);
    
    ~WlanQtUtilsEsockWrapper();
    
    void updateConnection(bool isOpened, int platformStatusCode);

    int lastStatusCode();

signals:

    void connectionStatusFromWrapper(bool isOpened);

public slots:

    void connectIap(int iapId);

    void disconnectIap();

protected:

protected slots:

private:

private slots:

private: // data
    
    //! Private implementation
    QScopedPointer<WlanQtUtilsEsockWrapperPrivate> d_ptr;
    
    //! Platform status code of latest connection creation attempt
    int mLastStatusCode;

    // Friend classes
};

#endif // WLANQTUTILSESOCKWRAPPER_H
