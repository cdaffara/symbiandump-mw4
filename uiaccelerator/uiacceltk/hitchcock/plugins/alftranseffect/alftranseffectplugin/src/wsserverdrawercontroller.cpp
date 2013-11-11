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
* Description:   controls the calls that comes to the wsserverplugin by the use
*				 of a state machine
*
*/



#include "wsserverdrawercontroller.h"
#include "alfdrawerengine.h"
#include <akntranseffect.h> // for Transition effect enumerations

#include <e32property.h>
#include <UikonInternalPSKeys.h>
#include <ScreensaverInternalPSKeys.h>
#include <startupdomainpskeys.h>
#include <alflogger.h>
#include <coemain.h>

#include <bautils.h>

//#include <winchangecrp.h>
#ifdef WSSERVERDRAWER_TIME_LOG
_LIT(KWsServerDrawerTimeLogFile, "WsServerDrawerTimeLog.txt");
_LIT(KWsServerDrawerTimeLogDir, "alfgfxtest");
#endif //WSSERVERDRAWER_TIME_LOG

const TInt KEndCheckTime(500000); //half second
const TInt KEndTimeOut(10);  // 10 * KEndCheckTime

// We use a huge timeout for tests as it seems we hit the timeout way too often
//const TInt KEndCheckTime(1000000); //1 second
//const TInt KEndTimeOut(10);  // 10 * KEndCheckTime

/////////////////////////////////////////////////////////////////////////////////////////

/** These apps will never get fullscreen effects to or from them */
const TUint KNoEffectApps[] = 
		{
		0x100059B5,	//	S60 autolock
		0x10207218, //	akncapserver
//		0x100058B3, // 	phone app
		0x10281EF2, //  AknNotifyServer
//		0x100056CF, //	screensaver
		0x10210CB5, // 	MIDP, JavaLauncherExe
		0x1000594D, // IRApp, disables effect when activating IR
		0x102818E7,	// VCommandManager, start in device startup
		0x100058F3, // sys app
		0x102823b5, // SendUiServiceResolver,
		0x101F8681, // VideoTelUi
		0x101F875A, // KSWInstSvrUid, prevent fullscreen effect on installing
		0x101F8543, // Voice Commands, no effects when pressing send key long
		0x10281855, // touch input server , no effects when the first time start up
		// startup blocked because startup exit leaves the screen black.
		// must be debugged
        0x100058F4, // startup blocked for the time being
        0x101f857A, // camera
        0x2002d07f	// ovistore_2002D07F.exe, this does not draw anything, but launcher 
        // Application shell is Ok, folder open and close have KAknApplicationShellViewId
        // as both to and from ids. There is only one visual, so some sort of blur effect works...
		//, KAknApplicationShellViewId
		};		

/** Switching between theese app pairs will also never get effects */
const TUint KNoEffectPairs[][2] =
    {
    {0x0, 0x0},
    };

/** switch to/from these applications will never get any effects and
    the current ongoing effect will be aborted */
const TUint KCustomNoEffectApps[] = 
    {
	0x0, // DO NOT REMOVE 0x0! this is here for compile reasons, it will not be included in the AllowedCustomUid check
    0x101f857A, // camera
    0x2002A540,  // conversation.exe , other launchable from messaging don't have effect either
    0x2002d07f // ovistore_2002D07F.exe, this does not draw anything, but launcher 
  //0xUID //Add UIds of applications that shouldn't have effects here
	};

// DISABLED! uncomment code in ConstructL that references this list to turn it on.
/** Switching between from or to theese apps should have always effects */
/*const TUint KExceptionEffectApps[] = 
	{
	0x100056CF, //	screensaver
	};
*/

/**
  *  Local funtion that checks if the uid is in the KCustomNoEffectApps list
*/
LOCAL_C TBool AllowedCustomUid(const TUid& aUid)
	{
	for(TInt i = 1; i < sizeof(KCustomNoEffectApps) / sizeof(KCustomNoEffectApps[0]); i++)
		{
		if(aUid.iUid == KCustomNoEffectApps[i])
			return EFalse;
		}
	return ETrue;
	}

/////////////////////////////////////////////////////////////////////////////////////////

class CPSListener : public CActive
    {
    public:
        inline TInt Value() const;
        inline TInt Key() const;
        CPSListener();
        void ListenL(const TUid& aUid, TInt aKey);
        ~CPSListener();
    private:
        void DoCancel();
        void RunL();
    private:
        RProperty iProperty;
        TInt iValue;
        TInt iKey;
    };
 
inline TInt CPSListener::Value() const
    {
    return iValue;
    }
    
inline TInt CPSListener::Key() const
    {
    return iKey;
    } 
    
