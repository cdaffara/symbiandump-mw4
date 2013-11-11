/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine
*
*/


#ifndef CSTATEMACHINE_H
#define CSTATEMACHINE_H

class CRequestsHandler;
class CEP0Reader;
class CEP0Writer;

/**
 *  State machine for the control transfers
 *  Switches usbdevcon states between Setup-Data-Status
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CStateMachine : public CBase 
    {

public:

    /**  States */
    enum TState
    {
        ENoneState,     // State is not defined 
        ESetupStage,    // ready for getting control setup transaction
        EDataStage,     // reading data from host, in Data stage        
        EStatusStage    // waiting for Status transaction, after writing data to host
    };

     /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @param aRequestHandler Handles requests
     * @param aLdd Link to LDD services
     * @return Constructed instance
     */ 
    static CStateMachine* NewL(CRequestsHandler& aRequestsHandler, RDevUsbcClient& aLdd);
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CStateMachine();
    
    /**
     * Call back from CEP0Reader
     *
     * @since S60 v.5.0
     * @param aBuffer Data read from EP0
     * @param aStatus status of Read request, completed
     */
    void ReadEP0L(RBuf8& aBuffer, const TRequestStatus& aStatus);
    
    /**
     * Call back from CEP0Writer
     *
     * @since S60 v.5.0
     * @param aStatus status of Write request, completed
     */
    void WroteEP0L(const TRequestStatus& aStatus);
    
    /**
     * Starts machine
     *
     * @since S60 v.5.0
     */
    void StartL();
    
    /**
     * Stops machine
     *
     * @since S60 v.5.0
     */
    void Stop();
    
    /**
     * Machine is started
     *
     * @since S60 v.5.0
     * @return ETrue if state machine is started
     */
    TBool IsStarted() const;
    
private:
    
    /**
     * Default construction
     *
     * @since S60 v.5.0
     * @param aRequestHandler Handles requests
     * @param aLdd Link to LDD services
     */
    CStateMachine(CRequestsHandler& aRequestsHandler, RDevUsbcClient& aLdd);
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     */
    void ConstructL();
    
    /**
     * Process setup packet
     *
     * @since S60 v.5.0
     * @param aSetupPacket Will be processed
     */
    void ProcessSetupPacketL(RBuf8& aSetupPacket);
    
    /**
     * Checks wheather data required to be sent from host to device 
     *
     * @since S60 v.5.0
     * @param aSetupPacket Contains data direction bit, and length of data
     * @param aDataLength Length of data to be sent
     * @return ETrue If data stage is required
     */
    TBool IsDataFromHostRequired(const RBuf8& aSetupPacket, TUint& aDatalength) const;
    
    /**
     * Checks wheather data required to be sent from device to host
     *
     * @since S60 v.5.0
     * @param aSetupPacket Contains data direction bit, and length of data
     * @param aDataLength Length of data to be sent
     * @return ETrue If data stage is required
     */
    TBool IsDataFromDeviceRequired(const RBuf8& aSetupPacket, TUint& aDatalength) const;
    
private: // data
    
    /**
     * EP0 reader
     * Own.  
     */
    CEP0Reader* iEP0Reader;
    
    /**
     * EP0 writer
     * Own.  
     */
    CEP0Writer* iEP0Writer;
    
    /**
     * Requests handler
     * Not own.  
     */
    CRequestsHandler& iRequestsHandler;
    
    /**
     * LDD
     * Not own.  
     */
    RDevUsbcClient& iLdd;
    
    /**
     * State
     */
    TState iState;
    
    /**
     * Buffer to keep request, if needed to receive data from host in Data stage
     * In case if request require data to be send from device to host,
     * iBuffer keeps result of that request
     */
    RBuf8 iBuffer;
        
    };

#endif // CSTATEMACHINE_H
