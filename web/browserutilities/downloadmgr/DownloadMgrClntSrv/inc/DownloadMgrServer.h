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
* Description:  This file contains the definition of the Download Mgr Server.
*
*/


#ifndef __DOWNLOADMGR_SERVER_H__
#define __DOWNLOADMGR_SERVER_H__

//  INCLUDES
#include <HttpDownloadManagerServerEngine.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CHttpClientAppInstance;
class CHttpDownload;
class CDownloadSubSession;
class CDownloadMgrEventQueue;
class CDefaultAttrib;
class CDownloadMgrServer;

// DATA TYPES
// panic reasons
enum TDownloadMgrSvrPanic
	{
	EBadRequest,
	EBadDescriptor,
	EDescriptorNonNumeric,
	EMainSchedulerError,
	ESvrCreateServer,
	ECreateTrapCleanup,
	EBadCounterRemove, 
	EBadSubsessionHandle,
    EMultipleInitDownloadEvent
	};

// CLASS DECLARATION
/**
*  Download manager server's shutdown timer.
*/
NONSHARABLE_CLASS( CDownloadMgrShutdown ) : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDownloadMgrShutdown* NewL( CDownloadMgrServer* aServer );

        /**
        * Destructor.
        */
        ~CDownloadMgrShutdown();

    private:

        /**
        * C++ default constructor.
        */
        CDownloadMgrShutdown( CDownloadMgrServer* aServer );

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
        void Start();

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

        RTimer              iTimer;
        CDownloadMgrServer* iServer;
    };

