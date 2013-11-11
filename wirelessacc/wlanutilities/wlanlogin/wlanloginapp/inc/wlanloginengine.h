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
 * Engine of Wlan Login application.
 */

#ifndef WLANLOGINENGINE_H
#define WLANLOGINENGINE_H

// System includes
#include <QObject>
#include <HbAction>

// User includes
#include "qnetworkconfigmanager.h"
#include "qnetworkconfiguration.h"
#include "qnetworksession.h"
#include "wlanloginservice.h"
#include "ictsqtwrapper.h"
#include "wlanloginnetworkaccessmanager.h"

// Forward declarations

// External data types

// Constants

/*!
    Total polling time for ICTS in microseconds
*/ 
const int totalPollingTime = 20000000;

/*!
    Polling interval time for ICTS in microseconds
*/ 
const int pollingInterval = 4000000;

/*!
    Message box time out value in milliseconds
*/ 
const int messageBoxTimeout = 3000;


QTM_USE_NAMESPACE

// Class declaration
class WlanLoginEngine : public QObject
    {
    Q_OBJECT

public:
    
    /*!
        Enumeration type for storing current engine state
    */ 
    enum EngineState { 
        WaitingForStart = 0, //!< Engine waiting start from Qt Highway
        UpdatingNetworkConfigurations, //!< Engine is updating netowk configuration states
        OpeningNetworkSession, //!< Engine is opening network session
        NetworkSessionOpened, //!< Network session is opened by engine
        RunningIcts, //!< Internet connectivity test ongoing
        IctsFailed, //!< Internet connectivity test has failed
        IctsOk, //!< Internet connectivity test has been ran successfully
        WaitingForStopRequest, //!< Waiting for stop service request
    };
    
    explicit WlanLoginEngine(QObject *parent = 0);
    ~WlanLoginEngine();

    void openSession();
    void stopIcts();
    void showConnectionDroppedMessageBox();
    void emitCompleteServiceRequest(WlanLoginService::WlanLoginStatus status);    
    WlanLoginNetworkAccessManager* networkAccessManager() const;
    EngineState engineState();
    void setEngineState(EngineState newState);
    bool bringSnifferToForeground();
    
signals:
    void completeServiceRequest(int status);
    void connectionReady(QUrl url);
    void ictsOk();    
    
private slots:
    void handleStartLogin(int iapId, int netId, QUrl redirectionUrl);
    void handleStopLogin();
    void handleUpdateCompleted();
    void handleSessionOpened();   

    void handleSessionStateChanged(QNetworkSession::State newState);
    void handleSessionError(QNetworkSession::SessionError error);
    void connectionDroppedMessageBoxClosed(HbAction* action);

    void handleStartIcts();
    void handleConnectivityTestResult(IctsWrapper::ConnectivityTestResult result, QUrl redirectUrl);
    void handleCancelTriggered();
    void handleNextTriggered();
    
private: // Data
    
    // Not owned data

    // Owned data
    EngineState mEngineState; //!< current state of the Engine
    bool mStartRequestCompleted; //!<  Variable for storing service request state
    QNetworkConfigurationManager* mNetConfigurationManager; //!< pointer to network configuration manager 
    WlanLoginNetworkAccessManager* mNetworkAccessManager; //!< pointer to network access manager
    QNetworkSession* mNetworkSession; //!< pointer to network session
    IctsWrapper* mIctsWrapper; //!< pointer to ICTS client wrapper
    bool mIctsFirstRun; //!< Flag to check if we are running ICTS first time
    int mIap; //!< IAP that was provided in startup
    int mNetId; //!< network ID that was provided in startup
    QUrl mRedirectionUrl; //!< redirection URL that was provided in startup
    };

#endif // WLANLOGINENGINE_H
