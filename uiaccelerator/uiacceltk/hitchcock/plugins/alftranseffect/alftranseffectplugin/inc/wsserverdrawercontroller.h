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
* Description:   window server plugin, used by transition server to control
*                framebuffers.
*
*/



#ifndef SERVERDRAWERCONTROLLER_H
#define SERVERDRAWERCONTROLLER_H

#include <e32base.h>
#include <apgcli.h>
#ifdef WSSERVERDRAWER_TIME_LOG
#include "debuglogger.h"
#endif //WSSERVERDRAWER_TIME_LOG

#include "statehandler.h"

class MAlfDrawerEngine;
class CAppInfoCache;
class CEndCheck;
class CPSListener;

enum TAbortFlags 
	{
	EAbortFullscreen = 0x1,
	EAbortControl = 0x2,
	};

const TUint KAknApplicationShellViewId= 0x101F4CD2; 
const TUint KCapServerUid =  0x10207218;
const TUint KScreensaverUid = 0x100056CF;

NONSHARABLE_CLASS(MEndChecker) 
	{
	public:
	virtual void EndExpired() = 0;
	};

class CEndCheck : public CTimer
	{
	public:
		static CEndCheck* NewL(MEndChecker& aCallBack);
		void Start(/*TInt aState,*/ TInt aMul = 1);
		~CEndCheck();
	private:
		void RunL();
		CEndCheck(MEndChecker& aCallBack);
		void DoCancel();
	private:
		MEndChecker& iCallBack;
		TBool iCancelled;
	};

/**
 * Class to hold application information. 
 */
class CAppInfoCache : public CBase
    {        
    public:
        static CAppInfoCache* NewL();
        ~CAppInfoCache();
        TInt SetUid(const TUid& aUid);
        void SetAvkonUid(const TUid& aUid);
        void RemoveAvkonUid(const TUid& aUid);
        TBool AvkonUid(const TUid& aUid) const;
        TBool GateFound() const;
        void Reset(const TUid& aUid);
        TBool IsEmbedded(const TUid& aUid) const;
        void SetParent(const TUid& aUid, const TUid& aParentUid);
        TInt SetAction(const TUid& aUid, TInt aAction);
        TInt Action(const TUid& aUid);
        TBool IsSameParent(const TUid& aUid1, const TUid& aUid2) const;
        
        const TUid& FocusUid() const;
        TBool SetFocusUid(TInt aWgId); 
        
        void SetSecureId( const TUid& aUid, const TSecureId& aSecureId );
        void SetWgId( const TUid& aUid, TInt aWgId );
        
        void ClearActions();
        
    private:
        class TAppInfo
            {
            public:
                static TInt Order(const TAppInfo& aNodeA, const TAppInfo& aNodeB);
                TAppInfo(const TUint aUid) : 
                iUid(aUid),
                iFlags(TAppInfo::ENone),
                iParent(KNullUid),
                iRootWgId(0),
                iAction(KErrNotFound),
                iSecureId(0)
                {}
            enum TFlags
                {
                ENone               = 0x0,
                EAvkonApp           = 0x1,
                ENotOkApp           = 0x4
                };  
            TUint   iUid;
            TUint   iFlags;
            TUid    iParent;
            TInt    iRootWgId;
            TInt    iAction;
            TUint   iSecureId;
            };
    private:
        void ConstructL();
        void ImportL();
        void ExportL();
        TInt Append(TUint aUid);
        TInt IndexOf(TUint aUid) const;
        void ReadArrayL(RFile& aFile);
        void WriteArrayL(RFile& aFile) const;
        void SetAppFlags(const TUid& aUid, TUint aFlag, TBool aSet);
        TBool GetAppFlags(const TUid& aUid, TUint aFlag) const;
    private:
        RApaLsSession iLs;
        RArray<TAppInfo> iAppInfo;
        TUid iFocusedUid;
    };

NONSHARABLE_CLASS(CWsServerDrawerController) : public CBase, public MEndChecker
	{
public:
	/** errors that the wsserverdrawer can inform the controller about through 
		the Error function */
	enum TServerDrawerError
		{
		EHandleMessageError
		};
	
	/**
    */  
	static CWsServerDrawerController* NewL(MAlfDrawerEngine* aEngine);

	/**
     */  
	~CWsServerDrawerController();
	
	/**
    */  
	void BeginFullscreen(TInt aType, const TUid aUid1, const TUid aUid2, TInt aData );

	/**
    */  
	void EndFullscreen(TBool aFromGfx);

	/**
    */  
    void FullscreenFinished(TInt aHandle);
	
	/**
    */  
	TInt BeginControlTransition();

	/**
    */  
	void EndControlTransition(TInt aHandle);
	
	/**
    */  
	void ScreenModeChange(TBool aBeforeScreenModeChange);

	/**
    */  
	void Error(TServerDrawerError aError);
	
	/**
     */
	void DSABegin();
	/**
     */
	void DSAEnd();
	

	/**
	* Stops the oncoming and current transition and sets
	* the state machine the resting state.
	*/
	void AbortTransition(TInt aToAbort = EAbortFullscreen|EAbortControl);


	const TUid& FocusUid();

	TBool SetFocusUid(TInt aUid);

	void StartEndChecker(TInt aMultiplier = 1);

    void CancelEndChecker();
    /**
    */
    void EndExpired();

    /** Saves the current Engine action and Uids */
    void SaveLastActionAndUid();

    TInt GetState( TInt aState ) const;
    
    TBool IsBlocked( const TUid& aFromUid, const TUid& aToUid );
    
public:
    
    CAppInfoCache& AppInfoCache()
        {
        return *iAppInfoCache;
        }
    
private:

    /**
    */
    CWsServerDrawerController(MAlfDrawerEngine* aEngine);

	/**
    */  
	void ConstructL();

	TBool StartCheck(TInt aFlags) const;

    void AddStateL(const TUid& aUid, TInt aKey);
private:
	TUid iCurrentUid; 
	CAppInfoCache* iAppInfoCache;
	CEndCheck* iEndCheck;
	MAlfDrawerEngine* iEngine; //not owning
	RPointerArray<CPSListener> iPSStates;
	CStateHandler::TFullscreenType iLastTypeTried;
	TInt iLastAction;
	TUid iLastFromUid;
	TUid iLastToUid;
    TBool iLayoutChangeActive;
    TBool iExitAborted;
	TBool iDSAActive;       //ETrue if dsa is currently active

	CStateHandler* iStates;

#ifdef WSSERVERDRAWER_TIME_LOG
	CDebugLogger *iLogger;
#endif //WSSERVERDRAWER_TIME_LOG
	};

#endif //SERVERDRAWERCONTROLLER_H

