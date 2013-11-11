/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Wlan Login network access manager class implementation
* 
*/


// System includes
#include <QObject>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QNetworkReply>

//User includes
#include "wlanloginnetworkaccessmanager.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginnetworkaccessmanagerTraces.h"
#endif

/*!
    \class WlanLoginNetworkAccessManager
    \brief WLAN Login network access manager class implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

WlanLoginNetworkAccessManager::WlanLoginNetworkAccessManager(QObject *parent): 
    QNetworkAccessManager(parent)
{
    OstTraceFunctionEntry0(WLANLOGINNETWORKACCESSMANAGER_WLANLOGINNETWORKACCESSMANAGER_ENTRY);

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);    
    QSslConfiguration::setDefaultConfiguration(sslConfig);
      
    bool connectStatus = connect(
        this,
        SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)),
        SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError>&))
        );
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        this,
        SIGNAL(finished(QNetworkReply*)), 
        this,
        SLOT(handleNetworkRequestFinished(QNetworkReply*))
        );
    Q_ASSERT(connectStatus == true);
    
    OstTraceFunctionExit0(WLANLOGINNETWORKACCESSMANAGER_WLANLOGINNETWORKACCESSMANAGER_EXIT);

}

WlanLoginNetworkAccessManager::~WlanLoginNetworkAccessManager()
{
    OstTraceFunctionEntry0( WLANLOGINNETWORKACCESSMANAGER_DESTRUCTOR_ENTRY );
    
    OstTraceFunctionExit0( WLANLOGINNETWORKACCESSMANAGER_DESTRUCTOR_EXIT );
}

/*!
    This function handles sslError signal from QNetworkAccessManager
        
 */
void WlanLoginNetworkAccessManager::handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    OstTraceFunctionEntry0(WLANLOGINNETWORKACCESSMANAGER_HANDLESSLERRORS_ENTRY);
    
#ifdef OST_TRACE_COMPILER_IN_USE    
    for (int i=0; i<errors.count(); i++) {
                  
        OstTrace1(
            TRACE_NORMAL,
            WLANLOGINNETWORKACCESSMANAGER_HANDLESSLERRORS_ERROR_CODE_TRACE,
            "WlanLoginView::handleSslErrors;SSL error code=%d",
            errors[i].error()
            );
        
        QString string = errors[i].errorString();
        TPtrC tmp(string.utf16(),string.length() );
        
        OstTraceExt1(TRACE_NORMAL,
            WLANLOGINNETWORKACCESSMANAGER_HANDLESSLERRORS_ERROR_STRING_TRACE, 
            "WlanLoginNetworkAccessManager::handleSslErrors;SSL error string=%S",
            tmp
            );  
       }
#endif //OST_TRACE_COMPILER_IN_USE
    
    reply->ignoreSslErrors();

    OstTraceFunctionExit0( WLANLOGINNETWORKACCESSMANAGER_HANDLESSLERRORS_EXIT );
}

/*!
    This function handles finished signal from QNetworkAccessManager
        
 */
void WlanLoginNetworkAccessManager::handleNetworkRequestFinished(QNetworkReply* reply)
{
    OstTraceFunctionEntry0(WLANLOGINNETWORKACCESSMANAGER_HANDLENETWORKREQUESTFINISHED_ENTRY);
    
    OstTrace1(
        TRACE_NORMAL, WLANLOGINNETWORKACCESSMANAGER_HANDLENETWORKREQUESTFINISHED_ERROR_CODE_TRACE, 
        "WlanLoginNetworkAccessManager::handleNetworkRequestFinished;QNetworkReply error code=%d",
        reply->error()
        );
    
#ifdef OST_TRACE_COMPILER_IN_USE 
    QString string = reply->errorString();
    TPtrC tmp(string.utf16(),string.length() );
            
    OstTraceExt1( TRACE_NORMAL, WLANLOGINNETWORKACCESSMANAGER_HANDLENETWORKREQUESTFINISHED_ERROR_STRING_TRACE, 
            "WlanLoginNetworkAccessManager::handleNetworkRequestFinished;QNetworkReply error string=%S", tmp );
#endif //OST_TRACE_COMPILER_IN_USE
    
    OstTraceFunctionExit0( WLANLOGINNETWORKACCESSMANAGER_HANDLENETWORKREQUESTFINISHED_EXIT );   
}
