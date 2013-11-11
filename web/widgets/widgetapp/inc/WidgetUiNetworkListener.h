//
//==============================================================================
// Name        : WidgetUiNetworkListener.h
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

#ifndef WIDGETUINETWORKLISTENER_H_
#define WIDGETUINETWORKLISTENER_H_

// INCLUDES
#include <in_sock.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum.h>
#else
#include <es_enum.h>
#include <es_enum_internal.h>
#include <es_enum_partner.h>
#endif

#include "WidgetUiWindowManager.h"

// CLASS DECLARATION

/**
* Listens real interfaces and reports changes to Widget UI.
*/
class CWidgetUiNetworkListener : public CActive
    {
    public: //Methods

            // Constructors and destructor

            /**
            * Static constructor
            */
            static CWidgetUiNetworkListener* NewL(CWidgetUiWindowManager& aWindowManager);

            /**
            * Destructor
            */
            ~CWidgetUiNetworkListener();

            /**
            * Starts listening of all intefaces.
            * @return: error code, KErrNone if successfull.
            */
            TInt StartListening();

    private: // Methods
            /**
            * Constructor.
            */
            CWidgetUiNetworkListener(CWidgetUiWindowManager& aWindowManager);

            /**
            * Implement RunL of class CActive.
            */
            void RunL();

            /**
            * Implement DoCancel of class CActive.
            */
            void DoCancel();

            /**
            * Connects to the socket server
            */
            TInt Connect();

            /**
            * Closes connection
            */
            void Close();

            /**
            * Check for existing active connections
            * @return: ETrue if there is an active network connection, else EFalse
            */
            TBool CheckActiveNetworkConnection();

    private: // Data

            // reference to WidgetUiWindowManager object
            CWidgetUiWindowManager& iWindowManager;

            // buffer to receive interface notification
            TInterfaceNotificationBuf iNote;

            // connection to interfaces
            RConnection iConn;

            // socket server
            RSocketServ iSocketServer;

    };

#endif /* WIDGETUINETWORKLISTENER_H_ */
