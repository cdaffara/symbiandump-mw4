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
* Private implementation of wrapper for Symbian Esock library.
*/

// System includes

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <extendedconnpref.h>

// User includes

#include "wlanqtutilsesockwrapper.h"
#include "wlanqtutilsesockwrapper_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutilsesockwrapper_symbianTraces.h"
#endif

/*!
    \class WlanQtUtilsWlanQtUtilsEsockWrapperPrivate
    \brief Private implementation of wrapper for Symbian Esock library.

    Provides functionality to connect and disconnect IAPs.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] wrapper Wrapper to report progress to.
 */

WlanQtUtilsEsockWrapperPrivate::WlanQtUtilsEsockWrapperPrivate(
    WlanQtUtilsEsockWrapper *wrapper) :
    CActive(EPriorityStandard),
    iConnectionActive(EFalse),
    q_ptr(wrapper)
{
    OstTraceFunctionEntry1(WLANQTUTILSESOCKWRAPPERPRIVATE_WLANQTUTILSESOCKWRAPPERPRIVATE_ENTRY, this);

    CActiveScheduler::Add(this);
    
    // Establish a session to Socket Server. Errors in Socket Server
    // connection are fatal so just throw them as exceptions
    qt_symbian_throwIfError(iSocketServer.Connect());

    OstTraceFunctionExit1(WLANQTUTILSESOCKWRAPPERPRIVATE_WLANQTUTILSESOCKWRAPPERPRIVATE_EXIT, this);
}

/*!
    Destructor.
 */

WlanQtUtilsEsockWrapperPrivate::~WlanQtUtilsEsockWrapperPrivate()
{
    OstTraceFunctionEntry1(DUP1_WLANQTUTILSESOCKWRAPPERPRIVATE_WLANQTUTILSESOCKWRAPPERPRIVATE_ENTRY, this);

    // Close any possibly ongoing connection
    Cancel();
    // Close Socket Server session
    iSocketServer.Close();
    
    OstTraceFunctionExit1(DUP1_WLANQTUTILSESOCKWRAPPERPRIVATE_WLANQTUTILSESOCKWRAPPERPRIVATE_EXIT, this);
}

/*!
   Starts connection creation to given IAP.

   @param [in] iapId IAP ID to connect.
 */

void WlanQtUtilsEsockWrapperPrivate::ConnectIap(int iapId)
{
    OstTraceFunctionEntry1(WLANQTUTILSESOCKWRAPPERPRIVATE_CONNECTIAP_ENTRY, this);
    
    // Cancel a (possibly) ongoing previous request
    Cancel();
    
    OstTrace1(
        TRACE_NORMAL,
        WLANQTUTILSESOCKWRAPPERPRIVATE_CONNECTIAP,
        "WlanQtUtilsEsockWrapperPrivate::connectIap;iapId=%d",
        iapId );
    
    // Open an RConnection object. Errors in RConnection opening are
    // fatal so just throw them as exceptions
    qt_symbian_throwIfError(iConnection.Open(iSocketServer));
    
    // Create overrides for connection preferences to force opening of the
    // given IAP without any user prompts.
    TConnPrefList prefList;
    TExtendedConnPref prefs;
    prefs.SetIapId(iapId);
    prefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
    QT_TRAP_THROWING(prefList.AppendL(&prefs));
     
    // Start a connection with connection preferences
    iConnection.Start(prefList, iStatus);

    iConnectionActive = ETrue;
    
    SetActive();
    
    OstTraceFunctionExit1(WLANQTUTILSESOCKWRAPPERPRIVATE_CONNECTIAP_EXIT, this);
}

/*!
   Disconnects connection, if one is active.
 */

void WlanQtUtilsEsockWrapperPrivate::DisconnectIap()
{      
    OstTraceFunctionEntry1(WLANQTUTILSESOCKWRAPPERPRIVATE_DISCONNECTIAP_ENTRY, this);
    
    if (iConnectionActive) {
        OstTrace0(
            TRACE_NORMAL,
            WLANQTUTILSESOCKWRAPPERPRIVATE_DISCONNECTIAP_DISCONNECT,
            "WlanQtUtilsEsockWrapperPrivate::disconnectIap Disconnecting connection");
        
        iConnectionActive = EFalse;
        iConnection.Close();            
    } else {
        OstTrace0(
            TRACE_NORMAL,
            WLANQTUTILSESOCKWRAPPERPRIVATE_DISCONNECTIAP_IGNORED,
            "WlanQtUtilsEsockWrapperPrivate::disconnectIap Ignored since no active connection");
    }
    
    OstTraceFunctionExit1(WLANQTUTILSESOCKWRAPPERPRIVATE_DISCONNECTIAP_EXIT, this);
}

/*!
   From CActive: called when async request (RConnection::Start())
   has been completed.
 */

void WlanQtUtilsEsockWrapperPrivate::RunL()
{
    OstTraceFunctionEntry1(WLANQTUTILSESOCKWRAPPERPRIVATE_RUNL_ENTRY, this);

    OstTrace1(
        TRACE_NORMAL,
        WLANQTUTILSESOCKWRAPPERPRIVATE_RUNL,
        "WlanQtUtilsEsockWrapperPrivate::RunL;iStatus=%d", iStatus.Int());
    
    bool success;
    if (iStatus == KErrNone) {
        success = true;
    } else {
        success = false;
        iConnectionActive = EFalse;
    }
    
    // Report to public wrapper
    q_ptr->updateConnection(success, iStatus.Int());
    
    OstTraceFunctionExit1(WLANQTUTILSESOCKWRAPPERPRIVATE_RUNL_EXIT, this);
}

/*!
   From CActive: called when active object is cancelled.
 */

void WlanQtUtilsEsockWrapperPrivate::DoCancel()
{
    OstTraceFunctionEntry1(WLANQTUTILSESOCKWRAPPERPRIVATE_DOCANCEL_ENTRY, this);
    
    // Disconnect, if needed.
    DisconnectIap();
    
    OstTraceFunctionExit1(WLANQTUTILSESOCKWRAPPERPRIVATE_DOCANCEL_EXIT, this);
}
