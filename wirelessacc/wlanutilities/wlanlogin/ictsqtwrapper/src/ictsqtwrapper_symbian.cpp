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
* Description: Private Symbian implementation of ICTS client wrapper
*
*/

// System includes
#include <QString>
#include <QUrl>


// User includes
#include "ictsqtwrapper.h"
#include "ictsqtwrapper_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ictsqtwrapper_symbianTraces.h"
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
IctsWrapperPrivate::IctsWrapperPrivate(int aIapId, int aNetId, IctsWrapper *aWrapper) : 
    q_ptr(aWrapper),
    iIct(NULL)
{
    OstTraceFunctionEntry0(ICTSWRAPPERPRIVATE_ICTSWRAPPERPRIVATE_ENTRY);
    
    TRAPD(error,iIct = CIctsClientInterface::NewL(aIapId, aNetId, *this)); 
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(ICTSWRAPPERPRIVATE_ICTSWRAPPERPRIVATE_EXIT);
}

/*!
    Destructor
        
 */
IctsWrapperPrivate::~IctsWrapperPrivate()
{
    OstTraceFunctionEntry0(ICTSWRAPPERPRIVATE_DESTRUCTOR_ENTRY);
   
    delete iIct;
    
    OstTraceFunctionExit0(ICTSWRAPPERPRIVATE_DESTRUCTOR_EXIT);
}

/*!
    This function is called when connectivity test is ready. This is from MICTSObserver.
    
    @param [in] aResult result of internet connectivity test
    @param [in] aString Possible redirection URL that was received. 
                        Valid only when result is EHttpAuthenticationNeeded
 */

void IctsWrapperPrivate::ConnectivityObserver(TIctsTestResult aResult, const TDesC& aString)
{
    OstTraceFunctionEntry0(ICTSWRAPPERPRIVATE_CONNECTIVITYOBSERVER_ENTRY);
    
    QUrl redirectUrl;
    
    switch (aResult) 
        {
        case EConnectionOk:
            q_ptr->emitConnectivityTestResult(IctsWrapper::ConnectionOk, QUrl(""));
            break;
            
        case EHttpAuthenticationNeeded :
            //Convert descriptor to QString 
            redirectUrl = QString::fromUtf16(aString.Ptr(), aString.Length());
            q_ptr->emitConnectivityTestResult(IctsWrapper::HttpAuthenticationNeeded, redirectUrl);
            break;
            
        case EConnectionNotOk :
            q_ptr->emitConnectivityTestResult(IctsWrapper::ConnectionNotOk, QUrl(""));
            break;
            
        case ETimeout :
            q_ptr->emitConnectivityTestResult(IctsWrapper::Timeout, QUrl(""));
            break;
            
        default:
            q_ptr->emitConnectivityTestResult(IctsWrapper::UnspecifiedError, QUrl(""));
            break;
        }
    
    OstTraceFunctionExit0(ICTSWRAPPERPRIVATE_CONNECTIVITYOBSERVER_EXIT);
}

/*!
    This non leaving function starts internet connectivity test
    
 */
void IctsWrapperPrivate::startConnectivityTest()
{
    OstTraceFunctionEntry0(ICTSWRAPPERPRIVATE_STARTCONNECTIVITYTEST_ENTRY);
    
    TRAPD(err, iIct->StartL());
    
    // Check if startConnectivityTestL() leaved.
    if (KErrNone != err)
        {
        q_ptr->emitConnectivityTestResult(IctsWrapper::UnspecifiedError, QUrl(""));
        }
    
    OstTraceFunctionExit0(ICTSWRAPPERPRIVATE_STARTCONNECTIVITYTEST_EXIT);
}

/*!
    This function starts connectivity test in polling mode
    
    @param [in] pollingTime Polling time in microseconds
    @param [in] pollingInterval Polling interval in microseconds   
 */
void IctsWrapperPrivate::startPolling(int pollingTime, int pollingInterval)
{
    OstTraceFunctionEntry0(ICTSWRAPPERPRIVATE_STARTPOLLING_ENTRY);
    
    iIct->StartPolling(pollingTime, pollingInterval);

    OstTraceFunctionExit0( ICTSWRAPPERPRIVATE_STARTPOLLING_EXIT );
}

/*!
    This function stops connectivity test polling mode 
 */
void IctsWrapperPrivate::stopPolling()
{
    OstTraceFunctionEntry0(ICTSWRAPPERPRIVATE_STOPPOLLING_ENTRY);
    
    iIct->StopPolling();

    OstTraceFunctionExit0(ICTSWRAPPERPRIVATE_STOPPOLLING_EXIT);
}