CPSListener::CPSListener() : CActive(CActive::EPriorityHigh)
    {
    CActiveScheduler::Add(this);
    }
    
void CPSListener::ListenL(const TUid& aUid, TInt aKey)
    {
    Cancel();
    iKey = aKey;
    User::LeaveIfError(iProperty.Attach(aUid, aKey));
    RunL(); //set initial value
    }
    
   
CPSListener::~CPSListener()
    {
    Cancel();
    }
    
void CPSListener::DoCancel()
    {
    iProperty.Cancel();
    }
    
void CPSListener::RunL()    
    {
    iProperty.Subscribe(iStatus);
    iProperty.Get(iValue); //errors are ignored!
    SetActive();
    }


////////////////////////////////////////////////////////////////////////////////////////



CEndCheck* CEndCheck::NewL(MEndChecker& aCallBack)
	{
	CEndCheck* c = new (ELeave) CEndCheck(aCallBack);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

void CEndCheck::Start(/*TInt aState,*/ TInt aMul)
	{
	Cancel();
	iCancelled = EFalse;
	After(KEndCheckTime * aMul);
	}

CEndCheck::~CEndCheck()
	{
	Cancel();
	}
	
void CEndCheck::DoCancel()
	{
	CTimer::DoCancel();
	iCancelled = ETrue;
	}
		
void CEndCheck::RunL()
	{
	if(!iCancelled)
		{
		iCallBack.EndExpired();
		}
	}

CEndCheck::CEndCheck(MEndChecker& aCallBack) : CTimer(CActive::EPriorityStandard), 
iCallBack(aCallBack)	
	{
	CActiveScheduler::Add(this);
	}
	
//////////////////////////////////////////////////////////////////////////////////////////

	
LOCAL_C void GetName(RFs& aFs, TDes& aName)
	{
	aFs.PrivatePath(aName);
	aName.Insert(0, _L("D:"));
	aName.Append(_L("data.bin"));	
	}


void CAppInfoCache::ReadArrayL(RFile& aFile) 
	{
	TPckgBuf<TUint> buf;
	User::LeaveIfError(aFile.Read(buf));
	const TUint count = buf();
	for(TUint i = 0; i < count; i++)
		{
		User::LeaveIfError(aFile.Read(buf));
		const TUint value = buf();
		User::LeaveIfError(aFile.Read(buf));
		const TUint flags = buf();
		User::LeaveIfError(aFile.Read(buf));
		const TUint aToUid = buf();
		User::LeaveIfError(aFile.Read(buf));
		const TUint root = buf();
		User::LeaveIfError(aFile.Read(buf));
		const TUint sid = buf();
		if(KErrNone == Append(value)) //dont allow duplicates
			{
			const TInt index = IndexOf(value);
			iAppInfo[index].iFlags = static_cast<TAppInfo::TFlags>(flags);
			iAppInfo[index].iParent = TUid::Uid(aToUid);
			iAppInfo[index].iRootWgId = static_cast<TInt>(root);
			iAppInfo[index].iSecureId = static_cast<TUint>(sid);
			}
		}
	}
	
void CAppInfoCache::WriteArrayL(RFile& aFile) const
	{
	const TUint count = static_cast<TUint>(iAppInfo.Count());
	User::LeaveIfError(aFile.Write(TPckgC<TUint>(count)));
	for(TUint i  = 0; i < count; i++)
		{
		const TAppInfo& info = iAppInfo[i];
		User::LeaveIfError(aFile.Write(TPckgC<TUint>(info.iUid)));
		User::LeaveIfError(aFile.Write(TPckgC<TUint>(info.iFlags)));
		User::LeaveIfError(aFile.Write(TPckgC<TUint>(info.iParent.iUid)));
		User::LeaveIfError(aFile.Write(TPckgC<TUint>(info.iRootWgId)));
		User::LeaveIfError(aFile.Write(TPckgC<TUint>(info.iSecureId)));
		}
	}

TInt CAppInfoCache::TAppInfo::Order(const TAppInfo& aNodeA, const TAppInfo& aNodeB)
	{
	if(aNodeA.iUid < aNodeB.iUid)
		return 1;
	if(aNodeA.iUid > aNodeB.iUid)
		return -1;
	return 0;
	}
	
TInt CAppInfoCache::Append(TUint aUid)
	{
	return iAppInfo.InsertInOrder(TAppInfo(aUid), TLinearOrder<TAppInfo>(TAppInfo::Order));
	}
	
TInt CAppInfoCache::IndexOf(TUint aUid) const
	{
	return iAppInfo.FindInOrder(TAppInfo(aUid), TLinearOrder<TAppInfo>(TAppInfo::Order));
	}
	

CAppInfoCache* CAppInfoCache::NewL()
	{
	CAppInfoCache* f = new (ELeave) CAppInfoCache();
	CleanupStack::PushL(f);
	f->ConstructL();
	CleanupStack::Pop();
	return f;
	}
		
CAppInfoCache::~CAppInfoCache()
	{
	TRAP_IGNORE(ExportL());
	iLs.Close();
	iAppInfo.Close();
	}
	
void CAppInfoCache::ConstructL()
	{	
	ImportL();
	User::LeaveIfError(iLs.Connect());	
	}
	
TBool CAppInfoCache::GateFound() const
	{
	return ETrue;
	}

void CAppInfoCache::ExportL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TFileName name;
	GetName(fs, name);
	BaflUtils::EnsurePathExistsL(fs, name);
	RFile file;
	User::LeaveIfError(file.Replace(fs, name, EFileWrite));
	CleanupClosePushL(file);
	
	User::LeaveIfError(file.Write(TPckgC<TUint>(iFocusedUid.iUid)));
	
	WriteArrayL(file);
	CleanupStack::PopAndDestroy(2); //file, fs
	}
	