/**
*  Download manager server.
*/
NONSHARABLE_CLASS( CDownloadMgrServer ) : public CPolicyServer

	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDownloadMgrServer* NewL();

        /**
        * Destructor.
        */
        ~CDownloadMgrServer();

    private:

        /**
        * C++ default constructor.
        */
        CDownloadMgrServer();

        /**
        * Default Symbian 2nd phase constructor .
        */
        void ConstructL();

    public: // From CServer2
        
        /**
        * Creates a server-side client session object.
        * @param aVersion Version information supplied by the client. 
        * @return A pointer to the newly created server-side client session.
        */
        CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

    public: // New functions

        /**
        * Creates the object container.
        * @param -
        * @return A pointer to the newly created CObjectCon.
        */
        CObjectCon* NewContainerL();

        /**
        * Removes the specified object container from this container index 
        * and deletes it.
        * @param aCon A pointer to the object container to be removed.
        * @return None.
        */
        void RemoveContainer( CObjectCon* aCon );

        /**
        * Create Client App Instance owned by CDownloadMgrSession.
        * @param aAppUid app uid of the cleant
        * @param aGetAllPendingDownloads
        * @return A pointer to the newly created CHttpClientAppInstance.
        */
        CHttpClientAppInstance* CreateNewClientAppInstanceL( 
                                        TUint32 aAppUid, 
                                        MDownloadStateObserver& aObserver, 
                                        TBool aGetAllPendingDownloads );

        /**
        * Closes Client App Instance owned by CDownloadMgrSession.
        * @param aAppInst client app instance
        * @return None.
        */
        void CloseClientInstance( CHttpClientAppInstance* aAppInst );

        /**
        * Keeps count of client sessions. Cancels shutdown timer.
        * @param -
        * @return None.
        */
        void IcrementSessions();

        /**
        * Keeps count of client sessions. Starts shutdown timer if needed.
        * @param -
        * @return None.
        */
        void DecrementSessions();
        
        /**
        * Generates and returns a unique session id
        */
        TInt NextSessionId();

        /**
        * returns number of active sessions
        */
		TInt SessionsCount() {return iSessionCount;}

    public:

        /**
        * Panics the server thread associated with this session.
        * @param aPanic The panic number
        * @return None.
        */
        static void PanicServer( TDownloadMgrSvrPanic aPanic );

        /**
        * Returns the number of active download in the engine. 
        * @since Series 60 v2.8
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt ActiveDownloads() const;

    private:     // Data

        // A container for object containers
        CObjectConIx*                     iContainerIndex; ///< Owned.
        // The Download Mgr Server Engine
        CHttpDownloadManagerServerEngine* iDmEngine; ///< Owned.
        // The number of sessions
        TInt                              iSessionCount;
        // Shutdown timer
        CDownloadMgrShutdown*             iShutdown; ///< Owned.
        // next unique session id
        TInt                              iNextSessionId;

    public:     // Friend classes

        friend class CHttpDownloadManagerServerEngine;
	};

/**
*  This class represents the server side client app instance.
*/
NONSHARABLE_CLASS( CDownloadMgrSession ) :
                            public CSession2,
                            public MDownloadStateObserver
	{

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDownloadMgrSession* NewL( CDownloadMgrServer* aServer );

        /**
        * Destructor.
        */
        ~CDownloadMgrSession();

    private:

        /**
        * C++ default constructor.
        */
        CDownloadMgrSession();

        /**
        * Default Symbian 2nd phase constructor.
        */
        void ConstructL( CDownloadMgrServer* aServer) ;

    public: // New functions

        /**
        * Returns the subsession identified by the handle.
        * @param aHandle the handle number that represents 
        * the subsession in the object index.
        * @return None.
        */
        CDownloadSubSession* DownloadFromHandle( TUint aHandle );

        /**
        * Dispatch request.
        * @param aMessage The message containing the client request
        * @return None.
        */
        void DispatchMessageL( const RMessage2& aMessage );

        /**
        * Sets the current message.
        * @param aMessage The message containing the client request
        * @return None.
        */
        void SetCurrentMessage( const RMessage2& aMessage );

        /**
        * Gets the current message.
        * @param -
        * @return The current message.
        */
        const RMessage2& CurrentMessage() const;

        /**
        * Creates a new download and subsession.
        * Writes the handle of the subsession to the client's address space
        * @param -
        * @return None.
        */
        void NewDownloadL();

        /**
        * Attach this session to the client, writes the handle of the 
        * current downloads to the client's address space.
        * @param -
        * @return None.
        */
        void DownloadMgrAttachL();

        /**
        * Attach a client to an existing download (PD download) 
        * @param -
        * @return None.
        */        
        void AttachToDownloadL();

        /**
        * Create a download subsession with CHttpDownload.
        * @param aDownload 
        * @param aDefAttrib - indicates if default setting attrib is needed
        * @return None.
        */
        void CreateDownloadL( CHttpDownload* aDownload, TBool aDefAttrib = EFalse );

        /**
        * Attach a download subsession to the client.
        * @param -
        * @return None.
        */
        void DownloadAttachL();

        /**
        * Delete the download subsession identified by the handle.
        * @param aHandle the handle number that represents 
        * the subsession in the object index.
        * @return None.
        */
        void DeleteDownload( TUint aHandle );
        
        /**
        * Panics the client thread associated with this session.
        * @param aPanic The panic number
        * @return None.
        */
        void PanicClient( TInt aPanic ) const;

		/**
		* Pauses all download.
		* @param -
        * @return None.
		*/
        void PauseAllL();

		/**
		* Starts all download.
		* @param -
        * @return None.
		*/
        void StartAllL();

		/**
		* Resets all download.
		* @param -
        * @return None.
		*/
        void ResetAll();

		/**
		* Deletes all download.
		* @param -
        * @return None.
		*/
        void DeleteAll();

        /**
        * Disconnects the instance's connection and pauses all downloads.
        * @since Series 60 2.8
        * @return none
        */
        void Disconnect();

		/**
		* Sets the IAP to the app instance reading from the client's address space.
		* @param -
        * @return None.
		*/
        void SetIapIdL();

		/**
		* Gets the IAP from the app instace and write to the client's address space.
		* @param -
        * @return None.
		*/
        void GetIapId();

		/**
		* Copies data from an 8 bit descriptor in the server address space 
        * to the client thread’s address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
        * @param aMessage This is the message of which param is set.
		* @param An 8 bit descriptor in the server address space.
        * @return None.
		*/
        void Write( TInt aParam, RMessage2 aMessage, TDesC8& aDes );

		/**
		* Copies data from an 8 bit descriptor in the server address space 
        * to the client thread’s address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
        * @param aMessage This is the message of which param is set.
		* @param A 16 bit descriptor in the server address space.
        * @return None.
		*/
        void Write( TInt aParam, RMessage2 aMessage, TDesC16& aDes );

		/**
		* Copies data from the client thread’s address space into an 8 bit 
        * descriptor in the server address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
		* @param aDes An 8 bit descriptor in the server address space.
        * @return None.
		*/
        void Read( TInt aParam, TPtr8& aDes );

		/**
		* Copies data from the client thread’s address space into an 8 bit 
        * descriptor in the server address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
		* @param aDes An 8 bit descriptor in the server address space.
        * @return None.
		*/
        void Read( TInt aParam, TPtr16& aDes );

		/**
		* Creates the app instance we are representing if needed.
		* @param aAppUid application uid
		* @param None.
		*/
        void CreateClientAppInstanceL( TUint32 aAppUid );

		/**
		* Copies unique handle in the server address space
        * to the client thread’s address space.
		* @param aHandle the handle number that represents 
        * the subsession in the object index.
		* @return None.
		*/
        void WriteHandle( TInt aHandle );

        /**
        * Initializes the session's event handler.
        * @param aMessage the message which should be completed.
        * @return None.
        */
        void InitSessionEvent( const RMessage2& aMessage  );

        /**
        * Cancels the moved event handler.
        * @param -
        * @return None.
        */
        void CancelMovedEvent();

        /**
        * Creates a new subsession with download and send event to the client.
        * @param -
        * @return ETrue, if event was requested and sent.
        */
        TBool CreateSubSessionAndSendEventL( CHttpDownload* aDownload,
                                             TInt32 aDownloadState,
                                             TInt32 aProgressState,
                                             TInt32 aMoIndex );

        /**
        * Removes download from the event queue.
        * @param aDownload
        * @return None.
        */
        void RemoveFromQueue( CHttpDownload* aDownload );
        
        /**
        * Return the session Id.
        * @return id of the session
        */
        TInt SessionId() const;

        /**
        * Return the client app instance.
        * @return client app instance
        */
        CHttpClientAppInstance* ClientAppInstance()const;

    public: // from MDownloadStateObserver

        /**
        * Indicates an event occoured by the download.
        * @param aDownload The download object which the event occured.
        * @return None.
        */
        void Event( CHttpDownload* aDownload, 
                    THttpDownloadState aDlState,
                    THttpProgressState aProgState,
                    TInt32 aMoIndex );

        /**
        *
        * @param aDownload The download object which the event occured.
        * @return None.
        */
        void SetComplete( TBool aComplete );

    protected:

        /**
        * Called if the event needs to be vompleted
        * @param aErrorStatus Leave code or KErrNone
        * @return None.
        */
        void EventComplete( TInt aErrorStatus );

        /**
		* Initialize the session with the parameters got from client side.
        * Write the number of the attachable downloads to the
        * client's address space.
		* @param -
        * @return None.
		*/
        void InitializeL();

        /**
		* Gets the value for a TInt attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetIntAttributeL();

        /**
		* Gets the value for a TBool attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetBoolAttributeL();

		/**
		* Gets the value for a string attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetStringAttributeL();

		/**
		* Gets the value for a string attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetString8AttributeL();

        /**
		* Sets the value for a TInt attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
		void SetIntAttributeL();
        
		/**
		* Sets the value for a TBool attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
	    void SetBoolAttributeL();
		
		/**
		* Sets the value for a string attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
		void SetStringAttributeL();

		/**
		* Sets the value for a string attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
		void SetString8AttributeL();

    private:      // from CSession

        /**
        * Handles the servicing of client requests to the server..
        * @param aMessage The message containing the client request.
        * @return None.
        */
        void ServiceL( const RMessage2& aMessage );

    private:     // Data

        // To the current message
        RMessage2               iCurrentMessage;
        // Object container for this session
        CObjectCon*             iObjectContainer;   ///< Owned.
        // Object index which stores objects (CDownloadSubSession instances) 
        // for this session
        CObjectIx*              iObjectIx;          ///< Owned.
        // Pointer to owning server
        CDownloadMgrServer*     iDownloadMgrServer; ///< NOT Owned.
        // The client app instance which we are representing
        CHttpClientAppInstance* iClientAppInstance; ///< Owned.
        // Queue the events comes from the engine.
        CDownloadMgrEventQueue* iEventQueue;        ///< Owned.
        // Indicates if the request may be completed
        TBool                   iComplete;
        // Used by moved event handler, the message which should be completed.
        RMessage2               iEventMessage;  ///< NOT Owned.
        // Indicates the moved event is initialized.
        TBool                   iEvent;
        // Initialization parameters set by the client side
        TBool                   iIsMaster;
        // Unique id of the session
        TInt                    iSessionId;
	};

