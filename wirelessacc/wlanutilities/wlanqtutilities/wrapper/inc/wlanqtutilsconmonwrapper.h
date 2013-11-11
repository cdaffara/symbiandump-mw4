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
* Wrapper for Symbian Connection Monitor library.
*/

#ifndef WLANQTUTILSCONMONWRAPPER_H
#define WLANQTUTILSCONMONWRAPPER_H

// System includes

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QScopedPointer>

// User includes

#include "wlanqtutils.h"

// Forward declarations

class WlanQtUtilsAp;
class WlanQtUtilsConnection;
class WlanQtUtilsConMonWrapperInfo;
class WlanQtUtilsConMonWrapperDisconnect;

// External data types

// Constants

// Class declaration

class WlanQtUtilsConMonWrapper : public QObject
{
    Q_OBJECT
    
public:

    // Data types

    explicit WlanQtUtilsConMonWrapper(QObject *parent = 0);
    
    ~WlanQtUtilsConMonWrapper();

    WlanQtUtilsConnection *activeConnection() const;

    WlanQtUtilsConnection *connectionInfo(uint connectionId) const;

    void disconnectIap(int iapId);
        
signals:

    /*!
       Signal indicating that a new connection has been created.

       @param [in] connectionId Connection ID.
     */
    void connCreatedEventFromWrapper(uint connectionId);

    /*!
       Signal indicating that a connection has been deleted.

       @param [in] connectionId Connection ID.
     */
    void connDeletedEventFromWrapper(uint connectionId);

    /*!
       Signal indicating that status of a connection has changed.

       @param [in] connectionId Connection ID.
       @param [in] connectionStatus Connection status.
     */
    void connStatusEventFromWrapper(
        uint connectionId,
        WlanQtUtils::ConnStatus connectionStatus);

public slots:

protected:

protected slots:

private:

private slots:

private: // data
    
    // Owned data
    
    //! Private implementation of connection info interface
    QScopedPointer<WlanQtUtilsConMonWrapperInfo> d_ptrInfo;

    //! Private implementation of connection disconnect interface
    QScopedPointer<WlanQtUtilsConMonWrapperDisconnect> d_ptrDisconnect;

    // Friend classes
    
    // Friend classes in order to be able to emit public signals directly
    // from private implementation classes.
    friend class WlanQtUtilsConMonWrapperInfo;
    
    // This is defined as a friend class in order to be able to call
    // event handlers of wrappers from unit tests.
    friend class TestWlanQtUtils;
};

#endif // WLANQTUTILSCONMONWRAPPER_H