void CAppInfoCache::ImportL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TFileName name;
	GetName(fs, name);
	RFile file;
	const TInt err = file.Open(fs, name, EFileRead);
	if(err == KErrNone)
		{
		TPckgBuf<TUint> buf;
		User::LeaveIfError(file.Read(buf));
		iFocusedUid = TUid::Uid(buf());
		
		CleanupClosePushL(file);
		ReadArrayL(file);
		CleanupStack::PopAndDestroy(); //file
		fs.Delete(name); //free up memory after import
		}
	CleanupStack::PopAndDestroy(); //fs
	}


TInt CAppInfoCache::SetUid(const TUid& aUid)
	{
	if(aUid == KNullUid)
		return KErrNone;
	
	TInt err = Append(aUid.iUid);
	
	if( err == KErrAlreadyExists )
	    {
	    err = KErrNone;
	    }
	__ALFFXLOGSTRING2("CAppInfoCache::SetUid - 0x%x, append result: %d (alf)", aUid, err);
	return err;
	}

void  CAppInfoCache::SetAppFlags(const TUid& aUid, TUint aFlag, TBool aSet)
	{
	const TInt index = IndexOf(aUid.iUid);
	if(index >= 0)
		{
		if(aSet)
			iAppInfo[index].iFlags |= aFlag;
		else
			iAppInfo[index].iFlags &= ~aFlag;
		}
	__ALFFXLOGSTRING2("CAppInfoCache::SetAppFlags - 0x%x, index: %d (alf)", aUid, index);
	}
	
 
TBool CAppInfoCache::GetAppFlags(const TUid& aUid, TUint aFlag) const
 	{
 	return TBool(iAppInfo[IndexOf(aUid.iUid)].iFlags & aFlag);
 	}

	
void CAppInfoCache::SetAvkonUid(const TUid& aUid)
	{
    __ALFFXLOGSTRING1("CAppInfoCache::SetAvkonUid 0x%x (alf)", aUid)
  	SetAppFlags(aUid, TAppInfo::EAvkonApp, ETrue);
	}

void CAppInfoCache::RemoveAvkonUid(const TUid& aUid)
	{
    __ALFFXLOGSTRING1("CAppInfoCache::RemoveAvkonUid 0x%x (alf)", aUid)
	SetAppFlags(aUid, TAppInfo::EAvkonApp, EFalse);
	}


TBool CAppInfoCache::AvkonUid(const TUid& aUid) const
	{
	TInt index = IndexOf(aUid.iUid);
	if (index!=KErrNotFound)
	    {
	    return (iAppInfo[index].iFlags & TAppInfo::EAvkonApp);
	    }
	else
	    {
	    return EFalse;
	    }
	}
	
	
void CAppInfoCache::Reset(const TUid& aUid)
	{
 	SetParent(aUid, KNullUid);
	}


void CAppInfoCache::SetParent(const TUid& aUid, const TUid& aParentUid)
	{
	iAppInfo[IndexOf(aUid.iUid)].iParent = aParentUid;
	}

void CAppInfoCache::SetSecureId( const TUid& aUid, const TSecureId& aSecureId )
    {
    iAppInfo[IndexOf(aUid.iUid)].iSecureId = aSecureId.iId;
    }
    
void CAppInfoCache::SetWgId( const TUid& aUid, TInt aWgId )
    {
    iAppInfo[IndexOf(aUid.iUid)].iRootWgId = aWgId;
    }

     
