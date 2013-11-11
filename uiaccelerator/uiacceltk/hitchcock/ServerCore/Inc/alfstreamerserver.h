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
* Description:   Server declaration, internal for Nokia
*
*/



#ifndef __ALF_STREAMER_SERVER_H__
#define __ALF_STREAMER_SERVER_H__

#include "graphics/surface.h"
#include "themerepositorylistener.h"

class CAlfStreamerServerSession;
class CAlfStreamerBridge;
class CAlfWindowManager;
class CAlfHierarchyModel;
class TSurfaceId;

NONSHARABLE_CLASS(CAlfStreamerServer): public CServer2
    {
public:
    static TInt LaunchServer(TThreadId& aThreadId, CAlfStreamerBridge* aBridge );
    static void NewLC(CAlfStreamerBridge* aBridge);
	~CAlfStreamerServer();
    void HandleClientExit(const CAlfStreamerServerSession* aClient);
    CAlfWindowManager* WindowMgr() 
        {
        return iWindowMgr;
        }
    CAlfStreamerBridge* Bridge()
        {
        return iBridge;
        }

    CAlfHierarchyModel* WindowTree()
        {
        return iWindowHierarcy;
        }

    /** composition control **/
    void AppendCompositionSessionL(CAlfStreamerServerSession* aSession, TBool aHost = EFalse );
    void RemoveCompositionSession(CAlfStreamerServerSession* aSession);
    void HandleCompositionRequestL(CAlfStreamerServerSession* aSession, TInt aOp, const RMessage2& aMessage);
    void QueueRequestForSessionL(CAlfStreamerServerSession* aSession, const TPtrC8& aPtr, TInt aOp);
    void QueueRequestAllSessionsL(const TPtrC8& aPtr, TInt aOp, TBool aAlsoWserv = EFalse);   
    void HandleCompositionEventL(CAlfStreamerServerSession* aSession, TInt aOp, const RMessage2& aMessage);

    TInt CreatePermissionToken(const RMessage2& aMessage, TInt aTarget, CAlfStreamerServerSession* aSession);
    void ValidateToken(CAlfStreamerServerSession* aSession, const RMessage2& aMessage);

    void FreeCompositionSessionExtents(CAlfStreamerServerSession* aSession);

    // ALF use
    void CreatePermissionTokenL(TInt aAlfToken, TInt aWindowHandle, TInt aWindowGroup);
    void ReleasePermissionTokenL(TInt aAlfToken);
    void QueueRequestSessionsL(TInt aAlfToken, const TPtrC8& aPtr, TInt aOp);  
    void HandleCompositionRequestL(CAlfStreamerServerSession* aSession, TInt aOp, TPtr8& aClientBuf);

    void RemoveTargetFromInactiveSurfaces(TInt aTarget);
    void AddTargetFromInactiveSurfaces(TInt aTarget);
    void GetListOfWGsHavingInactiveSurfacesL(const RMessage2& aMessage, TInt aActiveAlso = ETrue);
    void QueueRequestBGAnimSessionsL(const TPtrC8& aPtr, TInt aOp);

    void AddSignal(CAlfStreamerServerSession* aSession, const RMessage2& aMessage);
    void CompleteSignal(TInt aSignal, TInt aType);
    void FormAQueueL(TInt aOp, TInt aSecureId, TInt aWindowGroupId);

    
private:
    void ConstructL();

    CAlfStreamerServer( CAlfStreamerBridge* aBridge):
        CServer2(CActive::EPriorityStandard), iBridge(aBridge){}
	
    CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

    CAlfStreamerServerSession* WservSession(TInt aScreenNumber);
    
    TInt SearchCommonEntryForToken(const RMessage2& aMessage);
    CAlfStreamerServerSession* SearchSessionForToken(TInt aToken);
    
private:
    CAlfStreamerBridge* iBridge; // not owned
    mutable TUint iSessions;
    CAlfWindowManager* iWindowMgr;
    CAlfHierarchyModel* iWindowHierarcy;
    CThemeRepositoryListener* iThemesListener;
    
    RPointerArray<CAlfStreamerServerSession> iCompositionSessions;
    RPointerArray<CAlfStreamerServerSession> iCompositionHostSessions;
    RPointerArray<CAlfStreamerServerSession> iWindowServerSessions;
    
    class TCompositionToken
        {   
        public:
        
        /*
         * CCompositionToken is used by CAlfCompositionSource and CAlfCompositionHost
         * for maintaining layer sharing permissions.
         */
        TCompositionToken( TInt aKey, TInt aSecretKey, TInt aFlags, TInt aTarget, TInt aScreenNumber, TBool aCombinedTarget = EFalse) : 
            iKey(aKey),
            iSecretKey(aSecretKey), 
            iFlags(aFlags),
            iTarget(aTarget),
            iScreenNumber(aScreenNumber),
            iCombinedTarget(aCombinedTarget){};
        
            TInt iKey;
            TInt iSecretKey;
            TInt iFlags;
            TInt iTarget;
            TInt iScreenNumber;
            TBool iCombinedTarget;
            TInt iAlfKey;
            
    };
    
    RArray<TCompositionToken> iCompositionTokens;

    CAsyncCallBack * iRendezvous;

    struct TAlfSignal
        {
        CAlfStreamerServerSession* iSession;
        RMessagePtr2 iMessage;
        TInt iHandle;
        TInt iFlags;
        };
    
    RArray<TAlfSignal> iSignals;

public:    
    struct TAlfCompParams
        {
        TInt iTarget; 
        TInt iSecureId; // used with optional GRAM list
        TInt iWindowHandle;
        TInt iWindowGroup;
        }; 

    RArray<TAlfCompParams> iAlfTargets;
    RArray<TAlfCompParams> iInactiveSurfaces;
    RArray<TAlfCompParams> iOptionalGRAM;
    };

NONSHARABLE_CLASS(CAlfStreamerServerSession): public CSession2
    {
public:
    CAlfStreamerServerSession(const TThreadId& aThreadId);
    ~CAlfStreamerServerSession();
    void ServiceL(const RMessage2& aMessage);
    void HandleCompositionOpL(TInt aOp, const RMessage2& aMessage, CAlfStreamerServer* aServer);
    RMessagePtr2& MessagePtr()
        {
        return iMessagePtr;
        }
    TBool CompletedFromQueue(const RMessage2& aMessage);
    void QueueL(const TPtrC8& aPtr, TInt aCmd);    
    
    void SetScreenNumber(TInt aScreennumber);
    TInt ScreenNumber() const;
    TSurfaceId& ExtentSurfaceId();
    TInt& AlfToken(){ return iAlfToken; }
    TBool IsBgAnimSession(){return iIsBgAnimSession;}
    TThreadId ThreadId() const
		{
		return iThreadId;
		} 
    
private:
    RArray<TInt> iLoadedPlugins;
    RArray<TInt> iCmds;
    RPointerArray<HBufC8> iQueue;
    TBool iIsListeningCompositionEvents;
    RMessagePtr2 iMessagePtr;
    
    // Number of the screen where composition client has its window.
    // KErrNotFound if composition client does not have a window at all.
    TInt iScreenNumber;
    
    // Surface id of the composition client. If composition client has set extent to some other screen than where its actual window is. 
    // This is used to identify layers created for this composition client 
    TSurfaceId iExtentSurfaceId;
    TInt iAlfToken;
    TBool iIsBgAnimSession;
    TThreadId iThreadId;
    };

#endif
