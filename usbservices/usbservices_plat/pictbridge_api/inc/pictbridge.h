/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class defines and implements the API for UI engine. 
*
*/


#ifndef PICTBRIDGE_H
#define PICTBRIDGE_H

#include "dpsdefs.h"
#include <rptp.h>

class TDpsXmlString;
class CDpsUsbNotifier;
class TMDpsOperation;
class TDpsEvents;
class CDpsStateMachine;
class TDpsConfigPrintReq;

NONSHARABLE_CLASS(CDpsEngine) : public CBase
    {
    public:
        enum TConnectionStatus
            {
            ENotConnected = 1,
            // ptp printer is connected
            EPrinterConnected,
            // ptp printer is disconnected
            EPrinterDisconnected,
            // in ptp personality, but device other than printer connected
            EOtherConnected,
            // in personality other than ptp and device is connected 
            EWrongPrintModeConnected
            };
	        
    public:
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   The client should always call this function to get the Dps 
        *   engine object.
        *   This function guarantees there is only one engine in the 
        *   thread, a singleton.  	
        *   @return CDpsEngine* the only Dps Engine instance in a thread
        *   
        */
        IMPORT_C static CDpsEngine* GetEngineL();
        
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *	Deletes the dps engine object. 
        */
        IMPORT_C void Delete();
        
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *	Sets the personality to PTP. This must be the first call after 
        *   the client has got the CDpsEngine object and should only be
        *   called once.
        *   @param aStatus the asynchronous request and it has the connect
        *   state after returned. The client can use this value to get the
        *   current connect status.        
        */
        IMPORT_C void SetPrintMode(TRequestStatus& aStatus);
        							 
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Cancels the SetPrintMode request        
        */        							 
        IMPORT_C void CancelPrintMode();
						     
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Registers connection notification. This function can inform 
        *   the connection and the disconnect, two states. Connection: the
        *   personality has been set to PTP by SetPrintMode, but the cable
        *   is not connected at the moment. The connection will be informed
        *   by this function. 
        *   Disconnect: the user has unplugged the cable or changed 
        *   personality.    
        *   
        *   @param aStatus the asynchronous request status and it has the 
        *   connect state after returned.
        */     
        IMPORT_C void ConnectStateNotify(TRequestStatus& aStatus);
				     					
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Registers Dps event notification. There are two events: jobStatus
        *   and deviceStatus. This function is called immediately after 
        *   ConnecSatetNotify call. After this call, the client should issue 
        *   ConfigPrintService request to configure the printer.
        *   @param aParam this parameter serves as out parameter. After
        *   processing the event from the printer, Dps engine will put
        *   the correct value to this parameter. As the result, the client
        *   can get the event by accessing this parameter after this request
        *   gets answered. So the client should have it as a class variable
        *   @param aStatus the asynchronous status. 
        */
        IMPORT_C void DpsEventNotify(TDpsEvents& aParam, 
                                     TRequestStatus& aStatus);
        
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Cancels Dps event notification. The client only needs to call
        *   this to reset the state of the dps engine and it must be called
        *   after CancelDpsRequest().
        */
        IMPORT_C void CancelDpsEventNotify();
        
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Starts a Dps operation.
        *   If this function returned with error, e.g. leaving, the client
        *   should call CancelDpsRequest(), to cancel the pending request.
        *   @param aRequest the Dps operation. It includes both request and 
        *   reply. The client should fill in the request parameter and the 
        *   Dps engine will fill in the reply paramter when this operation
        *   is finished. The client should declare it as a class variable.
        *   @param aStatus the asynchronous status
        */
        IMPORT_C void DoDpsRequestL(TMDpsOperation* aRequest, 
                                    TRequestStatus& aStatus);
		
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Cancels the dps operation. Calling this will reset the state of 
        *   the dps engine, either because of error happened or the client
        *   wants to do this on purpose. In most cases, the client never need
        *   to cancel the ongoing request because the request will end very 
        *   quick, normally within several million seconds. The client normally
        *   waits until the request is finished, either succesfully or
        *   failed indicated by timeout.
        */        
        IMPORT_C void CancelDpsRequest();
        
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Reads the phone dps configuration from the resource file
        *   @param aConfig the dps configuration is returned by this parameter
        */
        IMPORT_C void GetDpsConfigL(TDpsConfigPrintReq& aConfig);
        
        /**
        *   @since 3.2
        *   @lib pictbridge.lib
        *
        *   Gets the folder where the printer configure file should be kept. 
        *   The print App needs a file to store the printer configure when it
        *   first calls configPrintService Dps request. The print app can 
        *   quit at anytime while the ptpserver (stack) is still running. 
        *   Since the ptpstack is keeping the session with the printer, the 
        *   printer always does not excute the second onward 
        *   configPrintService request. As the result, the restarted print app
        *   cannot get the printer configure. So there must be a file for 
        *   keeping this information and it will be deleted by ptpserver when
        *   it quits, e.g. when the connection with the printer is lost.
        *   @return TDesC& the folder descriptor
        */
        IMPORT_C const TDesC& DpsFolder() const;
            
        /**
        *   Gets the Dps event object
        *   @return TDpsEvents* the pointer to the Dps event.
        */
        TDpsEvents* Event() const;
            
        /**
        *  Gets the ptp server reference
        *  @return RPtp& the reference to ptp server.
        */
        RPtp& Ptp();
            
        /**
        *  Gets the dps constant strings
        *  @return TDpsGlobalData* the pointer to dps constant strings.
        */
        TDpsXmlString* DpsParameters() const;
         
        /**
        * @return dps event notify AO status
        *   
        */
        TRequestStatus*& EventRequest();
            
        /**
        * @return dps request AO status
        */
        TRequestStatus*& OperationRequest();
         
        /**
        * @return connection notify AO status
        */               
        TRequestStatus*& PrinterConnectRequest();
        
        /**
        *  Sets the Dps file folder. 
        *  @param aFolder the foler location, readed from Ptp server/stack 
        */
        void SetDpsFolder(const TDesC& aFolder);
        
            
    private:
        
        /**
        *   Prohibits the destructor called by the client. To delete engine object
        *   Delete() must be called
        */
        ~CDpsEngine();  
        
        /**
        *   Second phase constructor. Operations which might leave should 
        *   be called here
        */
        void ConstructL();
              
    private:
        // string constant, owned by this class
        TDpsXmlString* iDpsParameters;
        // dps engine state machine, owned by this class
        CDpsStateMachine *iDpsOperator;
        // dps operation AO request, owned by this class
        TRequestStatus* iDpsOperationRequest;
        // dps event AO request, owned by this class
        TRequestStatus* iDpsEventRequest;
        // printer connection/disconnection AO request, owned by this class
        TRequestStatus* iPrinterConnectRequest;
        
        // usb cable connection/disconnection notifier, owned by this class
        CDpsUsbNotifier *iUsbNotifier;
                
        // out parameter for events (NotifyJobStatus and
        // NotifyDeviceStauts), it is
        // passed from UI engine, not owned by this class  
        TDpsEvents* iOutEvent;
        
        // Ptp Server session, owned by this class
        RPtp    iPtp;
        // the folder where all dps releated files should be stored              
        TFileName iDpsFolder;  
    };

#endif
