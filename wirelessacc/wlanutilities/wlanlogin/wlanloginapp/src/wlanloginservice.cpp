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
* Wlan Login QtHighway Service.
*/

// System includes
#include <QUrl>
#include <xqaiwdeclplat.h> 

// User includes
#include "wlanloginservice.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginserviceTraces.h"
#endif

/*!
    \class WlanLoginService
    \brief Wlan Login QtHighway service implementation.

    This class implements a QtHighway service that is used to launch
    Wlan login application. 
*/

// External function prototypes

// Local constants
 /*!
     Full service name i.e "wlanlogin.com.nokia.symbian.IWlanLogin"
*/
static const QString wlanLoginFullServiceName = QString("wlanlogin") + QString(".") + QString(XQI_WLAN_LOGIN);

/*!
    Constructor.
    
    @param[in] parent Object parent.
 */
WlanLoginService::WlanLoginService(QObject* parent)
    : XQServiceProvider(wlanLoginFullServiceName, parent),
      mReturnValue(WlanLoginStatusNone)
{
    OstTraceFunctionEntry0(WLANLOGINSERVICE_WLANLOGINSERVICE_ENTRY);
    
    publishAll();
    
    OstTraceFunctionExit0(WLANLOGINSERVICE_WLANLOGINSERVICE_EXIT);
}

/*!
    Destructor.
 */
WlanLoginService::~WlanLoginService()
{
    OstTraceFunctionEntry0(WLANLOGINSERVICE_DESTRUCTOR_ENTRY);
    
    OstTraceFunctionExit0(WLANLOGINSERVICE_DESTRUCTOR_EXIT);
}

/*!
    Completes the current service.
    
    @param [in] status completeing status of the service request  
 */
void WlanLoginService::completeServiceRequest(int status)
{    
     OstTraceFunctionEntry0(WLANLOGINSERVICE_COMPLETESERVICEREQUEST_ENTRY);
     
    mReturnValue.setValue(status);
    bool ret = completeRequest(mAsyncRequestIndex, mReturnValue);
    Q_ASSERT(ret);
    
    OstTraceFunctionExit0(WLANLOGINSERVICE_COMPLETESERVICEREQUEST_EXIT);
}

/*!
    Launches Wlan Login application giving needed information.
    
    @param [in] iapId of the connection that we need to attach to
    @param [in] netId of the connection that we need to attach to
    @param [in] url redirection URL of the login page to be loaded
      
 */
void WlanLoginService::start(int iapId, int netId, QUrl url)
{
    OstTraceFunctionEntry0( WLANLOGINSERVICE_START_ENTRY );
    
    mAsyncRequestIndex = setCurrentRequestAsync();
    emit startLogin(iapId, netId, url);  

    OstTraceFunctionExit0(WLANLOGINSERVICE_START_EXIT);
}

/*!
    Stop launched Wlan Login application.
 */
void WlanLoginService::stop()
{
    OstTraceFunctionEntry0(WLANLOGINSERVICE_STOP_ENTRY);
    
    mAsyncRequestIndex = setCurrentRequestAsync();
    emit stopLogin();
    
    OstTraceFunctionExit0(WLANLOGINSERVICE_STOP_EXIT);
}
