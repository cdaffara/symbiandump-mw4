/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef HTTPCONNHANDLER_H
#define HTTPCONNHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <http.h>
#include <commdbconnpref.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CHttpClientAppInstance;
class CHttpClientApp;
class CHttpConnHandler;

// CLASS DECLARATION

class MConnectionCallback
    {
    public:

        virtual void Connected() = 0;
        virtual void Suspended() = 0;
        // called if the connection is closed
        // from outside (e.g. network loss)
        virtual void Disconnected() = 0;
        virtual void ConnectionFailed( TInt aError ) = 0;
    };

class MShutdownObserver
    {
    public:

        virtual void ShutDown( TBool aFromDestructor = EFalse ) = 0;
    };

/**
*  Connection handler's shutdown timer.
*/
NONSHARABLE_CLASS( CHttpConnShutdownTimer ) : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHttpConnShutdownTimer* NewL( MShutdownObserver* aObserver );

        /**
        * Destructor.
        */
        ~CHttpConnShutdownTimer();

    private:

        /**
        * C++ default constructor.
        */
        CHttpConnShutdownTimer( MShutdownObserver* aObserver );

        /**
        * Default Symbian 2nd phase constructor .
        */
        void ConstructL();

    public: // New functions

        /**
        * Starts the timer.
        * @param 
        * @return
        */
        void Start( TInt aTimeOut );

    private: // From CActive

        /**
        * Cancels the timer.
        * @param 
        * @return
        */
        void DoCancel();

        /**
        * Handles an active object’s request completion event.
        * @param 
        * @return
        */
        void RunL();

    private: // Data

        RTimer iTimer;
        MShutdownObserver* iObserver;   // owned
    };

class MConnectionObserver
    {
    public:

        virtual void ConnectionStageChanged( TInt aStage ) = 0;
        virtual void ConnectionError( TInt aError ) = 0;
    };

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/
NONSHARABLE_CLASS( CHttpConnStageNotifier ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHttpConnStageNotifier* NewL( CHttpConnHandler* aConnHandler );
        
        /**
        * Destructor.
        */
        virtual ~CHttpConnStageNotifier();

    public: // New functions
        
        /**
        * Starts connection stage notification
        * @since Series 60 v2.8
        * @return none.
        */
        void Start();

    public: // Functions from base classes

        /**
        * methods from CActive
        */
	    void DoCancel();
	    void RunL();
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpConnStageNotifier( CHttpConnHandler* aConnHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CHttpConnStageNotifier( const CHttpConnStageNotifier& );
        // Prohibit assigment operator if not deriving from CBase.
        // CHttpConnStageNotifier& operator=( const CHttpConnStageNotifier& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data

        CHttpConnHandler* iConnHandler;     // pointer to CHttpConnHandler.
                                            // Not owned by this class!
        TNifProgressBuf iProgressBuf;

    private:    // Data

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/
NONSHARABLE_CLASS( CHttpConnHandler ) : public CActive,
                                        public MConnectionObserver,
                                        public MShutdownObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHttpConnHandler* NewL( CHttpClientApp* aClientApp );
        
        /**
        * Destructor.
        */
        virtual ~CHttpConnHandler();

    public: // New functions
        
        /**
        * Starts asynchronous connect to the network.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void ConnectL();

        /**
        * Disconnects from the network. Counts how many download still
        * using this connection. If there is no more, starts disconnection 
        * timer.
        * @since Series 60 v2.8
        * @param aExit if ETrue disconnects at once
        * @return none.
        */
        void Disconnect( TBool aAtOnce = EFalse,
                         CHttpDownload* aDownload = NULL );

        /**
        * Returns associated IAP id.
        * @since Series 60 v2.8
        * @return IAPid used to create connection
        */
        TUint32 IapId() const;

        /**
        * Returns associated client application.
        * @since Series 60 v2.8
        * @return pointer to the client application. Ownership not passed.
        */
        CHttpClientApp* ClientApp() const;

        /**
        * Returns associated client application instance.
        * @since Series 60 v2.8
        * @return pointer to the client application instance. Ownership not passed.
        */
        CHttpClientAppInstance* ClientAppInst() const;

        /**
        * Returns the HTTP session.
        * @since Series 60 v2.8
        * @return HTTP session
        */
        RHTTPSession& Session();

        /**
        * Returns the RConnection.
        * @since Series 60 v2.8
        * @return reference to the RConnection
        */
        RConnection& Connection();

        /**
        * See EDlMgrConnectionName.
        * @since Series 60 v2.8
        * @return none. Leaves on failure.
        */
        HBufC* ConnectionNameL( TBool& aDelete );

        /**
        * See EDlMgrAPName.
        * @since Series 60 v2.8
        * @return none. Leaves on failure.
        */
        void APNameL( TPtrC16& aValue );

        /**
        * Returns the connection state.
        * @since Series 60 v2.8
        * @return ETrue if connected
        */
        TBool IsConnected();

        /**
        * Associates connection handler with the given client application
        * instance.
        * @since Series 60 v2.8
        * @param aClientInst client instance to use this connection handler
        * @return none.
        */
        void SetClientAppInst( CHttpClientAppInstance* aClientInst );

        /**
        * Sets IAP id to be used to create connection.
        * Cannot be called after connection is created.
        * @since Series 60 v2.8
        * @param IAP id to be used to create connection.
        * @return none.
        */
        void SetIapId( TUint32 aIapId );

        /**
        * Set connection name to be used to clone an already exist connection
        * IAP id is ignored in this case.
        */
        void SetConnectionNameL( const TDesC& aConnName );

    public: // Functions from base classes

        /**
        * methods from CActive
        */
        void DoCancel();
        void RunL();

        /**
        * methods from CHttpConnStageNotifier
        */
        void ConnectionStageChanged( TInt aStage );
        void ConnectionError( TInt aError );

        /**
        * methods from MShutdownObserver
        */
        void ShutDown( TBool aFromDestructor = EFalse );
        
    protected:  // New functions
        
        /**
        * Initializes HTTP session.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void InitSessionL();

        /**
        * Sets connection info in HTTP session.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void SetConnectionInfoL();

        /**
        * Called if connection is successfully created.
        * @since Series 60 v2.8
        * @return none.
        */
        void Connected();

        /**
        * Returns the number of downloads using this connection handler.
        * @since Series 60 v2.8
        * @return number of downloads using this connection handler
        */
        TInt Referencies();
        
        /**
        * Updates iIapId after connection made.
        * @since Series 60 v3.0
        * @return None.
        */
        void UpdateIapId();

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpConnHandler( CHttpClientApp* aClientApp );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        RHTTPSession    iHttpSession;
        RConnection     iConnection;
        TCommDbConnPref iPref;
        TUint32         iIapId;             // See EDlMgrIap
        CHttpClientAppInstance* iClientInst;// Associated client app instance. not owned
        CHttpClientApp* iClientApp;         // This is needed because instance can be
                                            // closed while associated conn handler 
                                            // stays active. not owned
        CHttpConnStageNotifier* iConnNotif; // Connection stage notifier. owner
        CHttpConnShutdownTimer* iShutDown;  // Shutdown timer after last reference is closed.
                                            // owned

        TInt    iConnStage;
        HBufC*  iConnName;                  // owned

    private:    // Data

        TBool iNewConnection;
        
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // HTTPCONNHANDLER_H   
            
// End of File
