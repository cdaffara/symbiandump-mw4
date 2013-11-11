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
* Description: Header file for IctsWrapper class
*
*/

#ifndef ICTSQTWRAPPER_H
#define ICTSQTWRAPPER_H

// System includes
#include <QObject>
#include <QUrl>

// User includes

// Forward declarations
class IctsWrapperPrivate;

// External data types
// Constants

// Library's export macro definitions
#ifdef BUILD_ICTSQTWRAPPER_DLL
#define ICTSQTWRAPPERDLL_EXPORT Q_DECL_EXPORT
#else
#define ICTSQTWRAPPERDLL_EXPORT Q_DECL_IMPORT
#endif

// Class declaration
class ICTSQTWRAPPERDLL_EXPORT IctsWrapper: public QObject
{
    Q_OBJECT
   
public:
    
    /*!
        Internet connectivity test result type
    */ 
    enum ConnectivityTestResult {
        ConnectionOk,               //!< Test passed
        HttpAuthenticationNeeded,   //!< HTTP authentication needed i.e. hotspot AP
        ConnectionNotOk,            //!< Connection failure
        Timeout,                    //!< Timeout for HTTP request
        UnspecifiedError            //!< Unspecified error
    };
    
    IctsWrapper(int iap, int netId, QObject *parent = 0);
    
    ~IctsWrapper();
    
    void startConnectivityTest();
    
    void startPolling(int pollingTime, int pollingInterval);
    
    void stopPolling();
    
    void emitConnectivityTestResult(IctsWrapper::ConnectivityTestResult result, QUrl redirectUrl);
    
    bool isPolling() const;
      
signals:

    void connectivityTestResult(IctsWrapper::ConnectivityTestResult, QUrl result);
 
private:
    
    bool mIsPolling; //!< variable to store whether ICTS polling is ongoing
    
    QScopedPointer<IctsWrapperPrivate> d_ptr; //!< Pointer to Symbian private implementation
};

#endif // ICTSQTWRAPPER_H
