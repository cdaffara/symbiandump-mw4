/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Server session base class
*
*/



#ifndef C_ALFAPPSRVSESSIONBASE_H
#define C_ALFAPPSRVSESSIONBASE_H

#include <AknServerApp.h>
#include <alf/alfappserver.h>
#include <alf/alfred.hrh>
#include <alf/alfextensionfactory.h>
#include <alf/alfconstants.h>

#include <uiacceltk/HuiEnv.h>

class CAlfAppServer;
class CAlfAppUi;
class CCoeControl;
class CAlfSrvSubSessionBase;
class CHuiControlGroup;

// For window server visibility notifications 
class MWindowVisibilityObserver
    {
public:
    virtual void WindowFullyVisible() = 0;
    virtual void WindowPartiallyVisible() = 0;
    virtual void WindowNotVisible() = 0;
    virtual CCoeControl* CoeControl() = 0; 
    };

NONSHARABLE_CLASS(CAlfClientMessage):public CBase
    {
public:    
    static CAlfClientMessage* NewLC(TInt aSubsessionIdentifier, const RMessage2& aMessage)
        {
        CAlfClientMessage* me = new (ELeave) CAlfClientMessage();
        me->iSubSession = aSubsessionIdentifier;
        me->iMessage = aMessage;        
        CleanupStack::PushL(me);
        return me;
        }

    ~CAlfClientMessage()
        {
        if (!iMessage.IsNull())
            {
            iMessage.Complete(KErrCancel);
            }
        delete iInBuf;
        delete iOutBuf;
        delete iReplyPtr;
        }
 
    void CompleteMessageD(TInt aError)
        {
        if (aError == KErrNone)
            {
            aError = iMessage.Write(2,*iOutBuf);
            }
        iMessage.Complete(aError);
        delete this;
        }

    void SetClientParams(const TInt2& aParams)
        {
        iDecodedOp = aParams.iInt1;
        iClientId = aParams.iInt2;
        }

    static TBool Compare(const TInt* aValue, const CAlfClientMessage& aStruct)
        {
        return (*aValue == aStruct.iClientId);
        }

    TPtr8& ReplyBufAsTDes8()
        {
        __ASSERT_ALWAYS(iOutBuf, User::Invariant());
        
        if (!iReplyPtr)
            {
            iReplyPtr = new (ELeave) TPtr8(iOutBuf->Des());
            }
        return *iReplyPtr;
        }

public:
    HBufC8* iInBuf;
    HBufC8* iOutBuf;
    TInt iSubSession;
    TBool iIsAsync;
    TInt iClientId;
    TInt iDecodedOp;
    RMessagePtr2 iMessage;
    TPtr8* iReplyPtr;
    };

/**
 *  Session/service class in the server side.
 *
 *  @since S60 v3.2
 */
