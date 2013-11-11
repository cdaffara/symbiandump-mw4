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
* Wlan Login engine class implementation
* 
*/

// System includes
#include <QObject>
#include <QString>
#include <QUrl>
#include <HbMessagebox>
#include <HbAction>
#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>
#include <qnetworksession.h>
#include <apgtask.h>

//User includes
#include "wlanloginengine.h"
#include "wlanloginapplication.h"
#include "wlanloginmainwindow.h"
#include "ictsqtwrapper.h"
#include "wlanloginnetworkaccessmanager.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginengineTraces.h"
#endif

/*!
    \class WlanLoginEngine
    \brief WLAN Login engine class implementation.
*/

// External function prototypes

// Local constants
const QString iapIdentifierPrefix = "I_";

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
 */
WlanLoginEngine::WlanLoginEngine(QObject *parent): 
    QObject(parent),
    mEngineState(WaitingForStart),
    mStartRequestCompleted(false),
    mNetConfigurationManager(new QNetworkConfigurationManager(this)),
    mNetworkAccessManager(new WlanLoginNetworkAccessManager(this)),
    mNetworkSession(NULL),
    mIctsWrapper(NULL),
    mIctsFirstRun(true),
    mIap(0),
    mNetId(0),
    mRedirectionUrl(QUrl(""))            
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_WLANLOGINENGINE_ENTRY);
    
    bool connectStatus = connect(
        mNetConfigurationManager,
        SIGNAL(updateCompleted()),
        this, 
        SLOT(handleUpdateCompleted()));  
    Q_ASSERT(connectStatus == true);
            
    OstTraceFunctionExit0(WLANLOGINENGINE_WLANLOGINENGINE_EXIT);
}


/*!
    Destructor.
 */
WlanLoginEngine::~WlanLoginEngine()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_DESTRUCTOR_ENTRY);

    if (mNetworkSession) {
        mNetworkSession->close();
    }
    
    OstTraceFunctionExit0(WLANLOGINENGINE_DESTRUCTOR_EXIT);
}


/*!
    Function that returns pointer to network access manager.
    
    @return Pointer to WlanLoginNetworkAccessManager
 */
WlanLoginNetworkAccessManager* WlanLoginEngine::networkAccessManager() const
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_NETWORKACCESSMANAGER_ENTRY);
    OstTraceFunctionExit0(WLANLOGINENGINE_NETWORKACCESSMANAGER_EXIT);
    
    return mNetworkAccessManager;

}


/*!
    Function that returns current state of the engine.
    
    @return engine state
 */
WlanLoginEngine::EngineState WlanLoginEngine::engineState()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_ENGINESTATE_ENTRY);
    OstTraceFunctionExit0(WLANLOGINENGINE_ENGINESTATE_EXIT);
    
    return mEngineState;
}


/*!
    Function that sets the current state of the engine.
    
    @param [in] newState New state of the engine to be set
 */
void WlanLoginEngine::setEngineState(WlanLoginEngine::EngineState newState)
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_SETENGINESTATE_ENTRY);
    
    mEngineState = newState;
    OstTrace1(
        TRACE_NORMAL,
        WLANLOGINENGINE_SETENGINESTATE_STATE_TRACE,
        "WlanLoginEngine::setEngineState;mEngineState=%d",
        mEngineState);
    
    OstTraceFunctionExit0(WLANLOGINENGINE_SETENGINESTATE_EXIT);
}

/*!
    Function that brings WLAN Sniffer to foreground
    
    @param [in] newState New state of the engine to be set
 */
bool WlanLoginEngine::bringSnifferToForeground()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_BRINGSNIFFERTOFOREGROUND_ENTRY);

    bool retVal = false;
    RWsSession wsSession;

    if(KErrNone != wsSession.Connect()){
        OstTrace0(TRACE_NORMAL, 
        WLANLOGINENGINE_BRINGSNIFFERTOFOREGROUND_WS_SESSION_FAILED, 
        "WARNING: Session could not be opened to Window server");
        
    } else {
        TApaTaskList taskList(wsSession);
        //Find WLAN Sniffer's task by using it's UID as reference
        TApaTask task = taskList.FindApp(TUid::Uid(0x10281CAA));
     
        if(task.Exists())
        {
            task.BringToForeground();
            retVal = true;
        } else {
            OstTrace0(TRACE_NORMAL, 
            WLANLOGINENGINE_BRINGSNIFFERTOFOREGROUND_TASK_NOT_FOUND,
            "WARNING: WLAN Sniffer was not found from the task list");
        }
        wsSession.Close();       
    }
    OstTraceFunctionExit0(WLANLOGINENGINE_BRINGSNIFFERTOFOREGROUND_EXIT);

    return retVal;
}