/**
*  This class represents the server side download object.
*/
NONSHARABLE_CLASS( CDownloadSubSession ) : public CObject
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDownloadSubSession * NewL( CDownloadMgrSession* aSession,
                                           CHttpDownload* aDownload );

        /**
        * destructor.
        */
        ~CDownloadSubSession();

    private:

        /**
        * C++ default constructor.
        */
        CDownloadSubSession();
        
        /**
        * Default Symbian 2nd phase constructor.
        */
        void ConstructL( CDownloadMgrSession *aSession,
                         CHttpDownload* aDownload );

    public:    // New functions

        /**
        * Closes the subsession.
        * @param -
        * @return None.
        */
        void CloseDownload();

        /**
        * Closes the completed subsession.
        * @param -
        * @return None.
        */
        void CloseCompletedDownload();

        /**
        * Starts the download.
        * @param -
        * @return None.
        */
        void StartL();

        /**
        * Pauses the download.
        * @param -
        * @return None.
        */
        void PauseL();

        /**
        * Resets the download.
        * @param -
        * @return None.
        */
        void Reset();

        /**
        * Deletes the download.
        * @param -
        * @return None.
        */
        void Delete();

        /**
        * Moves the downloaded content.
        * @param -
        * @return None.
        */
        void MoveL();

        /**
        * Returns the current message - gotten from session.
        * @param  -
        * @return RMessage2& gotten from the session.
        */
        const RMessage2& Message() const;

        /**
        * Initializes the download's event handler.
        * @param aMessage the message which should be completed.
        * @return None.
        */
        void InitDownloadEvent( const RMessage2& aMessage  );

        /**
        * Cancels the download's event handler.
        * @param -
        * @return None.
        */
        void CancelDownloadEvent();

        /**
        * Indicates an event occoured by the download.
        * @param aDownloadState
        * @param aProgressState
        * @return TBool True if the event was sent.
        */
        TBool EventL( TInt32 aDownloadState, TInt32 aProgressState, TInt32 aMoIndex );

        /**
        * Called if a leave from the download is trapped
        * @param aError Leave code
        * @return None.
        */
        void OnError( TInt aError,
                      THttpDownloadMgrError aDlError = EGeneral );

        /**
        * Called if the event needs to be vompleted
        * @param aErrorStatus Leave code or KErrNone
        * @return None.
        */
        void EventComplete( TInt aErrorStatus );

        /**
		* Gets the value for a TInt attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetIntAttributeL();

        /**
		* Gets the value for a TBool attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetBoolAttributeL();

		/**
		* Gets the value for a string attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetStringAttributeL();

		/**
		* Gets the value for a string attribute writing to the client's address space.
		* @param -
        * @return None.
		*/
		void GetString8AttributeL();

        /**
        * Gets the file handle from the download.
        */
        void GetFileHandleAttributeL();

        /**
		* Sets the value for a TInt attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
		void SetIntAttributeL();
        
		/**
		* Sets the value for a TBool attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
	    void SetBoolAttributeL();
		
		/**
		* Sets the value for a string attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
		void SetStringAttributeL();

		/**
		* Sets the value for a string attribute reading from the client's address space.
		* @param -
        * @return None.
		*/
		void SetString8AttributeL();

        /**
        * Sets the file handle for the download.
        */
        void SetFileHandleAttributeL();
        
        /**
        * Sets the download data (media info).
        */
        void SetDownloadDataAttributeL();
        
        /**
        * Sets the media object data (track info).
        */
        void SetTrackDataAttributeL();
        
		/**
		* Sets default attributes.
		* @param -
        * @return None.
		*/
        void SetDefAttributesL();

		/**
		* Copies data from an 8 bit descriptor in the server address space 
        * to the client thread’s address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
		* @param A 16 bit descriptor in the server address space.
        * @return None.
		*/
        void Write( TInt aParam, TDesC8& aDes );

		/**
		* Copies data from an 8 bit descriptor in the server address space 
        * to the client thread’s address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
		* @param An 8 bit descriptor in the server address space.
        * @return None.
		*/
        void Write( TInt aParam, TDesC16& aDes );

		/**
		* Copies data from the client thread’s address space into an 8 bit 
        * descriptor in the server address space.
		* @param aParam This is a value between 0 and 3 and indicates 
        * which of the four arguments in the original client message 
        * contains the pointer to the descriptor.
		* @param aDes A 16 bit descriptor in the server address space.
        * @return None.
		*/
        void Read( TInt aParam, TDes8& aDes );

		/**
		* Copies data from the client thread’s address space into an 8 bit 
        * descriptor in the server address space.
		* @param aPtr A pointer to a valid address within the client 
        * thread’s address space.
		* @param aDes An 8 bit descriptor in the server address space.
        * @return None.
		*/
        void Read( TInt aParam, TDes16& aDes );

		/**
		* Checks the identity of the download objects.
		* @param aDownload The download object.
		* @return True if the specified aDownload equals with the represented one.
		*/
        TBool IsDownload( CHttpDownload* aDownload );

		/**
		* Returns a pointer to the download.
		* @param .
		* @return CHttpDownload*.
		*/
        CHttpDownload* Download();

		/**
		* Returns the handle number.
		* @param -
		* @return TUint.
		*/
        TUint Handle();

		/**
		* Sets the handle number.
		* @param aHandle handle number.
		* @return None.
		*/
        void SetHandle( TUint aHandle );

        /**
        * Dispatches download message
        * @since Series v2.8 Series60
        * @param aMessage FThe current message.
        * @return none. Leaves on error.
        */
        void DispatchMessageL( const RMessage2& aMessage );

		/**
		* Buffers attributes which need when a client deletes download.
		* @param None.
        * @return None.
		*/
        void BufferAttributesL();

		/**
		* Create a package for attributes. Part of IPC optimalization.
		* @param aBuffered - indicates that all the buffered attributes needs to be passed.
        * @return HBufC8*.
		*/
        HBufC8* CreateAttribPackL( TBool aBuffered = EFalse);

    protected:   // Data
        // Session owning us.
        CDownloadMgrSession* iSession; // NOT Owned.
    
    private:     // Data
        // The download we are representing
        CHttpDownload* iDownload; ///< NOT Owned.
        // Used by event handler, the message which should be completed.
        RMessage2      iMessage;  ///< NOT Owned.
        // Indicates the event is initialized.
        TBool          iEvent;
        // Unique handle of the subsession
        TUint          iHandle;
        
        CArrayPtrFlat< CDefaultAttrib >* iAttribs;
	};

#endif /* __DOWNLOADMGR_SERVER_H__ */