TInt CAppInfoCache::SetAction(const TUid& aUid, TInt aAction)
    {
    if (aAction==AknTransEffect::ENone || aUid == KNullUid)
        {
        // previous action is more valid than this
        return aAction;
        }
    TInt index = IndexOf(aUid.iUid);
    
    if (index == KErrNotFound)
        {
        SetUid(aUid);
        index = IndexOf(aUid.iUid);
        if(index == KErrNotFound)
            {
            __ALFFXLOGSTRING1("CAppInfoCache::SetAction - Could not add Uid 0x%x to AppInfoCache!", aUid.iUid);
            return aAction;
			}
        }
        switch(iAppInfo[index].iAction)
            {
            case AknTransEffect::EAppStartupBackground:
                if (aAction == AknTransEffect::EApplicationExit)
                    {
                    aAction = AknTransEffect::EEmbeddedApplicationExit; 
                    }
                break;
  
            case AknTransEffect::EApplicationStartRect:
            case AknTransEffect::EApplicationStartSwitch:
            case AknTransEffect::EApplicationStartSwitchRect:
                {
                // these cannot be overwritten
                if (aAction!=KErrNotFound) // resetting the state is allowed regardless of earlier state
                    {
                    return iAppInfo[index].iAction;
                    }
                break;
                }
            }
        
        iAppInfo[index].iAction = aAction;
        __ALFFXLOGSTRING1("CAppInfoCache::SetAction - Returned action %d", iAppInfo[index].iAction);
        return iAppInfo[index].iAction;
    }

TInt CAppInfoCache::Action(const TUid& aUid)
    {
    TInt index = IndexOf(aUid.iUid);
    if (index != KErrNotFound)
        {
        return iAppInfo[index].iAction;
        }
    return KErrNotFound;
    }

TBool CAppInfoCache::IsEmbedded(const TUid& aUid) const		
	{
	return iAppInfo[IndexOf(aUid.iUid)].iParent != KNullUid;
	}


TBool CAppInfoCache::IsSameParent(const TUid& aUid1, const TUid& aUid2) const
	{
	
	if(aUid1 == KNullUid || aUid2 == KNullUid)
		return EFalse;
	
	TAppInfo appInfo = iAppInfo[IndexOf(aUid1.iUid)];
	appInfo.iParent.iUid = (appInfo.iParent == KNullUid) ? appInfo.iUid : appInfo.iParent.iUid;
	TAppInfo parentAppInfo = iAppInfo[IndexOf(aUid2.iUid)];
	parentAppInfo.iParent.iUid = (parentAppInfo.iParent == KNullUid) ? parentAppInfo.iUid : parentAppInfo.iParent.iUid;
	
	if(appInfo.iParent == parentAppInfo.iParent) // Same root?
		return ETrue;
	
	return EFalse;
	}	
	
	
const TUid& CAppInfoCache::FocusUid() const
	{
	return iFocusedUid;
	}


TBool CAppInfoCache::SetFocusUid(TInt aWgId)
	{
    if (aWgId)
    	{
		for(TInt i = 0; i < iAppInfo.Count(); i++)
			{
			if(iAppInfo[i].iRootWgId == aWgId)
				{
				iFocusedUid = TUid::Uid(iAppInfo[i].iUid);
				return ETrue;
				}
			}
		}
	return EFalse;
	}