/*!
    This slot handles startLogin signal from Qt Highway service provider
    
    @param [in] iapId Internet access point ID for a connection that we should attach to'
    @param [in] netId Network Id for the connection to be used
    @param [in] redirectionUrl The URL that we were redirected to by the first internet connectivity test
    
 */
void WlanLoginEngine::handleStartLogin(int iapId, int netId, QUrl redirectionUrl)
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLESTARTLOGIN_ENTRY);
   
    mIap = iapId; 
    OstTrace1(
        TRACE_NORMAL,
        WLANLOGINENGINE_HANDLESTARTLOGIN_IAP_TRACE,
        "WlanLoginEngine::handleStartLogin;mIap=%d",
        mIap);
  
    mNetId = netId;
    OstTrace1(
        TRACE_NORMAL,
        WLANLOGINENGINE_HANDLESTARTLOGIN_NETID_TRACE,
        "WlanLoginEngine::handleStartLogin;mNetId=%d",
        mNetId);
       
    mRedirectionUrl = redirectionUrl;
    
#ifdef OST_TRACE_COMPILER_IN_USE
    QString url = mRedirectionUrl.toString();
    TPtrC tmp(url.utf16(),url.length() );
    
    OstTraceExt1(
        TRACE_NORMAL,
        WLANLOGINENGINE_HANDLESTARTLOGIN_URL_TRACE, 
        "WlanLoginEngine::handleStartLogin;mRedirectionUrl=%S",
        tmp);
#endif
        
    //Create ICTS instance as we know needed parameters now
    mIctsWrapper = new IctsWrapper(mIap, mNetId, this);
    
    bool connectStatus = connect(
        mIctsWrapper,
        SIGNAL(connectivityTestResult(IctsWrapper::ConnectivityTestResult, QUrl)), 
        this,
        SLOT(handleConnectivityTestResult(IctsWrapper::ConnectivityTestResult, QUrl)));  
    Q_ASSERT(connectStatus == true);
    
    //Update network configurations as only states of the cellular IAP's are reliable 
    //after creating network configuration manager
    
    setEngineState(UpdatingNetworkConfigurations);
    mNetConfigurationManager->updateConfigurations();
    
    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLESTARTLOGIN_EXIT);
}

/*!
    This slot handles stopLogin signal from Qt Highway service provider
        
 */
void WlanLoginEngine::handleStopLogin()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLESTOPLOGIN_ENTRY);

    // Exit application
    qApp->quit();

    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLESTOPLOGIN_EXIT);
}


/*!
    This slot handles updateCompleted signal from QNetworkConfigManager
        
 */
void WlanLoginEngine::handleUpdateCompleted()
{
   OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLEUPDATECOMPLETED_ENTRY);
   
   //States of the available configurations are now updated and
   //we can open the network session to provided IAP
   
   //Check that cancel has not been pressed
   if (engineState() != WaitingForStopRequest) {
       openSession();
   }
 
   OstTraceFunctionExit0(WLANLOGINENGINE_HANDLEUPDATECOMPLETED_EXIT);
}

/*!
    This function handles opening of the network session to the specified IAP.
    If IAP is not already in connected state, function completes service request
    with connection dropped status and show a message box to user.
        
 */
void WlanLoginEngine::openSession()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_OPENSESSION_ENTRY);

    setEngineState(WlanLoginEngine::OpeningNetworkSession);
    
    //Get configuration that matches the provided IAP
	//Note that identifier must be given in format I_ + IAP ID
    QNetworkConfiguration netConfiguration = mNetConfigurationManager->configurationFromIdentifier(iapIdentifierPrefix + QString::number(mIap));    
    
    OstTrace1(
        TRACE_NORMAL,
        WLANLOGINENGINE_OPENSESSION_CONFIGURATION_IDENTIFIER_TRACE,
        "WlanLoginEngine::openSession;netConfiguration identifier=%d",
        netConfiguration.identifier().toInt());
    
    OstTrace1(TRACE_NORMAL,
        WLANLOGINENGINE_OPENSESSION_CONFIGURATION_STATE_TRACE,
        "WlanLoginEngine::openSession;netConfiguration state=%d",
        netConfiguration.state());
    
#ifdef OST_TRACE_COMPILER_IN_USE
    QString string = netConfiguration.name();
    TPtrC tmp(string.utf16(),string.length() );
    
    OstTraceExt1(
        TRACE_NORMAL,
        WLANLOGINENGINE_OPENSESSION_CONFIGURATION_NAME_TRACE, 
        "WlanLoginEngine::openSession;netConfiguration name=%S",
        tmp );
       
    string = netConfiguration.bearerName();
    tmp.Set(string.utf16(),string.length());
    
    OstTraceExt1(
        TRACE_NORMAL,
        WLANLOGINENGINE_OPENSESSION_CONFIGURATION_BEARER_NAME_TRACE, 
        "WlanLoginEngine::openSession;netConfiguration bearerName=%S",
        tmp );
