//
//==============================================================================
// Name        : WidgetUiNetworkListener.cpp
// Part of     : WidgetUI
// Interface   :
// Description :
// Version      : %version: 9 %
//
// Copyright  2008, 2009 Nokia Corporation.
// This material, including documentation and any related
// computer programs, is protected by copyright controlled by
// Nokia Corporation. All rights are reserved. Copying,
// including reproducing, storing, adapting or translating, any
// or all of this material requires the prior written consent of
// Nokia Corporation. This material also contains confidential
// information which may not be disclosed to others without the
// prior written consent of Nokia Corporation.
//==============================================================================
//

#include "WidgetUiNetworkListener.h"

CWidgetUiNetworkListener* CWidgetUiNetworkListener::NewL(CWidgetUiWindowManager& aWindowManager)
    {
    CWidgetUiNetworkListener* self = new (ELeave) CWidgetUiNetworkListener(aWindowManager);
    return self;
    }

CWidgetUiNetworkListener::CWidgetUiNetworkListener(CWidgetUiWindowManager& aWindowManager)
        : CActive(0), iWindowManager(aWindowManager)
    {
    CActiveScheduler::Add(this);
    StartListening();
    }

CWidgetUiNetworkListener::~CWidgetUiNetworkListener()
    {
    Cancel();
    Close();
    }

TInt CWidgetUiNetworkListener::StartListening()
    {
    TInt err = KErrNone;
    // If already listening do nothing
    if(!IsActive())
        {
        if((err = Connect()) == KErrNone)
            {
            // check if there is a current active connection
            if ( CheckActiveNetworkConnection() )
                {
                // notify widgets of an existing active network connection
                iWindowManager.NotifyConnecionChange( ETrue );
                }
            iConn.AllInterfaceNotification( iNote, iStatus );
            SetActive();
            }
        }
    return err;
    }

TBool CWidgetUiNetworkListener::CheckActiveNetworkConnection()
    {
    TUint iNumConnections, netConnections;
    TPckgBuf<TConnectionInfoV2> connectionInfo;
    
    iConn.EnumerateConnections(iNumConnections);
    netConnections  = iNumConnections;
    for(TUint i = 1; i <= iNumConnections; i++)
        {
        iConn.GetConnectionInfo(i, connectionInfo);
        // dont count bluetooth connection
        if(connectionInfo().ConnectionType() >= EConnectionBTPAN)
            {
            netConnections--;
            }
        }
    
    if (netConnections > 0)
        {
        return ETrue;
        }
    
    return EFalse;
    }

void CWidgetUiNetworkListener::RunL()
    {
    if ( iNote().iState == EInterfaceUp )
        { 
        // notify widgets of an active network connection
        iWindowManager.NotifyConnecionChange( ETrue );
        }
    else if ( iNote().iState == EInterfaceDown )
        {
        // check if there are no other active connections
        if ( !CheckActiveNetworkConnection() )
            {
            // notify widgets of a network connection going down
            iWindowManager.NotifyConnecionChange( EFalse );
            }
        }
         
    iConn.AllInterfaceNotification( iNote, iStatus );
    SetActive();
    }

void CWidgetUiNetworkListener::DoCancel()
    {
    iConn.CancelAllInterfaceNotification();
    Close();
    }

TInt CWidgetUiNetworkListener::Connect()
    {
    TInt err;
    
    if( ( err = iSocketServer.Connect() ) == KErrNone )
        {
        if( ( err = iConn.Open(iSocketServer, KAfInet) ) != KErrNone )
            {
            //failed, close server too
            iSocketServer.Close();
            }
        }
    
    return err;
    }

void CWidgetUiNetworkListener::Close()
    {
    iConn.Close();               // destructor does these
    iSocketServer.Close();
    }