class CAlfAppSrvSessionBase : public CAknAppServiceBase, public MAlfInterfaceProvider
    {
public:
    /**
    * Default constructor, stores pointer to server instance
    * 
    * @param aServer Server which the client has connected.
    */
    IMPORT_C CAlfAppSrvSessionBase(const CAlfAppServer* aServer);

    /**
    * Default constructor, fetches and stores pointer to server instance using ccoestatic
    */    
    IMPORT_C CAlfAppSrvSessionBase();
    
    /**
    * Destructor, singals server to free resources specific for this client
    */
    IMPORT_C ~CAlfAppSrvSessionBase();
        
// new methods    

    /**
     * From CAlfAppSrvSessionBase
     * Called when a message is received from the client.
     * 
     * @param aMessage Message parameters.
     */
    virtual void DoHandleCommandL(const RMessage2& aMessage) = 0;
        
    /**
     * Gets the drawing area of the session
     * 
     * @return TRect of the drawing area
     */
    IMPORT_C virtual TRect ClientDrawingArea() const;
    
    /**
     * From CAlfAppSrvSessionBase
     * Client's window group received focus.
     * 
     * @param aDoTransitionEffect Do fade in transision (RnD)
     */
    IMPORT_C virtual void FocusGainedL( TBool aDoTransitionEffect );
    
    /**
     * From CAlfAppSrvSessionBase
     * Client's window group lost focus.
     * 
     * @param aDoTransitionEffect Do fade out transision (RnD)
     * @return Did transition? (RnD)
     */
    IMPORT_C virtual TBool FocusLostL( TBool aDoTransitionEffect );
    
    // have to create new extension mechanism as CApaAppServiceBase breaks the chain
    // by defining CBase's ExtensionInterface() as private 
    IMPORT_C virtual void ExtensionInterfaceL(TUid aInterfaceId, TAny*& aImplementation);

    /**
    * Ecom implementation for specific factory has been removed
    * Close all subsessions referring to that
    */
    IMPORT_C virtual void FactoryDestroyed(TInt aFactoryUid);
    
     /**
     * Returns the preferred window group position of the session.
     *
     * @return Preferred position.
     */
    IMPORT_C virtual CAlfAppServer::TAlfWGPostion PreferredWindowGroupPosition() const;
    
    /**
     * Returns the preferred refresh mode of the session.
     *
     * @return Preferred refresh mode.
     */
    IMPORT_C virtual THuiRefreshMode PreferredRefreshMode() const;

    // for convenience, to avoid CCoeStatic    
    IMPORT_C CAlfAppServer* AlfServer();
    IMPORT_C CAlfAppUi* AlfAppUi();

    
public: // from MAlfInterfaceProvider
    IMPORT_C TAny* GetInterfaceL(const THuiInterfaceSupport& aType, TInt aHandle);
    IMPORT_C TInt GetHandleFromInterface(const THuiInterfaceSupport& aType, TAny* aInterface);
    IMPORT_C CAlfLayoutManager* LayoutManager() const;
    IMPORT_C CHuiEnv* SharedHuiEnv() const;
    IMPORT_C TInt HandleCurrentCommanndAsynch();
    IMPORT_C void CompleteCmd(TInt aCommadIdentifier, TInt aResult);
    IMPORT_C void AlfInterfaceProviderExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


public: // utilities

   /**
     * From CAlfAppSrvSessionBase
     * Returns the client's window group id.
     * 
     * @return Wg ID. KErrNotFound if not set.
     */
    TInt ClientWindowGroup() const;
    
    /**
     * Sets the window group ID.
     * Server uses this information to calculate its screen position in Symbian window tree
     *
     * @param aId Wg ID
     */
    IMPORT_C void SetClientWindowGroupId(TInt aId);

    /**
     * Finds all the subsession which implement the interface.
     *
     * @param aArray Array which the function fills of subsession on return.
     * @param aType Type of the interface that we are looking for.
     */
    void GetSubsessionsByTypeL( RPointerArray<CAlfSrvSubSessionBase>& aArray, const THuiInterfaceSupport& aType );

    // Closes the subsession
    void CloseSubSession( const RMessage2& aMessage );
    
    /**
     * Returns ETrue if this session owns specified subsession handle.
     * @param aHandle subsession handle to be checked.
     * @return ETrue if subsession exists, EFalse otherwise.
     */
    TBool HasSession( TInt aSubSessionHandle ) const;
     
    // Returns subsession basd on the handle
    CAlfSrvSubSessionBase& SubSession( TInt aSubSessionHandle );

    CAlfClientMessage* CreateNewMessageHolderL(const RMessage2& aMessage);
    
    void HandleEventL(TAny* aCtrlPtr, TAny* aVisualPtr, const THuiEvent& aEvent);
    
    void StartPointerHandling();
    void FlushPointerHandling();
    void TriggerPointerEvent(const RMessage2* aMessage);
    void CancelPointerEvents();
    
    // Methods for sending general system events to clients
    void GetSystemEvents(const RMessage2* aMessage);
    void TriggerSystemEvent(TInt aEvent);
    void CancelSystemEvents();

    // embedded apps support
    void SetParentWindowGroupId(TInt aParentId);
    TInt ParentWindowGroupId();
    void ResetRootlayerTransformationsL();    

    /**
     * Sets session's max fps limiter to half, used if client's window does not have keyboard focus
     * @param aOnBackground use lowered max fps (ETrue/EFalse)
     */
    virtual void SetBackgroundMaxFps( TBool /*aOnBackground*/ ) {}; //empty implementation for base class.


    //RnD
    void ActivateContainerLayoutL(TBool aActivate);    
        
protected: // new methods

    // must be called from derived class to complete the construction 
    IMPORT_C void BaseConstructL();

protected: // internal utilities

    // Checks if the function is meant for subsession creation
    TBool IsSubSessionCreationFunction(const RMessage2& aMessage) const;
    
    // creates subsession
    CAlfSrvSubSessionBase* CreateSubSessionLC(const RMessage2& aMessage);
    
    TUint SecureId();

protected: // from baseclasses

    IMPORT_C void ServiceError(const RMessage2& aMessage,TInt aError);
    IMPORT_C TInt CountResources();
    IMPORT_C void Disconnect(const RMessage2& aMessage);
	/** ! CSession2 */
	IMPORT_C TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

    

private:

    /**
     * From CAknAppServiceBase.
     * Called by the framework when a client requests a sevice.
     *
     * @param aMessage Client's message. 
     */
    IMPORT_C void ServiceL(const RMessage2& aMessage);
    
    TBool DoSubSessionCmdL(const RMessage2& aMessage);

    void CloseSubSession(TInt aHandle);

private: // data

    CAlfAppServer* iHost; // stores pointer to server instance 
                          // (we could utilise CSession2::Server() for this functionality, but just for sake of simplicity)
 
    class TPrivateData;                          
    TPrivateData* iData;
    };

#endif // C_ALFAPPSRVSESSIONBASE_H
