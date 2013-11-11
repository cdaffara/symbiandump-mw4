/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares the CUpnpServiceImplementation class
*
*/


#ifndef C_CUPNPSERVICEIMPLEMENTATION_H
#define C_CUPNPSERVICEIMPLEMENTATION_H

// INCLUDES

#include "upnpservice.h"
#include "upnperrors.h"
#include "upnpsubscriberlibraryobserver.h"
#include "upnpnotifytimer.h"
#include "upnphttpmessage.h"

// FORWARD DECLARATIONS

class CUpnpAction;
class CUpnpStateVariable;
class CUpnpService;
class CUpnpHttpMessage;
class CUpnpSoapMessage;
class CUpnpGenaMessage;
class CUpnpSubscriberLibrary;
class CUpnpContentHandlersController;
class CUpnpDeviceImplementationBase;
class CUpnpEventController;
class TUpnpSoapParser;
class MUpnpHttpServerTransactionCreator;
// CONSTANTS



// CLASS DECLARATION

/**
*  Interface
*  CUpnpServiceImplementation class is a virtual class where actual services
*  (like Content Directory) are inherited. It contains all mandatory methods
*  that a service must have and gives access to data that service requires.
*
*  @since Series60 2.6
*/
class CUpnpServiceImplementation :
        public CUpnpService,
        public MUpnpDispatcherCustomer

    {
    public: 

        /**
        * Two-phased constructor.
        */
        IMPORT_C CUpnpServiceImplementation( CUpnpDevice& aDevice );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUpnpServiceImplementation();

        /**
        * Sends a HTTP message. Forwards outgoing HTTP message to dispatcher.
        * @since Series60 2.6
        * @param aMessage HTTP-message.
        * @return ?description
        */
        IMPORT_C virtual void SendL( CUpnpHttpMessage* aMessage );

        /**
        * Get message dispatching path. For internal use only.
        * @since Series60 2.6
        * @return path
        */
        IMPORT_C const TDesC8& Path();
               
        /**
        * For internal use. Sets the dispatcher pointer of this service.
        * @since Series60 2.6
        * @param aDispatcher the dispatcher to be set
        */
        IMPORT_C void SetDispatcher( CUpnpDispatcher* aDispatcher );


        /**
        * Sets state variable.
        * @since Series60 2.6
        * @param aName Name of the variable (in SCPD).
        * @param aValue Requested value for state variable.
        */
        IMPORT_C void SetStateVariableL( const TDesC8& aName,
                                         const TDesC8& aValue,
                                         TBool aIsModerated = ETrue );

        /**
        * Returns requested state variable value.
        * @since Series60 2.6
        * @param aVariableName Name (16 bit) of the variable (in SCPD).
        * @return TPtrC8 Value of the variable or empty if not set.
        */
        IMPORT_C const TPtrC8 StateVariableValue( const TDesC8& aVariableName );

        /**
        * Returns requested state variable.
        * @since Series60 2.6
        * @param aVariableName Name of the variable (in SCPD).
        * @return CUpnpStateVariable*. Points to NULL if state variable not found.
        */
        IMPORT_C CUpnpStateVariable* StateVariable( const TDesC8& aVariableName );

        /**
        * Function used by CUpnpStateVariable.
        * @since Series60 2.6
        * @param aVariable This state variable is evented on next event message.
        */
        IMPORT_C void AddEventVariable( const CUpnpStateVariable& aVariable, TBool aIsModerated );

        /**
        * Converts incoming HTTP-message to appropriate class type and calls
        * appropriate action handler.
        * @since Series60 2.6
        * @param aMessage
        */
        IMPORT_C void MessageReceivedLD( CUpnpHttpMessage* aMessage );

        /**
        * Sets the minimum time that has to be expired before
        * the statevariable can be evented again
        * @param aVariable Reference to the wanted variable
        * @param aMaxRate  The minimum time between events
        */
        IMPORT_C void SetVariableMaxEventRate
                                (const TDesC8& aVariable, TInt aMaxRate);

        // From MNotifyTimeObserver
        /**
        * From MNotifyTimer Function which sends events.
        * Eventing is controlled by iEventTimer.
        * @since Series60 2.6
        * @param aTimer Timer.
        */
        IMPORT_C void TimerEventL( CUpnpNotifyTimer* aTimer );

        /**
        * Pure virtual function to be implemented in actual services.
        * Purpose of the function is to find correct action and execute it.
        * Ownership of CUpnpAction is passed by to the function.
        * @since Series60 2.6
        * @param aAction action object destruction of which should be guearanteed
        *                by implementation (also in case of leave)
        */
        virtual void ActionReceivedLD( CUpnpAction* aAction ) = 0;
     
        /**
        * Parses a CUpnpSoapMessage to a CUpnpHttpMessage and sends it then.
        * @since Series60 2.6
        * @param aMessage SOAP-message.
        * @param aError UPnP error code.
        * If code != EHttpOk, a error message is generated.
        */
        IMPORT_C void SendL( CUpnpSoapMessage* aMessage, TUpnpErrorCode aError );

        /**
        * Function to inform services about Eventing. Every derived service
        * can overwrite this function, thus being informed when
        * statevariable is evented.
        * @since Series60 2.6
        * @param aVariable Evented statevariable.
        */
        IMPORT_C virtual void StateVariableEvented(const TDesC8& aVariableName);

        /**
        * Shows number of subscribers
        * @since Series60 2.6
        */
        IMPORT_C TInt SubcribersAmount();

        IMPORT_C RPointerArray<CUpnpStateVariable>& EventedStateVariables();
        CUpnpDeviceImplementationBase& DeviceImpl();


    protected:  // New functions

        /**
        * Parses a CUpnpAction to a CUpnpHttpMessage and sends it then.
        * @since Series60 2.6
        * @param aAction Executed action.
        * @param aError UPnP error code. If code != EHttpOk,
        * a error message is generated.
        */
        IMPORT_C void SendL( CUpnpAction* aAction, TUpnpErrorCode aError );

        /**
         * Constructor. Initializes systems used in eventing.
         */
        IMPORT_C void BaseConstructL( const TDesC& aFileName, const TDesC8&  aServiceType );


        
    private: // New functions

        /**
        * Handles subscription, resubscription and subscription canceling.
        * Modifies subscriber library.
        * @since Series60 2.6
        * @param aMessage
        */
        void HandleActionL( CUpnpGenaMessage* aMessage );

        /**
        * Parses message to a CUpnpAction and then calls ActionReceivedCDL()
        * @since Series60 2.6
        * @param aMessage
        */
        void HandleActionL( CUpnpSoapMessage* aMessage );

        /**
        * Maps generic error to upnp
        * @since Series60 3.2
        * @param aError generic error
        * @return upnp error.
        */
        TUpnpErrorCode MapGenericErrorToUpnp( TInt aError );
        
        
    protected: // Data

        // iService, owned
        CUpnpService*                           iService;

        // State variable list, owned
        RPointerArray<CUpnpStateVariable>       iEventedVariables;

        //for parsing soap/gena
        TUpnpSoapParser*                        iSoapParser;

        // sax controller
        CUpnpContentHandlersController*         iSaxController;

        // dispatcher
        CUpnpDispatcher*                        iDispatcher;

        // event controller
        CUpnpEventController*                   iEventController;
    };


#endif // C_CUPNPSERVICEIMPLEMENTATION_H

// End of File