#endif
    
    //Check that configuration state is active before opening session
    //Otherwise show disconnected message box
    
    if (netConfiguration.state() == QNetworkConfiguration::Active)
        {
        //Create network session to IAP that was provided in startup
        mNetworkSession = new QNetworkSession(netConfiguration, this);
        
        bool connectStatus = connect(
            mNetworkSession,
            SIGNAL(opened()),
            this, 
            SLOT(handleSessionOpened()));
        Q_ASSERT(connectStatus == true);
        
        connectStatus = connect(
            mNetworkSession,
            SIGNAL(stateChanged(QNetworkSession::State)), 
            this,
            SLOT(handleSessionStateChanged(QNetworkSession::State)));       
        Q_ASSERT(connectStatus == true);
        
        connectStatus = connect(
            mNetworkSession,
            SIGNAL(error(QNetworkSession::SessionError)), 
            this,
            SLOT(handleSessionError(QNetworkSession::SessionError)));   
        Q_ASSERT(connectStatus == true);
        
        mNetworkSession->open();
        } else {
            showConnectionDroppedMessageBox();
        }
    
    OstTraceFunctionExit0(WLANLOGINENGINE_OPENSESSION_EXIT);
}


/*!
    This function handles sessionOpened signal from QNetworkSession. It informs view that
    connection is ready to be used.
        
 */
void WlanLoginEngine::handleSessionOpened()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLESESSIONOPENED_ENTRY);

    //Check that cancel has not been pressed
    if (engineState() != WaitingForStopRequest) {    
        setEngineState(NetworkSessionOpened);
        //Send indication to view that connection is ready
        emit connectionReady(mRedirectionUrl);
    }

    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLESESSIONOPENED_EXIT);
}


/*!
    This function handles sessionStateChanged signal from QNetworkSession.
    @param [in] newState New state of the session  
 */
void WlanLoginEngine::handleSessionStateChanged(QNetworkSession::State newState)
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLESESSIONSTATECHANGED_ENTRY);

    OstTrace1(
        TRACE_NORMAL,
        WLANLOGINENGINE_HANDLESESSIONSTATECHANGED_STATE_TRACE,
        "WlanLoginEngine::handleSessionStateChanged;newState=%d",
        newState );
    
    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLESESSIONSTATECHANGED_EXIT);
}


/*!
    This function handles sessionError signal from QNetworkSession
    @param [in] error session error     
 */
void WlanLoginEngine::handleSessionError(QNetworkSession::SessionError error)
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLESESSIONERROR_ENTRY);
        
    OstTrace1(
        TRACE_NORMAL,
        WLANLOGINENGINE_SESSION_ERROR_TRACE,
        "WlanLoginEngine::handleSessionError;error=%d",
        error);
    
    switch (engineState()) {    
    case OpeningNetworkSession:
        showConnectionDroppedMessageBox();
        break;
        
    case NetworkSessionOpened:
    case IctsOk:
    case IctsFailed:    
        if (error == QNetworkSession::SessionAbortedError ) {
            showConnectionDroppedMessageBox();
        }
        break;
        
    case RunningIcts:
        if (error == QNetworkSession::SessionAbortedError ) {
            stopIcts();
            showConnectionDroppedMessageBox();
        }
        break;
        
    default:
        break;
    }
        
    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLESESSIONERROR_EXIT);
}

/*!
    This function handles startIcts signal and starts connectivity test in polling mode
        
 */
void WlanLoginEngine::handleStartIcts()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLESTARTICTS_ENTRY);
    
    if (!mIctsWrapper->isPolling()) {
        setEngineState(RunningIcts);
        mIctsWrapper->startPolling(totalPollingTime, pollingInterval);
    }

    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLESTARTICTS_EXIT);
}

/*!
    This function stops internet connectivity test
        
 */
void WlanLoginEngine::stopIcts()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_STOPICTS_ENTRY);

    if (mIctsWrapper->isPolling()) {
        mIctsWrapper->stopPolling();
    }

    OstTraceFunctionExit0(WLANLOGINENGINE_STOPICTS_EXIT);
}

/*!
    This slot handles connectivityTestResult signal from ICTS wrapper
    @param [in] result Result of the internet connectivity test
    @param [in] redirectionUrl The URL that we are being redirected, valid only if result is HttpAuthenticationNeeded
 */

