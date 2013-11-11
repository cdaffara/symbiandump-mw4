/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This file contains the header file of the
*                 CWidgetRegistryServer class.
*
*                 This class implements the CWidgetRegistryServer class.
*
*/


#ifndef WIDGETREGISTRYSERVER_H
#define WIDGETREGISTRYSERVER_H

// INCLUDES
#include <WidgetRegistryConstants.h>

// FORWARD DECLARATIONS

// ----------------------------------------------------------------------------------------
// Server's policy here
// ----------------------------------------------------------------------------------------

//Total number of ranges
const TUint widgetRegistryCount = 3;

//Definition of the ranges of IPC numbers
const TInt widgetRegistryRanges[widgetRegistryCount] =
        {
        EOpCodeRegisterWidget, // 0 ; EOpCodeRegisterWidget,EOpCodeDeRegisterWidget
        EOpCodeDeRegisterWidget + 1, // 2 20 ; EOpCodeGetLprojName
        EOpCodeNotSupported
        };

//Policy to implement for each of the above ranges
const TUint8 widgetRegistryElementsIndex[widgetRegistryCount] =
        {
        0,
        1,
        CPolicyServer::ENotSupported  //applies to 3rd range (out of range IPC)
        };

//Specific capability checks
const CPolicyServer::TPolicyElement widgetRegistryElements[] =
        {
        {_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient}
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy widgetRegistryPolicy =
        {
        CPolicyServer::EAlwaysPass, //Allows clients to connect
        widgetRegistryCount,        //number of ranges
        widgetRegistryRanges,       //ranges array
        widgetRegistryElementsIndex,//elements<->ranges index
        widgetRegistryElements,     //array of elements
        };

// FORWARD DECLARATIONS

/**
*
*  This class defines shut down timer usage for the widget registry server
*  @since 3.1
*/
class CShutdown : public CTimer
    {

public:
    /**
    * Constructor
    */
    inline CShutdown():CTimer( -1 )
        {
        CActiveScheduler::Add( this );
        }

    /**
    * 2-phase constructor
    */
    inline void ConstructL()
        {
        CTimer::ConstructL();
        }

    /**
    * Start timer
    */
    inline void Start()
        {
        After( KShutdownDelay );
        }
    ~CShutdown()
        {
        }

private:
    /*
    * From CActive, see base class header.
    */
    void RunL();

    };


/**
*
*  This class defines the widget registry server
*  @since 3.1
*/
class CWidgetRegistryServer : public CPolicyServer
    {
public:

    /**
    * Returns singleton of factory.
    */
    static CServer2* NewLC();

    /**
    * Initialize and run the server.
    */
    static void RunServerL();

    /**
    * Cancel the shutdown timer, the new session is connected
    */
    void AddSession();

    /**
    * Decrement the session counter and start the shutdown timer if the last client
    * has disconnected
    */
    void RemoveSession();

    /**
    * Return session count
    */
    TInt SessionCount(){ return iSessionCount; }

private:
    /**
    * Constructor
    */
    CWidgetRegistryServer();

    /**
    * Destructor.
    */
    ~CWidgetRegistryServer();
    
    /**
    * 2-phase constructor
    */
    void ConstructL();

    /**
    * From CServer2
    */
    virtual CSession2* NewSessionL(
        const TVersion& aVersion, const RMessage2& aMessage ) const;

private: // Data members
    TInt        iSessionCount;// number of open sessions
    CShutdown   iShutDown;// shut down timer
    };

#endif