void CAppInfoCache::ClearActions()
    {
    for (TInt i = 0; i < iAppInfo.Count(); ++i)
        {
        if ( iAppInfo[i].iAction != AknTransEffect::EAppStartupBackground )
            {
            iAppInfo[i].iAction = KErrNotFound;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWsServerDrawerController* CWsServerDrawerController::NewL(MAlfDrawerEngine* aEngine)
	{
	CWsServerDrawerController* self = new (ELeave) CWsServerDrawerController(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
CWsServerDrawerController::~CWsServerDrawerController()
	{
#ifdef WSSERVERDRAWER_TIME_LOG //time log
	delete iLogger;
#endif //WSSERVERDRAWER_TIME_LOG	
	delete iAppInfoCache;
	delete iEndCheck;
	delete iStates;
	
	iPSStates.ResetAndDestroy();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWsServerDrawerController::CWsServerDrawerController(MAlfDrawerEngine* aEngine) :
    iEngine( aEngine )
    {
    iLastAction = AknTransEffect::ENone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::AddStateL(const TUid& aUid, TInt aKey)
	{
	CPSListener* s = new (ELeave) CPSListener();
	CleanupStack::PushL(s);
	s->ListenL(aUid, aKey);
	User::LeaveIfError(iPSStates.Append(s));
	CleanupStack::Pop(); //s
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWsServerDrawerController::GetState(TInt aState) const
	{
	for(TInt i = 0; i < iPSStates.Count() ; i++)
		{
		const CPSListener* const ps = iPSStates[i]; 
		if(ps->Key() == aState)
			return ps->Value();
		}
	return 0;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::ConstructL()
	{
#ifdef WSSERVERDRAWER_TIME_LOG
	iLogger = new (ELeave)CDebugLogger();
#endif //WSSERVERDRAWER_TIME_LOG	
	iAppInfoCache = CAppInfoCache::NewL();
	iEndCheck = CEndCheck::NewL(*this);
	iStates = CStateHandler::NewL(iEngine);

	// Fill blocklist from the array
	for(TInt i = 0; i < sizeof(KNoEffectApps) / sizeof(KNoEffectApps[0]); i++)
		{
		iStates->AddBlockUid(TUid::Uid( KNoEffectApps[i] ));
		}

	for(TInt i = 1; i < sizeof(KNoEffectPairs) / sizeof(KNoEffectPairs[0]); i++)
		{
		iStates->AddBlockUidPair(TUid::Uid( KNoEffectPairs[i][0] ), TUid::Uid( KNoEffectPairs[i][1] ));
		}

/*	Disabled adding to exception list, since we have no exceptions atm.
	for(TInt i = 0; i < sizeof(KExceptionEffectApps) / sizeof(KExceptionEffectApps[0]); i++)
		{
		iStates->AddExceptionUid(TUid::Uid( KExceptionEffectApps[i] ));
		}
*/

    //AddStateL( KPSUidUikon, KUikGlobalNotesAllowed );
    AddStateL( KPSUidScreenSaver, KScreenSaverOn );
    AddStateL( KPSUidStartup, KPSStartupUiPhase );
    AddStateL( KPSUidScreenSaver, KScreenSaverPreviewMode );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
const TUid& CWsServerDrawerController::FocusUid()
	{
	return iAppInfoCache->FocusUid();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TBool CWsServerDrawerController::SetFocusUid(TInt aUid)
	{
	return iAppInfoCache->SetFocusUid(aUid);
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CWsServerDrawerController::BeginFullscreen(TInt aType, const TUid aUid1, const TUid aUid2, TInt aData )
	{	
	TUid toUid = iEngine->ToUid();
	TSecureId toSid = iEngine->ToSid();
	TInt toWg = iEngine->ToWg();
	TUid fromUid = iEngine->FromUid();
	TSecureId fromSid = iEngine->FromSid();
	TInt fromWg = iEngine->FromWg();
	TInt flags = iEngine->Flags();
#ifdef WSSERVERDRAWER_TIME_LOG //time log
	if(aFromGfx)
		{
		iLogger->Log1(_L("CWsServerDrawerController::BeginFullscreen from Gfx time %d"), iLogger->TimeMs());
		}
	else 
		{
		iLogger->Log1(_L("CWsServerDrawerController::BeginFullscreen time %d"), iLogger->TimeMs());
		}
#endif //WSSERVERDRAWER_TIME_LOG

/////////////////////
	if(aType == AknTransEffect::EParameterAvkonInternal)	
		{ 
		const TUid appuid = aUid1;
		if(appuid != KNullUid)
			{
			iAppInfoCache->SetUid(appuid);
			iAppInfoCache->SetAvkonUid(appuid);
			iAppInfoCache->SetParent(appuid, aUid2);
			iAppInfoCache->SetFocusUid(aData);
			}
		}

	// App Filter uses application uid for the unique key in the table. That causes problems as
	// there can be several instances of applications in the system which have the same uid,
	// e.g. stand-alone application and embedded applications. Because of this:
	// - We try to prevent updating App Filter flags if the instance is embedded.
	// - The existence of the parent uid do not necessarily mean that the instance is embedded.

	if ((toUid != KNullUid) && (iEngine->Action() == AknTransEffect::EEmbeddedApplicationExit))
		iAppInfoCache->SetParent(toUid, KNullUid); // Clear parent uid on exit of embedded application
		
    TBool isEmbeddedAppContext = 
    	(iEngine->Action() == AknTransEffect::EEmbeddedApplicationExit) ||
    	(iEngine->Action() == AknTransEffect::EEmbeddedApplicationStart);
    	
    iAppInfoCache->SetUid(toUid);
    if ( toUid != TUid::Null() )
        {
        iAppInfoCache->SetSecureId( toUid, toSid );
        iAppInfoCache->SetWgId( toUid, toWg );
        }
    iAppInfoCache->SetUid(fromUid);
    if ( fromUid != TUid::Null() )
        {
        iAppInfoCache->SetSecureId( fromUid, fromSid );
        iAppInfoCache->SetWgId( fromUid, fromWg );
        }
		
	if ((toUid != KNullUid) && !isEmbeddedAppContext) 
		{
		//the ok is always reset in the begining		
		if(flags & AknTransEffect::TParameter::EResetServerStats)
			{ //this should be called always before avkoncheck or noeffects etc for apps
			iAppInfoCache->Reset(toUid);
			}	
		
		//Adding uid to block list			
		if(flags & AknTransEffect::TParameter::ENoEffects)
			{
            __ALFFXLOGSTRING1("CWsServerDrawerController::BeginFullscreen - Add to block list 0x%x", toUid);
			iStates->AddBlockUid(toUid);
			if (iLastToUid == aUid1)
			    {
                AbortTransition(EAbortFullscreen);
			    }
			}
		//Removing uid from blocklist
		if(flags & AknTransEffect::TParameter::EEnableEffects)	
			{
            __ALFFXLOGSTRING1("CWsServerDrawerController::BeginFullscreen - Remove from block list 0x%x", toUid);
			iStates->RemoveBlockUid(toUid);
			}
		}
		
	CStateHandler::TFullscreenType fstype = iStates->GetFullscreenType(iEngine->Action());
	CStateHandler::TFullscreenType currtype = iStates->GetCurrentFullscreenType();

	//application has to be avkon app, therefore thise
	//flag has to be set twice!	
	if(flags & AknTransEffect::TParameter::EAvkonCheck)
		{
		if(fstype == CStateHandler::EExit) //application will exit and avkon flags must be reset (otherwise
		    {
			iAppInfoCache->RemoveAvkonUid(toUid); //repeating calls may allow check pass
		    }
		else
		    {
			iAppInfoCache->SetAvkonUid(toUid);
		    }
		}
		
// TODO: All control has been disabled as it seems to block too much,
// it appears that the states are not handled correctly or something else is missing		

    // if we are currenly running a layoutswitch let it only be aborted by
    // another layoutswitch.
    if( (iStates->GetCurrentFullscreenType() == CStateHandler::ELayoutSwitch) &&
        (iEngine->Action() != AknTransEffect::ELayoutSwitch))
        {
        return;
        }

	//activation after exit is not allowed (also if exit didnt result in an effect)
	if(iLastTypeTried == CStateHandler::EExit &&
       fstype == CStateHandler::EActivation)
		{ //the current uid is not valid
        __ALFFXLOGSTRING("CWsServerDrawerController::BeginFullscreen - Last tried type was exit. Abort.");
        return; // activation not ok if exiting
		}
		
    iLastTypeTried = fstype;

    //activation after exit is not allowed...even it was aborted
	if(iExitAborted &&
	 CStateHandler::EActivation == fstype) 
		{ //the current uid is not valid
        __ALFFXLOGSTRING("CWsServerDrawerController::BeginFullscreen - Exit was aborted. Activation should not happen. Abort.");
		AbortTransition();  //if exit is aborted, we dont want either activation	
    	return; // activation not ok if exiting
		}

	//dsa apps do not have effects
	//this is one reason why for activation end cannot be called immediately after begin
	//we should know if its a dsa app before end can be called
	if(iDSAActive)
	    {
        __ALFFXLOGSTRING("CWsServerDrawerController::BeginFullscreen - DSA active. Abort.");
	    return; //Do nothing if dsa active.
	    }

	//Special argument calls does not initiate any FS effect and is caught here.
	if(fstype == CStateHandler::ENone) 
		{
        __ALFFXLOGSTRING("CWsServerDrawerController::BeginFullscreen - Special argument. Abort.");
		return;
		}


	//Phone is booting, stop any FS effect.
	if(!StartCheck(flags)) 
		{
        __ALFFXLOGSTRING("CWsServerDrawerController::BeginFullscreen - Phone is booting. Abort.");
        AbortTransition();
		return;
		}		
	
	// if either toUid or fromUid is in the custom list for applications
	// that should be blocked then we abort all ongoing transitions.
	if(!(AllowedCustomUid(toUid) && AllowedCustomUid(fromUid)))
        {
        __ALFFXLOGSTRING2("CWsServerDrawerController::BeginFullscreen - Blocked uid 0x%x or 0x%x. Abort.", toUid, fromUid);
        AbortTransition();
        return;
        }
			
	// No effect if a second exit effect comes in while exit effect is ongoing.
    // Double exit actually looks quite nice...
    // We are doing separate visuals, not screen capture, so there is no need to
    // filter this out
    /*
	if((currtype == fstype) && currtype == CStateHandler::EExit) 
		{
		return;
		}
	*/	
	
	//Filter out anything but exit effect if start effect is running.
    /*
	if((currtype == CStateHandler::EStart) && (fstype != CStateHandler::EExit)) 
		{
		return;
		}
	*/	

	// this check is for embedded apps/operator menu case      ///// This check aborts ongoing. Is this right?
	// abort and skip transition if no current  uid		          // Maybe should be added to statemachine.
	// TODO: Revise when appuids available from wserv
	/*
	if( toUid == KNullUid )
		{
    	return;	
		}		
*/
	//activation switch effects are not allowed for non avkon apps as they never will
	//inform their foregound - so no transition.
	if(CStateHandler::EActivation == fstype && currtype != CStateHandler::EStart &&
	 !iAppInfoCache->AvkonUid(toUid))
		{
        __ALFFXLOGSTRING1("CWsServerDrawerController::BeginFullscreen - Non avkon app 0x%x. Abort.", toUid);
		return;
		}

    if(iEngine->Action() == AknTransEffect::ELayoutSwitch) // ELayoutSwitchOut is for two phases solution
		{
		if( iLastFromUid.iUid == KScreensaverUid ) //TODO: Better test needed here?
            {
            iEngine->Action() = 1001; // 1002: start, 1001, exit
            iEngine->ToUid() = TUid::Uid(KScreensaverUid);
            iEngine->FromUid() = TUid::Uid(0);
            iEngine->Flags() |= AknTransEffect::TParameter::EActivateExplicitContinue;
            }
        else if( iLastToUid.iUid == KScreensaverUid ) //TODO: Better test needed here?
            {    
            iEngine->Action() = 1002; // 1002: start, 1001, exit
            iEngine->ToUid() = TUid::Uid(KScreensaverUid);
            iEngine->FromUid() = TUid::Uid(0);
            iEngine->Flags() |= AknTransEffect::TParameter::EActivateExplicitContinue;
            }
        else 
            {
            switch(iLastAction)
                {
                case AknTransEffect::EApplicationExit:
                    iEngine->Action() = AknTransEffect::ELayoutSwitchExit;
                    break;
                case AknTransEffect::EApplicationStartRect:
                case AknTransEffect::EApplicationStart:
                    iEngine->Action() = AknTransEffect::ELayoutSwitchStart;
                    break;
                //case AknTransEffect::EApplicationActivate: // Does it work only for application active?
                default: 
                   break;
                }
            }    
        }

    if (!iStates->IsBlocked(fromUid, toUid))
        {
		iLastAction = AknTransEffect::ENone;
		iLastFromUid = KNullUid; 
		iLastToUid = KNullUid;
        iLayoutChangeActive = EFalse;
        }
    // Convert 

    // Signal statemachine
    SaveLastActionAndUid();
    iStates->Signal(CStateBase::EBeginFullscreen);
	// Start end checker always.
	if(flags & AknTransEffect::TParameter::EEndCheck)
		{
		StartEndChecker();
		}
	else
		{
		StartEndChecker(KEndTimeOut);
		}
	}
	
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//


TBool CWsServerDrawerController::StartCheck(TInt aFlags) const
	{
	if((aFlags & AknTransEffect::TParameter::EActivateExplicitContinue)) //always allow this
	    {
	    return true;
	    }
	else if(iEngine->Action() == AknTransEffect::ELayoutSwitch) //always allow layoutswitch
	    {
	    return true;
	    }
	else if (aFlags & AknTransEffect::TParameter::EAllowAtBoot) //allow unless screensaver is on
	    {
		return !TBool(GetState(KScreenSaverOn)) &&
		       !TBool(GetState(KScreenSaverPreviewMode));  //off
	    }
	else
	    {
    	return /*TBool(GetState(KUikGlobalNotesAllowed)) //on
  			&& */ iAppInfoCache->GateFound()
   		 	&& !TBool(GetState(KScreenSaverOn))
   		 	&& !TBool(GetState(KScreenSaverPreviewMode));  //off
  		 // && TBool(GetState(ESplashRunning))
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::StartEndChecker(TInt aMultiplier)
	{
	iEndCheck->Start(aMultiplier);
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::CancelEndChecker()
	{
	iEndCheck->Cancel();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::EndExpired()
	{
	__ALFFXLOGSTRING("CWsServerDrawerController::EndExpired (Alf)>>");
	AbortTransition(EAbortFullscreen);
	iExitAborted = EFalse; //This is not valid when we have a time-out
	__ALFFXLOGSTRING("CWsServerDrawerController::EndExpired (Alf)<<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CWsServerDrawerController::EndFullscreen(TBool /*aFromGfx*/)
	{
    __ALFFXLOGSTRING("CWsServerDrawerController::EndFullscreen (Alf)>>");
 	/*
	DSA end fix
	*/
	if(iDSAActive)
	    {
        __ALFFXLOGSTRING("CWsServerDrawerController::EndFullscreen - DSA Active - ABORTING (Alf)");
	    AbortTransition();
	    return; //Do nothing if dsa active.
	    }
	    

#ifdef WSSERVERDRAWER_TIME_LOG //time log
	iLogger->Log1(_L("CWsServerDrawerController::EndFullscreen time %d"), iLogger->TimeMs());
#endif	//WSSERVERDRAWER_TIME_LOG

	iStates->Signal(CStateBase::EEndFullscreen);
	// the callback for FullScreenFinished will not come. this must be resetted here.
	iLastTypeTried = CStateHandler::ENone; 
	}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CWsServerDrawerController::FullscreenFinished(TInt aHandle)
	{
	if(aHandle == iEngine->CurrentFullScreenHandle()) // Filter away stray finish signals.
		{
		iLastTypeTried = CStateHandler::ENone;
		iStates->Signal(CStateBase::EFinishFullscreen);
		}
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TInt CWsServerDrawerController::BeginControlTransition()
	{
	if(iDSAActive)
	    {
	    return KErrAbort;
	    }

    if(iLayoutChangeActive)
        {
        return KErrAbort;
        }
    // Check booting time disabling & screensaver
    __ALFFXLOGSTRING2("GetState(KUikGlobalNotesAllowed) %d, GetState(KScreenSaverOn)), %d GetState(KScreenSaverPreviewMode) %d",
    		/*TBool(GetState(KUikGlobalNotesAllowed)),*/
    		TBool(GetState(KScreenSaverOn)),
    		TBool(GetState(KScreenSaverPreviewMode)));
    
    if(/*TBool(GetState(KUikGlobalNotesAllowed))==EFalse || */
	   TBool(GetState(KScreenSaverOn)) ||
	   TBool(GetState(KScreenSaverPreviewMode)))
		{
		return KErrAbort;
		}

	iStates->Signal(CStateBase::EBeginComponent);
	
	if(iStates->GetState() == CStateBase::EComponent) 
		{
		return iEngine->CurrentControlHandle();
		}
	else
		{
		return KErrAbort;
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CWsServerDrawerController::EndControlTransition(TInt aHandle)
	{
	if(aHandle == iEngine->CurrentControlHandle()) //Filter out stray endcomponent.
		{
		iStates->Signal(CStateBase::EFinishComponent);
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::ScreenModeChange(TBool aBeforeScreenModeChange)
	{
	if( aBeforeScreenModeChange )
		{
		AbortTransition();
        iLayoutChangeActive = ETrue;
        }
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::Error( TServerDrawerError /*aError*/ )
    {
    AbortTransition();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::SaveLastActionAndUid()
    {
    iLastAction = iEngine->Action();
    iLastFromUid = iEngine->FromUid(); 
    iLastToUid = iEngine->ToUid();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::DSABegin()
    {
    AbortTransition();
    iDSAActive = ETrue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::DSAEnd()
    {
    iDSAActive = EFalse;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWsServerDrawerController::AbortTransition(TInt aToAbort)
    {
    __ALFFXLOGSTRING("CWsServerDrawerController::AbortTransition (Alf) >>");
    if ( aToAbort == EAbortFullscreen )
        {
        iLastTypeTried = CStateHandler::ENone;
	    iExitAborted = iStates->GetCurrentFullscreenType() == CStateHandler::EExit;
	    __ALFFXLOGSTRING1("CWsServerDrawerController::AbortTransition , iExitAborted %d (Alf)", iExitAborted);
        iStates->Signal(CStateBase::EAbortFullscreen);
        iExitAborted = EFalse; // end of story for exit effect.
        }
    else if ( aToAbort == EAbortControl )
        {
        iStates->Signal(CStateBase::EAbortComponent);
        }
    else if( (aToAbort & EAbortControl) && (aToAbort & EAbortFullscreen) ) 
    	{
    	iLastTypeTried = CStateHandler::ENone;
	    iExitAborted = iStates->GetCurrentFullscreenType() == CStateHandler::EExit;
	    __ALFFXLOGSTRING1("CWsServerDrawerController::AbortTransition , iExitAborted %d (Alf)", iExitAborted);
	    iStates->Signal(CStateBase::EAbort);
    	iExitAborted = EFalse; // end of story for exit effect.
    	}
	__ALFFXLOGSTRING("CWsServerDrawerController::AbortTransition (Alf) <<");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CWsServerDrawerController::IsBlocked( const TUid& aFromUid, const TUid& aToUid )
    {
    TBool result = iStates->IsBlocked( aFromUid, aToUid );
    __ALFFXLOGSTRING1("CWsServerDrawerController::IsBlocked - return %d (Alf)", result);
    return result;
    }