void WlanLoginEngine::handleConnectivityTestResult(IctsWrapper::ConnectivityTestResult result, QUrl redirectUrl)
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLECONNECTIVITYTESTRESULT_ENTRY);

    Q_UNUSED(redirectUrl);
    
    mIctsFirstRun = false;
    
    if (result == IctsWrapper::ConnectionOk){
        setEngineState(IctsOk);
        emit ictsOk();
    } else {
        setEngineState(IctsFailed);
    }
    
    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLECONNECTIVITYTESTRESULT_EXIT);
}

/*!
    This slot handles cancelTriggered signal from the view
        
 */
void WlanLoginEngine::handleCancelTriggered()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLECANCELTRIGGERED_ENTRY);
    
    //Check if Qt Highway request is completed
     if (mStartRequestCompleted == false) {
        
         if (!bringSnifferToForeground()) {
             //WLAN Sniffer was not found so make exit as there is nobody to close us
             qApp->exit();
         }
         
         switch (engineState()) {
            
         case UpdatingNetworkConfigurations:
         case OpeningNetworkSession:
         case NetworkSessionOpened:
             setEngineState(WaitingForStopRequest);
             emitCompleteServiceRequest(WlanLoginService::WlanLoginStatusCancel);
             break;   
            
         case RunningIcts:      
             stopIcts();
             setEngineState(WaitingForStopRequest);
             if (mIctsFirstRun) {
                 emitCompleteServiceRequest(WlanLoginService::WlanLoginStatusCancel);
             } else {
                 emitCompleteServiceRequest(WlanLoginService::WlanLoginStatusFailed);
             } 
             break;
             
         case IctsFailed:
             setEngineState(WaitingForStopRequest);
             emitCompleteServiceRequest(WlanLoginService::WlanLoginStatusFailed);
             break;
                 
         default:          
             break;
         }
    } else {
        //Check race condition
        if(engineState() != WaitingForStopRequest) {
            qApp->exit();
        }
    }

    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLECANCELTRIGGERED_EXIT);
}

/*!
    This slot handles nextTriggered signal from the view
        
 */
void WlanLoginEngine::handleNextTriggered()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLENEXTTRIGGERED_ENTRY);

    if (mStartRequestCompleted == false ) {
        if (bringSnifferToForeground()) {
            emitCompleteServiceRequest(WlanLoginService::WlanLoginStatusNext);
        } else {
            qApp->exit();
        }
    } else {
        //put application to background:
        WlanLoginApplication* app = static_cast<WlanLoginApplication *>(this->parent());    
        app->mainWindow()->lower();
    }
    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLENEXTTRIGGERED_EXIT);
}


/*!
    This function shows connection dropped message box to the user
        
 */
void WlanLoginEngine::showConnectionDroppedMessageBox()
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_SHOWCONNECTIONDROPPEDMESSAGEBOX_ENTRY);
    
    HbMessageBox *messageBox = new HbMessageBox(hbTrId("txt_occ_info_wlan_connection_closed_login_applica"));
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout(messageBoxTimeout);
    messageBox->open(this,SLOT(connectionDroppedMessageBoxClosed(HbAction*)));
    
    OstTraceFunctionExit0(WLANLOGINENGINE_SHOWCONNECTIONDROPPEDMESSAGEBOX_EXIT);
}

void WlanLoginEngine::connectionDroppedMessageBoxClosed(HbAction* action)
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_HANDLEMESSAGEBOXCLOSED_ENTRY);
    
    Q_UNUSED(action);
        
    if (mStartRequestCompleted == false ) {
        emitCompleteServiceRequest(WlanLoginService::WlanLoginStatusConnectionDropped);
                
    } else {
        //If Qt Highway service request is already completed when connection drops, we have to make exit by our own
        qApp->exit();
    }               
      
    OstTraceFunctionExit0(WLANLOGINENGINE_HANDLEMESSAGEBOXCLOSED_EXIT);    
}

/*!
    This function completes Qt Highway service request
    @param [in] status completeing status of the service request   
 */
void WlanLoginEngine::emitCompleteServiceRequest(WlanLoginService::WlanLoginStatus status )
{
    OstTraceFunctionEntry0(WLANLOGINENGINE_EMITCOMPLETESERVICEREQUEST_ENTRY);
        
    mStartRequestCompleted = true;
    OstTrace1(
        TRACE_BORDER,
        WLANLOGINENGINE_EMITCOMPLETESERVICEREQUEST_EMIT_TRACE,
        "WlanLoginEngine::emitCompleteServiceRequest with status:;status=%d",
        status);
    emit completeServiceRequest(status);

    OstTraceFunctionExit0(WLANLOGINENGINE_EMITCOMPLETESERVICEREQUEST_EXIT);
}
