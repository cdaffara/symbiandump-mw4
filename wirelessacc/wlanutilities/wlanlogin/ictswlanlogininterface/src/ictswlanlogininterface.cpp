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
* Entry library for launching Wlan Login application.
*/

// System includes

#include <xqservicerequest.h>
#include <xqappmgr.h>
#include <xqaiwdeclplat.h> 

// User includes

#include "ictswlanlogininterface.h"
#include "wlanloginservice.h"
#include "ictsqtwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ictswlanlogininterfaceTraces.h"
#endif

/*!
    \class IctsWlanLoginInterface
    \brief This class provides a Qt API for running Internet connectivity test
           against WLAN accesspoint. If authentication is needed this component
           launches WLAN Login application through Qt Highway to render 
           authentication Web page.
*/

// External function prototypes

// Local constants
 /*!
     Service application name
*/
static const QString wlanLoginServiceAppName = QString("wlanlogin");

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
IctsWlanLoginInterface::IctsWlanLoginInterface(QObject *parent) :
    QObject(parent),
    mIctsWrapper(NULL),
    mAiwMgr(NULL),
    mAsyncRequest(NULL),
    mIapId(0),
    mNetId(0)
{   
   OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_CONSTRUCTOR_ENTRY);
   
   mAiwMgr = new XQApplicationManager();
   
   OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_CONSTRUCTOR_EXIT);
}

/*!
    Destructor.
*/
IctsWlanLoginInterface::~IctsWlanLoginInterface()
{ 
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_DESTRUCTOR_ENTRY);
    
    if (mAsyncRequest){
        delete mAsyncRequest;
    }
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_DESTRUCTOR_EXIT);
}

/*!
    Function for launching Wlan login application.
    
    @param [in] iapId Internet accesspoint id to be tested
    @param [in] netId network identifier of the connection to be used
*/
void IctsWlanLoginInterface::start(int iapId, int netId)
{    
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_START_ENTRY);
    
    // Create ICTS instance as we know needed parameters now
    mIctsWrapper = new IctsWrapper(iapId, netId, this);
    
    // Store identifiers for later use
    mIapId = iapId;
    mNetId = netId;
        
    bool connectStatus = connect( 
        mIctsWrapper,
        SIGNAL(connectivityTestResult(IctsWrapper::ConnectivityTestResult, QUrl)), 
        this, 
        SLOT(handleConnectivityTestResult(IctsWrapper::ConnectivityTestResult, QUrl)));    
    Q_ASSERT(connectStatus);
    
    mIctsWrapper->startConnectivityTest();
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_START_EXIT);
}

/*!
    Function for launching Wlan login application.
    
    @param [in] iapId Internet accesspoint id to be used
    @param [in] netId network id of the connection to be used
    @param [in] url Redirection URL to be loaded
    
*/
void IctsWlanLoginInterface::wlanLoginAppStart(int iapId, int netId, QUrl url)
{    
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_WLANLOGINAPPSTART_ENTRY);
    
    // Execution of asynchronous and embedded Wlan Login start
    mAsyncRequest = mAiwMgr->create(
        wlanLoginServiceAppName,
        XQI_WLAN_LOGIN,
        XQOP1_WLAN_LOGIN,
        false);
   
    // The Wlan Login service must always exist
    Q_ASSERT(mAsyncRequest);

    // Connect result handling signal
    bool connectStatus = connect(
        mAsyncRequest, 
        SIGNAL(requestOk(const QVariant&)),
        this,
        SLOT(handleOk(const QVariant&)));
    Q_ASSERT(connectStatus == true);
    
    // Connect error handling signal or apply lastError function instead.
    connectStatus = connect(
        mAsyncRequest,
        SIGNAL(requestError(int,const QString&)),
        this,
        SLOT(handleError(int)));        // Error string is ignored.
    Q_ASSERT(connectStatus == true);

    // Set function parameters
    QList<QVariant> args;
    args << iapId;
    args << netId;
    args << url;
     
    mAsyncRequest->setArguments(args);
   
    // The service is set asynchronous
    mAsyncRequest->setSynchronous(false);
   
    // Send the request
    bool status = mAsyncRequest->send(); 
    Q_ASSERT(status);
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_WLANLOGINAPPSTART_EXIT);
}

