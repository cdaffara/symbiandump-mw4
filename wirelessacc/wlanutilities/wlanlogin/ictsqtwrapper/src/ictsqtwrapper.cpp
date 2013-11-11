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
* Implementation of IctsWrapper class
*
*/

// System includes

// User includes

#include "ictsqtwrapper.h"
#include "ictsqtwrapper_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ictsqtwrapperTraces.h"
#endif

/*!
    \class IctsWrapper
    \brief This is a wrapper implementation for symbian side ICTS client interface
*/

// External function prototypes

// Local constants


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

/*!
    Constructor
        
 */
IctsWrapper::IctsWrapper(int iap, int netId, QObject *parent) : 
    QObject(parent), 
    mIsPolling(false),
    d_ptr(new IctsWrapperPrivate(iap, netId, this))
{
    OstTraceFunctionEntry0( ICTSWRAPPER_ICTSWRAPPER_ENTRY );
    
    OstTraceFunctionExit0( ICTSWRAPPER_ICTSWRAPPER_EXIT );
}

/*!
    Destructor
        
 */
IctsWrapper::~IctsWrapper()
{
    OstTraceFunctionEntry0( ICTSWRAPPER_DESTRUCTOR_ENTRY );
    
    OstTraceFunctionExit0( ICTSWRAPPER_DESTRUCTOR_EXIT );
}

/*!
    This function emits connectivityTestResult signal
    @param [in] result result of internet connectivity test
    @param [in] redirectUrl Possible redirection URL that was received
 */
void IctsWrapper::emitConnectivityTestResult(IctsWrapper::ConnectivityTestResult result, QUrl redirectUrl)
{
    OstTraceFunctionEntry0( ICTSWRAPPER_EMITCONNECTIVITYTESTRESULT_ENTRY );

    OstTrace1( TRACE_NORMAL, ICTSWRAPPER_EMITCONNECTIVITYTESTRESULT_RESULT_TRACE, "IctsWrapper::emitConnectivityTestResult;result=%d", result );
          
#ifdef OST_TRACE_COMPILER_IN_USE
    QString url = redirectUrl.toString();
    TPtrC tmp(url.utf16(),url.length() );
    
    OstTraceExt1( TRACE_NORMAL, ICTSWRAPPER_EMITCONNECTIVITYTESTRESULT_URL_TRACE, 
        "IctsWrapper::emitConnectivityTestResult;redirectUrl=%S", tmp );
#endif
    
    emit connectivityTestResult(result, redirectUrl);
    mIsPolling = false;

    OstTraceFunctionExit0( ICTSWRAPPER_EMITCONNECTIVITYTESTRESULT_EXIT );
}

/*!
    This function starts connectivity test
 */

void IctsWrapper::startConnectivityTest()
{
    OstTraceFunctionEntry0( ICTSWRAPPER_STARTCONNECTIVITYTEST_ENTRY );
    
    d_ptr->startConnectivityTest();

    OstTraceFunctionExit0( ICTSWRAPPER_STARTCONNECTIVITYTEST_EXIT );
}

/*!
    This function starts connectivity test in polling mode
    
    @param [in] pollingTime Total polling time
    @param [in] pollingInterval Interval between polls
 */
void IctsWrapper::startPolling(int pollingTime, int pollingInterval)
{
    OstTraceFunctionEntry0( ICTSWRAPPER_STARTPOLLING_ENTRY );
    
    d_ptr->startPolling(pollingTime, pollingInterval);
    mIsPolling = true;

    OstTraceFunctionExit0( ICTSWRAPPER_STARTPOLLING_EXIT );
}

/*!
    This function stops polling mode
 */
void IctsWrapper::stopPolling()
{
    OstTraceFunctionEntry0( ICTSWRAPPER_STOPPOLLING_ENTRY );

    d_ptr->stopPolling();
    
    OstTraceFunctionExit0( ICTSWRAPPER_STOPPOLLING_EXIT );
}

/*!
    This function returns whether we are polling or not
 */
bool IctsWrapper::isPolling() const
{
    OstTraceFunctionEntry0( ICTSWRAPPER_ISPOLLING_ENTRY );
    
    OstTraceFunctionExit0( ICTSWRAPPER_ISPOLLING_EXIT );
    
    return mIsPolling;
}