/*!
    Function for handling request's response from QtHighway framework
    when transaction is OK. This is used only with asynchronous requests.
    
    @param [in] result Status received from WLAN Login application
*/
void IctsWlanLoginInterface::handleOk(const QVariant& result)
{
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_HANDLEOK_ENTRY);
    
    ictsResultType ictsRes = IctsFailed;
    
    // Handling of result received from service.
    int i = result.toInt();
    WlanLoginService::WlanLoginStatus status = 
        static_cast<WlanLoginService::WlanLoginStatus>(i);
    
    switch (status) {
        case WlanLoginService::WlanLoginStatusConnectionDropped:
        case WlanLoginService::WlanLoginStatusCancel:
            OstTrace0(
                TRACE_NORMAL, 
                ICTSWLANLOGININTERFACE_HANDLEOK_CANCEL_TRACE,
                "WlanLoginStatusCancel or WlanLoginStatusConnectionDropped received ");
            // User has canceled Wlan Login and it has to be stopped
            stop();
            ictsRes = IctsCanceled;
            break;
            
        case WlanLoginService::WlanLoginStatusNext:
            OstTrace0(
                TRACE_NORMAL, 
                ICTSWLANLOGININTERFACE_HANDLEOK_NEXT_TRACE,
                "WlanLoginStatusNext received");
            ictsRes = IctsHotspotPassed;
            break;
            
        case WlanLoginService::WlanLoginStatusFailed:
            OstTrace0( TRACE_NORMAL, 
                ICTSWLANLOGININTERFACE_HANDLEOK_FAILED_TRACE,
                "WlanLoginStatusFailed received ");
            stop();
            ictsRes = IctsFailed;
            break;
                  
        default:
            OstTrace0(
			    TRACE_NORMAL,
                ICTSWLANLOGININTERFACE_HANDLEOK_UNEXPECTED_STATUS_TRACE,
                "Unexpected WlanLoginStatus received");
            break;
            
    }
    emit ictsResult(ictsRes);
    
    delete mAsyncRequest;
    mAsyncRequest = NULL;
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_HANDLEOK_EXIT);
}

/*!
    Function for handling request's response from QtHighway framework
    when errors occured. This is used only with asynchronous requests.
    
    @param [in] errorCode Error code that was received
*/
void IctsWlanLoginInterface::handleError(int errorCode)
{    
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_HANDLEERROR_ENTRY);
    
    OstTrace1(
        TRACE_NORMAL,
        ICTSWLANLOGININTERFACE_ERRORCODE_TRACE,
        "errorCode=%d",
        errorCode);
    
    delete mAsyncRequest;
    mAsyncRequest = NULL;
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_HANDLEERROR_EXIT);
}

/*!
    Function for stopping launched Wlan login application.
*/
void IctsWlanLoginInterface::stop()
{ 
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_STOP_ENTRY);
    
    // Execution of synchronous and embedded Wlan Login stop
    QScopedPointer<XQAiwRequest> syncRequest(mAiwMgr->create(
         wlanLoginServiceAppName,
         XQI_WLAN_LOGIN,
         XQOP2_WLAN_LOGIN,
         false));
   
    // The Wlan Login service must always exist
    Q_ASSERT(syncRequest);
   
    // The service is set synchronous
    syncRequest->setSynchronous(true);
   
    // Send the request
    bool status = syncRequest->send(); 
    Q_ASSERT(status);
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_STOP_EXIT);
}

/*!
    Function for handling connectivityTestResult signal
    
    @param [in] result Internet connectivity test result
    @param [in] redirectUrl Possible redirection URL. Valid only when HttpAuthenticationNeeded result received.
*/
void IctsWlanLoginInterface::handleConnectivityTestResult(IctsWrapper::ConnectivityTestResult result, QUrl redirectUrl)
{
    OstTraceFunctionEntry0(ICTSWLANLOGININTERFACE_HANDLECONNECTIVITYTESTRESULT_ENTRY);
   
    OstTrace1(TRACE_NORMAL,
        ICTSWLANLOGININTERFACE_HANDLECONNECTIVITYRESULT_RESULT_TRACE,
        "ConnectivityTestResult=%d",
        result);
        
    switch (result) 
        {
        case IctsWrapper::ConnectionOk:
            
            OstTrace1(
                TRACE_BORDER,
                ICTSWLANLOGININTERFACE_HANDLECONNECTIVITYTESTRESULT_EMIT_ICTSRESULT_PASSED,
                "IctsWlanLoginInterface::handleConnectivityTestResult emit ictsResult;result=%d",
                IctsPassed);
            
            emit ictsResult(IctsPassed);
            break;
            
        case IctsWrapper::HttpAuthenticationNeeded:
            // Inform client that Hotspot authentication is needed. 
            OstTrace0(
                TRACE_BORDER,
                ICTSWLANLOGININTERFACE_HANDLECONNECTIVITYTESTRESULT_EMIT_HOTSPOTCASE,
                "IctsWlanLoginInterface::handleConnectivityTestResult emit hotspotCase");
            
            emit hotspotCase();
            wlanLoginAppStart(mIapId, mNetId, redirectUrl);
            break;
            
        default:
            OstTrace1(
                TRACE_BORDER,
                ICTSWLANLOGININTERFACE_HANDLECONNECTIVITYTESTRESULT_EMIT_ICTS_RESULT_FAILED,
                "IctsWlanLoginInterface::handleConnectivityTestResult emit ictsResult;result=%d",
                IctsFailed);
            emit ictsResult(IctsFailed);
            break;
            
        }
    
    OstTraceFunctionExit0(ICTSWLANLOGININTERFACE_HANDLECONNECTIVITYTESTRESULT_EXIT);
}
